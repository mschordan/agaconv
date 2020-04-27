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

#include "IffChunk.hpp"
#include "IffANIMForm.hpp"
#include "IffILBMChunk.hpp"
#include <sstream>
#include <iostream>

IffANIMForm::IffANIMForm() {
}
IffANIMForm::~IffANIMForm() {
}

IffILBMChunk* IffANIMForm::getChunk(int i) {
  return dynamic_cast<IffILBMChunk*>(chunkList[i]);
}

void IffANIMForm::addChunk(IffILBMChunk* ilbmChunk) {
  chunkList.push_back(ilbmChunk);
}

size_t IffANIMForm::numberOfChunks() {
  return chunkList.size();
}

string IffANIMForm::readName() {
  return readChunkName();
}

bool IffANIMForm::endOfFile() {
  // need to read past the end ...
  file->get();
  if(file->eof()) {
    return true;
  } else {
    file->unget();
    return false;
  }
}

void IffANIMForm::readChunk() {
  uint32_t pos=0;
  string formTag=readName();
  if(formTag!="FORM") {
    cerr<<"Error: expected FORM. Found "<<formTag<<"."<<endl;
    exit(1);
  }
  this->readChunkSize();
  string formName=readName();
  if(formName!="ANIM") {
    cerr<<"Error: expected ANIM. Found "<<formTag<<"."<<endl;
    exit(1);
  }
  pos+=12;
  while(!endOfFile()) {
    IffILBMChunk* newChunk;
    newChunk=new IffILBMChunk();
    newChunk->setAnimStartPos(pos);
    newChunk->setFile(file);
    newChunk->setOutFile(outFile);
    newChunk->readChunk();
    addChunk(newChunk);
    pos+=newChunk->getTotalChunkSize();
  }
  cout<<"ANIM file: read "<<chunkList.size()<<" ILBM frames."<<endl;
}


void IffANIMForm::writeChunk() {
  writeName("FORM");
  writeULong(dataSize);
  writeName("ANIM");
  for(IffChunk* chunk : chunkList) {
    if(IffChunk::debug) {
      cout<<"Writing ILBM chunk:"<<endl;
      cout<<(chunk->toString())<<endl;
    }
    chunk->writeChunk();
  }
}

string IffANIMForm::indent() {
  return "";
}

string IffANIMForm::toString() {
  stringstream ss;
  ss<<indent()<<"FORM"<<" ["<<dataSize<<"] "<<"ANIM"<<endl;
  for(IffChunk* chunk : chunkList) {
    ss<<chunk->toString();
  }
  return ss.str();
}

IffChunk* IffANIMForm::getFirstChunk() {
  if(chunkList.size()>0) {
    return *chunkList.begin();
  } else {
    return 0;
  }
}
