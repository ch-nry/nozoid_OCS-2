#include <Arduino.h>

#define PORTLED1 PIOC
#define LED1 12
#define PORTLEDVCO1 PIOA
#define LEDVCO1 13
#define PORTLEDVCO2 PIOA
#define LEDVCO2 12

// not used
#define PORTPWM4 PIOC 
#define PWM4 22
#define PORTPWM3 PIOC
#define PWM3 24
#define PORTPWM1 PIOC
#define PWM1 21
#define PORTPWM2 PIOC
#define PWM2 23


uint32_t pwm1, pwm2, pwm3, pwm4; // 10 bits variables
uint32_t pwm1_tmp, pwm2_tmp, pwm3_tmp, pwm4_tmp;
uint32_t led_ADSR;

inline void init_led() {
  PORTLED1->PIO_PER = 1 << LED1;
  PORTLED1->PIO_OER = 1 << LED1; 
  PORTPWM1->PIO_PER = 1 << PWM1;
  PORTPWM1->PIO_OER = 1 << PWM1;
  PORTPWM2->PIO_PER = 1 << PWM2;
  PORTPWM2->PIO_OER = 1 << PWM2;
  PORTPWM3->PIO_PER = 1 << PWM3;
  PORTPWM3->PIO_OER = 1 << PWM3;
  PORTPWM4->PIO_PER = 1 << PWM4;
  PORTPWM4->PIO_OER = 1 << PWM4;
  PORTLEDVCO1->PIO_PER = 1 << LEDVCO1;
  PORTLEDVCO1->PIO_OER = 1 << LEDVCO1;
  PORTLEDVCO2->PIO_PER = 1 << LEDVCO2;
  PORTLEDVCO2->PIO_OER = 1 << LEDVCO2;
  led1_off();
  pwm1 = 0;
  pwm2 = 0;
  pwm3 = 0;
  pwm4 = 0;

  led1_time = 300; // small flash at initialisation

  analogWrite(9, 0); // lazy PWM configuration
  analogWrite(8, 0); 
  analogWrite(7, 0);
  analogWrite(6, 0);
}


inline void led1_on() {
  PORTLED1->PIO_SODR =  1 << LED1;
}

inline void led1_off() {
  PORTLED1->PIO_CODR =  1 << LED1;
}

inline void led_VCO1_on() {
  PORTLEDVCO1->PIO_SODR =  1 << LEDVCO1;
}

inline void led_VCO1_off() {
  PORTLEDVCO1->PIO_CODR =  1 << LEDVCO1;
}

inline void led_VCO2_on() {
  PORTLEDVCO2->PIO_SODR =  1 << LEDVCO2;
}

inline void led_VCO2_off() {
  PORTLEDVCO2->PIO_CODR =  1 << LEDVCO2;
}

inline void led_gate_on() {
  led_ADSR = 1;
}

inline void led_gate_off() {
  led_ADSR = 0;  
}

inline void update_leds() { 
  uint32_t time_tmp, tmp32;
  bool led1;

  if (led_ADSR != 0) {
    led1_on();
  }
  else {
    if ((led1_time != 0) ){ 
      led1_time--;
      if (MODE_MENU) // || currentMenu) 
        led1_off();
      else 
        led1_on();
    }
    else {
      if (MODE_MENU) // || currentMenu)
        led1_on();
      else 
        led1_off();
    }
  }
  
  // todo : optimized, and more precision PWM, 
  tmp32 = (masque_led==0)? and_led1:(modulation_data[mod_LFO1] + (1<<15))>>7;
  //analogWrite(9, table_led[tmp32]); 
  PWMC_SetDutyCycle(PWM_INTERFACE, 4, table_led[tmp32]);
  tmp32 = (masque_led==0)? and_led2:(modulation_data[mod_LFO2] + (1<<15))>>7;
  //analogWrite(7, table_led[tmp32]);
  PWMC_SetDutyCycle(PWM_INTERFACE, 6, table_led[tmp32]);
  tmp32 = (masque_led==0)? and_led3:(modulation_data[mod_LFO3] + (1<<15))>>7;
  //analogWrite(6, table_led[tmp32]);
  PWMC_SetDutyCycle(PWM_INTERFACE, 7, table_led[tmp32]);
  tmp32 = (masque_led==0)? and_led4:(modulation_data[mod_CVG] + (1<<15))>>7;
  //analogWrite(8, table_led[tmp32]);
  PWMC_SetDutyCycle(PWM_INTERFACE, 5, table_led[tmp32]);
}


