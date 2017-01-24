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

inline void init_analog_out() {
  REG_PMC_PCER1 = 0X00000040; // switch on peripherical clock
  REG_DACC_MR = 0x01000000;
  REG_DACC_CHER = 0X00000003; 
  REG_DACC_ACR = 0x0000001A;
  REG_DACC_CDR = 1<<10; // mise a 0.5 le dac
}

inline void analog_out_1(uint32_t signal_out) {
  while ((REG_DACC_ISR & 0x00000002) == 0); // wait the converter to be ready
  REG_DACC_MR &= ~(0x00010000); // channel 0
  REG_DACC_CDR = (signal_out >> 20) & 0xFFF; // 12 MSB
}

inline void analog_out_2(uint32_t signal_out) {
  while ((REG_DACC_ISR & 0x00000002) == 0); // wait the converter to be ready
  REG_DACC_MR |= 0x00010000; // channel 1
  REG_DACC_CDR = (signal_out >> 20) & 0xFFF; // 12 MSB
}

