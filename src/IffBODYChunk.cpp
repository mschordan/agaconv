/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019, 2020 Markus Schordan

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

#define IFFANIM_PITCH  (32 / 8)           //pitch of scanline for frame output ("GetFrame()") can be set: multiple of 1, 2, 3 or 4 bytes



IffBODYChunk::IffBODYChunk():
  IffDataChunk(), 
  dataCompressed(nullptr),
  dataUncompressed(nullptr),
  dataCompressedSize(0),
  dataUncompressedSize(0)
{
  name="BODY";
}

IffBODYChunk::~IffBODYChunk() {
  if(dataCompressed) 
    delete dataCompressed;
  if(dataUncompressed)
    delete dataUncompressed;
  if(chunky24BitData)
    delete chunky24BitData;
}

void IffBODYChunk::duplicateDataVector() {
  assert(dataCompressed==nullptr);
  dataCompressed=new char[data.size()];
  int pos=0;
  for(UBYTE b : data) {
    dataCompressed[pos++]=b;
  }
  dataCompressedSize=pos;
}

void IffBODYChunk::uncompress(int width, int height, int bpp, int mask) {
  cerr<<"Decompression not implemented yet."<<endl;
  exit(1);
}

int IffBODYChunk::compressedLength() {
  return dataCompressedSize;
}
int IffBODYChunk::uncompressedLength() {
  return dataUncompressedSize;
}

RGBColor IffBODYChunk::get24BitColor(int pixelOffset) {
  if(chunky24BitData) {
    return RGBColor(chunky24BitData[pixelOffset*3],
                 chunky24BitData[pixelOffset*3+1],
                 chunky24BitData[pixelOffset*3+2]);
  } else {
    cerr<<"Error: chunky24Bit data not available."<<endl;
    exit(1);
  }
}

ByteSequence* IffBODYChunk::getUncompressedData() {
  assert(this!=0);
  //cout<<"DEBUG: IffBODYChunk::getUncompressedData() - START"<<endl;
  ByteSequence* byteSequence=new ByteSequence();
  // TODO: should alrady be a ByteSequence
  if(uncompressedLength()>0) {
    //cout<<"DEBUG: IffBODYChunk::getUncompressedData(): uncompressedLength:"<<uncompressedLength()<<endl;
    for(int i=0; i<uncompressedLength(); i++) {
      byteSequence->add(dataUncompressed[i]);
    }
  } else {
    //cout<<"DEBUG: IffBODYChunk::getUncompressedData(): body was never compressed"<<endl;
    // BODY was never compressed. Copy original BODY.
    for(IffDataChunkIterator i=begin();i!=end();++i) {
      byteSequence->add(*i);
    }
  }
  //cout<<"DEBUG: IffBODYChunk::getUncompressedData() - END"<<endl;
  return byteSequence;
}
