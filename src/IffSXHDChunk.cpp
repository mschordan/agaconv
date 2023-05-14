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

#include "IffSXHDChunk.hpp"

#include <iostream>
#include <sstream>

using namespace std;

namespace AGAConv {

void IffSXHDChunk::readChunk() {
  readChunkSize();
  sampleDepth=readUBYTE();
  fixedVolume=readUBYTE();
  length=readULONG();
  playRate=readULONG();
  compressionMethod=readULONG();
  usedChannels=readUBYTE();
  usedMode=readUBYTE();
  playFreq=readULONG();
  loop=readUWORD();
}

void IffSXHDChunk::writeChunk() {
  writeChunkNameAndSize();
  writeUByte(sampleDepth);
  writeUByte(fixedVolume);
  writeULong(length);
  writeULong(playRate);
  writeULong(compressionMethod);
  writeUByte(usedChannels);
  writeUByte(usedMode);
  writeULong(playFreq);
  writeUWord(loop);
}

IffSXHDChunk::IffSXHDChunk():
  sampleDepth(0),
  fixedVolume(64),
  length(0),
  playRate(0), // previous default 162
  compressionMethod(0),
  usedChannels(0),
  usedMode(0),
  playFreq(0),
  loop(0)
{
  name="SXHD";
  dataSize=22;
}

IffSXHDChunk::IffSXHDChunk(UBYTE sampleDepth, ULONG lengthPerFrame, ULONG playRate, UBYTE usedChannels, UBYTE usedMode, ULONG playFreq):
  IffSXHDChunk()
{
  this->sampleDepth=sampleDepth;
  this->length=lengthPerFrame;
  this->playRate=playRate;
  this->usedChannels=usedChannels;
  this->usedMode=usedMode;
  this->playFreq=playFreq;
}

string IffSXHDChunk::indent() {
  return "    ";
}
string IffSXHDChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]"<<endl;
  if(getLongToString()) {
    // '+' converts unsigned char to unsigned int, otherwise it would be printed as char.
    ss<<indent()<<"Sample Depth: "<<+sampleDepth<<endl; 
    ss<<indent()<<"Fixed Volume: "<<+fixedVolume<<endl;
    ss<<indent()<<"Length: "<<+length<<endl;
    ss<<indent()<<"Play rate: "<<playRate<<endl;
    ss<<indent()<<"Compression method: "<<compressionMethod<<endl;
    ss<<indent()<<"Used channels: "<<+usedChannels<<endl;
    ss<<indent()<<"Used mode: "<<+usedMode<<endl;
    ss<<indent()<<"Play Frequency: "<<playFreq<<endl;
    ss<<indent()<<"Loop: "<<loop<<endl;
    ss<<endl;
  }
  return ss.str();
}

} // namespace AGAConv
