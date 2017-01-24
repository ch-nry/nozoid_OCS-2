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

uint32_t freq, freq_old, reso, reso_local;
int32_t in1_old,in2_old, in3_old, in4_old, in5_old;

inline void set_LP24() { 
  G1 = 0;
  G2 = 0;
  G3 = 0;
  G5 = 13;
}

inline void set_LP12() {
  G1 = 0;
  G2 = 0;
  G3 = 8;
  G5 = 0;
}

inline void set_BP12() {
  G1 = 0;
  G2 = 9;
  G3 = -9;
  G5 = 0;
}

inline void set_HP12() {
  G1 = 4;
  G2 = -8;
  G3 = 4;
  G5 = 0;
}

inline void init_VCF() {
  in1_old = 0;
  in2_old = 0;
  in3_old = 0;
  in4_old = 0;
  in5_old = 0;
}

inline void VCF_freq() {
  uint32_t tmp, tmp2, mod1, mod2, tmp_interpol1, tmp_interpol2;
  int32_t tmpS;
  
  freq_old = hysteresis16(adc_value16[VCF_FQ], freq_old);
  tmp = freq_old;

  tmp = (tmp<<13) + (tmp<<12);
  tmp += (1<<30) + (1<<27) + (1<<26) - (69<<23); 
  tmp += (VCF_pitch)? KEY_LOCAL<<3: (69<<23); 
  
  mod1 = clip_ana_low(adc_value[VCF_MOD1]);
  mod2 = clip_ana_low(adc_value[VCF_MOD2]);
  tmpS  = (mod1 * modulation_data[modulation_index[index_VCF_MOD1]]);
  tmpS += (mod2 * modulation_data[modulation_index[index_VCF_MOD2]]);
  tmp += tmpS;
  tmp_interpol1 = (tmp > 0x7DE80000)? 0x7DE80000: tmp; 
  
  tmp_interpol2 = tmp_interpol1 & 0x001FFFFF; //21 low byte
  tmp_interpol1 >>= 21;
  tmp = table_CV2cutof[tmp_interpol1];
  tmp2 = table_CV2cutof[tmp_interpol1+1] - tmp; //24 bits max
  tmp_interpol2 >>= 13; // keep 8 bits
  tmp2 *= tmp_interpol2;
  tmp2 >>= 8;
  tmp += tmp2;
  freq = tmp;

  reso_local = hysteresis16(adc_value16[VCF_Q], reso_local); // frequency compensation is not so important, we can skip it.

  reso = reso_local<<15;
  //analog_out_1(reso_local<<16);
}

inline int32_t VCF(int32_t MIX_out) {
  int32_t in, out, tmp, tmp1, VCF_out;

  in = MIX_out<<4; // 28 bits
  in -= 1<<27; // center

  // feedback
  tmp = in5_old - (in >> 1) ;
  tmp = m_s32xs32_s32H(tmp, reso);
  tmp <<= 2;
 
  //analog_out_1((tmp<<2)^0x80000000);

  // distortion
  tmp1 = tmp;
  tmp = m_s32xs32_s32H(tmp1, abs(tmp1));
  tmp = tmp1 - (tmp<<4);
  tmp += tmp >> 2;
  tmp += tmp1;
  
  tmp += (tmp>>2) + (tmp>>3); // a bit more gain that is not mandatory in the simulation

  //analog_out_2((tmp<<2)^0x80000000);

  tmp = in - tmp;

  // LOP 1
  tmp = (tmp >> 8) * 197;  
  tmp += (in1_old >> 8) * 59;
  in1_old = in;
  tmp = m_s32xs32_s32H(tmp-in2_old, freq);
  tmp <<= 2;
  tmp += in2_old;
  tmp1 = tmp; 
  // LOP 2
  tmp = (tmp >> 8) * 197;  
  tmp += (in2_old >> 8) * 59;
  in2_old = tmp1;
  tmp = m_s32xs32_s32H(tmp-in3_old, freq);
  tmp <<= 2;
  tmp += in3_old;
  tmp1 = tmp; 
  // LOP 3
  tmp = (tmp >> 8) * 197;  
  tmp += (in3_old >> 8) * 59;
  in3_old = tmp1;
  tmp = m_s32xs32_s32H(tmp-in4_old, freq);
  tmp <<= 2;
  tmp += in4_old;
  tmp1 = tmp; 
  // LOP 4
  tmp = (tmp >> 8) * 197;  
  tmp += (in4_old >> 8) * 59;
  in4_old = tmp1;
  tmp = m_s32xs32_s32H(tmp-in5_old, freq);
  tmp <<= 2;
  tmp += in5_old;
  in5_old = tmp;
  
  VCF_out = G1*in1_old + G2*in2_old + G3*in3_old + G5*in5_old; // G4 is not used with this filter selection

  //analog_out_1(MIX_out^0x80000000);
  //analog_out_2(VCF_out^0x80000000);
  return(VCF_out);
}

