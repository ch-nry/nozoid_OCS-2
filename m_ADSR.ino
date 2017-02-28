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

uint32_t ADSR_status;
uint32_t ADSR_goal;
uint32_t ADSR_filter;
bool GATE;

// status : 0 = attack, 1 = decay, 2 = release

inline void init_ADSR() {
  ADSR_status = 2;
  ADSR_goal = 0;
}

inline void ADSR_update() {
  uint32_t tmp;
  bool GATE_local;
  uint32_t ADSR_status_local, ADSR_goal_local;
  
  GATE_local = (NOTE_ON || MIDI_gate || GATE_EXT());
  GATE = GATE_local;
  switch (GATE_mode) { // for the envelope
    case 0:
//      GATE_local = GATE;
    break;
    case 1:
      GATE_local = (modulation_data[mod_LFO3] > 0);
    break;
    case 2:
      GATE_local = modulation_data[mod_ENV] > (1<<10); 
    break;
    case 3:
      GATE_local = (NOTE_ON || MIDI_gate || GATE_EXT()); // in this mode, the envelope is trigged like in mode 0, but VCA is always on
    break;
  }

  #ifdef gate_out
    if (GATE_local)
      pin_gate_on();
    else
      pin_gate_off();
  #endif
  
  ADSR_status_local = ADSR_status;
  
  // did we change state?
  if (!GATE_local) { // no gate -> release
    ADSR_status_local = 2;  // release status
    ADSR_goal_local = 0;
  }
  else {
    if ((ADSR_status_local == 0) & (ADSR_out >= 0x3FFFFFFF)) { // attack and signal is high, so we start a decay
      ADSR_status_local = 1; // decay status
    }
    else if (ADSR_status_local == 2) { // we got a gate, but use to be in release mode, so we start an attack
      ADSR_status_local=0;
      ADSR_goal_local = ADSR_Goal_value;
      if (sync_LFO1) LFO1_phase = 0; // reset LFO phase if needed
    }
  }
  
  switch (ADSR_status_local) {
  case 0 :
    tmp = adc_value16[ADSR_A];
    break;
  case 1 :
    ADSR_goal_local = adc_value16[ADSR_S] << 14; // 0x3FFFFFFF max
    tmp = adc_value16[ADSR_D];
    break;
  case 2 :
    tmp = adc_value16[ADSR_R];
    break;
  }
  tmp = (table_CV2freq[0x2B0-(tmp>>7)]);
  
  
  
  noInterrupts();
  ADSR_status = ADSR_status_local;
  ADSR_goal = ADSR_goal_local;
  ADSR_filter = tmp;
  interrupts();
}

inline void ADSR() { 
  uint32_t ADSR_out_tmp;
  
  ADSR_out_tmp = ADSR_out;
  ADSR_out_tmp += m_s32xs32_s32H(((int32_t)ADSR_goal - (int32_t)(ADSR_out_tmp)), ADSR_filter); 
  
  ADSR_out_tmp = (ADSR_out_tmp > 0x3FFFFFFF)? 0x3FFFFFFF: ADSR_out_tmp;
  ADSR_out = ADSR_out_tmp;
  modulation_data[mod_ADSR] = ((ADSR_out_tmp >> 14) - (1<<15));
  
  //analog_out_1(ADSR_out);
}
