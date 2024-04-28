/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2024 Markus Schordan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef IFF_ANHD_CHUNK_HPP
#define IFF_ANHD_CHUNK_HPP

#include "IffChunk.hpp"

namespace AGAConv {

enum ANHDOperation {
                    // ANIM-0. Frame uses the ILBM image format. The actual compression method (usually PackBits)
                    // is given by a field in the frame's BMHD chunk.
                    ILBM_BODY=0,
                    // ANIM 1, rarely supported. 
                    ILBM_XOR=1,
                    // ANIM 2, rarely supported. 
                    LONG_DELTA=2,
                    // ANIM 3
                    SHORT_DELTA=3,
                    // ANIM 4, rarely supported.
                    GENERALIZED_DELTA=4,
                    // ANIM 5, most common frame format.
                    BYTE_VERTICAL_DELTA=5,
                    // ANIM 6, rarely supported. 
                    STEREO_BYTE_VERTICAL_DELTA=6,                    
                    // ANIM 7 (also called SHORT_LONG_VERTICAL_DELTA)
                    GENERALIZED_VERTICAL_DELTA_SEPARATE_DATA=7,
                    // ANIM 8
                    GENERALIZED_VERTICAL_DELTA_INLINE_DATA=8
};

class IffANHDChunk : public IffChunk {
 public:
  IffANHDChunk();
  void readChunk();
  void writeChunk();
  std::string toString();
  std::string indent();
 private:
  UBYTE operation; // 0=normal ILBM BODY
  UBYTE mask;
  UWORD w,h;
  UWORD x,y;
  ULONG absTime;
  ULONG relTime;
  UBYTE interleave;
  UBYTE pad1; // not used
  ULONG bits;
  UBYTE pad2[16]; // not used
};

} // namespace AGAConv

#endif
