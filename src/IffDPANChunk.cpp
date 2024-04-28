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

#include "IffDPANChunk.hpp"

#include <iostream>
#include <sstream>

#include "AGAConvException.hpp"

using namespace std;

namespace AGAConv {

void IffDPANChunk::setFrames(UWORD nframes) {
  this->nframes=nframes;
}

UWORD IffDPANChunk::getFrames() {
  return nframes;
}

void IffDPANChunk::readChunk() {
  readChunkSize();
  version=readUWORD();
  nframes=readUWORD();
  flags=readULONG();
}

void IffDPANChunk::writeChunk() {
  if(outFile==0) {
    throw AGAConvException(145, "no outfile set in chunk "+getName());
  }
  writeChunkNameAndSize();
  writeUWord(version);
  writeUWord(nframes);
  writeULong(flags);
}

void IffDPANChunk::init() {
  dataSize=sizeof(UWORD)+sizeof(UWORD)+sizeof(ULONG);
  name="DPAN";
}

IffDPANChunk::IffDPANChunk():
  version(3),
  nframes(0),
  flags(0)
{
  init();
}

IffDPANChunk::IffDPANChunk(UWORD nframes):
  version(3),
  nframes(nframes),
  flags(0)
{
  init();
}

string IffDPANChunk::indent() {
  return "    ";
}
string IffDPANChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]"<<endl;
  if(getLongToString()) {
    ss<<indent()<<"Version: "<<version<<endl; 
    ss<<indent()<<"Number of frames: "<<nframes<<endl;
    ss<<indent()<<"Flags: "<<flags<<endl;
  }
  return ss.str();
}

} // namespace AGAConv

