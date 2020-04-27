/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019, 2020 Markus Schordan

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

#ifndef IFF_BODY_CHUNK_H
#define IFF_BODY_CHUNK_H

#include "IffDataChunk.hpp"
#include "RGBColor.hpp"
#include "ByteSequence.hpp"
#include <vector>

using namespace std;

class IffBODYChunk : public IffDataChunk {
 public:
  IffBODYChunk();
  ~IffBODYChunk();
  RGBColor get24BitColor(int pixelOffset);
  void uncompress(int width, int height, int bpp, int mask);
  int compressedLength();
  int uncompressedLength();
  ByteSequence* getUncompressedData();
  void duplicateDataVector(); // TODO: temporary
 private:
  // should be ByteSequence
  char* dataCompressed=nullptr;
  // should be ByteSequence
  char* dataUncompressed=nullptr;
  // should be ByteSequence
  char* chunky24BitData=nullptr;
  size_t dataCompressedSize;
  size_t dataUncompressedSize;
};

#endif
