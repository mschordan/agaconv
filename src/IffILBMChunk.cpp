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
#include "IffILBMChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffBMHDChunk.hpp"
#include "IffANHDChunk.hpp"
#include "IffSXHDChunk.hpp"
#include "IffSBDYChunk.hpp"
#include "IffDPANChunk.hpp"
#include "IffCAMGChunk.hpp"
#include "IffBODYChunk.hpp"
#include "IffUnknownChunk.hpp"
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;

IffILBMChunk::IffILBMChunk() {
  isFormFlag=true;
  name="ILBM";
  animStartPos=0;
}

IffILBMChunk::~IffILBMChunk() {
  // delete all chunks
  for (auto chunk : chunkList) {
    delete chunk;
  }
}

void IffILBMChunk::setAnimStartPos(uint32_t animStartPos) {
  this->animStartPos=animStartPos;
}

uint32_t IffILBMChunk::getAnimStartPos() {
  return animStartPos;
}

uint16_t IffILBMChunk::numberOfChunks() {
  return chunkList.size();
}

string IffILBMChunk::readName() {
  return readChunkName();
}

void IffILBMChunk::readChunk() {
  string formTag=readName();
  if(formTag!="FORM") {
    cerr<<"Error: expected FORM (of ILBM). Found "<<formTag<<"."<<endl;
    exit(1);
  }
  readChunkSize();
  setDataSize(getDataSize()-4); // correction for ILBM only

  if(this->getDataSize()==0) {
    cout<<"WARNING: empty FORM."<<endl;
    return;
  }
  string formName=readName();
  if(formName!="ILBM") {
    cerr<<"Error: expected ILBM. Found "<<formTag<<"."<<endl;
    exit(1);
  }
  if(IffChunk::debug) cout<<"DEBUG: ILBM READY - reading chunks now."<<endl;
  uint32_t accumulatedDataSize=0;
  while(accumulatedDataSize<this->getDataSize()) {
    string nextChunkName=readChunkName();
    IffChunk* iffChunk=0;
    if(nextChunkName=="BMHD") {
      iffChunk=new IffBMHDChunk();
    } else if(nextChunkName=="CMAP") {
      iffChunk=new IffCMAPChunk();
    } else if(nextChunkName=="BODY") {
      iffChunk=new IffBODYChunk();
    } else if(nextChunkName=="ANHD") {
      iffChunk=new IffANHDChunk();
    } else if(nextChunkName=="SXHD") {
      iffChunk=new IffSXHDChunk();
    } else if(nextChunkName=="SBDY") {
      iffChunk=new IffSBDYChunk();
    } else if(nextChunkName=="DPAN") {
      iffChunk=new IffDPANChunk();
    } else if(nextChunkName=="CAMG") {
      iffChunk=new IffCAMGChunk();
    } else {
      iffChunk=new IffUnknownChunk(nextChunkName);
    }
    iffChunk->setFile(this->file);
    iffChunk->setOutFile(this->outFile);
    iffChunk->readChunk();
    chunkList.push_back(iffChunk);
    if(nextChunkName=="SXHD") {
      iffChunk->setLongToString(true);
      if(IffChunk::debug) cout<<"DEBUG:\n"<<iffChunk->toString()<<endl;
    }
    if(nextChunkName=="DPAN") {
      iffChunk->setLongToString(true);
      if(IffChunk::debug) cout<<"DEBUG:\n"<<iffChunk->toString()<<endl;
    }
    if(IffChunk::debug) cout<<"Adding "<<iffChunk->getAdjustedTotalChunkSize()<<"."<<endl;
    accumulatedDataSize+=iffChunk->getAdjustedTotalChunkSize();
    if(IffChunk::debug) cout<<"DEBUG: accumulated chunk size: "<<accumulatedDataSize<<" of "<<this->getDataSize()<<endl;
  }
}

void IffILBMChunk::writeChunk() {
  writeName("FORM");
  writeULong(getDataSize()+4); // correction for ILBM only
  writeName("ILBM");
  for(IffChunk* chunk : chunkList) {
    if(IffChunk::debug) cerr<<"DEBUG: Writing chunk "<<chunk->getName()<<endl;
    chunk->writeChunk();
  }
}

string IffILBMChunk::indent() {
  return "  ";
}

string IffILBMChunk::toString() {
  stringstream ss;
  // exception: add 4 to (internal) dataSize (different to all other chunks)
  // the corrected value is the value stored in the ILBM file (see writeChunk)
  ss<<indent()<<"FORM "<<"["<<getDataSize()+4<<"] "<<getChunkName()<<" @"<<getAnimStartPos()<<endl;
  for(IffChunk* chunk : chunkList) {
    ss<<chunk->toString();
  }
  return ss.str();
}

string IffILBMChunk::toDetailedString() {
  stringstream ss;
  // exception: add 4 to (internal) dataSize (different to all other chunks)
  // the corrected value is the value stored in the ILBM file (see writeChunk)
  ss<<indent()<<"FORM "<<"["<<getDataSize()+4<<"] "<<getChunkName()<<" @"<<getAnimStartPos()<<endl;
  for(IffChunk* chunk : chunkList) {
    ss<<chunk->toDetailedString();
  }
  return ss.str();
}

void IffILBMChunk::insertFirst(IffChunk* newChunk) {
  chunkList.push_front(newChunk);
  dataSize+=newChunk->getTotalChunkSize();
}

void IffILBMChunk::insertLast(IffChunk* newChunk) {
  chunkList.push_back(newChunk);
  dataSize+=newChunk->getTotalChunkSize();
}

bool IffILBMChunk::insertBeforeBODYChunk(IffChunk* newChunk) {
  for (ChunkListType::iterator i=chunkList.begin();
       i!=chunkList.end();
       ++i) {
    if((*i)->getName()=="BODY") {
      chunkList.insert(i,newChunk);
      // update chunk length now
      dataSize+=newChunk->getTotalChunkSize();
      return true;
    }
  }
  return false;
}

bool IffILBMChunk::insertBeforeChunk(IffChunk* newChunk, string chunkName) {
  for (ChunkListType::iterator i=chunkList.begin();
       i!=chunkList.end();
       ++i) {
    if((*i)->getName()==chunkName) {
      chunkList.insert(i,newChunk);
      // update chunk length now
      dataSize+=newChunk->getTotalChunkSize();
      return true;
    }
  }
  return false;
}

bool IffILBMChunk::hasBODYChunk() {
  return getBODYChunk()!=nullptr;
}

bool IffILBMChunk::hasBMHDChunk() {
  return getBMHDChunk()!=nullptr;
}

bool IffILBMChunk::uncompressBODYChunk() {
  if(hasBMHDChunk() && hasBODYChunk()) {
    IffBMHDChunk* bmhdChunk=getBMHDChunk();
    int width=bmhdChunk->getWidth();
    int height=bmhdChunk->getHeight();
    int bpp=bmhdChunk->getNumPlanes();
    int mask=bmhdChunk->getMask();
    int compression=bmhdChunk->getCompression();
    if(compression!=0) {
      getBODYChunk()->uncompress(width, height, bpp, mask);
      return true;
    }
  }
  return false;
}

int IffILBMChunk::compressedBODYLength() {
  return getBODYChunk()->compressedLength();
}

int IffILBMChunk::uncompressedBODYLength() {
  return getBODYChunk()->uncompressedLength();
}

IffChunk* IffILBMChunk::getChunkByName(std::string name) {
  // usually short lists of 1-10 chunks
  for(IffChunk* chunk : chunkList) {
    if(chunk->getName()==name)
      return chunk;
  }
  return 0;
}

IffBODYChunk* IffILBMChunk::getBODYChunk() {
  return dynamic_cast<IffBODYChunk*>(getChunkByName("BODY"));
}

IffBMHDChunk* IffILBMChunk::getBMHDChunk() {
  return dynamic_cast<IffBMHDChunk*>(getChunkByName("BMHD"));
}
