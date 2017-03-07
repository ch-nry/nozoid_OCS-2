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

#define PORTTEST1 PIOC
#define TEST1 6
#define PORTTEST2 PIOA
#define TEST2 19
#define PORTGATE PIOC
#define PINGATE 4

volatile uint32_t rnd_seed;

static inline void nop() {
  asm volatile("nop");
}

inline void init_random() {
  REG_PMC_PCER1 = 1 << 9; // enable la clock du port PIO A pour les entree
  REG_TRNG_CR = 0x524e4701; // start random number generation 
}

inline uint32_t pseudo_rnd(uint32_t seed) {
  uint32_t rnd_bit;
  uint32_t tmp = seed;

  rnd_bit  = ((seed >> 0) ^ (seed >> 2) ^ (seed >> 3) ^ (seed >> 5) );
  tmp =  (seed >> 1) | (rnd_bit << 31);
  return(tmp);
}

inline uint32_t random32() {
  rnd_seed =  (REG_TRNG_ISR & 0x01)? REG_TRNG_ODATA : pseudo_rnd(rnd_seed); 
  // return a random value generated by the pross
  // if no random is available, then compute a pseudo - random number based on last one
  return(rnd_seed);
}

inline uint32_t clip_ana_low(uint32_t in) { // todo : checker la valeur a croper
uint32_t out;
  out = (in < 0x25)? 0: in - 0x25;
  //out = max(in,25)-25; // slower
  return(out);
}

inline uint32_t clip_ana_low16(uint32_t in) {
uint32_t out;
  out = (in < 0x100)? 0: in - 0x100;
  //out = max(in,0x100)-0x100; //slower
  return(out);
}

inline uint32_t clip_ana_low17(uint32_t in) {
uint32_t out;
  out = (in < 0x200)? 0: in - 0x200;
  //out = max(in,0x100)-0x100; //slower
  return(out);
}


inline uint32_t clip_anaLH(uint32_t in) { 
uint32_t out;
  out = (in < 0x10)? 0 : in - 0x10;
  out =  (out > 0xFDF)? 0xFFF : out + (out >> 7);
  return(out);
}


inline uint32_t clip_anaLH16(uint32_t in) {
uint32_t out;
  out = (in < 0xF0)? 0 : in - 0xF0;
  out =  (out > 0xFDFF)? 0xFFFF : out + (out >> 7);
  return(out);
  
}

inline uint32_t hysteresis(uint32_t in, uint32_t old) {
  int32_t tmp;
  uint32_t out;
  tmp = in-old;
   
  out = (tmp > 7)? in-7: old;
  out = (tmp < -7)? in+7: out;
  
  return(out);
}

inline uint32_t hysteresis16(uint32_t in, uint32_t old) {
  int32_t tmp;
  uint32_t out;
  tmp = in-old;
  
  out = (tmp >  128)? in-128: old; 
  out = (tmp < -128)? in+128: out;

  return(out);
}

inline uint32_t hysteresis17(uint32_t in, uint32_t old) {
  int32_t tmp;
  uint32_t out;
  tmp = in-old;
  
  out = (tmp >  256)? in-256: old; 
  out = (tmp < -256)? in+256: out;

  return(out);
}

inline uint32_t hysteresis32(uint32_t in, uint32_t old) {
  int32_t tmp;
  uint32_t out;
  tmp = in-old;
  
  out = (tmp >  (128<<15))? in-(128<<15): old; 
  out = (tmp < -(128<<15))? in+(128<<15): out;

  return(out);
}

inline int32_t gain2_cliped_S32(int32_t in) {  
  return(clip_S32_31(in)<<1);
}

inline void init_debug() {

  PORTTEST1->PIO_PER  = 1 << TEST1;
  PORTTEST1->PIO_OER  = 1 << TEST1;
  PORTTEST1->PIO_OWER = 1 << TEST1;
  PORTTEST1->PIO_PUDR = 1 << TEST1;
  PORTTEST2->PIO_PER  = 1 << TEST2;
  PORTTEST2->PIO_OER  = 1 << TEST2;
  PORTTEST2->PIO_OWER = 1 << TEST2;
  PORTTEST2->PIO_PUDR = 1 << TEST2;
  PORTGATE->PIO_OWER = 1 << PINGATE;
  PORTGATE->PIO_PUDR = 1 << PINGATE;
}

inline void toggle_test1() {
  PORTTEST1->PIO_ODSR ^=  1 << TEST1;
}

inline void toggle_test2() {
  PORTTEST2->PIO_ODSR ^=  1 << TEST2;
}

inline void test1_on() {
  PORTTEST1->PIO_SODR =  1 << TEST1;
}

inline void test1_off() {
  PORTTEST1->PIO_CODR =  1 << TEST1;
}

inline void test2_on() {
  PORTTEST2->PIO_SODR =  1 << TEST2;
}

inline void test2_off() {
  PORTTEST2->PIO_CODR =  1 << TEST2;
}

inline void pin_gate_on() {
  PORTGATE->PIO_SODR =  1 << PINGATE;
}

inline void pin_gate_off() {
  PORTGATE->PIO_CODR =  1 << PINGATE;
}

inline uint32_t MIX16U(uint32_t src, uint32_t dest, uint32_t factor) { 
  int32_t tmp;
  tmp = dest-src;
  tmp *= factor;
  tmp >>= 16;
  tmp += src;
  return(tmp);
}

inline uint32_t median3 (uint32_t last3, uint32_t last2, uint32_t last1) {
 /*
  uint32_t min3, max3, median;
 min3 = min(min(last1, last2), last3);
 max3 = max(max(last1, last2), last3);
 median = last1 + last2 + last3 - min3 - max3;
 */
 return(max(max(min(last1, last2), min(last1, last3)),min(last3, last2)));
}

