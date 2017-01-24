inline void init_CVG() {
  LFO4_phase = 0x80000000;
  LFO5_phase = 0x80000000;
  LFO6_phase = 0x80000000;
  LFO7_phase = 0x80000000;
  AR_value = 0;
}

inline void CVG_mod() {
  switch(cvg_type) {
  case 0:
    AR_mod();
    break;
  case 1:
    LFO4_mod();
    break;
  case 2:
    RANDOM_mod();
    break;
  case 3:
    NL_mod();
    break;
  }
}

inline void RANDOM_mod() {
  uint32_t freq;
  uint32_t tmp, filter;
  uint64_t tmp64;

  freq = adc_value16[CVG1];
  freq *= freq;
  freq >>= 8;
  freq += 6000;
  LFO4_phase += freq;
    
  if (LFO4_phase > 0x80000000) {
    LFO4_phase -= 0x80000000;  
    random_goal = random32()>>1;
  }

  filter = (table_CV2freq[0x140 + (adc_value[CVG2]>>3)])>>14; 
  filter += filter>>1;
 
  random_filter1 -= (((int32_t)random_filter1-(int32_t)random_goal)>>16)    * filter;
  random_filter2 -= (((int32_t)random_filter2-(int32_t)random_filter1)>>16) * filter;
  
  tmp = random_filter2 >> 15 ; // keep only 16bit
  tmp -= (1<<15); // To make it symetrical
  modulation_data[mod_CVG] = tmp;
}

inline void AR_mod() {
  uint32_t AR_goal, AR_filter;
  uint32_t tmp;
  
  if (GATE) {
    tmp = adc_value16[CVG1];
    AR_goal = 0x7FFFFFFF;
  }
  else {
    tmp = adc_value16[CVG2];
    AR_goal = 0x00000FFF;
  }
  AR_filter = (table_CV2freq[0x300-(tmp>>7)])<<3;
  AR_value += ((int32_t)(AR_goal - AR_value)>>16) * (int32_t)(AR_filter>>16); 
  tmp = AR_value>>15;
  tmp -= (1<<15);
  modulation_data[mod_CVG] = tmp;
}

inline void LFO4_mod() {
  uint32_t tmp1, freq ;

  freq = adc_value16[CVG1];
  freq *= freq;
  freq >>= 8;
  freq += 6000;
  LFO4_phase += freq;

  freq = adc_value16[CVG2];
  freq *= freq;
  freq >>= 8;
  freq += 6000;
  LFO5_phase += freq;
  
  tmp1 = table_cos[LFO4_phase>>19] >> 17;
  tmp1 += table_cos[LFO5_phase>>19] >> 17;
  tmp1 -= (1<<15); // To make it symetrical
  modulation_data[mod_CVG] = tmp1;
}

inline void NL_mod() {
  uint32_t tmp1, tmp2, freq ;

  freq = adc_value16[CVG1];
  freq *= freq;
  freq >>= 8;
  freq += 15000;
  
  LFO6_phase += freq;
  LFO7_phase += (LFO6_phase < freq) ? (adc_value16[CVG2]<<15) : 0;
  
  tmp1 = abs((int32_t)LFO7_phase) >> 15;
  tmp1 -= (1<<15); // To make it symetrical
  modulation_data[mod_CVG] = tmp1;
}

