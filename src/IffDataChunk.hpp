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

#ifndef IFF_DATA_CHUNK_H
#define IFF_DATA_CHUNK_H

#include "IffChunk.hpp"

#include <vector>

using namespace std;

typedef vector<UBYTE>::iterator IffDataChunkIterator;

 class IffDataChunk : public IffChunk {
 public:
   IffDataChunk();
   void readChunk();
   void writeChunk();
   string toString();
   string indent();
   void removeData();
   void add(UBYTE byte);
   IffDataChunkIterator begin();
   IffDataChunkIterator end();
   //size_t getDataSize();
 protected:
  void readData(ULONG dataSize);
  void writeData();
  vector<UBYTE> data;
 };

#endif
