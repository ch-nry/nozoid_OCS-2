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

#define PORTADC1OUT1 PIOB
#define ADC1OUT1 25
#define PORTADC1OUT2 PIOC
#define ADC1OUT2 28
#define PORTADC1OUT3 PIOC
#define ADC1OUT3 26
#define PORTADC2OUT1 PIOA
#define ADC2OUT1 4
#define PORTADC2OUT2 PIOA
#define ADC2OUT2 3
#define PORTADC2OUT3 PIOA
#define ADC2OUT3 2
 
uint32_t adc_channel;
uint32_t adc_value_filter[34];
uint32_t median_data[3][2];
 
inline void put_ADD_ADC1(uint32_t address) { 
  if (address & 0b001) PORTADC1OUT1->PIO_SODR = 1<<ADC1OUT1; else PORTADC1OUT1->PIO_CODR = 1<<ADC1OUT1;
  if (address & 0b010) PORTADC1OUT2->PIO_SODR = 1<<ADC1OUT2; else PORTADC1OUT2->PIO_CODR = 1<<ADC1OUT2;
  if (address & 0b100) PORTADC1OUT3->PIO_SODR = 1<<ADC1OUT3; else PORTADC1OUT3->PIO_CODR = 1<<ADC1OUT3;
}

inline void put_ADD_ADC2(uint32_t address) { 
  if (address & 0b001) PORTADC2OUT1->PIO_SODR = 1<<ADC2OUT1; else PORTADC2OUT1->PIO_CODR = 1<<ADC2OUT1;
  if (address & 0b010) PORTADC2OUT2->PIO_SODR = 1<<ADC2OUT2; else PORTADC2OUT2->PIO_CODR = 1<<ADC2OUT2;
  if (address & 0b100) PORTADC2OUT3->PIO_SODR = 1<<ADC2OUT3; else PORTADC2OUT3->PIO_CODR = 1<<ADC2OUT3;
}

inline void sel_adc_channel(uint32_t channel) {
  REG_ADC_CHDR = 0xFFFFFFFF; //disable all channeles
  REG_ADC_CHER = 1 << channel; // enable only 1 chanel to convert 
}

inline void init_analog_in() {
  int i;
  
  PORTADC1OUT1->PIO_PER  = 1 << ADC1OUT1; // out to control demux1 bus
  PORTADC1OUT1->PIO_OER  = 1 << ADC1OUT1; 
  PORTADC1OUT1->PIO_PUDR = 1 << ADC1OUT1; // disable pull_up
  PORTADC1OUT2->PIO_PER  = 1 << ADC1OUT2; // out to control demux1 bus
  PORTADC1OUT2->PIO_OER  = 1 << ADC1OUT2; 
  PORTADC1OUT2->PIO_PUDR = 1 << ADC1OUT2; // disable pull_up
  PORTADC1OUT3->PIO_PER  = 1 << ADC1OUT3; // out to control demux1 bus
  PORTADC1OUT3->PIO_OER  = 1 << ADC1OUT3; 
  PORTADC1OUT3->PIO_PUDR = 1 << ADC1OUT3; // disable pull_up
  
  PORTADC2OUT1->PIO_PER  = 1 << ADC2OUT1; // out to control demux2 bus
  PORTADC2OUT1->PIO_OER  = 1 << ADC2OUT1; 
  PORTADC2OUT1->PIO_PUDR = 1 << ADC2OUT1;
  PORTADC2OUT2->PIO_PER  = 1 << ADC2OUT2; 
  PORTADC2OUT2->PIO_OER  = 1 << ADC2OUT2; 
  PORTADC2OUT2->PIO_PUDR = 1 << ADC2OUT2;
  PORTADC2OUT3->PIO_PER  = 1 << ADC2OUT3; 
  PORTADC2OUT3->PIO_OER  = 1 << ADC2OUT3; 
  PORTADC2OUT3->PIO_PUDR = 1 << ADC2OUT3;

  REG_PMC_PCER1 = 1 << 5; // switch on ADC peripheral clock

  // disable pullup on analog input
  PIOA->PIO_PUDR = (1 << 22) + (1 << 23) + (1 << 24) + (1 << 6);
  PIOB->PIO_PUDR = (1 << 19) + (1 << 20);
  
  REG_ADC_MR  = 0x3F331F00; //0x3F030F00
  REG_ADC_EMR = 0x00000000;
  adc_channel = 0;

   // First conversion
  put_ADD_ADC1(5);
  sel_adc_channel(13); 
  
  //for (i=0; i < 34; i++) adc_value_filter[i] = 0x00; // reset filter
  
  for (i = 0; i < 1000; i++) { // resetting faders digital filter 
    analog_in(); 
    analog_start_1();
    analog_get_1();
    analog_start_1();
    analog_get_1();
  }
}

inline void analog_get_1() {
  while ((REG_ADC_ISR & 1<<24)==0) {} // wait the end of conversion
  adc_value_accum[adc_channel] += REG_ADC_LCDR;
}

inline void analog_start_1() {
  REG_ADC_CR = 1<<1; // start next conversion  
}

inline void analog_in() {
  uint32_t tmp, tmp2, median_1, median_2, median_3;
  // filter the analog data
  if(adc_channel<EXT_1) {
    tmp = adc_value_filter[adc_channel] << 3;
    tmp -= adc_value_filter[adc_channel];
    tmp += adc_value_accum[adc_channel] << 9; // already 1 bit because of 2 samples
    tmp >>= 3;
    adc_value_filter[adc_channel] = tmp;
    tmp >>= 6;
  } 
  else {
    tmp2 = adc_channel - EXT_1; // 0, 1 ou 2
    median_1 = median_data[tmp2][0];
    median_2 = median_data[tmp2][1];
    median_3 = adc_value_accum[adc_channel] << 3; // 13 bit + 3
    tmp = median3(median_1, median_2, median_3);
    median_data[tmp2][0] = median_2;
    median_data[tmp2][1] = median_3;
  }
  
  tmp += MIDI_fader[adc_channel];
  tmp = min(tmp, 0xFFFF); //(tmp & 0xFFFF0000)? 0x0000FFFF: tmp; // test de depassement a cause du MIDI
  
  adc_value[adc_channel] = tmp >> 4;
  adc_value16[adc_channel] = tmp;
  //analog_out_1(tmp<<10);

  adc_value_accum[adc_channel] = 0;
  //adc_value[adc_channel] = REG_ADC_LCDR; // get last convertion


  #ifndef EXT_MIDI
    if (++adc_channel == 34) adc_channel=0;
  #endif
  #ifdef EXT_MIDI // no need for last 3 adc since they are not used
    if (++adc_channel == 31) adc_channel=0;
  #endif
  
  switch ( adc_channel ) {
  case 0:
    put_ADD_ADC1(5);
    sel_adc_channel(13);
    break;
  case 1:
    put_ADD_ADC1(7);
    sel_adc_channel(13);  
    break;
  case 2:
    put_ADD_ADC1(6);
    sel_adc_channel(13);
    break;
  case 3:
    put_ADD_ADC1(4);
    sel_adc_channel(13);
    break;
  case 4:
    put_ADD_ADC1(3);
    sel_adc_channel(13);
    break;
  case 5:
    put_ADD_ADC1(1);
    sel_adc_channel(13);
    break;
  case 6:
    put_ADD_ADC1(2);
    sel_adc_channel(13);
    break;
  case 7:
    put_ADD_ADC1(5);
    sel_adc_channel(12);
    break;
  case 8:
    put_ADD_ADC1(7);
    sel_adc_channel(12);
    break;
  case 9:
    put_ADD_ADC1(6);
    sel_adc_channel(12);
    break;
  case 10:
    put_ADD_ADC1(4);
    sel_adc_channel(12);
    break;
  case 11:
    put_ADD_ADC1(0);
    sel_adc_channel(12);
    break;
  case 12:
    put_ADD_ADC1(3);
    sel_adc_channel(12);
    break;
  case 13:
    put_ADD_ADC1(1);
    sel_adc_channel(12);
    break;
  case 14:
    put_ADD_ADC1(2);
    sel_adc_channel(12);
    break;
  case 15:
    put_ADD_ADC1(2);
    sel_adc_channel(3);
    break;
  case 16:
    put_ADD_ADC1(1);
    sel_adc_channel(3);
    break;
  case 17:
    put_ADD_ADC1(3);
    sel_adc_channel(3);
    break;
  case 18:
    put_ADD_ADC1(0);
    sel_adc_channel(3);
    break;
  case 19:
    put_ADD_ADC1(4);
    sel_adc_channel(3);
    break;
  case 20:
    put_ADD_ADC1(6);
    sel_adc_channel(3);
    break;
  case 21:
    put_ADD_ADC1(7);
    sel_adc_channel(3);
    break;
  case 22:
    put_ADD_ADC1(5);
    sel_adc_channel(3);
    break;
  case 23:
    put_ADD_ADC2(7);
    sel_adc_channel(4);
    break;
  case 24:
    sel_adc_channel(5); // no demux here
    break;
  case 25:
    put_ADD_ADC2(4);
    sel_adc_channel(4);
    break;
  case 26:
    put_ADD_ADC2(6);
    sel_adc_channel(4);
    break;
  case 27:
    put_ADD_ADC2(5);
    sel_adc_channel(4);
    break;
  case 28:
    put_ADD_ADC2(2);
    sel_adc_channel(4);
    break;
  case 29:
    sel_adc_channel(6);// no demux here
    break;
  case 30:
    put_ADD_ADC1(0);
    sel_adc_channel(13);
    break;
  case 31:
    put_ADD_ADC2(1);
    sel_adc_channel(4);
    break;
  case 32:
    put_ADD_ADC2(0);
    sel_adc_channel(4);
    break;
  case 33:
    put_ADD_ADC2(3);
    sel_adc_channel(4);
    break;
  }
}

