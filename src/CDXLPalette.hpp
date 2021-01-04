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

#ifndef CDXL_PALETTE_H
#define CDXL_PALETTE_H

#include "CDXLBlock.hpp"
#include "IffCMAPChunk.hpp"
#include "RGBColor.hpp"

typedef UWORD CDXLColorType;

class CDXLPalette : public CDXLBlock {
  /*
    CDXL 12Bit Palette := UWORD * NumberOfColors
    Bit 0-3  : Red
    Bit 4-7  : Green
    Bit 8-11 : Blue
    Bit 12-16: unused
    CDXL 24Bit Palette is the same as CMAP (3-byte values)
  */
  /* colors are always maintained as RGB colors. CDXL 12bit colors are
     supported through setting the color mode. The color mode only
     impacts the functions 'length' and when generating the color
     palette with the overridden function 'writeChunk'. ReadChunk
     converts 12Bit values to 24Bit values. The conversion from 24bit
     to 12bit is an inverse function and therefore no information is
     lost even when reading in 12bit color palettes from a CDXL file
     and generating a 12bit CDXL file (e.g. to remove padding bytes).
  */
 public:
  enum COLOR_DEPTH { COL_12BIT, COL_24BIT };
  void readChunk();
  void writeChunk();
  std::string toString();
  void addColor(UBYTE red, UBYTE green, UBYTE blue);
  void addColor(RGBColor color);
  void addColor(CDXLColorType color);
  CDXLColorType get12BitColor(UWORD colorNr);
  // independent of color mode
  size_t numberOfColors();
  // uses color mode to compute data length.
  ULONG getLength();
  // imports RGB colors from CMAP chunk.
  void importColors(IffCMAPChunk* cmap);

  void setColorMode(CDXLPalette::COLOR_DEPTH colorMode);
  CDXLPalette::COLOR_DEPTH getColorMode();
 private:
  std::vector<RGBColor> rgbColors;
  COLOR_DEPTH _colorMode=COL_12BIT;
};

#endif
