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

#ifndef ILBM_CHUNK_HPP
#define ILBM_CHUNK_HPP

#include <cstdint>
#include <list>
#include <string>

#include "IffBMHDChunk.hpp"
#include "IffBODYChunk.hpp"
#include "IffCAMGChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffChunk.hpp"

namespace AGAConv {

class IffILBMChunk : public IffChunk {
 public:
  IffILBMChunk();
  ~IffILBMChunk();
  void readChunk();
  void writeChunk();
  std::string toString();
  virtual std::string toDetailedString();
  uint16_t numberOfChunks();
  std::string indent();
  void insertFirst(IffChunk* chunk); // Inserts chunk as first chunk
  void insertLast(IffChunk* chunk);  // Inserts chunk as last chunk
  bool insertBeforeChunk(IffChunk* chunk, std::string chunkName);
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

private:
  std::string readName();
  typedef std::list<IffChunk*> ChunkListType;
  ChunkListType chunkList;
  uint32_t animStartPos;
};

} // namespace AGAConv

#endif
