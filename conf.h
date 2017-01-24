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

//you can adjust few option bellow:

// sound caractere.  
#define DAC_clear 1 // 1 for "clear", 0 for "fat"
  
// LFO1 Range
#define LFO1_range 0 // 1 for extended range
#define LFO1_offset 1000 // offset on fader position, (increase for higer frequency, decrease for lower freq)
// default is 0, 1000
// 0, 1<<20 : for high frequency only
// 1, 500, for extended range

// LFO2 Range
#define LFO2_range 0 // 1 for extended range
#define LFO2_offset 1000 // offset on fader position, 
// default is 0, 0
// 0, 1<<20 : for high frequency ony
// 1, 500, for extended range
  
// VCO FQ fader range
#define VCO_FQ_reduce 0
#define VCO_FQ_offset 0
// default is 0, 0 : for about 12 octave
// 2, 1<<25 : for 4 octaves : the rest can be obtain using a midi keyboard

// ADSR : Attack curve
// this variable can be in betwwen this 2 exterm value:
// 0X47000000 : for slow curve
// 0x7FFFFFFF : for hard curve
// default is 0x7FFFFFFF
#define ADSR_Goal_value 0x7FFFFFFF

// variable waveforme
// change the rate of the WF variation
// 0 fort fixed waveforme, increase for faster evolution
// loop time is : 0xFFFFFFFF / (VCO_WF2_speed * 48000)
#define VCO_WF2_speed 10000
//#define VCO_WF2_speed 0
#define VCO_WF3_speed 200000
//#define VCO_WF3_speed 0
#define VCO_WF4_speed 0

// EXT : analogue // midi // auto
// ana : use only the 3 analog input (-6 / 6V)
// midi : use only midi pitch wheel / mod wheel (cc1) / breath controler (cc2)
// auto : use ana until a midi parametter is send
#define EXT_AUTO
//#define EXT_ANA
//#define EXT_MIDI

// do not save current preset, and load default config on every startup
//#define nosave

// output LFO analog value on the "LFO OUT" pad on the PCB
// analogue value range from about 0.5V to 2.8V 
#define LFO_out

// output syncro (audio / data loop) for benchmarck purpose 
#define syncro_out

// output GATE on the dedicated pin
#define gate_out
