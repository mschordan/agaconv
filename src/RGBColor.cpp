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

#include "RGBColor.hpp"
#include <sstream>
#include <iostream>

using namespace std;

RGBColor::RGBColor() {
}
 
RGBColor::RGBColor(ULONG raw) {
  _red=(UBYTE)(raw>>16);
  _green=(UBYTE)(raw>>8);
  _blue=(UBYTE)(raw);
}

RGBColor::RGBColor(UBYTE red, UBYTE green, UBYTE blue) {
  _red=red;
  _green=green;
  _blue=blue;
}

ULONG RGBColor::getULONG() {
  return ((ULONG) _red)<<16|((ULONG)_green)<<8|((ULONG)_blue);
}

UWORD RGBColor::get12BitColor() {
  UBYTE red4Bit=convert8BitTo4Bit(getRed());
  UBYTE green4Bit=convert8BitTo4Bit(getGreen());
  UBYTE blue4Bit=convert8BitTo4Bit(getBlue());
  return ((UWORD) red4Bit)<<8|((UWORD)green4Bit)<<4|((UWORD)blue4Bit);
}

UBYTE RGBColor::getRed() {
  return _red;
}

UBYTE RGBColor::getGreen() {
  return _green;
}

UBYTE RGBColor::getBlue() {
  return _blue;
}

void RGBColor::setRed(UBYTE color) {
  _red=color;
}

void RGBColor::setGreen(UBYTE color) {
  _green=color;
}

void RGBColor::setBlue(UBYTE color) {
  _blue=color;
}

string RGBColor::toHexString(UBYTE byte) {
  stringstream ss;
  if(byte<=15)
    ss<<"0";
  ss<<std::hex<<+byte;
  return ss.str();
}

string RGBColor::toHexString() {
  return toHexString(getRed())
    +toHexString(getGreen())
    +toHexString(getBlue());
}

UBYTE RGBColor::convert8BitTo4Bit(UBYTE col) {
  return ((UWORD)col)>>4;
}

UBYTE RGBColor::convert4BitTo8Bit(UBYTE col) {
  return ((UWORD) col)<<4;
}
