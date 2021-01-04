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

#include <fstream>
#include <memory.h>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include "Options.hpp"
#include "Chunk.hpp"

bool Chunk::debug=false;
bool Chunk::longToString=false;

Chunk::Chunk():dataSize(0),file(0),outFile(0),isFormFlag(false) {
}

Chunk::~Chunk() {
}

void Chunk::setDebug(bool flag) {
  Chunk::debug=flag;
}

void Chunk::msg(string s) {
  if(Chunk::debug) {
    cout<<s<<endl;
  }
}

void Chunk::setLongToString(bool flag) {
  Chunk::longToString=flag;
}

bool Chunk::getLongToString() {
  return Chunk::longToString;
}

uint8_t Chunk::readUBYTE() {
  uint8_t byte=(uint8_t)file->get();
  return byte;
}

uint16_t Chunk::readUWORD() {
  return (((uint16_t)file->get())<<8)+((uint16_t)file->get());
}

uint32_t Chunk::readULONG() {
  return (((uint32_t)readUWORD())<<16)+(uint32_t)readUWORD();
}

int8_t Chunk::readSBYTE() {
  int8_t byte=(int8_t)file->get();
  return byte;
}

int16_t Chunk::readSWORD() {
  return (((int16_t)file->get())<<8)+((int16_t)file->get());
}

int32_t Chunk::readSLONG() {
  return (((int32_t)readUWORD())<<16)+(int32_t)readUWORD();
}

void Chunk::writeUBYTE(uint8_t x) {
  writeUByte(x);
}
void Chunk::writeUWORD(uint16_t x) {
  writeUWord(x);
}
void Chunk::writeULONG(uint32_t x) {
 writeULong(x);
}

void Chunk::writeUByte(uint8_t x) {
  assert(outFile);
  outFile->put((uint8_t)x);
}

void Chunk::writeUWord(uint16_t x) {
  writeUByte((uint8_t)(x>>8));
  writeUByte((uint8_t)x);
}

void Chunk::writeULong(uint32_t x) {
  writeUWord((uint16_t)(x>>16));
  writeUWord((uint16_t)x);
}

void Chunk::writeSBYTE(int8_t x) {
  assert(outFile);
  outFile->put((int8_t)x);
}

void Chunk::writeSWORD(int16_t x) {
  writeSBYTE((int8_t)(x>>8));
  writeUBYTE((uint8_t)(x&0xff));
}

void Chunk::writeSLONG(int32_t x) {
  writeSWORD((int16_t)(x>>16));
  writeUWORD((uint16_t)(x&0xffff));
}

void Chunk::readAdjustPadding(uint32_t readDataSize) {
  if(readDataSize%2==1) {
    // read padding byte if data size is odd
    UBYTE padByte=file->peek();
    if(padByte!=0) {
      cout<<"WARNING: chunk "<<getName()<<" of size "<<getDataSize()<<": ";
      if(options.paddingFix) {
        cout<<"padding byte not 0 ("<<+padByte<<") - assuming padding is missing (not reading padding byte)."<<endl;
      } else {
        cout<<"expected padding byte not 0 ("<<+padByte<<")."<<endl;
        file->get();
      }
    } else {
      if(Chunk::debug) cout<<"Reading padding byte."<<endl;
      file->get();
    }
  }
}

void Chunk::writeAdjustPadding(uint32_t writeDataSize) {
  if(writeDataSize%2==1) {
    // write padding byte if data size is odd
    outFile->put(0);
  }
}

string Chunk::indent() {
  return "";
}

string Chunk::peekName() {
  string peekName;
  for(int i=3;i>=0;i--) {
    peekName+=file->get();
  }
  file->seekg(-4,ios::cur);
  return peekName;
}

string Chunk::getName() {
  return name;
}

void Chunk::setName(string name) {
  this->name=name;
}

string Chunk::getChunkName() {
  return name;
}

void Chunk::setFile(fstream* fstream) {
  file=fstream;
}

void Chunk::setOutFile(fstream* fstream) {
  outFile=fstream;
}

uint32_t Chunk::getDataSize() const {
  return dataSize;
}

void Chunk::setDataSize(ULONG dataSize) {
  this->dataSize=dataSize;
}

uint32_t Chunk::getTotalChunkSize() const {
  return getDataSize()+8; // 8 equals size of chunk name and size of chunk length field.
}

uint32_t Chunk::getAdjustedTotalChunkSize() const {
  ULONG totalChunkSize=getTotalChunkSize();
  if(totalChunkSize%2==1) {
    return totalChunkSize+1;
  } else {
    return totalChunkSize;
  }
}

string Chunk::readChunkName() {
  if(Chunk::debug) cout<<"DEBUG: readChunkName - start"<<endl;
  string name="";
  for(int i=3;i>=0;i--) {
    char c=file->get();
    if(Chunk::debug) cout<<"DEBUG: readChunkName - read char "<<c<<endl;
    name+=c;
  }
  if(Chunk::debug) cout<<"DEBUG: readChunkName - end"<<endl;
  return name;
}

void Chunk::writeName(string cname) {
  for(size_t i=0;i<=3;i++) {
    if(i>=cname.size())
      outFile->put(' ');
    else
      outFile->put(cname[i]);
  }
}

void Chunk::writeChunkName() {
  for(size_t i=0;i<=3;i++) {
    if(i>=name.size())
      outFile->put(' ');
    else
      outFile->put(name[i]);
  }
}

void Chunk::readChunkSize() {
  dataSize=readULONG();
  if(Chunk::debug) cout<<"DEBUG: readChunkSize: dataSize:"<<dataSize<<endl;
}

void Chunk::writeChunkSize() {
  writeULong(dataSize);
}

void Chunk::readChunkNameAndSize() {
  readChunkName();
  readChunkSize();
}

void Chunk::writeChunkNameAndSize() {
  writeChunkName();
  writeChunkSize();
}

bool Chunk::isForm() {
  return isFormFlag;
}

string Chunk::toDetailedString() {
  return toString();
}
