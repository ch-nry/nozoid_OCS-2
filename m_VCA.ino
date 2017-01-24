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

uint32_t VCO_FILTER_data[4];
uint32_t MIX_fader1, MIX_fader2;
uint32_t filter1, filter2, filter3, filter4, filter5;
uint32_t VCA_MIX_value, VCA_MIX_value_old;
uint32_t VCA_GAIN_value, VCA_GAIN_value_old, gain_filter;
uint32_t MIX_fader1_filter, MIX_fader2_filter;

inline void init_VCA() {
  VCO_FILTER_data[0] = 0x80000000;
  VCO_FILTER_data[1] = 0x80000000;
  VCO_FILTER_data[2] = 0x80000000;
  VCO_FILTER_data[3] = 0x80000000;
  MIX_fader1 = 0; 
  MIX_fader2 = 0;
}

inline void VCA_update(){
  uint32_t VCA_modulation, VCA_MIX_value_local, VCA_GAIN_value_local;
  uint32_t tmp = clip_ana_low16(adc_value16[VCA_MOD]);
  
  VCA_MIX_value_old = hysteresis16(clip_anaLH16(adc_value16[VCA_MIX]), VCA_MIX_value_old);

  VCA_MIX_value_local = VCA_MIX_value_old; 

  if(MIX_type == 0) {
    VCA_modulation = ((modulation_data[modulation_index[index_VCA_MOD]])+(1<<15)) * tmp;
    VCA_modulation >>= 16;
    VCA_modulation += VCA_MIX_value_local;
    tmp >>= 1;
    VCA_MIX_value_local = (VCA_modulation > tmp)? VCA_modulation - tmp : 0;
    VCA_MIX_value_local = (VCA_MIX_value_local > 0xFFF0)? 0xFFF0 : VCA_MIX_value_local;
    //analog_out_1(VCA_MIX_value<<16);
  }
  VCA_MIX_value = VCA_MIX_value_local;
  
  MIX_fader2 = VCA_MIX_value << 8; // 24 bit max
  
  VCA_GAIN_value_old = hysteresis16(clip_ana_low16(adc_value16[VCA_GAIN]), VCA_GAIN_value_old);
  VCA_GAIN_value_local = VCA_GAIN_value_old;

  if(MIX_type != 0) {
    VCA_modulation = (0xFFFF - (1<<15) - (modulation_data[modulation_index[index_VCA_MOD]])) * tmp;
    VCA_modulation >>= 16;
    VCA_GAIN_value_local = VCA_GAIN_value_local * (0xFFFF - VCA_modulation);
    VCA_GAIN_value_local >>= 16;
  }
  
  VCA_GAIN_value_local *= VCA_GAIN_value_local; // pow 2 for a nice amplitude curve
  VCA_GAIN_value_local >>= 16;
  VCA_GAIN_value = VCA_GAIN_value_local;
}

inline int32_t MIX(uint32_t VCO1_out, uint32_t VCO2_out) {
  uint32_t tmp1, tmp2, tmp3;
  uint32_t VCO2;
  int32_t VCO_out;

  MIX_fader2_filter += (MIX_fader2 >> 10) - (MIX_fader2_filter >> 10);
  MIX_fader1_filter = 0x00FFF000-MIX_fader2_filter;
  VCO2 = (RINGMOD)? (m_s32xs32_s32H(VCO1_out^0x80000000, VCO2_out^0x80000000)<<1)^0x80000000: VCO2_out;
  VCO_out  =  m_u32xu32_u32H(VCO1_out,  MIX_fader1_filter) + m_u32xu32_u32H(VCO2,  MIX_fader2_filter);
  
  //analog_out_1(VCO_out<<8);
  //MIX_out = VCO_out;
  return(VCO_out);
}

inline int32_t VCA(int32_t EFFECT_out) {
  uint32_t gain;
  int32_t tmp32, env;

  gain_filter += (int32_t)(VCA_GAIN_value - gain_filter) >> 10;
  gain = gain_filter << 15;
  tmp32 = m_s32xs32_s32H(EFFECT_out, gain); 
  env = (GATE_mode != 3)? ADSR_out<<1 : 0X7FFFFFFF; // on est en signed, le MSB est le signe
  return(m_s32xs32_s32H(tmp32, env)<<2);
}

