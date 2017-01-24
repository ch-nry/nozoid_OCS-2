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

// DATA format :
// audio data : uint32_t : (32 significant bit)
//
// modulation coef : int32_t
// 0 for no mudulation
// 0x0000FFFF for highest modulation (16 significant bits + sign)
// 0x8000FFFF for lowest modulation
//
// pitch : uint32_t
// 0 for lowest frequency
// 0x0FFFFFFF for highest freq (28 significat bits)
// half note every 0xO0100000
// 256 half tone (twice midi range)
// midi note range : -126 to 130;
//       from 0.005Hz to about 15KHz
//       69 is 440Hz


union uint64 {
  uint64_t U64;
  uint32_t U32[2];
};

enum adc_pos {
  VCO1_FQ,
  VCO1_MOD1,
  VCO1_MOD2,
  PORTAMENTO_VALUE,
  VCO2_FQ,
  VCO2_MOD1,
  VCO2_MOD2,
  VCF_FQ,
  VCF_Q,
  VCF_MOD1,
  VCF_MOD2,
  LFO1_FQ,
  LFO1_WF,
  LFO1_SYM,
  ADSR_A,
  LFO2_FQ,
  LFO2_WF,
  LFO2_SYM,
  ADSR_D,
  LFO3_FQ,
  LFO3_MOD,
  EFFECT1,
  ADSR_S,
  CVG1,
  CVG2,
  EFFECT2,
  ADSR_R,
  VCA_MIX,
  VCA_MOD,
  VCA_GAIN,
  LDR,
  EXT_1,
  EXT_2,
  EXT_3
};

enum modulation_value {
  mod_VCO1,
  mod_VCO2,
  mod_LFO1,
  mod_LFO2,
  mod_LFO3,
  mod_CVG,
  mod_ADSR,
  mod_LDR, // light
  mod_ENV, // audio in envelope
  mod_VEL, // midi velocity
  mod_EXT1,
  mod_EXT2,
  mod_EXT3
};

enum modulation_adresse {
  index_VCO1_MOD1,
  index_VCO1_MOD2,
  index_VCO2_MOD1,
  index_VCO2_MOD2,
  index_VCA_MOD,
  index_VCF_MOD1,
  index_VCF_MOD2,
  index_EFFECT_MOD,
  index_LFO3_MOD
};

const uint32_t midi_pos[] = { 
  0,
  34,
  1,
  2,
  3,
  4,
  35,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29
};

