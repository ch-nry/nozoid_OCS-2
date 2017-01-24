// uint32_t envelope_filter;

// external CV
inline void update_ext() {
uint32_t tmp;

  #ifdef EXT_ANA
    modulation_data[mod_EXT1] = adc_value16[EXT_1]-(1<<15);
    modulation_data[mod_EXT2] = adc_value16[EXT_2]-(1<<15);
    modulation_data[mod_EXT3] = adc_value16[EXT_3]-(1<<15);
  #endif
  #ifdef EXT_MIDI
    modulation_data[mod_EXT1] = MIDI_PITCHWHEEL-(1<<15);
    modulation_data[mod_EXT2] = MIDI_MODWHEEL-(1<<15);
    modulation_data[mod_EXT3] = MIDI_CC2-(1<<15);
  #endif
  #ifdef EXT_AUTO
    modulation_data[mod_EXT1] = (use_midi_pitchwheel == 0)? adc_value16[EXT_1]-(1<<15):MIDI_PITCHWHEEL-(1<<15);
    modulation_data[mod_EXT2] = (use_midi_mod_wheel == 0)?  adc_value16[EXT_2]-(1<<15):MIDI_MODWHEEL-(1<<15);
    modulation_data[mod_EXT3] = (use_midi_cc2 == 0)?        adc_value16[EXT_3]-(1<<15):MIDI_CC2-(1<<15);
  #endif
  
  tmp = 0xFFFF-adc_value16[LDR];
  modulation_data[mod_LDR] = tmp - (1<<15);  
  //analog_out_1(tmp<<16);
}

// Audio In Envelope
inline void init_ENV() {
 envelope = 0; 
}

inline void ENVELOPE() {
}

inline void ENVELOPE_modulation() {
  uint32_t env_local;
  uint32_t tmp = abs(audio_inR);
  
  env_local = envelope;
  env_local = (env_local > tmp)? env_local : tmp; // audio in > curent env
  env_local = env_local>0x30000?env_local-0x30000:0; // decease env
  envelope = env_local;
  
  tmp =  (env_local>0x3FFFFFFF)? 0xFFFF: env_local>>14;  
  modulation_data[mod_ENV] = tmp-(1<<15);
  //analog_out_2(tmp<<16);
}


