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

#include "IffCMAPChunk.hpp"
#include <sstream>
#include <iostream>
#include <cassert>

void IffCMAPChunk::addColor(RGBColor col) {
  add(col.getRed());
  add(col.getGreen());
  add(col.getBlue());
}

void IffCMAPChunk::reserveNumColors(int num) {
  removeData();
  int numBytes=num*3;
  for(int i=0;i<numBytes;i++) {
    add(0);
  }
}

void IffCMAPChunk::checkColorIndex(int idx) {
  if(idx>=numberOfColors()) {
    cerr<<"Error: IffCMAPChunk::setColor out of bounds in CMAP chunk."<<endl;
    exit(1);
  }
}

void IffCMAPChunk::setColor(int idx, RGBColor col) {
  checkColorIndex(idx);
  UBYTE* address=&data[0]+idx*3;
  address[0]=col.getRed();
  address[1]=col.getGreen();
  address[2]=col.getBlue();
}

RGBColor IffCMAPChunk::getColor(int idx) {
  checkColorIndex(idx);
  UBYTE* address=&data[0]+idx*3;
  return RGBColor(address[0],
                  address[1],
                  address[2]
                  );
}

IffCMAPChunk::IffCMAPChunk() {
  name="CMAP";
}

string IffCMAPChunk::indent() {
  return "    ";
}

string IffCMAPChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]";
  ss<<" colors: "<<numberOfColors();
  ss<<endl;
  return ss.str();
}

string IffCMAPChunk::toDetailedString() {
  string s=toString();
  return s+indent()+"Palette: "+paletteToString()+"\n";
}
  
string IffCMAPChunk::paletteToString() {
  stringstream ss;
  for(int i=0;i<numberOfColors();i++) {
    if(i!=0)
      ss<<" ";
    RGBColor c=getColor(i);
    ss<<i<<":"<<c.toHexString();
  }
  return ss.str();
}

int IffCMAPChunk::numberOfColors() {
  assert(data.size()%3==0);
  return (int)(data.size()/3);
}
