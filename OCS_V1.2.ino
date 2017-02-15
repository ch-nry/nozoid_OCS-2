// OCS-2
// based on arduino duo
//
// main file
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

#include <DueFlashStorage.h>
#include "ARS.h"
#include "conf.h"

#pragma GCC optimize ("-O3")

DueFlashStorage dueFlashStorage;

// global variables
int32_t audio_out, audio_out2, audio_inL, audio_inR2, audio_inR;

// in_ADC
uint32_t adc_value[34], adc_value16[34], adc_value_accum[34];

// in keyboard
uint32_t KEY_LOCAL_goal, NOTE_ON;
uint32_t KEY_LOCAL, KEY_global;

// in MIDI
uint32_t MIDI_gate, MIDI_pitch, MIDI_fader[36];

// portamento
uint32_t portamento;

// in_WF
uint32_t VCO1_WF, VCO2_WF;

// module VCO1
uint32_t VCO1_increment;

// module VCO2
uint32_t VCO2_increment;

// module VCF
int32_t  filter_type;
int32_t G1, G2, G3, G5;

// module VCA

// module LFO
uint32_t LFO3_WF;
uint32_t LFO1_phase, LFO1_increment;
uint32_t LFO2_phase, LFO2_increment;
uint32_t LFO3_phase, LFO3_increment;

// module cvg
//   Random
uint32_t LFO4_phase, LFO4_increment;
uint32_t random_goal, random_filter1, random_filter2;
//   AR
uint32_t AR_value;
//   LFO4
uint32_t LFO5_phase; 
//   NL
uint32_t LFO6_phase, LFO7_phase; 

// module_ADSR
uint32_t ADSR_out;

// module EFFECT 
uint32_t EFFECT_type;

// modulation
int32_t modulation_data[13];
uint32_t modulation_index[9];

// leds 
uint32_t led1_time; // time in data loop number

// config
uint32_t GATE_mode, VCO_link, RINGMOD, VCF_pitch, sync_LFO1, cvg_type, MIX_type;

// env follower
uint32_t envelope;

void setup() {
  uint32_t i;

  REG_PMC_PCER0 = 1 << 11; // enable la clock du port PIO A pour les entree
  REG_PMC_PCER0 = 1 << 12; // enable la clock du port PIO B pour les entree
  REG_PMC_PCER0 = 1 << 13; // enable la clock du port PIO C pour les entree
  REG_PMC_PCER0 = 1 << 14; // enable la clock du port PIO D pour les entree

  REG_SUPC_SMMR = 0x0000110B; // suply monitor reset at 3V
  REG_SUPC_MR = 0xA5005A00;

  EFC0->EEFC_FMR = 0X00000400; // mandatory to keep program speed when loading the dueFlashStorage library. go wonder why.

  init_dac();

  init_debug();
  init_led();
  init_analog_out();
  init_random();
  init_analog_in();
  init_keyboard();
  init_WF();
  init_midi();
  init_save();

  init_VCO();
  init_VCF();
  init_LFO1();
  init_LFO2();
  init_LFO3();
  init_CVG();
  init_ADSR();
  init_VCA();
  init_ENV();
  init_EFFECT();
  
  VCF_freq();
  VCO1_freq();
  VCO2_freq();
  PORTAMENTO_update();
  VCA_update();
  
  test(); // hardware test mode
 
  start_dac();
  
  while (true) main_loop(); // do not go into arduino loop
}

inline void main_loop() { // as fast as possible
  uint32_t compteur, i, sound_in;
  uint32_t tmpU32;
  int32_t tmp32;
  
  #ifdef syncro_out
    test2_on();
  #endif
  
  analog_in();
  keyboard_in();
  WF_in();
  analog_start_1(); // start 1 sample
  VCO1_freq();
  VCO2_freq();
  
  #ifdef syncro_out
    test2_off();
  #endif  
  
  VCF_freq();
  LFO3_freq();
  CVG_mod();
  LFO1_modulation();
  analog_get_1(); // get 1 sample
  analog_start_1(); // start 2nd sample
  LFO2_modulation();
  LFO3_modulation();
  ENVELOPE_modulation();
  MIDI_in();
  LFO1_freq();
  LFO2_freq();
  PORTAMENTO_update();
  ADSR_update();
  VCA_update();
  EFFECT_update();
  update_leds(); // gate and midi leds
  update_ext(); // external analog value
  analog_get_1(); // 2nd sample
}

void loop() {
  //not used
}

inline void compute_audio_sample() {
  uint32_t VCO1_out, VCO2_out;
  int32_t MIX_out, VCF_out, VCA_out;
  
  PORTAMENTO();                               // 0.1 µs
  VCO1_out = VCO1();                          // 2.9 µs
  VCO2_out = VCO2();                          // 2.9 µs
  MIX_out = MIX(VCO1_out, VCO2_out);          // 0.8 µs
  VCF_out = VCF(MIX_out);                     // 2.4 µs
  ADSR();                                     // 0.1 µs
  VCA_out = EFFECT(VCF_out);                  // 0.8 µs

  // modulation signal
  LFO1();         // 0.4 µs 
  LFO2();         // 0.4 µs
  LFO3();         // 0.4 µs
  ENVELOPE();     // 0.4 µs
 
  audio_out = VCA_out;
}

void SSC_Handler(void){
  
  #ifdef syncro_out
    test1_on();
  #endif
  
  if (!(REG_SSC_SR & (1<<10))) {
    REG_SSC_THR = REG_SSC_RHR; // just to initialise properlly (not to invert R and L)
    NVIC_ClearPendingIRQ(SSC_IRQn); // next sample is allready here, no need to go to an other interuption to get it (it save time)
  }
  else {
    audio_inL = REG_SSC_RHR;
    REG_SSC_THR = audio_out2;
    audio_out2 = audio_out; // Why is that mandatory to have the L and R in sync???
    
    compute_audio_sample();
    
    while(!(REG_SSC_SR & (1<<4))) {} // wait for the next sample to be ready (it should mostlly be here, but somtimes not)
    NVIC_ClearPendingIRQ(SSC_IRQn); // next sample is allready here, no need to go to an other interuption to get it (it save time)
    
    audio_inR = audio_inR2; // to get the L and R in phase
    audio_inR2 = REG_SSC_RHR;
    REG_SSC_THR = audio_out;
  }
  #ifdef syncro_out
    test1_off();
  #endif 
}


