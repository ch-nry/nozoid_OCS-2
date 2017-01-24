#define codec_ctl_port PIOC

#define codec_ctl_start (1<<14)
#define codec_ctl_clock (1<<18)
#define codec_ctl_data  (1<<16)

inline void send_dac(uint32_t dac_register, uint32_t dac_data) {
  // 3 wire protocole to send 9 bit to a 7bit adresse
  
  uint32_t i, j, to_send;

  to_send = (dac_register & 0x7F) << 9; // 7bit addresse
  to_send += dac_data & 0x1FF; // 9 bit of data

  codec_ctl_port->PIO_CODR = codec_ctl_clock; // clock to 0
  codec_ctl_port->PIO_CODR = codec_ctl_start; // latch to 0

  for (i=0; i<16; i++) {
    if (to_send & 1<<15)  
      codec_ctl_port->PIO_SODR = codec_ctl_data; // prepare the bit to send
    else
      codec_ctl_port->PIO_CODR = codec_ctl_data;
    
    codec_ctl_port->PIO_SODR = codec_ctl_clock; 
    to_send <<= 1;
    for(j=0; j<50; j++){nop();}; // wait
    codec_ctl_port->PIO_CODR = codec_ctl_clock; // latch the bit
    for(j=0; j<50; j++){nop();}; // wait
  }
  codec_ctl_port->PIO_SODR = codec_ctl_start; // latch the data
}

inline void init_dac() {
  // configuration of the 3 wire control
  
  // set the 3 control pins as output
  codec_ctl_port->PIO_PER = codec_ctl_start + codec_ctl_clock + codec_ctl_data;
  codec_ctl_port->PIO_OER = codec_ctl_start + codec_ctl_clock + codec_ctl_data;

  // configure register using 3 wire protocol
  // using only right channel
  send_dac(0x06,0b000010010); // powerdown
  send_dac(0x00,0b000011111); // left line in 
  send_dac(0x01,0b000011111); // right line in 
  send_dac(0x02,0b000000000); // left headphone to 0
  send_dac(0x03,0b000000000); // right headphone
  send_dac(0x04,0b011010010); // analogue audio path
  if (DAC_clear == 1)
    send_dac(0x05,0b000010000); // digital audio path (high pass , no desenphasis, store offset)
  else
    send_dac(0x05,0b000010110); // digital audio path (high pass , desenphasis, store offset)

  send_dac(0x07,0b001000110); // digital audio interface (I2S / 32 bit / right channel when daclrc is low / not swap / master/ don't invert blck)
  send_dac(0x08,0b000000001); // sampling control (usb , 250fs,  48K, clock div 2, clk out, active)
  send_dac(0x09,0b000000001);

  // configuration of the 12S slave communication
  // TK : transmiter clock (input)    : PA 14 (B)
  // TF : transmitter synchro (input) : PA 15 (B)
  // TD : transmit data (output)      : PA 16 (B)
  // RF : receive synchro (input)     : PB 17 (A)
  // RD : receive data : input)       : PB 18 (A)
}

inline void start_dac() {
  PIOA->PIO_PDR  = (1 << 14) + (1 << 15) + (1 << 16); // peripheral control of the pin
  PIOB->PIO_PDR  = (1 << 17) + (1 << 18);
  PIOA->PIO_PUDR = (1 << 14) + (1 << 15) + (1 << 16); // disable pull_up
  PIOB->PIO_PUDR = (1 << 17) + (1 << 18); 
  PIOA->PIO_ABSR =  PIOA->PIO_ABSR | ((1 << 14) + (1 << 15) + (1 << 16)); // peripherique B 
  PIOB->PIO_ABSR =  PIOB->PIO_ABSR & (!((1 << 17) + (1 << 18)));
  
  pmc_enable_periph_clk(ID_SSC); 
  NVIC_EnableIRQ(SSC_IRQn);

  REG_SSC_TCMR = 0x00010702;
  REG_SSC_TFMR = 0x0000019F;
  REG_SSC_RCMR = 0x00010121;
  REG_SSC_RFMR = 0x0000009F;
  
  ssc_enable_interrupt(SSC, SSC_IER_RXRDY);

  ssc_enable_rx(SSC);
  ssc_enable_tx(SSC);
 
  send_dac(0x06,0b000000010); // switch dac on
}
