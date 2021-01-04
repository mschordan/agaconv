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

#include "IffDataChunk.hpp"
#include <sstream>
#include <iostream>

void IffDataChunk::removeData() {
  data.clear();
  dataSize=0;
}

void IffDataChunk::add(UBYTE byte) {
  data.push_back((UBYTE)byte);
  dataSize++;
}

void IffDataChunk::readData(ULONG dataSize0) {
  if(IffChunk::debug) cout<<"DEBUG: IffDataChunk: readData - dataSize:"<<dataSize0<<endl;
  for(ULONG i=0;i<dataSize0;i++) {
    int getByte=file->get();
    UBYTE byte=(UBYTE)getByte;
    data.push_back(byte);
    dataSize++;
  }
  readAdjustPadding(dataSize0);
  if(IffChunk::debug) cout<<"DEBUG: readDataChunk: done."<<dataSize<<endl;
}

void IffDataChunk::readChunk() {
  if(IffChunk::debug) cout<<"DEBUG: readChunk DataChunk start."<<endl;
  readChunkSize();
  if(IffChunk::debug) cout<<"DEBUG: readChunk: size: "<<getDataSize()<<endl;
  readData(getDataSize());
}

void IffDataChunk::writeChunk() {
  writeChunkNameAndSize();
  writeData();
}

IffDataChunk::IffDataChunk() {
  name="DATA";
}

string IffDataChunk::indent() {
  return "    ";
}

string IffDataChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]"<<endl;
  return ss.str();
}

void IffDataChunk::writeData() {
  for(ULONG i=0;i<dataSize;i++) {
    outFile->put(data[i]);
  }
  writeAdjustPadding(dataSize);
}

IffDataChunkIterator IffDataChunk::begin() {
  return data.begin();
}

IffDataChunkIterator IffDataChunk::end() {
  return data.end();
}

//size_t IffDataChunk::getDataSize() {
//  return data.size();
//}
