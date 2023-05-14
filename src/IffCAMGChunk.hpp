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

#ifndef IFF_CAMG_CHUNK_HPP
#define IFF_CAMG_CHUNK_HPP

#include <string>
#include "IffChunk.hpp"

namespace AGAConv {

/*
  BPLCON
  LACE: bit 2; y>=400; 0x4
  SUPERHIRES: bit 5; 0x20; 1280
  EHB: bit 7 ; 0x80 ; CMAP has only 32 colors, but BODY has 6 bitplanes
  HAM: bit 11 ; 0x800
  HIRES: bit 15; 0x8000; x>=640
*/
#define CAMG_LACE 0x4
#define CAMG_SUPERHIRES 0x20
#define CAMG_EHB 0x80
#define CAMG_HAM 0x800
#define CAMG_HIRES 0x8000

class IffCAMGChunk : public IffChunk {
 public:
  IffCAMGChunk();
  void readChunk();
  void writeChunk();
  std::string toString();
  std::string indent();
  ULONG getViewMode();
  bool isHalfBrite();
  bool isHam();
  bool isHires();
  bool isSuperHires();
  bool isLace();
  std::string infoToString();
  void setLores();
  void setHires();
  void setSuperHires();
  void setUltraHires();
 private:
  ULONG viewMode; // viewmode bits
};

} // namespace AGAConv

#endif
