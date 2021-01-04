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

#include "CDXLPalette.hpp"
#include <sstream>
#include <cassert>
#include <iostream>
#include "Options.hpp"
using namespace std;

// TODO: this only works for 12Bit CDXL files
void CDXLPalette::readChunk() {
  ULONG length=getLength();
  assert(length!=0);
  int numColors=0;
  switch(_colorMode) {
  case COL_12BIT:
    if(Chunk::debug) cout<<"DEBUG: readChunk: color mode: 12 bit"<<endl;
    numColors=length/2;
    for(int i=0;i<numColors;i++) {
      UWORD uword=readUWORD();
      CDXLColorType color=uword;
      addColor(color);
    }
    break;
  case COL_24BIT:
    if(Chunk::debug) cout<<"DEBUG: readChunk: color mode: 24 bit"<<endl;
    numColors=length/3;
    for(int i=0;i<numColors;i++) {
      UBYTE r=readUBYTE();
      UBYTE g=readUBYTE();
      UBYTE b=readUBYTE();
      addColor(RGBColor(r,g,b));
    }
    break;
  default:
    cerr<<"Unsupported color mode in reading palette chunk. Mode: "<<_colorMode<<endl;
    exit(1);
  }
}

void CDXLPalette::writeChunk() {
  if(Chunk::debug) cout<<"DEBUG: writeChunk: writing palette with "<<rgbColors.size()<<" colors."<<endl;
  switch(_colorMode) {
  case COL_12BIT:
    for (auto rgbCol : rgbColors) {
      UWORD word12BitColor=rgbCol.get12BitColor();
      writeUWord(word12BitColor);
      //cout<<rgbCol.toHexString()<<":"<<hex<<word12BitColor<<dec<<" ";
    }
    //cout<<endl;
    break;
  case COL_24BIT:
    for (auto rgbCol : rgbColors) {
      writeUBYTE(rgbCol.getRed());
      writeUBYTE(rgbCol.getGreen());
      writeUBYTE(rgbCol.getBlue());
    }
    break;
  default:
    cerr<<"Unsupported color mode in palette generation. Mode: "<<_colorMode<<endl;
    exit(1);
  }
}

string CDXLPalette::toString() {
  stringstream ss;
  int i=0;
  for (auto color : rgbColors) {
    ss<<i<<":";
    ss<<color.toHexString()<<" ";
    i++;
  }
  return ss.str();
}

void CDXLPalette::addColor(RGBColor col) {
  addColor(col.getRed(),col.getGreen(),col.getBlue());
}

void CDXLPalette::addColor(UBYTE red, UBYTE green, UBYTE blue) {
  // colors are always stored as 24bit RGB colors (but not in file if not selected)
  switch(_colorMode) {
  case COL_12BIT:
  case COL_24BIT: 
    rgbColors.push_back(RGBColor(red,green,blue));
    break;
  default:
    cerr<<"Error: addColor: unknown palette color mode: "<<_colorMode<<endl;
    exit(1);
  }
}

// this method is only relevant when *reading* 12 bit palette and is not supported yet
void CDXLPalette::addColor(CDXLColorType color) {
  cerr<<"Error: adding 12Bit colors to palette not supported."<<endl;
  exit(1);
}

void CDXLPalette::importColors(IffCMAPChunk* cmap) {
  int num=cmap->numberOfColors();
  for(int i=0;i<num;i++) {
    addColor(RGBColor(cmap->getColor(i)));
  }
}

void CDXLPalette::setColorMode(CDXLPalette::COLOR_DEPTH colorMode) {
  _colorMode=colorMode;
}

CDXLPalette::COLOR_DEPTH CDXLPalette::getColorMode() {
  return _colorMode;
}

size_t CDXLPalette::numberOfColors() {
  return rgbColors.size();
}

CDXLColorType CDXLPalette::get12BitColor(UWORD ColorNr) {
  assert(ColorNr<rgbColors.size());
  return rgbColors[ColorNr].get12BitColor();
}

ULONG CDXLPalette::getLength() {
  if(dataSize!=0) {
    return dataSize;
  } else {
    switch(_colorMode) {
    case COL_12BIT:
      return rgbColors.size()*sizeof(CDXLColorType);
    case COL_24BIT:
      return rgbColors.size()*3;
    default:
      cerr<<"Error: unknown palette color mode: "<<_colorMode<<endl;
      exit(1);
    }
  }
}
