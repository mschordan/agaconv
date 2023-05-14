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

#include "IffANHDChunk.hpp"
#include <sstream>

using namespace std;

namespace AGAConv {

void IffANHDChunk::readChunk() {
  readChunkSize();
  operation=readUBYTE();
  mask=readUBYTE();
  w=readUWORD();
  h=readUWORD();
  x=readUWORD();
  y=readUWORD();
  absTime=readULONG();
  relTime=readULONG();
  interleave=readUBYTE();
  pad1=readUBYTE();
  bits=readULONG();
  for(int i: pad2) {
    pad2[i]=readUBYTE();
  }
}

void IffANHDChunk::writeChunk() {
  writeChunkNameAndSize();
  writeUByte(operation);
  writeUByte(mask);
  writeUWord(w);
  writeUWord(h);
  writeUWord(x);
  writeUWord(y);
  writeULong(absTime);
  writeULong(relTime);
  writeUByte(interleave);
  writeUByte(pad1);
  writeULong(bits);
  for(int i: pad2) {
    writeUByte(i);
  }
}

IffANHDChunk::IffANHDChunk():
  operation(ILBM_BODY),
  mask(0),
  w(0),h(0),
  x(0),y(0),
  absTime(0),
  relTime(0),
  interleave(0),
  pad1(0), // Not used
  bits(0),
  pad2{}   // Not used (initializes to 16 zero bytes)
{
  name="ANHD";
}

string IffANHDChunk::indent() {
  return "    ";
}
string IffANHDChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]"<<endl;
  if(getLongToString()) {
    // '+' converts unsigned char to unsigned int, otherwise it is printed as char.
    ss<<indent()<<"Operation: "<<+operation<<endl; 
    ss<<indent()<<"Mask: "<<+mask<<endl;
    ss<<indent()<<"W: "<<w<<endl;
    ss<<indent()<<"H: "<<h<<endl;
    ss<<indent()<<"X: "<<x<<endl;
    ss<<indent()<<"Y: "<<y<<endl;
    ss<<indent()<<"AbsTime: "<<absTime<<endl;
    ss<<indent()<<"RelTime: "<<relTime<<endl;
    ss<<indent()<<"Interleave: "<<+interleave<<endl;
    ss<<indent()<<"Pad: "<<+pad1<<endl;
    ss<<indent()<<"Bits: "<<bits<<endl;
    ss<<indent()<<"Pad: ";
    for(int i: pad2) {
      if(i!=0)
        ss<<",";
      ss<<i;
    }
    ss<<endl;
  }
  return ss.str();
}

} // namespace AGAConv
