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

inline void init_save() {
  
  DueFlashStorage();

  flash_lock_bit = 0;
  
  if (!(PORTKEY25->PIO_PDSR & (1<<KEY25)) || (read(0x100)==0xFF)) { // errase assignations and conf
    load_default_config();
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
    
    flash_lock_bit = 1; // on sauve la conf
  }
  else {
    #ifdef nosave
      load_default_config();
    #endif
    #ifndef nosave
      load_conf0(0);
    #endif
  }
}

inline void load_default_config() {
  // default config
  LFO3_WF     = 0;
  GATE_mode   = 0;
  VCO_link    = 0;
  RINGMOD     = 0;
  VCF_pitch   = 1; 
  sync_LFO1   = 0;
  MIX_type    = 0;
  cvg_type    = 1;
  filter_type = 0;
  set_LP24();
  EFFECT_type = 0;

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


inline void save_conf(uint32_t nb) { 
  //uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
  uint8_t all_data[10];

  #ifdef nosave
  if (nb != 0)
  #endif
  #ifndef nosave
  if (true)
  #endif
  {
    all_data[0] =  modulation_index[index_VCO1_MOD1] << 4;
    all_data[0] += modulation_index[index_VCO1_MOD2];
    all_data[1] =  modulation_index[index_VCO2_MOD1] << 4;
    all_data[1] += modulation_index[index_VCO2_MOD2];
    all_data[2] =  modulation_index[index_VCF_MOD1]  << 4;
    all_data[2] += modulation_index[index_VCF_MOD2];
    all_data[3] =  modulation_index[index_LFO3_MOD]  << 4;
    all_data[3] += modulation_index[index_VCA_MOD];
    all_data[4] = modulation_index[index_EFFECT_MOD];
    all_data[4] +=  LFO3_WF << 4;  

    all_data[5] =  GATE_mode   << 6;
    all_data[5] += EFFECT_type << 4;
    all_data[5] += cvg_type    << 2;
    all_data[5] += filter_type;
    
    all_data[6] = VCO_link   << 4;
    all_data[6] += RINGMOD   << 3;
    all_data[6] += VCF_pitch << 2;
    all_data[6] += sync_LFO1 << 1;
    all_data[6] += MIX_type;

   write( 256*(nb+20), all_data, 7);
  }
}

inline void load_conf(uint32_t nb) {
  uint8_t byte1, byte2, byte3, byte4, byte5, byte6, byte7;
  uint32_t adresse;
  adresse = 256*(nb+20);
  
  byte1  = read( adresse   );
  byte2  = read( adresse+1 );
  byte3  = read( adresse+2 );
  byte4  = read( adresse+3 );
  byte5  = read( adresse+4 );
  byte6  = read( adresse+5 );
  byte7  = read( adresse+6 );
  
  modulation_index[index_VCO1_MOD1] = (byte1 >> 4) & 0x0F;
  modulation_index[index_VCO1_MOD2] =  byte1       & 0x0F;
  modulation_index[index_VCO2_MOD1] = (byte2 >> 4) & 0x0F;
  modulation_index[index_VCO2_MOD2] =  byte2       & 0x0F;
  modulation_index[index_VCF_MOD1]  = (byte3 >> 4) & 0x0F;
  modulation_index[index_VCF_MOD2]  =  byte3       & 0x0F;
  modulation_index[index_LFO3_MOD]  = (byte4 >> 4) & 0x0F;
  modulation_index[index_VCA_MOD]   =  byte4       & 0x0F;
  modulation_index[index_EFFECT_MOD]=  byte5       & 0x0F;
  LFO3_WF     =  (byte5 >> 4) & 0x0F;
  
  GATE_mode   = (byte6 >> 6) & 0x03;
  EFFECT_type = (byte6 >> 4) & 0x03;
  cvg_type    = (byte6 >> 2) & 0x03;
  filter_type = (byte6 >> 0) & 0x03;
  
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
}
 
inline void save_conf0() {
  uint32_t data;
  
  switch (flash_lock_bit) {
    case 1: // il faut sauver qqq chose
      data = modulation_index[index_VCO1_MOD1];
      if ((uint8_t)data != read( 0x100))
        write_noblocking(0x100,(uint8_t)data);
      flash_lock_bit = 2;
    break;
    case 2:
      write_end(0x100);
      data = modulation_index[index_VCO1_MOD2];
      if ((uint8_t)data != read( 0x200 ))
        write_noblocking(0x200,(uint8_t)data);
      flash_lock_bit = 3;
    break;
    case 3:
      write_end(0x200);
      data = modulation_index[index_VCO2_MOD1];
      if ((uint8_t)data != read( 0x300 ))
        write_noblocking(0x300,(uint8_t)data);
      flash_lock_bit = 4;
    break;
    case 4:
      write_end(0x300);
      data = modulation_index[index_VCO2_MOD2];
      if ((uint8_t)data != read( 0x400 ))
        write_noblocking(0x400,(uint8_t)data);
      flash_lock_bit = 5;
    break;
    case 5:
      write_end(0x400);
      data = modulation_index[index_VCF_MOD1];
      if ((uint8_t)data != read( 0x500 ))
        write_noblocking(0x500,(uint8_t)data);
      flash_lock_bit = 6;
    break;
    case 6:
      write_end(0x500);
      data = modulation_index[index_VCF_MOD2];
      if ((uint8_t)data != read( 0x600 ))
        write_noblocking(0x600,(uint8_t)data);
      flash_lock_bit = 7;
    break;
    case 7:
      write_end(0x600);
      data = modulation_index[index_LFO3_MOD];
      if ((uint8_t)data != read( 0x700 ))
        write_noblocking(0x700,(uint8_t)data);
      flash_lock_bit = 8;
    break;
    case 8:
      write_end(0x700);
      data = modulation_index[index_VCA_MOD];
      if ((uint8_t)data != read( 0x800 ))
        write_noblocking(0x800,(uint8_t)data);
      flash_lock_bit = 9;
    break;
    case 9:
      write_end(0x800);
      data = modulation_index[index_EFFECT_MOD];
      if ((uint8_t)data != read( 0x900 ))
        write_noblocking(0x900,(uint8_t)data);
      flash_lock_bit = 10;
    break;
    case 10:
      write_end(0x900);
      data = LFO3_WF;
      if ((uint8_t)data != read( 0xA00 ))
        write_noblocking(0xA00,(uint8_t)data);
      flash_lock_bit = 11;
    break;
    case 11:
      write_end(0xA00);
      data = GATE_mode;
      if ((uint8_t)data != read( 0xB00 ))
        write_noblocking(0xB00,(uint8_t)data);
      flash_lock_bit = 12;
    break;
    case 12:
      write_end(0xB00);
      data = EFFECT_type;
      if ((uint8_t)data != read( 0xC00 ))
        write_noblocking(0xC00,(uint8_t)data);
      flash_lock_bit = 13;
    break;
    case 13:
      write_end(0xC00);
      data = cvg_type;
      if ((uint8_t)data != read( 0xD00 ))
        write_noblocking(0xD00,(uint8_t)data);
      flash_lock_bit = 14;
    break;
    case 14:
      write_end(0xD00);
      data = filter_type;
      if ((uint8_t)data != read( 0xE00 ))
        write_noblocking(0xE00,(uint8_t)data);
      flash_lock_bit = 15;
    break;
    case 15:
      write_end(0xE00);
      data = VCO_link;
      if ((uint8_t)data != read( 0xF00 ))
        write_noblocking(0xF00,(uint8_t)data);
      flash_lock_bit = 16;
    break;
    case 16:
      write_end(0xF00);
      data = RINGMOD;
      if ((uint8_t)data != read( 0x1000 ))
        write_noblocking(0x1000,(uint8_t)data);
      flash_lock_bit = 17;
    break;
    case 17:
      write_end(0x1000);
      data = VCF_pitch;
      if ((uint8_t)data != read( 0x1100 ))
        write_noblocking(0x1100,(uint8_t)data);
      flash_lock_bit = 18;
    break;
    case 18:
      write_end(0x1100);
      data = sync_LFO1;
      if ((uint8_t)data != read( 0x1200 ))
        write_noblocking(0x1200,(uint8_t)data);
      flash_lock_bit = 19;
    break;
    case 19:
      write_end(0x1200);
      data = MIX_type;
      if ((uint8_t)data != read( 0x1300 ))
        write_noblocking(0x1300,(uint8_t)data);
      flash_lock_bit = 20;
    break;
    case 20:
      write_end(0x1300);
      flash_lock_bit = 0; // on a fini de sauver
    break;
  }
}

inline void load_conf0(uint32_t nb) { // mandatory to keep the arg, otherwise optimisation occure and flash is not read (since the compiler consider it as a constant)
  uint32_t adresse;
  adresse = 256*(nb) + 0x100;
  
  modulation_index[index_VCO1_MOD1] = read( adresse         );
  modulation_index[index_VCO1_MOD2] = read( adresse + 0x100 );
  modulation_index[index_VCO2_MOD1] = read( adresse + 0x200 );
  modulation_index[index_VCO2_MOD2] = read( adresse + 0x300 );
  modulation_index[index_VCF_MOD1]  = read( adresse + 0x400 );
  modulation_index[index_VCF_MOD2]  = read( adresse + 0x500 );
  modulation_index[index_LFO3_MOD]  = read( adresse + 0x600 );
  modulation_index[index_VCA_MOD]   = read( adresse + 0x700 );
  modulation_index[index_EFFECT_MOD]= read( adresse + 0x800 );
  LFO3_WF     = read( adresse + 0x900 );
  GATE_mode   = read( adresse + 0xA00 );
  EFFECT_type = read( adresse + 0xB00 );
  cvg_type    = read( adresse + 0xC00 );
  filter_type = read( adresse + 0xD00 );
  VCO_link    = read( adresse + 0xE00 );
  RINGMOD     = read( adresse + 0xF00 );
  VCF_pitch   = read( adresse + 0x1000);
  sync_LFO1   = read( adresse + 0x1100);
  MIX_type    = read( adresse + 0x1200);

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
}
