uint32_t BITCRUSH_gain, BITCRUSH_xor;
uint32_t DISTO_gain_value, DISTO_gain_old;
int32_t old_mix_out;
uint32_t effect, waveshape, waveshape_filter, effect_filter, disto_gain_filter;
int16_t delay_line[43000];
uint32_t delay_write, delay_time, delay_time_filter, delay_feedback_filter; 
int32_t delay_feedback, delay_feedback_old;

inline void init_EFFECT() {
}

inline void EFFECT_update() {
  switch(EFFECT_type) {
  case 0:
    DISTO_update();
    break;
  case 1:
    BITCRUSH_update();
    break;
  case 2:
    DELAY_update();
    break;
  case 3:
    WAVESHAPE_update();
    break;
  }
}

inline int32_t EFFECT(int32_t VCF_out) {
  int32_t effect_out, VCA_out;
  
  switch(EFFECT_type) {
  case 0:
    effect_out = DISTO(VCF_out);
    VCA_out = VCA(effect_out);
    break;
  case 1:
    effect_out = BITCRUSH(VCF_out);
    VCA_out = VCA(effect_out);
    break;
  case 2:
    VCA_out = VCA(VCF_out);
    VCA_out = DELAY(VCA_out);
    break;
  case 3:
    effect_out = WAVESHAPE(VCF_out);
    VCA_out = VCA(effect_out);
    break;
  }
  //analog_out_2(EFFECT_out ^ 0x80000000);
  return(VCA_out);
}

inline void BITCRUSH_update() {
  uint32_t tmp, tmp2;
  
  tmp = clip_ana_low16(adc_value16[EFFECT1]);
  tmp *= tmp;
  tmp >>= 2;
  tmp &= 0x3F000000;
  
  tmp = 0xFFFFFFFF - tmp;  
  BITCRUSH_gain = tmp;

  tmp = adc_value16[EFFECT2] * ((int32_t)modulation_data[modulation_index[index_EFFECT_MOD]] + (1<<15));
  BITCRUSH_xor = tmp>>1;
}

inline int32_t BITCRUSH(int32_t VCF_out)  {
  int32_t EFFECT_out;
  
  //EFFECT_out = (VCF_out & BITCRUSH_xor) & BITCRUSH_gain ; // bitcrush
  EFFECT_out = (VCF_out ^ BITCRUSH_xor) ^ BITCRUSH_gain ; // Not a real bitcrush, but i like it.
  return(EFFECT_out);
}

inline void DISTO_update() {
  uint32_t tmp;
  int32_t tmp2;
  
  tmp = clip_ana_low16(adc_value16[EFFECT1]);
  DISTO_gain_old = hysteresis16(tmp, DISTO_gain_old); 
  tmp = DISTO_gain_old;
  tmp <<= 15; // 31 bits (will be used as signed : must be positive)
  tmp2 = adc_value16[EFFECT2] * modulation_data[modulation_index[index_EFFECT_MOD]];
  tmp2 >>= 1;
  tmp2 += 0x3FFFFFFF;
  tmp += tmp2;
  tmp = tmp < 0x40000000? 0: tmp-0x40000000;
  tmp = tmp > 0x7FFFFFFF? 0x7FFFFFFF: tmp;
  DISTO_gain_value = tmp;
}

inline int32_t disto1 (int32_t in) {
  return(gain2_cliped_S32(in-m_s32xs32_s32H(in, abs(in)))); 
}

inline int32_t DISTO(int32_t VCF_out)  {
  int32_t DISTO_in, DISTO_out;
  uint32_t DISTO_gain, tmp32;
  int32_t tmpS;
 
  DISTO_in = VCF_out;
  //analog_out_1(VCF_out ^ 0x80000000);
  
  disto_gain_filter += (int32_t)(DISTO_gain_value - disto_gain_filter) >> 10;
  DISTO_gain = (disto_gain_filter<<3) & 0x7FFFFFFF; 

  DISTO_in = (disto_gain_filter & 1<<28)? disto1(DISTO_in) : DISTO_in;
  DISTO_in = (disto_gain_filter & 1<<29)? disto1(disto1(DISTO_in)) : DISTO_in;
  DISTO_in = (disto_gain_filter & 1<<30)? disto1(disto1(disto1(disto1(DISTO_in)))) : DISTO_in;
 
  //analog_out_2(DISTO_in^0x80000000);

  tmpS = m_s32xs32_s32H(DISTO_in, abs(DISTO_in))<<1;
  tmpS = m_s32xs32_s32H(DISTO_gain, DISTO_in-tmpS)<<1;

  tmpS += DISTO_in;

  DISTO_gain = (disto_gain_filter < 0x3FFFFFFF)? 0x7FFFFFFF - disto_gain_filter: 0x3FFFFFFF;

  tmpS = m_s32xs32_s32H(tmpS, DISTO_gain)<<1;
  DISTO_out = tmpS;
  //analog_out_2(DISTO_out ^ 0x80000000);
  //EFFECT_out = DISTO_out;
  return(DISTO_out);
}

inline void WAVESHAPE_update() {
  uint32_t tmp;
  int32_t tmp2;
  
  tmp = clip_ana_low16(adc_value16[EFFECT1]);
  tmp <<= 15; // 31 bits (will be used as signed : must be positive)
  
  tmp2 = adc_value16[EFFECT2] * modulation_data[modulation_index[index_EFFECT_MOD]];
  tmp2 >>= 1;
  tmp2 += 0x3FFFFFFF;
  tmp += tmp2;
  tmp = tmp < 0x40000000? 0: tmp-0x40000000;
  tmp = tmp > 0x7FFFFFFF? 0x7FFFFFFF: tmp;
  effect =  tmp>0x07FFFFFF? 0x07FFFFFF: tmp;     // mix amplitude
  waveshape = tmp>0x10000000? tmp: 0x10000000;   // forme du waveshaper
}

inline int32_t WAVESHAPE(int32_t VCF_out)  {
  int32_t tmp1, VCF_local;
  uint32_t tmp2, tmp3;
  uint32_t tmp, phaseH, phaseL, a_effect;

  VCF_local = VCF_out;
  // old_mix_out += (VCF_local >> 4) - (old_mix_out >> 4); // filter the audio in
  old_mix_out = VCF_local;

  waveshape_filter += (int32_t)(waveshape - waveshape_filter) >> 11; // filter on waveshape size fader
  effect_filter += (int32_t)(effect - effect_filter) >> 11; // filter on waveshape wet 

  tmp1 = m_s32xs32_s32H(old_mix_out, waveshape_filter);
  tmp1 <<= 4;
  tmp1 += 0X10000000; // disymetrie
  
  tmp = tmp1^0x80000000;
  tmp += 1 << 30; // symetrie 
  phaseH = tmp>>19; // 13 bits
  phaseL = tmp & 0x7FFFF; // 19 bits
  tmp = table_cos[phaseH];
  tmp2 = table_cos[(phaseH + 1)];
  tmp3 = m_s32xs32_s32H(tmp2-tmp, phaseL<<12); 
  tmp3 <<= 1;
  tmp3 += tmp;
  tmp3 >>= 1;
  tmp3 += 0x3FFFFFFF;

  a_effect = effect_filter;
  tmp1 = (m_s32xs32_s32H(tmp3^0x80000000,a_effect) + m_s32xs32_s32H(VCF_local,0x07FFFFFF-a_effect))<<5;
  
  return(tmp1);
}

inline void DELAY_update() { 
  uint32_t tmp;
  tmp = adc_value16[EFFECT2];
  tmp *= tmp;
  tmp = (tmp>>2) + (tmp>>3);
  tmp += (1000<<14);
  tmp = (tmp > (43000<<15))? (43000<<15) : tmp; 
  
  delay_time = hysteresis32(tmp, delay_time); 
  delay_feedback_old = hysteresis16(adc_value16[EFFECT1], delay_feedback_old);
  delay_feedback = clip_anaLH16(delay_feedback_old)<<15; 
}

inline int32_t DELAY(int32_t VCA_out)  { 
  uint32_t delay_read, tmp;
  int32_t delay_value, read_pos, sound, sound2;
  int32_t out;

  delay_time_filter += (int32_t)(delay_time - delay_time_filter) >> 12;
  delay_feedback_filter += (int32_t)(delay_feedback - delay_feedback_filter) >> 12;
  delay_write++;
  delay_write = (delay_write >= 43000)? 0: delay_write;

  tmp = (delay_time_filter>>15);
  read_pos = delay_write - tmp;
  delay_read = (read_pos < 0)? read_pos+43000: read_pos; 

  // delay line interpolation
  sound = delay_line[delay_read];
  sound2 = delay_line[(delay_read == 0)? 43000-1:delay_read-1]-sound;
  sound <<= 16;
  sound += (sound2 * (delay_time_filter & 0x00007FFF))<<1;

  out = m_s32xs32_s32H(sound, delay_feedback_filter);  // todo : une seule  multiplication
  out += m_s32xs32_s32H(VCA_out, 0x7FFFFFFF - delay_feedback_filter);
  
  out = gain2_cliped_S32(out);
  
  //VCA_out = out;
  delay_line[delay_write] = out>>16;
  return(out);
  //analog_out_2((delay_line[delay_read]<<16)^0x80000000);
}


