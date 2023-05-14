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

#ifndef IFF_BMHD_CHUNK_HPP
#define IFF_BMHD_CHUNK_HPP

#include "IffChunk.hpp"

namespace AGAConv {

class IffBMHDChunk : public IffChunk {
 public:
  IffBMHDChunk();
  // Only sets values, does not allocate memory. Also sets page width and height.
  // the values are also used to check whether the size of added bitplanes is consistent.
  IffBMHDChunk(UWORD width, UWORD height, UBYTE numPlanes);
  void readChunk();
  void writeChunk();
  std::string toString();
  std::string indent();
  UWORD getWidth() { return width; }
  UWORD getHeight() { return height; }
  UBYTE getNumPlanes() { return numPlanes; }
  UBYTE getMask() { return mask; }
  UBYTE getCompression() { return compression; }
  void setNumPlanes(UBYTE num) { numPlanes=num; }
private:
  UWORD width; 	     // Image width in pixels
  UWORD height;      // Image height in pixels
  SWORD xOrigin;     // Image's top-left corner on screen. Value is usually (0,0) unless image is part of a larger image or not fullscreen.
  SWORD yOrigin;
  UBYTE numPlanes;   // Number of planes in bitmap (0 if there is only a colormap and no image data. (i.e. this file is just a colormap.)).
  UBYTE mask;        // 1 = masked, 2 = transparent color, 3 = lasso (for MacPaint). Mask data is not considered a bit plane.
  UBYTE compression; // 0: uncompressed, 1: RLE compressed.
  UBYTE pad1;
  UWORD transClr;    // Transparent colour, useful only when mask >= 2
  UBYTE xAspect;     // Pixel aspect, a ratio width:height; used for displaying the image on a variety of different screen resolutions.
  UBYTE yAspect;
  SWORD pageWidth;   // The size of the screen the image is to be displayed on, in pixels
  SWORD pageHeight;
};

} // namespace AGAConv

#endif
