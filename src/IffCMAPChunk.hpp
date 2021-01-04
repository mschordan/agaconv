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

#ifndef IFF_CMAP_CHUNK_H
#define IFF_CMAP_CHUNK_H

#include "IffDataChunk.hpp"
#include "RGBColor.hpp"
#include <string>
#include <vector>

using namespace std;

class IffCMAPChunk : public IffDataChunk {
 public:
  IffCMAPChunk();
  string toString();
  string toDetailedString();
  string indent();
  void addColor(RGBColor col);
  // the index of the color is 0..num-1 (corresponding to the Amiga
  // register number)
  RGBColor getColor(int idx);
  void setColor(int idx, RGBColor col);
  int numberOfColors();
   // clears all colors and redefines size. Initializes all colors
   // with 000000.
  void reserveNumColors(int num);
  void checkColorIndex(int idx);
  std::string paletteToString();
 protected:
};

#endif
