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

#include "ByteSequence.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;

ByteSequence::ByteSequence():
  debug(false),
  dataSize(0)
{
}

ByteSequence::ByteSequence(ULONG size):
  debug(false),
  dataSize(0)
{
  for(ULONG i=0;i<size;i++) {
    add(0);
  }
  assert(dataSize==size);
}

// nothing todo, all data default deallocated
ByteSequence::~ByteSequence() {
}

UBYTE* ByteSequence::address(ULONG offset) {
  assert(offset<data.size());
  return &data[offset];
}

UBYTE ByteSequence::getByte(ULONG offset) {
  return *address(offset);
}

void ByteSequence::setByte(ULONG offset, UBYTE byte) {
  *address(offset)=byte;
}

void ByteSequence::removeData() {
  dataSize=0;
  data.clear();
}

void ByteSequence::add(UBYTE byte) {
  data.push_back((UBYTE)byte);
  dataSize++;
}

void ByteSequence::readData(ULONG dataSize0) {
  if(ByteSequence::debug) cout<<"DEBUG: ByteSequence: readData - dataSize:"<<dataSize0<<endl;
  for(ULONG i=0;i<dataSize0;i++) {
    data.push_back((UBYTE)inFile->get());
  }
}

void ByteSequence::readDataAdjusted(ULONG dataSize0) {
  readData(dataSize0);
  readAdjustPadding(dataSize0);
  if(ByteSequence::debug) cout<<"DEBUG: readadjusted: done."<<dataSize0<<endl;
}

void ByteSequence::readAdjustPadding(uint32_t readDataSize) {
  if(readDataSize%2==1) {
    // read padding byte if data size is odd
    inFile->get();
  }
}

void ByteSequence::writeData() {
  for(ULONG i=0;i<dataSize;i++) {
    outFile->put(data[i]);
  }
}

void ByteSequence::printData() {
  for(ULONG i=0;i<dataSize;i++) {
    if(i!=0)
      cout<<", ";
    cout<<std::hex<<"0x"<<+data[i];
  }
  cout<<endl;
}

void ByteSequence::writeDataAdjusted() {
  writeData();
  writeAdjustPadding(dataSize);
}

void ByteSequence::writeAdjustPadding(uint32_t writeDataSize) {
  if(writeDataSize%2==1) {
    // write padding byte if data size is odd
    outFile->put(0);
  }
}

uint32_t ByteSequence::getDataSize() const {
  return (uint32_t)data.size();
}

void ByteSequence::setInFile(fstream* inFile) {
  this->inFile=inFile;
}

void ByteSequence::setOutFile(fstream* outFile) {
  this->outFile=outFile;
}
