/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2024 Markus Schordan

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

#include "IffBMHDChunk.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

namespace AGAConv {

void IffBMHDChunk::readChunk() {
  readChunkSize();
  width=readUWORD();
  height=readUWORD();
  xOrigin=readSWORD();
  yOrigin=readSWORD();
  numPlanes=readUBYTE();
  mask=readUBYTE();
  compression=readUBYTE();
  pad1=readUBYTE();
  transClr=readUWORD();
  xAspect=readUBYTE();
  yAspect=readUBYTE();
  pageWidth=readSWORD();
  pageHeight=readSWORD();
}

void IffBMHDChunk::writeChunk() {
  writeChunkNameAndSize();
  writeUWORD(width);
  writeUWORD(height);
  writeSWORD(xOrigin);
  writeSWORD(yOrigin);
  writeUBYTE(numPlanes);
  writeUBYTE(mask);
  writeUBYTE(compression);
  writeUBYTE(pad1);
  writeUWORD(transClr);
  writeUBYTE(xAspect);
  writeUBYTE(yAspect);
  writeSWORD(pageWidth);
  writeSWORD(pageHeight);
}

IffBMHDChunk::IffBMHDChunk():
  width(0),
  height(0),
  xOrigin(0),
  yOrigin(0),
  numPlanes(0),
  mask(0),
  compression(0),
  pad1(0),
  transClr(0),
  xAspect(0),
  yAspect(0),
  pageWidth(0),
  pageHeight(0)
{
  name="BMHD";
  dataSize=20;
}

IffBMHDChunk::IffBMHDChunk(UWORD width, UWORD height, UBYTE numPlanes):
  IffBMHDChunk()
{
  this->width=this->pageWidth=width;
  this->height=this->pageHeight=height;
  this->numPlanes=numPlanes;
}

string IffBMHDChunk::indent() {
  return "    ";
}

string IffBMHDChunk::toString() {
  stringstream ss;
  ss<<indent()<<getName()<<" ["<<getDataSize()<<"]"<<endl;
  if(getLongToString()) {
    const uint16_t colWidth=15;
    // '+' converts unsigned char to unsigned int, otherwise it would be printed as char.
    ss<<indent()<<setw(colWidth)<<left<<"Width: "<<width<<endl; 
    ss<<indent()<<setw(colWidth)<<left<<"Height: "<<height<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"XOrigin: "<<xOrigin<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"YOrigin: "<<yOrigin<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Num Planes: "<<+numPlanes<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Mask: "<<+mask<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Compression: "<<+compression<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Pad1: "<<+pad1 <<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Transp. col: "<<transClr<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"XAspect: "<< xAspect<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"YAspect: "<< yAspect<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Page width: "<<pageWidth<<endl;
    ss<<indent()<<setw(colWidth)<<left<<"Page height: "<<pageHeight<<endl;
    ss<<endl;
  }
  return ss.str();
}

} // namespace AGAConv
