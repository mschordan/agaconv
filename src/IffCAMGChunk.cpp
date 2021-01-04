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

#include "IffCAMGChunk.hpp"
#include <sstream>
#include <iostream>

void IffCAMGChunk::readChunk() {
  readChunkSize();
  viewMode=readULONG();
}

void IffCAMGChunk::writeChunk() {
  writeChunkNameAndSize();
  writeULONG(viewMode);
}

IffCAMGChunk::IffCAMGChunk():
  viewMode(0)
{
  name="CAMG";
  dataSize=4;
}

string IffCAMGChunk::indent() {
  return "    ";
}

string IffCAMGChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]"<<endl;
  if(getLongToString()) {
    // '+' converts unsigned char to unsigned int, otherwise it would be printed as char.
    ss<<indent()<<"HAM: "<<isHam()<<endl; 
    ss<<indent()<<"Hires: "<<isHires()<<endl;
    ss<<indent()<<"SuperHires: "<<isSuperHires()<<endl;
    ss<<indent()<<"HalfBrite: "<<isHalfBrite()<<endl;
    ss<<indent()<<"Lace: "<<isLace()<<endl;
    ss<<endl;
  }
  return ss.str();
}

string IffCAMGChunk::infoToString() {
  string s;
  if(isHam()) {
    s+="HAM ";
  }
  if(isSuperHires()) {
    s+="SUPERHIRES";
  } else if(isHires()) {
    s+="HIRES";
  } else {
    s+="LORES";
  }
  if(isLace()) {
    s+=" LACE";
  }
  return s;
}

ULONG IffCAMGChunk::getViewMode() {
  return viewMode;
}

bool IffCAMGChunk::isLace() {
  return viewMode&CAMG_LACE;
}

bool IffCAMGChunk::isHalfBrite() {
  return viewMode&CAMG_EHB;
}

bool IffCAMGChunk::isHam() {
  return viewMode&CAMG_HAM;
}

bool IffCAMGChunk::isHires() {
  return viewMode&CAMG_HIRES;
}
bool IffCAMGChunk::isSuperHires() {
  return viewMode&CAMG_SUPERHIRES;
}
void IffCAMGChunk::setLores() {
  // nothing to do. Lores is default of no other mode is set.
}
void IffCAMGChunk::setHires() {
  viewMode|=CAMG_HIRES;
}
void IffCAMGChunk::setSuperHires() {
  viewMode|=CAMG_SUPERHIRES;
}
void IffCAMGChunk::setUltraHires() {
  cerr<<"Error: Ultrahires not supported in CAMG chunk."<<endl;
  exit(1);
}
