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

#ifndef RGB_COLOR_HPP
#define RGB_COLOR_HPP

#include <string>
#include "AmigaTypeDefs.hpp"
#include "IffDataChunk.hpp"

namespace AGAConv {

// Represents 24 bit color
class RGBColor {
 public:
  // Reads 3 color bytes from BYTE stream in order RGB
  //  RGBColor(IffDataChunkIterator ptr);
  // Default constructor for default initializer in containers
  RGBColor() = default;
  // Color from three BYTE values
  RGBColor(UBYTE red, UBYTE green, UBYTE blue);
  // Color from a LONG value xxRRGGBB
  RGBColor(ULONG col);
  // RGB color as 4 bytes: 00RRGGBB.
  ULONG getULONG();
  UBYTE getRed();
  UBYTE getGreen();
  UBYTE getBlue();
  // RGB Color converted to 12 Bit color 0RGB..
  UWORD get12BitColor();
  void setRed(UBYTE);
  void setGreen(UBYTE);
  void setBlue(UBYTE);
  std::string toHexString();
  static UBYTE convert8BitTo4Bit(UBYTE col);
  static UBYTE convert4BitTo8Bit(UBYTE col);
 private:
  std::string toHexString(UBYTE);
  UBYTE _red=0;
  UBYTE _green=0;
  UBYTE _blue=0;
};

} // namespace AGAConv

#endif
