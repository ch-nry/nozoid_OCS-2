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
