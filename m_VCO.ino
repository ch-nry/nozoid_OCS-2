// --------------------------------------------------------------------------
// This file is part of the OCS-2 firmware.
//
//    OCS-2 firmware is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OCS-2 firmware is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OCS-2 firmware.  If not, see <http://www.gnu.org/licenses/>.
// --------------------------------------------------------------------------

uint32_t VCO1_phase, VCO2_phase, last_out, last_out2;
uint32_t VCO1_pink_filter, VCO2_pink_filter;
uint32_t freq1_old, freq2_old;
uint32_t VCO1_BLsize, VCO2_BLsize;
uint32_t VCO1_goal, VCO2_goal;
uint32_t VCO1_fq, VCO2_fq;
uint32_t white1, white2;
uint32_t dephasage12, dephasage13, dephasage14;
uint32_t dephasage22, dephasage23, dephasage24;
uint32_t VCO1_out_save, VCO2_out_save;

inline void init_VCO() {
  VCO1_WF = 0;
  VCO2_WF = 0;
  dephasage12 = 0;
  dephasage13 = 0;
  dephasage14 = 0;
  dephasage22 = 0;
  dephasage23 = 0;
  dephasage24 = 0;
}

inline uint32_t VCO_noise() {
   uint32_t tmp;
   tmp = random32();
   tmp -= tmp>>2;
   tmp += 0x20000000; 
   return(tmp);
 }

inline void VCO1_freq() {
  uint32_t freq, freq_MSB, freq_LSB, increment1, increment2;
  int32_t tmpS;
  uint32_t mod1, mod2;

  mod1 = clip_ana_low(adc_value[VCO1_MOD1]);
  mod2 = clip_ana_low(adc_value[VCO1_MOD2]);
  
  freq1_old = hysteresis16(adc_value16[VCO1_FQ], freq1_old);
  freq = freq1_old;

  freq <<= 10; // 27 significant bit
  freq += (freq>>1);// + (freq>>2);
  
  freq >>= VCO_FQ_reduce;
  freq += VCO_FQ_offset;
  
  tmpS  = (mod1 * modulation_data[modulation_index[index_VCO1_MOD1]]);
  tmpS += (mod2 * modulation_data[modulation_index[index_VCO1_MOD2]]);
  tmpS >>= 2;
  freq += tmpS;
  freq += KEY_LOCAL;
  
  freq += (1<<26) + 0x10000000;
  freq = freq<0x10000000?0:freq-0x10000000;
  freq = (freq>0X0FFFFFFF)? 0X0FFFFFFF: freq;
  
  VCO1_fq = freq; // saved for link mode
  
  freq_MSB = freq >> 18; // keep the 1st 10 bits
  freq_LSB = freq & 0x3FFFF; // other 18 bits
  increment1 = table_CV2increment[freq_MSB];
  increment2 = table_CV2increment[freq_MSB+1];
  increment2 -= increment1;
  increment1 += ((increment2>>8)*(freq_LSB>>2))>>8;
  
  noInterrupts();
  VCO1_increment = increment1;
  VCO1_BLsize = (0xFFFFFFFF / increment1)<<4; 
  interrupts();

}

inline void VCO2_freq() {
  uint32_t freq, freq_MSB, freq_LSB, increment1, increment2;
  int32_t tmpS;
  uint32_t mod1, mod2;

  mod1 = clip_ana_low(adc_value[VCO2_MOD1]);
  mod2 = clip_ana_low(adc_value[VCO2_MOD2]);
  
  freq2_old = hysteresis16(adc_value16[VCO2_FQ], freq2_old);
  freq = freq2_old;

  if (VCO_link) { 
    freq = (freq < 0xC0)? 0: freq - 0xC0;
    freq = (freq > 0xFEFF)? 0xFEFF: freq;
    freq += freq >> 15; 
    freq += freq >> 8;
    freq = VCO1_fq + (192*freq);
  }
  else {    
    freq <<= 10; // 27 significant bit
    freq += (freq>>1);// + (freq>>2);
    freq >>= VCO_FQ_reduce;
    freq += VCO_FQ_offset;
    freq += (1<<26);
    freq += KEY_LOCAL;
  }
  
  tmpS  = (mod1 * modulation_data[modulation_index[index_VCO2_MOD1]]);
  tmpS += (mod2 * modulation_data[modulation_index[index_VCO2_MOD2]]);
  tmpS >>= 2;
  freq += tmpS;
  
  VCO2_fq = freq; // for white noise filter
  
  freq_MSB = freq >> 18; // keep the 1st 10 bits
  freq_LSB = freq & 0x3FFFF; // other 18 bits
  increment1 = table_CV2increment[freq_MSB];
  increment2 = table_CV2increment[freq_MSB+1];
  increment2 -= increment1;
  increment1 += ((increment2>>8)*(freq_LSB>>2))>>8;
  VCO2_increment = increment1;
  
  noInterrupts();
  VCO2_increment = increment1;
  VCO2_BLsize = (0xFFFFFFFF / increment1)<<4; 
  interrupts();
}

inline uint32_t sawBL(uint32_t phase, uint32_t BLsize) {
uint32_t sinc, table_index;

    table_index = phase;
    table_index = (table_index & 0x80000000)? -table_index: table_index;
    sinc = m_u32xu32_u32H(table_index, BLsize); 
    sinc = (sinc & 0xFFFFF000)? 0xFFF : sinc; 
    sinc = table_Isinc[sinc];
    sinc = m_s32xs32_s32H(phase^0x80000000, (sinc << 16)^0x80000000);
    sinc -=  phase >> 1;
    sinc -= 0x40000000;
    sinc += sinc >>1;
    sinc -= 0x40000000;
    return(sinc);
}

inline uint32_t VCO1() {
  uint32_t freq, i,j, tmp, inc, phaseL, phaseH, Wfilter;
  uint32_t tmp1, tmp2, tmp3, tmp4; 
  int32_t tmp32S, tmpS, tmpS2, test;
  uint32_t sinc, tmp32;
  uint64_t tmp64;
  uint32_t VCO1_out;
  
  inc = VCO1_increment;

  switch (VCO1_WF) {
    case 0: // sinus // todo : optimiser interpolation comme ds les WF par table d'onde sinus (?)
      VCO1_phase += inc<<3;
      // linear interpolation
      phaseH = VCO1_phase>>19; // 13 bits
      phaseL = (VCO1_phase) & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp32S = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp32S += tmp1>>1;
      tmp32S += (1<<26) + (tmp32S>>1);
      VCO1_out = tmp32S + 0x20000000;
      break;

    case 1: // sin(sin)
      VCO1_phase += inc<<2;
      
      tmp4 = VCO1_phase << 1; // phase sin 1
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp1 = tmp3 >> 1;
      last_out -= last_out >> 3;
      last_out += tmp1 >> 3;

      tmp1 = last_out;
      //tmp1 += 0; // symetrie central
      // tmp1 += 1 << 30; // symetrie 
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
            
      VCO1_out = tmp3 + (tmp3 << 1) + 0x50000000;
      break;
 
    case 2: // Mod Frequency
      VCO1_phase += inc<<3;
      dephasage12 += VCO_WF2_speed; 
      
      tmp4 = VCO1_phase << 1; // phase sin 1
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp4 += VCO1_phase; // phase sin 2
      tmp4 += 0x23456789;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp4 = (tmp2-tmp1) * phaseL >> 9;
      tmp4 += tmp1>>1;

      tmp1 = tmp3 >> 1;
      tmp1 += tmp4 >> 2;      
      last_out -= last_out >> 4;
      last_out += tmp1 >> 4;

      tmp1 = last_out << 2;
      //tmp1 += 0; // symetrie central
      // tmp1 += 1 << 30; // symetrie 
      //tmp1 += 0x23456789;
      tmp1 += dephasage12;
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
      
      VCO1_out = tmp3  + (tmp3 >> 2) + 0x30000000;
      break;

    case 3: // Mod Frequency
      VCO1_phase += inc<<3;
      dephasage13 += VCO_WF3_speed; 

      tmp4 = VCO1_phase << 1; // phase sin 1
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp4 += VCO1_phase; // phase sin 2
      tmp4 += dephasage13;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp4 = (tmp2-tmp1) * phaseL >> 9;
      tmp4 += tmp1>>1;

      tmp1 = tmp3 >> 2;
      tmp1 += tmp4 >> 2;      
      last_out -= last_out >> 4;
      last_out += tmp1 >> 4;

      tmp1 = last_out << 2;
      //tmp1 += 0; // symetrie central
      //tmp1 += 1 << 30; // symetrie 
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
      
      VCO1_out = tmp3  + (tmp3 >> 2) + 0x30000000;
      break;

    case 4: // Mod Frequency
      VCO1_phase += inc<<3;
      dephasage14 += VCO_WF4_speed; 
      
      tmp4 = VCO1_phase << 1; // phase sin 1
      tmp4 += 0x40000000;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp4 += VCO1_phase; // phase sin 2
      tmp4 += dephasage14;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp4 = (tmp2-tmp1) * phaseL >> 9;
      tmp4 += tmp1>>1;

      tmp1 = tmp3 >> 2;
      tmp1 += tmp4 >> 2;      
      last_out -= last_out >> 3;
      last_out += tmp1 >> 3;

      tmp1 = last_out << 2;
      tmp1 += 0x40000000;
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
  
      VCO1_out = tmp3  + (tmp3 >> 1) + 0x20000000;
      break;

    case 5: // triangle
      VCO1_phase += inc<<3;
      tmp1 = sqr_s32_s32(VCO1_phase<<1);
      tmp2 = ((VCO1_phase+0x3FFFFFFF) & 0x80000000) ? tmp1 : 0x7FFFFFFF - tmp1;
      tmp1 = (last_out-tmp2); // signed
      last_out = tmp2;
      tmp2 = 0xFFFFFFFF/inc; // correction d'amplitude
      tmp64 = m_s32xs32_s64(tmp1, tmp2);
      tmp64 >>= 5;
      tmp1 = tmp64;
      tmp1 ^= 0x80000000;
      tmp1 -= tmp1 >> 3;
      tmp1 += 0x10000000;
      VCO1_out = tmp1;
      break;
      
    case 6: // square
      VCO1_phase += inc<<3;
      tmp1 = sawBL(VCO1_phase, VCO1_BLsize);
      tmp32 = sawBL(VCO1_phase+0x80000000, VCO1_BLsize);
      tmp1 -= tmp32;
      tmp1 += 0x80000000;
      tmp1 -= tmp1 >> 4;
      tmp1 += 0x08000000;
      VCO1_out = tmp1;
      break;
      
    case 7: // impulse
      VCO1_phase += inc<<3;
      tmp1 = sawBL(VCO1_phase, VCO1_BLsize)>>1;
      tmp1 -= sawBL(VCO1_phase+0xF0000000, VCO1_BLsize)>>1;
      tmp1 +=0x20000000;
      tmp1 += (tmp1 >> 1) + (tmp1 >> 2);
      VCO1_out = tmp1;
      break;

    case 8: // PWM on LFO1
      VCO1_phase += inc<<3;
      tmp = (modulation_data[mod_LFO1])<<15;
      tmp2 = (tmp + (1<<30))>>1;
      tmp += 0x50000000; 
      tmpS  = sawBL(VCO1_phase, VCO1_BLsize) ^ 0x80000000;
      tmpS -= tmpS>>3;
      tmpS2 = sawBL(VCO1_phase+tmp, VCO1_BLsize) ^ 0x80000000;
      tmpS2 -= tmpS2>>3;
      tmpS2 -= tmpS; // signed : no offset
      tmp = tmpS2 + tmp2 + 0X48000000;
      VCO1_out = tmp;
      break;
      
    case 9: // saw step
      VCO1_phase += inc<<3;
      tmp1 = sawBL(VCO1_phase, VCO1_BLsize);
      tmp1 -= sawBL((VCO1_phase<<2), VCO1_BLsize>>2)>>2;
      tmp1 += tmp1 >> 2;
      VCO1_out = tmp1;
      break;
      
    case 10: // saw
      VCO1_phase += inc<<2;
      tmp1 = sawBL(VCO1_phase, VCO1_BLsize<<1);
      tmp1 += sawBL(VCO1_phase + 0x80000000, VCO1_BLsize<<1);
      VCO1_out = tmp1 + 0x80000000;
      break;
      
    case 11: // multi saw
      VCO1_phase += inc<<3;
      tmp1 = sawBL(VCO1_phase, VCO1_BLsize)>>1;
      tmp2 = sawBL(VCO1_phase+0x30000000, VCO1_BLsize)>>1;
      tmp1 += tmp2;
      tmp1 += tmp1 >> 2;
      VCO1_out = tmp1 - 0x18000000;
      break;

    case 12: // multi saw pwm
      tmp = (modulation_data[mod_LFO1])<<14;
      tmp += 0x40000000;
      VCO1_phase += inc<<3;
      tmp1 = sawBL(VCO1_phase, VCO1_BLsize)>>1;
      tmp1 += sawBL(VCO1_phase+tmp, VCO1_BLsize)>>1;
      tmp1 += tmp1 >> 2;
      VCO1_out = tmp1 - 0x20000000;
      break;

    case 13: //white filtered
      test = VCO1_fq - (1<<27);
      Wfilter = (test<0)? 0: test;
      Wfilter = (Wfilter  >> 12);
      Wfilter *= Wfilter;
      Wfilter >>= 15;
  
      VCO1_goal = random32() >> 17;
      white1 += (int32_t)((int32_t)(VCO1_goal-white1) *  Wfilter)>>16;
      VCO1_out = white1 << 17;
      break;
      
    case 14: //binary noise;
      tmp1 = VCO1_phase;
      VCO1_phase += inc<<2;
      VCO1_out_save = (tmp1 > VCO1_phase)? VCO_noise() : VCO1_out_save;
      VCO1_out = VCO1_out_save;
      break;

    case 15: //audio in
      VCO1_out = (audio_inR - (audio_inR >> 2)) ^ 0x80000000;
      break;
  }
  
  tmp = VCO1_out; 
  tmp >>= 16; // keep only 16bit
  tmp -= (1<<15); // switch to signed
  modulation_data[mod_VCO1] = tmp;
  //analog_out_2(VCO1_out);
  //analog_out_1(LFO1_phase);
  return(VCO1_out);
}
 
inline uint32_t VCO2() {
  uint32_t freq, i,j, tmp, inc, phaseL, phaseH, Wfilter;
  uint32_t tmp1, tmp2, tmp3, tmp4;
  int32_t tmp32S, tmpS, tmpS2, test;
  uint32_t sinc, tmp32;
  uint64_t tmp64;
  uint32_t VCO2_out;

  inc = VCO2_increment;

 switch (VCO2_WF) {
    case 0: // sinus
      VCO2_phase += inc<<3;
      // linear interpolation
      phaseH = VCO2_phase>>19; // 13 bits
      phaseL = (VCO2_phase) & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp32S = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp32S += tmp1>>1;
      tmp32S += (1<<26) + (tmp32S>>1);
      VCO2_out = tmp32S + 0x20000000;
      break;

    case 1: // sin(sin)
      VCO2_phase += inc<<2;
      
      tmp4 = VCO2_phase << 1; // phase sin 2
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp1 = tmp3 >> 1;
      last_out2 -= last_out2 >> 3;
      last_out2 += tmp1 >> 3;

      tmp1 = last_out2;
      //tmp1 += 0; // symetrie central
      // tmp1 += 1 << 30; // symetrie 
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
      
      VCO2_out = tmp3 + (tmp3 << 1) + 0x50000000;
      break;
 
    case 2: // Mod Frequency
      VCO2_phase += inc<<3;
      dephasage22 += VCO_WF2_speed;
            
      tmp4 = VCO2_phase << 1; // phase sin 1
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp4 += VCO2_phase; // phase sin 2
      tmp4 += 0x23456789;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp4 = (tmp2-tmp1) * phaseL >> 9;
      tmp4 += tmp1>>1;

      tmp1 = tmp3 >> 1;
      tmp1 += tmp4 >> 2;      
      last_out2 -= last_out2 >> 4;
      last_out2 += tmp1 >> 4;

      tmp1 = last_out2 << 2;
      //tmp1 += 0; // symetrie central
      // tmp1 += 1 << 30; // symetrie 
      //tmp1 += 0x23456789;
      tmp1 += dephasage22;
            
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
      
      VCO2_out = tmp3  + (tmp3 >> 2) + 0x30000000;
      break;

    case 3: // Mod Frequency
      VCO2_phase += inc<<3;
      dephasage23 += VCO_WF3_speed; 

      tmp4 = VCO2_phase << 1; // phase sin 1
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp4 += VCO2_phase; // phase sin 2
      tmp4 += VCO_WF3_speed;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp4 = (tmp2-tmp1) * phaseL >> 9;
      tmp4 += tmp1>>1;

      tmp1 = tmp3 >> 2;
      tmp1 += tmp4 >> 2;      
      last_out2 -= last_out2 >> 4;
      last_out2 += tmp1 >> 4;

      tmp1 = last_out2 << 2;
      //tmp1 += 0; // symetrie central
      //tmp1 += 1 << 30; // symetrie 
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
      
      VCO2_out = tmp3  + (tmp3 >> 2) + 0x30000000;
      break;

    case 4: // Mod Frequency
      VCO2_phase += inc<<3;
      dephasage24 += VCO_WF4_speed; 

      tmp4 = VCO2_phase << 1; // phase sin 1
      tmp4 += 0x40000000;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = (tmp2-tmp1) * phaseL >> 9;
      tmp3 += tmp1>>1;

      tmp4 += VCO2_phase; // phase sin 2
      tmp4 += dephasage14;
      phaseH = tmp4>>19; // 13 bits
      phaseL = tmp4 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp4 = (tmp2-tmp1) * phaseL >> 9;
      tmp4 += tmp1>>1;

      tmp1 = tmp3 >> 2;
      tmp1 += tmp4 >> 2;      
      last_out2 -= last_out2 >> 3;
      last_out2 += tmp1 >> 3;

      tmp1 = last_out2 << 2;
      tmp1 += 0x40000000;
      
      phaseH = tmp1>>19; // 13 bits
      phaseL = tmp1 & 0x7FFFF; // 19 bits
      tmp1 = table_cos[phaseH];
      tmp2 = table_cos[(phaseH + 1)];
      tmp3 = m_s32xs32_s32H(tmp2-tmp1, phaseL<<12);
      tmp3 += tmp1>>1;
  
      VCO2_out = tmp3  + (tmp3 >> 1) + 0x20000000;
      break;

    case 5: // triangle
      VCO2_phase += inc<<3;
      tmp1 = sqr_s32_s32(VCO2_phase<<1);
      tmp2 = ((VCO2_phase+0x3FFFFFFF) & 0x80000000) ? tmp1 : 0x7FFFFFFF - tmp1;
      tmp1 = (last_out2-tmp2); // signed
      last_out2 = tmp2;
      tmp2 = 0xFFFFFFFF/inc; // correction d'amplitude
      tmp64 = m_s32xs32_s64(tmp1, tmp2);
      tmp64 >>= 5;
      tmp1 = tmp64;      
      tmp1 ^= 0x80000000;
      tmp1 -= tmp1 >> 3;
      tmp1 += 0x10000000;
      VCO2_out = tmp1;
      break;

    case 6: // square
      VCO2_phase += inc<<3;
      tmp1 = sawBL(VCO2_phase, VCO2_BLsize);
      tmp32 = sawBL(VCO2_phase+0x80000000, VCO2_BLsize);
      tmp1 -= tmp32;
      tmp1 += 0x80000000;
      tmp1 -= tmp1 >> 4;
      tmp1 += 0x08000000;
      VCO2_out = tmp1;
      break;

    case 7: // impulse
      VCO2_phase += inc<<3;
      tmp1 = sawBL(VCO2_phase, VCO2_BLsize)>>1;
      tmp1 -= sawBL(VCO2_phase+0xF0000000, VCO2_BLsize)>>1;
      tmp1 +=0x20000000;
      tmp1 += (tmp1 >> 1) + (tmp1 >> 2);
      VCO2_out = tmp1;
      break;

    case 8: // PWM on LFO2
      VCO2_phase += inc<<3;
      tmp = (modulation_data[mod_LFO2])<<15;
      tmp2 = (tmp + (1<<30))>>1;
      tmp += 0x50000000; 
      tmpS  = sawBL(VCO2_phase, VCO2_BLsize) ^ 0x80000000;
      tmpS -= tmpS>>3;
      tmpS2 = sawBL(VCO2_phase+tmp, VCO2_BLsize) ^ 0x80000000;
      tmpS2 -= tmpS2>>3;
      tmpS2 -= tmpS; // signed : no offset
      tmp = tmpS2 + tmp2 + 0X48000000;
      VCO2_out = tmp;
      break;
      
    case 9: // saw step
      VCO2_phase += inc<<3;
      tmp1 = sawBL(VCO2_phase, VCO2_BLsize);
      tmp1 -= sawBL(VCO2_phase<<2, VCO2_BLsize>>2)>>2;
      tmp1 += tmp1 >> 2;
      VCO2_out = tmp1;
      break;

    case 10: // saw
      VCO2_phase += inc<<2;
      tmp1 = sawBL(VCO2_phase, VCO2_BLsize);
      tmp1 += sawBL(VCO2_phase + 0x80000000, VCO2_BLsize);
      VCO2_out = tmp1 + 0x80000000;
      break;
      
    case 11: // multi saw
      VCO2_phase += inc<<3;
      tmp1 = sawBL(VCO2_phase, VCO2_BLsize)>>1;
      tmp2 = sawBL(VCO2_phase+0x30000000, VCO2_BLsize)>>1;
      tmp1 += tmp2;
      tmp1 += tmp1 >> 2;
      VCO2_out = tmp1 - 0x18000000;
      break;

    case 12: // multi saw pwm
      tmp = (modulation_data[mod_LFO2])<<14;
      tmp += 0x40000000;
      VCO2_phase += inc<<3;
      tmp1 = sawBL(VCO2_phase, VCO2_BLsize)>>1;
      tmp1 += sawBL(VCO2_phase+tmp, VCO2_BLsize)>>1;
      tmp1 += tmp1 >> 2;
      VCO2_out = tmp1 - 0x20000000;
      break;

    case 13: //white filtered
      test = VCO2_fq - (1<<27);
      Wfilter = (test<0)? 0: test;
      Wfilter = (Wfilter  >> 12);
      Wfilter *= Wfilter;
      Wfilter >>= 15;
      
      VCO2_goal = random32() >> 17;
      white2 += (int32_t)((int32_t)(VCO2_goal-white2) *  Wfilter)>>16;
      VCO2_out = white2 << 17;
      break;
      
    case 14: //binary noise;
      tmp1 = VCO2_phase;
      VCO2_phase += inc<<2;
      VCO2_out_save = (tmp1 > VCO2_phase)? VCO_noise() : VCO2_out_save;
      VCO2_out = VCO2_out_save;
      break;

    case 15: //audio in
      VCO2_out = (audio_inL - (audio_inL >> 2)) ^ 0x80000000;
      break;
  }
  tmp = VCO2_out; 
  tmp >>= 16; // keep only 16bit
  tmp -= (1<<15); // switch to signed
  modulation_data[mod_VCO2] = tmp;
  //analog_out_2(VCO2_out);
  return(VCO2_out);
}
