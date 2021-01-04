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

#ifndef STAGE_ANIM_EDIT_H
#define STAGE_ANIM_EDIT_H

#include "Stage.hpp"

class StageAnimEdit : public Stage {
 public:
  StageAnimEdit();
  // uses options: inFile,outFile,injectDPANChunk,sndFile when
  // inserting DPAN chunk: uses frequency/fps and sets playrate if not specified.  
  // sets inFile and outFile in IffANIMForm
  void run(Options& options);
};

#endif
