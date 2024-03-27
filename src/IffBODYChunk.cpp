/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2023 Markus Schordan

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

#include "IffBODYChunk.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

#include "AGAConvException.hpp"

using namespace std;

namespace AGAConv {

IffBODYChunk::IffBODYChunk():
  IffDataChunk()
{
  name="BODY";
}

IffBODYChunk::~IffBODYChunk() {
}

void IffBODYChunk::uncompress(UWORD width, UWORD height, UBYTE bpp, UBYTE mask) {
  throw AGAConvException(142, "Decompression of ILBM data not supported.");
}

size_t IffBODYChunk::uncompressedLength() {
  return data.size(); // protected data inherited from IffDataChunk
}

ByteSequence* IffBODYChunk::getUncompressedData() {
  ByteSequence* byteSequence=new ByteSequence();
  // BODY was never compressed. Copy original BODY.
  for(IffDataChunkIterator i=begin();i!=end();++i) {
    UBYTE b=*i;
    byteSequence->add(b);
  }
  return byteSequence;
}

} // namespace AGAConv
