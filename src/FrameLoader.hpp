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

#ifndef FRAME_LOADER_H
#define FRAME_LOADER_H

#include <string>

class FrameLoader {

 public:
  FrameLoader();
  virtual ~FrameLoader();
  // called before first frame is read. Can be used for initialization.
  virtual void readFile(std::string FileName)=0;
  // for processing each ILBM chunk (frame)
  //virtual void visitILBMChunk(IffILBMChunk*);
  // for printing infos after processing
  //virtual void postVisitLastILBMChunk(IffILBMChunk* ilbmChunk);

};

#endif
