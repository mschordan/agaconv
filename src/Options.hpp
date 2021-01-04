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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <cstdint>

enum AudioDataType {
  AUDIO_DATA_TYPE_UNDEFINED,
  AUDIO_DATA_TYPE_UBYTE,
  AUDIO_DATA_TYPE_SBYTE,
  AUDIO_DATA_TYPE_UWORD,
  AUDIO_DATA_TYPE_SWORD
};

struct Options {
  Options();
  bool status=false;
  //  bool info=false;
  bool quiet=false;
  bool cdxlInfo=false;
  bool cdxlEncode=false;
  bool cdxlDecode=false;
  bool ilbmInfo=false;
  bool detailed=false;
  //bool frameInfo=false;
  bool chunkInfo=false;
  bool firstChunkInfo=false;
  bool injectDPANChunk=false;
  bool paddingFix=true;
  bool readAnim=false;
  bool readCdxl=false;
  bool readIlbm=false;
  bool writeAnim=false;
  bool writeCdxl=false;
  bool writeIlbm=false;
  bool stereo=false;
  enum COLOR_DEPTH { COL_12BIT, COL_24BIT } colorSize=COL_12BIT;
  long fps=0;
  long playRate=0;
  char* inFileName=nullptr;
  char* outFileName=nullptr;
  char* sndFileName=nullptr;
  AudioDataType audioDataType=AUDIO_DATA_TYPE_UNDEFINED;
  long frequency=0;
  bool hasInFile();
  bool hasOutFile();
  bool hasSndFile();
  bool checkConsistency();
  int systemTimingConstant=3546895; // PAL system
  enum GFX_RESOLUTION { GFX_UNSPECIFIED=0, GFX_LORES=1, GFX_HIRES=2, GFX_SUPERHIRES=3, GFX_ULTRAHIRES=4 };
  GFX_RESOLUTION resMode=GFX_LORES;
  enum PADDING_TYPE { PAD_UNSPECIFIED=0, PAD_NONE=1, PAD_16BIT=2, PAD_32BIT=3, PAD_64BIT=4 };
  PADDING_TYPE paddingMode;
  bool debug=false;
  long paddingSize=2;
  bool optimize=true; // eliminates empty bitplanes, remaps colors
  bool stdCdxl=false;
  uint32_t fixedPlanes=0; // if different to 0, then fixed number of planes is requested
};

// use options object globally;
extern Options options;

#endif
