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

#ifndef IFF_ANIM_FORM_H
#define IFF_ANIM_FORM_H

#include "IffChunk.hpp"
#include "IffILBMChunk.hpp"
#include <vector>

using namespace std;

class IffANIMForm : public IffChunk {
 public:
  IffANIMForm();
  virtual ~IffANIMForm();
  void readChunk();
  void writeChunk();
  string toString();
  size_t numberOfChunks();
  string indent();
  IffChunk* getFirstChunk();
  typedef vector<IffChunk*> ChunkListType;
  IffILBMChunk* getChunk(int i); // i is chunk number starting at 0.
  void addChunk(IffILBMChunk*);
 private:
  bool endOfFile();
  string readName();
  ChunkListType chunkList;
};

#endif
