/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2021 Markus Schordan

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

#ifndef IFF_ANHD_CHUNK_H
#define IFF_ANHD_CHUNK_H

#include "IffChunk.hpp"

enum ANHDOperation { ILBM_BODY, ILBM_XOR, LONG_DELTA, SHORT_DELTA, GENERALIZED_DELTA, BYTE_VERTICAL_DELTA, SHORT_LONG_VERTICAL_DELTA=7};

class IffANHDChunk : public IffChunk {
 public:
  IffANHDChunk();
  void readChunk();
  void writeChunk();
  string toString();
  string indent();
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

#endif
