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

uint32_t lastKEY;

inline void test(){ // hardware test
  uint32_t tmp32, KEY_ROWL_tmp, test;
  
  all_key();
  test = (KEY_global & 1<<5);
  
  while(test) { // do not exit test mode
    all_key();
    WF_in();
    analog_in();
    analog_start_1();
    analog_get_1();
    analog_start_1();
    analog_get_1(); 
        
    if (KEY_global & 1<<5)  KEY_ROWL_tmp = 1;
    if (KEY_global & 1<<11) KEY_ROWL_tmp = 2;
    if (KEY_global & 1<<17) KEY_ROWL_tmp = 3;
    if (KEY_global & 1<<20) KEY_ROWL_tmp = 4;
    if (KEY_global & 1<<23) KEY_ROWL_tmp = 5;
    if (KEY_global & 1<<4)  KEY_ROWL_tmp = 6;
    if (KEY_global & 1<<10) KEY_ROWL_tmp = 7;
    if (KEY_global & 1<<16) KEY_ROWL_tmp = 8;
    if (KEY_global & 1<<19) KEY_ROWL_tmp = 9;
    if (KEY_global & 1<<9)  KEY_ROWL_tmp = 10;
    if (KEY_global & 1<<3)  KEY_ROWL_tmp = 11;
    if (KEY_global & 1<<12) KEY_ROWL_tmp = 12;
    if (KEY_global & 1<<6)  KEY_ROWL_tmp = 13;
    if (KEY_global & 1<<18) KEY_ROWL_tmp = 14;
    if (KEY_global & 1<<0)  KEY_ROWL_tmp = 15;
      
    if (KEY_ROWL_tmp == 0) KEY_ROWL_tmp = lastKEY;
    lastKEY = KEY_ROWL_tmp;
    
    switch (KEY_ROWL_tmp) {
    case 1: // all led on
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, 255);
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, 255);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, 255);
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 255);
      led1_on();
      led_VCO1_on();
      led_VCO2_on();
    break;
    case 2: // all led off
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, 0);
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 0);
      led1_off();
      led_VCO1_off();
      led_VCO2_off();
    break;
    case 3: // analog value
      tmp32 = adc_value[VCO1_FQ] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[VCO1_MOD1] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[VCO1_MOD2] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[PORTAMENTO_VALUE] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);      
    break;
    case 4:
      tmp32 = adc_value[VCO2_FQ] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[VCO2_MOD1] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[VCO2_MOD2] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = 0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);       
    break;
    case 5:
      tmp32 = adc_value[VCF_FQ] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[VCF_Q] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[VCF_MOD1] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[VCF_MOD2] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);        
    break;
    case 6:
      tmp32 = adc_value[LFO1_FQ] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[LFO1_WF] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[LFO1_SYM] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[ADSR_A] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);        
    break;
    case 7:
      tmp32 = adc_value[LFO2_FQ] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[LFO2_WF] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[LFO2_SYM] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[ADSR_D] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);        
    break;
    case 8:
      tmp32 = adc_value[LFO3_FQ] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[LFO3_MOD] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[EFFECT1] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[ADSR_S] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);        
    break;
    case 9:
      tmp32 = adc_value[CVG1] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[CVG2] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[EFFECT2] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[ADSR_R] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);         
    break;
    case 10:
      tmp32 = adc_value[VCA_MIX] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[VCA_MOD] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[VCA_GAIN] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[LDR] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);  
    break;
    case 11:
      tmp32 = 0; 
      if (GATE_EXT()) tmp32 = 511;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
      tmp32 = adc_value[EXT_1] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
      tmp32 = adc_value[EXT_2] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
      tmp32 = adc_value[EXT_3] >> 3;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]); 
    break;
    case 12: // WF1 sel
      tmp32 = (VCO1_WF & 8)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, tmp32);
      tmp32 = (VCO1_WF & 4)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
      tmp32 = (VCO1_WF & 2)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, tmp32);
      tmp32 = (VCO1_WF & 1)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, tmp32);
    break;
    case 13: // WF2 sel
      tmp32 = (VCO2_WF & 8)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 4, tmp32);
      tmp32 = (VCO2_WF & 4)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
      tmp32 = (VCO2_WF & 2)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 7, tmp32);
      tmp32 = (VCO2_WF & 1)? 255:0;
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, tmp32); 
    break;
    case 14: // keyboard : up row
      if (KEY_global & 1<<2)  led_VCO1_on(); else led_VCO1_off(); 
      if (KEY_global & 1<<8)  led_VCO2_on(); else led_VCO2_off(); 
      if (KEY_global & 1<<14) tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 4, tmp32);
      if (KEY_global & 1<<1)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
      if (KEY_global & 1<<7)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 7, tmp32);
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 0);
    break;
    case 15: // key + inverseur
      if (KEY_global & 1<<13)  led_VCO1_on(); else led_VCO1_off(); 
      if (KEY_global & 1<<22)  led_VCO2_on(); else led_VCO2_off(); 
      if (KEY_global & 1<<21) tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 4, tmp32);
      if (KEY_global & 1<<24)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 6, tmp32);
      if (KEY_global & 1<<15)  tmp32 = 255; else tmp32 = 0; 
        PWMC_SetDutyCycle(PWM_INTERFACE, 7, tmp32);
      PWMC_SetDutyCycle(PWM_INTERFACE, 5, 0);
      if (KEY_MENU()) led1_on(); else led1_off();
    break;
    }
  }
}
