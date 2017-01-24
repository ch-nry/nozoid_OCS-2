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

#define PORTKEYOUT1 PIOD
#define KEYOUT1 7
#define PORTKEYOUT2 PIOC
#define KEYOUT2 29
#define PORTKEYOUT3 PIOC
#define KEYOUT3 25
#define PORTKEYIN1 PIOA
#define KEYIN1 17
#define PORTKEYIN2 PIOC
#define KEYIN2 3
#define PORTKEYIN3 PIOB
#define KEYIN3 26
#define PORTKEY25 PIOB
#define KEY25 27
#define PORTKEYMENU PIOD
#define KEYMENU 8
#define PORTGATEEXT PIOA
#define GATEEXT 18

uint32_t keyboard_state;
uint32_t KEY_old, KEY;
bool MODE_MENU; 
uint32_t masque_led, and_led1, and_led2, and_led3, and_led4;

inline void init_keyboard() {
  PORTKEYOUT1->PIO_PER  = 1 << KEYOUT1; // out to control demux bus
  PORTKEYOUT1->PIO_OER  = 1 << KEYOUT1; 
  PORTKEYOUT2->PIO_PER  = 1 << KEYOUT2;
  PORTKEYOUT2->PIO_OER  = 1 << KEYOUT2; 
  PORTKEYOUT3->PIO_PER  = 1 << KEYOUT3; // out to control demux bus
  PORTKEYOUT3->PIO_OER  = 1 << KEYOUT3; 
  
  PORTKEYIN1->PIO_PER   = 1 << KEYIN1; // input with pull up to get switch value
  PORTKEYIN1->PIO_PUER  = 1 << KEYIN1;
  PORTKEYIN1->PIO_ODR   = 1 << KEYIN1;
  PORTKEYIN1->PIO_IDR   = 1 << KEYIN1;
  PORTKEYIN1->PIO_ABSR  = 1 << KEYIN1;
  PORTKEYIN1->PIO_AIMDR  = 1 << KEYIN1;
  
  PORTKEYIN2->PIO_PER   = 1 << KEYIN2;
  PORTKEYIN2->PIO_PUER  = 1 << KEYIN2;
  PORTKEYIN3->PIO_PER   = 1 << KEYIN3; 
  PORTKEYIN3->PIO_PUER  = 1 << KEYIN3;
  PORTKEY25->PIO_PER    = 1 << KEY25;
  PORTKEY25->PIO_PUER   = 1 << KEY25;
  PORTKEYMENU->PIO_PER  = 1 << KEYMENU;
  PORTKEYMENU->PIO_PUER = 1 << KEYMENU;
  PORTGATEEXT->PIO_PER  = 1 << GATEEXT;
  PORTGATEEXT->PIO_PUER = 1 << GATEEXT;

  keyboard_state = 0;
  KEY_LOCAL = 69 << 20;
  KEY_LOCAL_goal = KEY_LOCAL;
  MODE_MENU = 0;
  masque_led = 0xFFFFFFFF;
}

inline bool KEY_MENU(){
  return(!(PORTKEYMENU->PIO_PDSR & (1<<KEYMENU)));
}

inline bool GATE_EXT(){
  return(PORTGATEEXT->PIO_PDSR & (1<<GATEEXT));
}

inline void all_key() {
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);
    KEY = 0;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;
    KEY <<= 1;

    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);
    
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++; 
    KEY <<= 1;
    
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;
    KEY <<= 1;
    
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;

    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    delayMicroseconds(10);

    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY++; 
    KEY <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY++;  
    KEY <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY++;  
    KEY <<= 1;
    if (PORTKEY25->PIO_PDSR & (1<<KEY25)) KEY++;  

    KEY_global = KEY^0x1FFFFFF; // invert value
}

inline void keyboard_in() { 
  uint32_t velocity;
  uint32_t test_Y_N, test_module;
  uint32_t KEY_tmp;
  uint32_t KEY_LOCAL_tmp, KEY_ROWL_tmp, KEY_ROWH_tmp, KEY_ROW3_tmp;
  
  switch (keyboard_state) {
  case 0 :
    // get value in no specific order
    KEY_tmp = 0;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++; 
    KEY = KEY_tmp;
    
    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;
    
  case 1 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++; 
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;

  case 2 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;

  case 3 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 4 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 5 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 6 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_SODR = 1<<KEYOUT1; 
    //PORTKEYOUT2->PIO_SODR = 1<<KEYOUT2; 
    //PORTKEYOUT3->PIO_SODR = 1<<KEYOUT3;
    break;
    
  case 7 :
    KEY_tmp = KEY << 1;
    if (PORTKEYIN1->PIO_PDSR & (1<<KEYIN1)) KEY_tmp++; 
    KEY_tmp <<= 1;
    if (PORTKEYIN2->PIO_PDSR & (1<<KEYIN2)) KEY_tmp++;  
    KEY_tmp <<= 1;
    if (PORTKEYIN3->PIO_PDSR & (1<<KEYIN3)) KEY_tmp++;  
    KEY = KEY_tmp;

    keyboard_state++;
    PORTKEYOUT1->PIO_CODR = 1<<KEYOUT1; 
    PORTKEYOUT2->PIO_CODR = 1<<KEYOUT2; 
    PORTKEYOUT3->PIO_CODR = 1<<KEYOUT3;
    break;
    
  case 8 :
    // get last key
    KEY_tmp = KEY << 1;
    if (PORTKEY25->PIO_PDSR & (1<<KEY25)) KEY_tmp++;  
  
    KEY = KEY_tmp^0x1FFFFFF; // invert value
    keyboard_state = 0;
    MODE_MENU = KEY_MENU();
    
    if (KEY == KEY_old) break; 
    KEY_old = KEY;
    
    if (!MODE_MENU) { // menu
      KEY_tmp = KEY;
      KEY_ROWH_tmp = 0;
      KEY_ROWL_tmp = 0;
      KEY_ROW3_tmp = 0;
      
      if (KEY_tmp & 1<<2)  KEY_ROWH_tmp = 1; 
      if (KEY_tmp & 1<<8)  KEY_ROWH_tmp = 2; 
      if (KEY_tmp & 1<<14) KEY_ROWH_tmp = 3; 
      if (KEY_tmp & 1<<1)  KEY_ROWH_tmp = 4;
      if (KEY_tmp & 1<<7)  KEY_ROWH_tmp = 5;
      if (KEY_tmp & 1<<13) KEY_ROWH_tmp = 6;
      if (KEY_tmp & 1<<22) KEY_ROWH_tmp = 7;
      if (KEY_tmp & 1<<21) KEY_ROWH_tmp = 8;
      if (KEY_tmp & 1<<24) KEY_ROWH_tmp = 9;
      if (KEY_tmp & 1<<15) KEY_ROWH_tmp = 10;
      
      if (KEY_tmp & 1<<5)  KEY_ROWL_tmp = 1;
      if (KEY_tmp & 1<<11) KEY_ROWL_tmp = 2;
      if (KEY_tmp & 1<<17) KEY_ROWL_tmp = 3;
      if (KEY_tmp & 1<<20) KEY_ROWL_tmp = 4;
      if (KEY_tmp & 1<<23) KEY_ROWL_tmp = 5;
      if (KEY_tmp & 1<<4)  KEY_ROWL_tmp = 6;
      if (KEY_tmp & 1<<10) KEY_ROWL_tmp = 7;
      if (KEY_tmp & 1<<16) KEY_ROWL_tmp = 8;
      if (KEY_tmp & 1<<19) KEY_ROWL_tmp = 9;
      if (KEY_tmp & 1<<9)  KEY_ROWL_tmp = 10;
      if (KEY_tmp & 1<<3)  KEY_ROWL_tmp = 11;
      if (KEY_tmp & 1<<12) KEY_ROWL_tmp = 12;
      if (KEY_tmp & 1<<6)  KEY_ROWL_tmp = 13;
    
      if (KEY_tmp & 1<<18) KEY_ROW3_tmp = 1;
      if (KEY_tmp & 1<<0)  KEY_ROW3_tmp += 2;

      if ((KEY_ROWL_tmp == 0) && (KEY_ROWH_tmp == 0)) { // il n'y a rien d'important : on efface toutes les leds
        led_VCO1_off();
        led_VCO2_off();
        masque_led = 0xFFFFFFFF;
        led_gate_off();
      }
      else if ((KEY_ROW3_tmp == 2) && (KEY_ROWH_tmp > 0)) { // load conf
        led1_time = 300; // blink
        load_conf(KEY_ROWH_tmp);
      }
      else if ((KEY_ROW3_tmp == 1) && (KEY_ROWH_tmp > 0)) { // save conf
       led1_time = 300; // blink
       save_conf(KEY_ROWH_tmp);
      }
      else if ((KEY_ROWH_tmp >  0) && (KEY_ROWH_tmp != 10) ) { // modulation visualisation
        masque_led = 0;
        test_module = modulation_index[KEY_ROWH_tmp - 1];
        switch (test_module) {
          case 0:
            led_VCO1_on();
            led_VCO2_off();
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
            led_gate_off();
          break;
          case 1:
            led_VCO1_off();
            led_VCO2_on();
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
            led_gate_off();
          break;          
          case 2:
            led_VCO1_off();
            led_VCO2_off();
            and_led1 = 511;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
            led_gate_off();
          break;          
          case 3:
            led_VCO1_off();
            led_VCO2_off();
            and_led1 = 512;
            and_led2 = 511;
            and_led3 = 512;
            and_led4 = 512;
            led_gate_off();
          break;          
          case 4:
            led_VCO1_off();
            led_VCO2_off();
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 511;
            and_led4 = 512;
            led_gate_off();
          break;          
          case 5:
            led_VCO1_off();
            led_VCO2_off();
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 511;
            led_gate_off();
          break;          
          case 6:
            led_VCO1_off();
            led_VCO2_off();
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
            led_gate_on();
          break;          
          case 7:
            led_VCO1_off();
            led_VCO2_on();
            and_led1 = 511;
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
            led_gate_on();
          break;          
          case 8:
            led_VCO1_on();
            led_VCO2_off();
            and_led1 = 511;
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
            led_gate_on();
          break;          
          case 9:
            led_VCO1_on();
            led_VCO2_on();
            and_led1 = 512;
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 511;
            led_gate_on();
          break;          
          case 10:
            led_VCO1_on();
            led_VCO2_on();
            and_led1 = 511;
            and_led2 = 512;
            and_led3 = 511;
            and_led4 = 511;
            led_gate_on();
          break;          
          case 11:
            led_VCO1_on();
            led_VCO2_on();
            and_led1 = 511;
            and_led2 = 511;
            and_led3 = 512;
            and_led4 = 511;
            led_gate_on();
          break;          
          case 12:
            led_VCO1_on();
            led_VCO2_on();
            and_led1 = 511;
            and_led2 = 511;
            and_led3 = 511;
            and_led4 = 512;
            led_gate_on();
          break;
        }
      }
      if ((KEY_ROWL_tmp >  0) && (KEY_ROWH_tmp > 0)) { // 1 inter sur la rangé du haut + 1 inter sur la rangé du bas
        if (KEY_ROWH_tmp < 10) 
          modulation_index[KEY_ROWH_tmp-1] = KEY_ROWL_tmp-1;
        else 
          LFO3_WF = KEY_ROWL_tmp-1;
      }
      else if ((KEY_ROW3_tmp == 2) && (KEY_ROWL_tmp > 0)) { // NO  or conf
        switch (KEY_ROWL_tmp) {
        case 1:
          VCO_link = 0;
          break;
        case 2:
          RINGMOD = 0;
          break;
        case 3:
          VCF_pitch = 0;
          break;
        case 4:
          sync_LFO1 = 0;
          break;
        case 5:
          MIX_type = 1;
          break;
        case 6:
          EFFECT_type = 0;
          break;
        case 7:
          EFFECT_type = 1;
          break;
        case 8:
          EFFECT_type = 2;
          break;
        case 9:
          EFFECT_type = 3;
          break;
        case 10:
          cvg_type = 0;
          break;
        case 11:
          cvg_type = 1;
          break;
        case 12:
          cvg_type = 2;
          break;
        case 13:
          cvg_type = 3;
          break;
        }
      }
      else if ((KEY_ROW3_tmp == 1) && (KEY_ROWL_tmp > 0)) { // YES or conf
        switch (KEY_ROWL_tmp) {
        case 1:
          VCO_link = 1;
          break;
        case 2:
          RINGMOD = 1;
          break;
        case 3:
          VCF_pitch = 1;
          break;
        case 4:
          sync_LFO1 = 1;
          break;
        case 5:
          MIX_type = 0;
          break;
        case 6:
          filter_type = 0;
          set_LP24();
          break;
        case 7:
          filter_type = 1;
          set_LP12();
          break;
        case 8:
          filter_type = 2;
          set_BP12();
          break;
        case 9:
          filter_type = 3;
          set_HP12();
          break;
        case 10:
          GATE_mode = 0;
          break;
        case 11:
          GATE_mode = 1;
          break;
        case 12:
          GATE_mode = 2;
          break;
        case 13:
          GATE_mode = 3;
          break;
        }
      }
      if ((KEY_ROWH_tmp == 0) && (KEY_ROWL_tmp >   0) ) { // YES / NO visualisation
        switch (KEY_ROWL_tmp) {
          case 1:
            test_Y_N = VCO_link;
            masque_led = 0xFFFFFFFF;
          break;
          case 2:
            test_Y_N = RINGMOD;
            masque_led = 0xFFFFFFFF;
          break;
          case 3:
            test_Y_N = VCF_pitch;
            masque_led = 0xFFFFFFFF;
          break;
          case 4:
            test_Y_N = sync_LFO1;
            masque_led = 0xFFFFFFFF;
          break;
          case 5:
            test_Y_N = MIX_type;
            masque_led = 0xFFFFFFFF;
          break;
          case 6: // Filter type and effect type
          case 7: 
          case 8: 
          case 9: 
            test_Y_N = filter_type + 2;
            masque_led = 0;
            test_module = EFFECT_type;
          break;
          case 10: // gate mode and CV gen type
          case 11: 
          case 12: 
          case 13: 
            test_Y_N = GATE_mode + 2;
            masque_led = 0;
            test_module = cvg_type;
          break;
        }
        switch (test_Y_N) {
          case 0: // No
            led_VCO1_off();
            led_VCO2_on();
          break;
          case 1: // Yes 
            led_VCO1_on();
            led_VCO2_off();
          break;
          case 2: // 0
            led_VCO1_off();
            led_VCO2_off();
          break;
          case 3: // 1
            led_VCO1_off();
            led_VCO2_on();
          break;
          case 4: // 2
            led_VCO1_on();
            led_VCO2_off();
          break;
          case 5: // 3
            led_VCO1_on();
            led_VCO2_on();
        }
        switch (test_module) {
          case 0: // 
            and_led1 = 511;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 512;
          break;
          case 1: // 
            and_led1 = 512;
            and_led2 = 511;
            and_led3 = 512;
            and_led4 = 512;
          break;
          case 2: // 
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 511;
            and_led4 = 512;
          break;  
          case 3: // 
            and_led1 = 512;
            and_led2 = 512;
            and_led3 = 512;
            and_led4 = 511;
          break;
        } 
      }
      save_conf(0);
    }
    else { // note keyboard Mode
      KEY_LOCAL_tmp = 0;
      velocity = -(1<<15)-1;
      KEY_tmp = KEY;
      if (KEY_tmp & 1<<5)  KEY_LOCAL_tmp = 52;
      if (KEY_tmp & 1<<2)  KEY_LOCAL_tmp = 53;
      if (KEY_tmp & 1<<11) KEY_LOCAL_tmp = 54;
      if (KEY_tmp & 1<<8)  KEY_LOCAL_tmp = 55;
      if (KEY_tmp & 1<<17) KEY_LOCAL_tmp = 56;
      if (KEY_tmp & 1<<20) KEY_LOCAL_tmp = 57;
      if (KEY_tmp & 1<<14) KEY_LOCAL_tmp = 58;
      if (KEY_tmp & 1<<23) KEY_LOCAL_tmp = 59;
      if (KEY_tmp & 1<<1)  KEY_LOCAL_tmp = 60;
      if (KEY_tmp & 1<<4)  KEY_LOCAL_tmp = 61;
      if (KEY_tmp & 1<<7)  KEY_LOCAL_tmp = 62;
      if (KEY_tmp & 1<<10) KEY_LOCAL_tmp = 63;
      if (KEY_tmp & 1<<16) KEY_LOCAL_tmp = 64;
      if (KEY_tmp & 1<<13) KEY_LOCAL_tmp = 65;
      if (KEY_tmp & 1<<19) KEY_LOCAL_tmp = 66;
      if (KEY_tmp & 1<<22) KEY_LOCAL_tmp = 67;
      if (KEY_tmp & 1<<9)  KEY_LOCAL_tmp = 68;
      if (KEY_tmp & 1<<3)  KEY_LOCAL_tmp = 69;
      if (KEY_tmp & 1<<21) KEY_LOCAL_tmp = 70;
      if (KEY_tmp & 1<<12) KEY_LOCAL_tmp = 71;
      if (KEY_tmp & 1<<24) KEY_LOCAL_tmp = 72;
      if (KEY_tmp & 1<<6)  KEY_LOCAL_tmp = 73;
      if (KEY_tmp & 1<<15) KEY_LOCAL_tmp = 74;
      if (KEY_tmp & 1<<18) KEY_LOCAL_tmp = 75;
      if (KEY_tmp & 1<<0)  KEY_LOCAL_tmp = 76;    
      if (KEY_LOCAL_tmp) {
        KEY_LOCAL_goal = KEY_LOCAL_tmp  << (2+18);
        velocity = (1<<15)-1;
      }
      NOTE_ON = KEY_LOCAL_tmp;
      modulation_data[mod_VEL] = velocity;
    }
    break;
  }
}

inline void PORTAMENTO_update() {
  uint32_t tmp32;
  tmp32 = adc_value16[PORTAMENTO_VALUE];
  portamento = (table_CV2freq[0x350-(tmp32>>7)]);
}

inline void PORTAMENTO() {
    KEY_LOCAL +=  m_s32xs32_s32H(KEY_LOCAL_goal - KEY_LOCAL, portamento);
}

