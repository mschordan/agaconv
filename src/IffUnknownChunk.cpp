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

#include "IffUnknownChunk.hpp"

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

namespace AGAConv {

void IffUnknownChunk::readData(ULONG dataSize0) {
  if(IffChunk::debug) cout<<"DEBUG: IffUnknownChunk: readData - dataSize:"<<dataSize0<<endl;
  for(ULONG i=0;i<dataSize0;i++) {
    data.push_back((UBYTE)file->get());
  }
  readAdjustPadding(dataSize);
  if(IffChunk::debug) cout<<"DEBUG: readUnkownChunk: done."<<dataSize<<endl;
}

void IffUnknownChunk::readChunk() {
  if(IffChunk::debug) cout<<"DEBUG: readChunk UnknownChunk start."<<endl;
  readChunkSize();
  if(IffChunk::debug) cout<<"DEBUG: readChunk: size="<<getDataSize()<<endl;
  readData(getDataSize());
}

void IffUnknownChunk::writeChunk() {
  writeChunkNameAndSize();
  writeData();
}

IffUnknownChunk::IffUnknownChunk() {
  name="UNKNOWN";
}

IffUnknownChunk::IffUnknownChunk(std::string chunkName) {
  name=chunkName;
}

string IffUnknownChunk::indent() {
  return "    ";
}

string IffUnknownChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<"*["<<getDataSize()<<"]"<<endl;
  return ss.str();
}

void IffUnknownChunk::writeData() {
  for(ULONG i=0;i<dataSize;i++) {
    outFile->put(data[i]);
  }
  writeAdjustPadding(dataSize);
}

} // namespace AGAConv
