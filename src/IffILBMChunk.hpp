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

#ifndef ILBM_CHUNK_H
#define ILBM_CHUNK_H

#include "IffChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffBMHDChunk.hpp"
#include "IffCAMGChunk.hpp"
#include "IffBODYChunk.hpp"
#include <list>
#include <cstdint>
#include <string>

using namespace std;

class IffILBMChunk : public IffChunk {
 public:
  IffILBMChunk();
  ~IffILBMChunk();
  void readChunk();
  void writeChunk();
  string toString();
  virtual string toDetailedString();
  uint16_t numberOfChunks();
  string indent();
  void insertFirst(IffChunk* chunk); // inserts chunk as first chunk
  void insertLast(IffChunk* chunk); // inserts chunk as last chunk
  bool insertBeforeChunk(IffChunk* chunk, string chunkName);
  bool insertBeforeBODYChunk(IffChunk* chunk);
  void setAnimStartPos(uint32_t animStartPos);
  uint32_t getAnimStartPos();
  bool hasBMHDChunk();
  bool hasCAMGChunk();
  bool hasCMAPChunk();
  bool hasBODYChunk();
  int uncompressedBODYLength();
  int compressedBODYLength();
  bool uncompressBODYChunk();
  IffChunk* getChunkByName(std::string name);
  IffBMHDChunk* getBMHDChunk();
  IffCAMGChunk* getCAMGChunk();
  IffCMAPChunk* getCMAPChunk();
  IffBODYChunk* getBODYChunk();
  /*
    int getIndexValue(int x, int y);
    void setIndexValue(int x, int y, int IndexValue);
    RGBColor getColorValue(int x, int y);
    void setColorValue(int x, int y, RGBColor color);
   
    // changes every Index according to the provided mapping. Also updates color palette.
    void changeIndex(map<int,int>);
  */
 private:
  string readName();
  typedef list<IffChunk*> ChunkListType;
  ChunkListType chunkList;
  uint32_t animStartPos;
};

#endif
