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

#ifndef IFF_SXHD_CHUNK_H
#define IFF_SXHD_CHUNK_H

#include "IffChunk.hpp"

class IffSXHDChunk : public IffChunk {
 public:
  IffSXHDChunk();
  IffSXHDChunk(UBYTE sampleDepth, ULONG lengthPerFrame, ULONG playRate, UBYTE usedChannels, UBYTE usedMode, ULONG playFreq);
  void readChunk();
  void writeChunk();
  string toString();
  string indent();
 private:
  UBYTE sampleDepth;
  UBYTE fixedVolume;
  ULONG length; // length of sound data for one channel per frame
  ULONG playRate; // set to 0 if playFreq is used
  ULONG compressionMethod; // =0
  UBYTE usedChannels; // bit 0: left channel, bit 1: right channel, bit 2: center channels
  UBYTE usedMode; // bit 0: mono (center channel only), bit 1: stereo (left+right)
  ULONG playFreq; // sample freqeuency, specifies correct playspeed
  UWORD loop; // number of loops
};

#endif
