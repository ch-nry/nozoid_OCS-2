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

// uint32_t envelope_filter;

int32_t ext1_filter, ext2_filter, ext3_filter;

// external CV
inline void update_ext() {
uint32_t tmp;

  #ifdef EXT_ANA
    tmp = hysteresis16(adc_value16[EXT_1], ext1_filter);
    ext1_filter = tmp;
    modulation_data[mod_EXT1] = 3*(tmp-(1<<15));
    
    tmp = hysteresis16(adc_value16[EXT_2], ext2_filter);
    ext2_filter = tmp;    
    modulation_data[mod_EXT2] = 3*(tmp-(1<<15));
    
    tmp = hysteresis16(adc_value16[EXT_3], ext3_filter);
    ext3_filter = tmp;
    modulation_data[mod_EXT3] = 3*(tmp-(1<<15));
  #endif
  #ifdef EXT_MIDI
    modulation_data[mod_EXT1] = MIDI_PITCHWHEEL-(1<<15);
    modulation_data[mod_EXT2] = MIDI_MODWHEEL-(1<<15);
    modulation_data[mod_EXT3] = MIDI_CC2-(1<<15);
  #endif
  #ifdef EXT_AUTO
    tmp = hysteresis16(adc_value16[EXT_1], ext1_filter);
    ext1_filter = tmp;
    modulation_data[mod_EXT1] = (use_midi_pitchwheel == 0)? 3*(tmp-(1<<15)):MIDI_PITCHWHEEL-(1<<15); // attention, les modulation peuvent etre superieur a 16 bit
    
    tmp = hysteresis16(adc_value16[EXT_2], ext2_filter);
    ext2_filter = tmp;    
    modulation_data[mod_EXT2] = (use_midi_mod_wheel == 0)?  3*(tmp-(1<<15)):MIDI_MODWHEEL-(1<<15);
    
    tmp = hysteresis16(adc_value16[EXT_3], ext3_filter);
    ext3_filter = tmp;
    modulation_data[mod_EXT3] = (use_midi_cc2 == 0)?        3*(tmp-(1<<15)):MIDI_CC2-(1<<15);
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


