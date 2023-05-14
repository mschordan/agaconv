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

#include "RGBColor.hpp"

#include <iostream>
#include <sstream>

using namespace std;

namespace AGAConv {

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
  // First map to XX then shift (mapping only relevant for HAM8/24 to HAM8/12 conversion)
  if(col<=8) col=0;
  else if(col>=9&&col<=25) col=17;
  else if(col>=26&&col<=42) col=34;
  else if(col>=43&&col<=59) col=51;
  else if(col>=60&&col<=76) col=68;
  else if(col>=77&&col<=93) col=85;
  else if(col>=94&&col<=110) col=102;
  else if(col>=111&&col<=127) col=119;
  else if(col>=128&&col<=144) col=136;
  else if(col>=145&&col<=161) col=153;
  else if(col>=162&&col<=178) col=170;
  else if(col>=179&&col<=195) col=187;
  else if(col>=196&&col<=212) col=204;
  else if(col>=213&&col<=229) col=221;
  else if(col>=230&&col<=246) col=238;
  else if(col>=247) col=255;
  return col>>4;
}

UBYTE RGBColor::convert4BitTo8Bit(UBYTE col) {
  // Duplicate bits3-0 => 7-4/3-0
  return (col<<4)|col;
}

} // namespace AGAConv
