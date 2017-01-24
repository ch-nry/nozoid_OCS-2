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

inline uint64_t m_u32xu32_u64(uint32_t in1, uint32_t in2) {
  // mul uint32_t with uint32_t,
  uint64 out;
  asm volatile("umull %0, %1, %2, %3" : "=r" (out.U32[0]), "=r" (out.U32[1]) : "r" (in1), "r" (in2));
  return(out.U64);
}

inline uint64_t m_s32xs32_s64(uint32_t in1, uint32_t in2) {
  // mul uint32_t with uint32_t,
  uint64 out;
  asm volatile("smull %0, %1, %2, %3" : "=r" (out.U32[0]), "=r" (out.U32[1]) : "r" (in1), "r" (in2));
  return(out.U64);
}

inline uint64_t m_u32xu32_a_u64_u64(uint32_t in1, uint32_t in2, uint64_t sum) {
  // mul uint32_t with uint32_t, then add uint64_t
  uint64 tmp;
  tmp.U64=sum;
  asm volatile("umlal %0, %1, %2, %3" : : "r" (tmp.U32[0]), "r" (tmp.U32[1]), "r" (in1), "r" (in2));
  return (tmp.U64);
}

inline uint64_t m_s32xs32_a_s64_s64(uint32_t in1, uint32_t in2, uint64_t sum) {
  uint64 tmp;
  tmp.U64=sum;
  asm volatile("smlal %0, %1, %2, %3" : : "r" (tmp.U32[0]), "r" (tmp.U32[1]), "r" (in1), "r" (in2));
  return (tmp.U64);
}

inline uint64_t m_u32xu32_a_u64_u32H(uint32_t in1, uint32_t in2, uint64_t sum) {
  // mul uint32_t with uint32_t, then add uint64_t
  uint64 tmp;
  tmp.U64=sum;
  asm volatile("umlal %0, %1, %2, %3" : : "r" (tmp.U32[0]), "r" (tmp.U32[1]), "r" (in1), "r" (in2));
  return (tmp.U32[1]);
}

inline uint32_t m_u32xu32_u32H(uint32_t in1, uint32_t in2) {
  uint32_t outH, outL;
  asm volatile("umull %0, %1, %2, %3" : "=r" (outL), "=r" (outH) : "r" (in1), "r" (in2));
  return(outH);
}

/*
inline uint32_t m_u32xu32_u32H(uint32_t in1, uint32_t in2) {
  return(((uint64_t)in1 * (uint64_t)in2)>>32);
}*/

inline uint32_t m_s32xs32_s32H(int32_t in1, int32_t in2) {
  int32_t outH, outL;
  asm volatile("smull %0, %1, %2, %3" : "=r" (outL), "=r" (outH) : "r" (in1), "r" (in2));
  return(outH);
}
/*
inline int32_t m_s32xs32_s32H(uint32_t in1, uint32_t in2) {
  return(((int64_t)in1 * (int64_t)in2)>>32);
}*/

inline uint32_t sqr_u32_u32(uint32_t in1) {
  uint32_t outH, outL;
  asm volatile("umull %0, %1, %2, %2" : "=r" (outL), "=r" (outH) : "r" (in1));
  return(outH);
}

inline uint32_t sqr_s32_s32(uint32_t in1) {
  uint32_t outH, outL;
  asm volatile("smull %0, %1, %2, %2" : "=r" (outL), "=r" (outH) : "r" (in1));
  return(outH);
}

inline uint32_t m_u32xu32_u32L(uint32_t in1, uint32_t in2) {
  uint32_t out;
  asm volatile("mul %0, %1, %2" : "=r" (out) : "r" (in1), "r" (in2));
  return(out);
}

inline uint32_t m_u32xu32_a_u32_u32L(uint32_t in1, uint32_t in2, uint32_t add) {
  uint32_t out;
  asm volatile("mla %0, %1, %2, %3" : "=r" (out) : "r" (in1), "r" (in2), "r" (add));
  return(out);
}

inline uint32_t m_u32xu32_s_u32_u32L(uint32_t in1, uint32_t in2, uint32_t add) {
  uint32_t out;
  asm volatile("mls %0, %1, %2, %3" : "=r" (out) : "r" (in1), "r" (in2), "r" (add));
  return(out);
}

inline int32_t clip_S32_31(int32_t in1) {
  uint32_t out;
  asm volatile("ssat %0, 31, %1" : "=r" (out) : "r" (in1));
  return(out);
}
