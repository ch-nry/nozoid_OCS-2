uint8_t current_byte1, current_byte2, current_byte3, current_byte4, current_byte5, current_byte6, current_byte7, current_byte8;

inline void load_default_config() {
  // default config
  LFO3_WF     = 0;
  GATE_mode   = 0;
  VCO_link    = 0;
  RINGMOD     = 0;
  VCF_pitch   = 1; 
  sync_LFO1   = 0;
  MIX_type    = 0;
  cvg_type    = 0;
  filter_type = 0;
  set_LP24();
  EFFECT_type = 0;
  current_byte1 = 0xFF;
  current_byte2 = 0xFF;
  current_byte3 = 0xFF;
  current_byte4 = 0xFF;
  current_byte5 = 0xFF;
  current_byte6 = 0xFF;
  current_byte7 = 0xFF;
  current_byte8 = 0xFF;

  modulation_index[index_VCO1_MOD1] = mod_LFO1;
  modulation_index[index_VCO1_MOD2] = mod_VCO2;
  modulation_index[index_VCO2_MOD1] = mod_LFO2;
  modulation_index[index_VCO2_MOD2] = mod_VCO1;
  modulation_index[index_VCF_MOD1]  = mod_LFO1; 
  modulation_index[index_VCF_MOD2]  = mod_LFO3;
  modulation_index[index_LFO3_MOD]  = mod_LFO1;
  modulation_index[index_VCA_MOD]   = mod_LFO2;
  modulation_index[index_EFFECT_MOD]= mod_LFO3;
}

inline void init_save() {
  #ifdef nosave
    load_default_config();
  #endif
  
  if (!(PORTKEY25->PIO_PDSR & (1<<KEY25)) || dueFlashStorage.read(0)) { // errase assignations and conf
    dueFlashStorage.write(0, 0); // to remember that configuration is already done
    load_default_config();
    save_conf(0);
    save_conf(1);
    save_conf(2);
    save_conf(3);
    save_conf(4);
    save_conf(5);
    save_conf(6);
    save_conf(7);
    save_conf(8);
    save_conf(9);
    save_conf(10);
  }
  load_conf(0);
}
  
inline void save_conf(uint32_t nb) { 
  uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;

  #ifdef nosave
  if (nb != 0)
  #endif
  #ifndef nosave
  if (true)
  #endif
  {
    byte1 =  modulation_index[index_VCO1_MOD1] << 4;
    byte1 += modulation_index[index_VCO1_MOD2];
    byte2 =  modulation_index[index_VCO2_MOD1] << 4;
    byte2 += modulation_index[index_VCO2_MOD2];
    byte3 =  modulation_index[index_VCF_MOD1]  << 4;
    byte3 += modulation_index[index_VCF_MOD2];
    byte4 =  modulation_index[index_LFO3_MOD]  << 4;
    byte4 += modulation_index[index_VCA_MOD];
    byte8 = modulation_index[index_EFFECT_MOD];
    byte5 =  GATE_mode   << 6;
    byte5 += EFFECT_type << 4;
    byte5 += cvg_type    << 2;
    byte5 += filter_type;
    byte6 =  LFO3_WF;  
    byte7 = VCO_link   << 4;
    byte7 += RINGMOD   << 3;
    byte7 += VCF_pitch << 2;
    byte7 += sync_LFO1 << 1;
    byte7 += MIX_type;
    
    if ( (current_byte1 != byte1) || (nb != 0) ) { dueFlashStorage.write(1+8*nb, byte1); current_byte1 = byte1; }
    if ( (current_byte2 != byte2) || (nb != 0) ) { dueFlashStorage.write(2+8*nb, byte2); current_byte2 = byte2; }
    if ( (current_byte3 != byte3) || (nb != 0) ) { dueFlashStorage.write(3+8*nb, byte3); current_byte3 = byte3; }
    if ( (current_byte4 != byte4) || (nb != 0) ) { dueFlashStorage.write(4+8*nb, byte4); current_byte4 = byte4; }
    if ( (current_byte5 != byte5) || (nb != 0) ) { dueFlashStorage.write(5+8*nb, byte5); current_byte5 = byte5; }
    if ( (current_byte6 != byte6) || (nb != 0) ) { dueFlashStorage.write(6+8*nb, byte6); current_byte6 = byte6; }
    if ( (current_byte7 != byte7) || (nb != 0) ) { dueFlashStorage.write(7+8*nb, byte7); current_byte7 = byte7; }
    if ( (current_byte8 != byte8) || (nb != 0) ) { dueFlashStorage.write(8+8*nb, byte8); current_byte8 = byte8; }
  }
}

inline void load_conf(uint32_t nb) {
  uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
  
  #ifdef nosave
  if (nb != 0)
  #endif
  #ifndef nosave
  if (true)
  #endif
  {
    byte1 = dueFlashStorage.read(1+8*nb);
    byte2 = dueFlashStorage.read(2+8*nb);
    byte3 = dueFlashStorage.read(3+8*nb);
    byte4 = dueFlashStorage.read(4+8*nb);
    byte5 = dueFlashStorage.read(5+8*nb);
    byte6 = dueFlashStorage.read(6+8*nb);
    byte7 = dueFlashStorage.read(7+8*nb);
    byte8 = dueFlashStorage.read(8+8*nb);
    modulation_index[index_VCA_MOD]   =  byte4       & 0x0F;
    modulation_index[index_LFO3_MOD]  = (byte4 >> 4) & 0x0F;
    modulation_index[index_VCF_MOD2]  =  byte3       & 0x0F;
    modulation_index[index_VCF_MOD1]  = (byte3 >> 4) & 0x0F;
    modulation_index[index_VCO2_MOD2] =  byte2       & 0x0F;
    modulation_index[index_VCO2_MOD1] = (byte2 >> 4) & 0x0F;
    modulation_index[index_VCO1_MOD2] =  byte1       & 0x0F;
    modulation_index[index_VCO1_MOD1] = (byte1 >> 4) & 0x0F;
    modulation_index[index_EFFECT_MOD]=  byte8       & 0x0F;
    GATE_mode   = (byte5 >> 6) & 0x03;
    EFFECT_type = (byte5 >> 4) & 0x03;
    cvg_type    = (byte5 >> 2) & 0x03;
    filter_type = (byte5 >> 0) & 0x03;
    LFO3_WF     =  byte6;
    VCO_link    = (byte7 >> 4) & 0x01;
    RINGMOD     = (byte7 >> 3) & 0x01;
    VCF_pitch   = (byte7 >> 2) & 0x01;
    sync_LFO1   = (byte7 >> 1) & 0x01;
    MIX_type    = (byte7 >> 0) & 0x01;  
  
    switch (filter_type){
    case 0:
      set_LP24();
    break;
    case 1:
      set_LP12();
    break;
    case 2:
      set_BP12();
    break;
    case 3:
      set_HP12();
    break;
    }
    current_byte1 = byte1;
    current_byte2 = byte2;
    current_byte3 = byte3;
    current_byte4 = byte4;
    current_byte5 = byte5;
    current_byte6 = byte6;
    current_byte7 = byte7;
    current_byte8 = byte8;
  }
}
 

