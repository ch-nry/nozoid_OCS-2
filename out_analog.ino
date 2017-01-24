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

