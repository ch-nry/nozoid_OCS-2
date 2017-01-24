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

// RX MIDI
// PD5 : RXD3, peripheric B, USART3
// Midi is 31.25 Kb/s
// chip is at 84 000 KHz
// CD = 168

uint32_t MIDI_status, MIDI_data1, MIDI_data2, MIDI_state;
uint32_t MIDI_PITCHWHEEL, MIDI_MODWHEEL, MIDI_MODWHEEL_LSB, MIDI_CC2, MIDI_CC2_LSB;
uint32_t use_midi_pitchwheel, use_midi_mod_wheel, use_midi_cc2;
uint32_t MIDI_fader_LSB[34];
uint32_t MIDI_LFO1_speed, MIDI_LFO1_speed_LSB, MIDI_LFO2_speed, MIDI_LFO2_speed_LSB, MIDI_LFO3_speed, MIDI_LFO3_speed_LSB;
// MIDI_state :
//    0 : no status received
//    1 : 1 status received, waiting for data1
//    2 : data 1 received, waiting for data2

inline void init_midi() {
  // RX3D : PD5, peripherical B  
  REG_PMC_PCER0 = 1 << 20; //switch on uart3 clock
  USART3->US_MR = 0x000008C0; // misc configuration
  USART3->US_BRGR = 168; // baude rate
  PIOD->PIO_ABSR |= 1<<5; // peripherical B for midi RX pin 
  USART3->US_CR = 0x00000010; // enable receiver
  MIDI_state = 0;
  use_midi_pitchwheel = 0;
  use_midi_mod_wheel = 0;
  use_midi_cc2 = 0;
}

inline void MIDI_in() { // the main loop is faster than midi message, no need to get midi data in interuption
  uint32_t midi_data;
  uint32_t tmp;
  
  if (USART3->US_CSR  & (1<<0)) { // RXRDY
    midi_data = (USART3->US_RHR) & 0xFF;
    led1_time = 150; // small blink
    
    if (midi_data & 0x80) { // test for status
      if ((midi_data & 0xF0) != 0xF0) { // remove real time messages
        MIDI_status = midi_data;
        MIDI_state = 1;
      }
    }
    else { // data
      switch (MIDI_state) {
      case 0 :
        // data without any status : discard
        break;
      case 1:
        MIDI_data1 = midi_data;
        MIDI_state = 2;
        
        if (MIDI_status == 0b11000000) { // progam change
          if (MIDI_data1 < 10) load_conf(1+MIDI_data1); // load a memory
          }
        break;
      case 2:
        MIDI_data2 = midi_data;
        MIDI_state = 1; // go back waiting for data 1 (running status)

        // we have a full midi message
        if((MIDI_status == 0x90) && (MIDI_data2 != 0) ) { // note on
          MIDI_gate = 1;
          MIDI_pitch = MIDI_data1;
          
          KEY_LOCAL_goal = (16+MIDI_pitch) << (2+18);
          modulation_data[mod_VEL] = (MIDI_data2<<9) - (1<<15); 
        }
        else if(((MIDI_status == 0x90) && (MIDI_data2 == 0)) || (MIDI_status == 0x80)) { // note off
          if (MIDI_pitch == MIDI_data1){
            MIDI_gate = 0;
            //test2_off();
          }
        }
        #ifndef EXT_ANA
        //else if ((MIDI_status & 0xF0) == 0b11100000) 
        else if ((MIDI_status) == 0b11100000) // cannal 0
        { // Pitch bend
          MIDI_PITCHWHEEL = (MIDI_data1 << 2) + (MIDI_data2 << 9); // 16 bits
          use_midi_pitchwheel = 1;
        }
        //else if ((MIDI_status & 0xF0) == 0b10110000) 
        else if ((MIDI_status) == 0b10110000) // cannal 0
        { // CC
          if (MIDI_data1 == 01) { // mod wheel
            MIDI_MODWHEEL = (MIDI_data2 << 9) + MIDI_MODWHEEL_LSB;
            use_midi_mod_wheel = 1;
          }
          else if (MIDI_data1 == 02){ //breath controler
            MIDI_CC2 = (MIDI_data2 << 9) + MIDI_CC2_LSB;
            use_midi_cc2 = 1;
          }       
          else if (MIDI_data1 == 33) { // mod wheel LSB
            MIDI_MODWHEEL_LSB = MIDI_data2 << 2;
          }
          else if (MIDI_data1 == 34) { // CC2 LSB
            MIDI_CC2_LSB = MIDI_data2 << 2;
          }
          else if (MIDI_data1 == 40) { // LFO1 MSB
            tmp = ((MIDI_data2 << 7) + MIDI_LFO1_speed_LSB);
            MIDI_LFO1_speed = (tmp == 0)? 0 : 89478485 / tmp;
          }
          else if (MIDI_data1 == 41) { // LFO1 LSB
            MIDI_LFO1_speed_LSB = MIDI_data2;
          }
          else if (MIDI_data1 == 42) { // LFO1 MSB
            tmp = ((MIDI_data2 << 7) + MIDI_LFO2_speed_LSB);
            MIDI_LFO2_speed = (tmp == 0)? 0 : 89478485 / tmp;
          }
          else if (MIDI_data1 == 43) { // LFO1 LSB
            MIDI_LFO2_speed_LSB = MIDI_data2;
          }
          else if (MIDI_data1 == 44) { // LFO1 MSB
            tmp = ((MIDI_data2 << 7) + MIDI_LFO3_speed_LSB);
            MIDI_LFO3_speed = (tmp == 0)? 0 : 89478485 / tmp;
          }
          else if (MIDI_data1 == 45) { // LFO1 LSB
            MIDI_LFO3_speed_LSB = MIDI_data2;
          }
        }
        #endif
        else if ((MIDI_status) == 0b10110001) { // CC cannal 1 : control every fader
          if (MIDI_data1 < 32) 
            MIDI_fader[midi_pos[MIDI_data1]] = (MIDI_data2<<9) + MIDI_fader_LSB[MIDI_data1] ;
          else {
            MIDI_data1 -= 32;
            if (MIDI_data1 <= 32)
              MIDI_fader_LSB[MIDI_data1] = MIDI_data2 << 2;
          }
        }
        break;
      }
    }
  }
}
