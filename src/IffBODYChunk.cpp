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

#include "IffBODYChunk.hpp"
#include <sstream>
#include <iostream>
#include <memory.h>
#include <cassert>
using namespace std;

IffBODYChunk::IffBODYChunk():
  IffDataChunk()
{
  name="BODY";
}

IffBODYChunk::~IffBODYChunk() {
}

void IffBODYChunk::uncompress(int width, int height, int bpp, int mask) {
  cerr<<"Decompression of ILBM data not supported."<<endl;
  exit(1);
}

size_t IffBODYChunk::uncompressedLength() {
  return data.size(); // protected data inherited from IffDataChunk
}

ByteSequence* IffBODYChunk::getUncompressedData() {
  assert(this!=0);
  ByteSequence* byteSequence=new ByteSequence();
  // BODY was never compressed. Copy original BODY.
  for(IffDataChunkIterator i=begin();i!=end();++i) {
    UBYTE b=*i;
    byteSequence->add(b);
  }
  return byteSequence;
}
