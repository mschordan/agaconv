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

#ifndef IFF_DPAN_CHUNK_H
#define IFF_DPAN_CHUNK_H

#include "IffChunk.hpp"

class IffDPANChunk : public IffChunk {
 public:
  IffDPANChunk();
  IffDPANChunk(UWORD nframes);
  void readChunk();
  void writeChunk();
  string toString();
  string indent();
  void setFrames(UWORD nframes);
  UWORD getFrames();
 private:
  void init();
   UWORD version;   /* current version=4 */
   UWORD nframes;   /* number of frames in the animation.*/
   ULONG flags;     /* Not used */
};

#endif
