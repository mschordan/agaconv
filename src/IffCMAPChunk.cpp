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

#include <cassert>
#include <iostream>
#include <sstream>

#include "AGAConvException.hpp"
#include "IffCMAPChunk.hpp"

using namespace std;

namespace AGAConv {

void IffCMAPChunk::addColor(RGBColor col) {
  add(col.getRed());
  add(col.getGreen());
  add(col.getBlue());
}

void IffCMAPChunk::reserveNumColors(uint32_t num) {
  removeData();
  uint32_t numBytes=num*3;
  for(uint32_t i=0;i<numBytes;i++) {
    add(0);
  }
}

void IffCMAPChunk::checkColorIndex(uint32_t idx) {
  if(idx>=numberOfColors()) {
    throw AGAConvException(144, "IffCMAPChunk::setColor out of bounds in CMAP chunk.");
  }
}

void IffCMAPChunk::setColor(uint32_t idx, RGBColor col) {
  checkColorIndex(idx);
  UBYTE* address=&data[0]+idx*3;
  address[0]=col.getRed();
  address[1]=col.getGreen();
  address[2]=col.getBlue();
}

RGBColor IffCMAPChunk::getColor(uint32_t idx) {
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
  for(uint32_t i=0;i<numberOfColors();i++) {
    if(i!=0)
      ss<<" ";
    RGBColor c=getColor(i);
    ss<<i<<":"<<c.toHexString();
  }
  return ss.str();
}

uint32_t IffCMAPChunk::numberOfColors() {
  assert(data.size()%3==0);
  return (uint32_t)(data.size()/3);
}

} // namespace AGAConv
