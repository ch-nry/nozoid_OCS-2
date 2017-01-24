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

#define PORTWFOUT3 PIOD
#define WFOUT3 0
#define PORTWFOUT2 PIOD
#define WFOUT2 2
#define PORTWFOUT1 PIOD
#define WFOUT1 6
#define PORTWFIN1 PIOB
#define WFIN1 12

uint32_t WF_state;
uint32_t WF1, WF2;

inline void init_WF(){
  PORTWFOUT1->PIO_PER = 1 << WFOUT1; // out to control demux bus
  PORTWFOUT1->PIO_OER = 1 << WFOUT1;
  PORTWFOUT1->PIO_PUDR= 1 << WFOUT1;
  PORTWFOUT2->PIO_PER = 1 << WFOUT2;
  PORTWFOUT2->PIO_OER = 1 << WFOUT2;
  PORTWFOUT2->PIO_PUDR= 1 << WFOUT2;
  PORTWFOUT3->PIO_PER = 1 << WFOUT3;
  PORTWFOUT3->PIO_OER = 1 << WFOUT3;
  PORTWFOUT3->PIO_PUDR= 1 << WFOUT3;

  PORTWFIN1->PIO_PER  = 1 << WFIN1; // input with pull up to get switch value
  PORTWFIN1->PIO_PUER = 1 << WFIN1;
}

inline void WF_in(){
  switch (WF_state) {
  case 0:
    WF1=0;
    WF2=0;
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF1 += 1;
    PORTWFOUT1->PIO_SODR = 1<<WFOUT1;
    //PORTWFOUT2->PIO_CODR = 1<<WFOUT2;
    //PORTWFOUT3->PIO_CODR = 1<<WFOUT3;
    WF_state = 1;
    break;
    
  case 1:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF1 += 2;
    PORTWFOUT1->PIO_CODR = 1<<WFOUT1;
    PORTWFOUT2->PIO_SODR = 1<<WFOUT2;
    //PORTWFOUT3->PIO_CODR = 1<<WFOUT3;
    WF_state = 2;
    break;
  case 2:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF1 += 4;
    PORTWFOUT1->PIO_SODR = 1<<WFOUT1;
    //PORTWFOUT2->PIO_SODR = 1<<WFOUT2;
    //PORTWFOUT3->PIO_CODR = 1<<WFOUT3;
    WF_state = 3;
    break;    
  case 3:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF1 += 8;
    PORTWFOUT1->PIO_CODR = 1<<WFOUT1;
    PORTWFOUT2->PIO_CODR = 1<<WFOUT2;
    PORTWFOUT3->PIO_SODR = 1<<WFOUT3;
    WF_state = 4;
    break;    
  case 4:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF2 += 1;
    PORTWFOUT1->PIO_SODR = 1<<WFOUT1;
    //PORTWFOUT2->PIO_CODR = 1<<WFOUT2;
    //PORTWFOUT3->PIO_SODR = 1<<WFOUT3;
    WF_state = 5;
    break;    
  case 5:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF2 += 2;
    PORTWFOUT1->PIO_CODR = 1<<WFOUT1;
    PORTWFOUT2->PIO_SODR = 1<<WFOUT2;
    //PORTWFOUT3->PIO_SODR = 1<<WFOUT3;
    WF_state = 6;
    break;    
  case 6:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF2 += 4;
    PORTWFOUT1->PIO_SODR = 1<<WFOUT1;
    //PORTWFOUT2->PIO_SODR = 1<<WFOUT2;
    //PORTWFOUT3->PIO_SODR = 1<<WFOUT3;
    WF_state = 7;
    break;    
  case 7:
    if (!(PORTWFIN1->PIO_PDSR & (1<<WFIN1))) WF2 += 8;
    PORTWFOUT1->PIO_CODR = 1<<WFOUT1;
    PORTWFOUT2->PIO_CODR = 1<<WFOUT2;
    PORTWFOUT3->PIO_CODR = 1<<WFOUT3;
    WF_state = 8;
    break;    
  case 8:
    VCO1_WF = (WF1 + (MIDI_fader[34]>>12)) & 0b1111;
    VCO2_WF = (WF2 + (MIDI_fader[35]>>12)) & 0b1111;
    
    WF_state = 0;
    break;
  }
}

