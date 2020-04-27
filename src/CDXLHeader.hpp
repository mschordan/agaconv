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

#ifndef CDXL_HEADER_H
#define CDXL_HEADER_H

#include "CDXLBlock.hpp"
#include "IffBMHDChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffCAMGChunk.hpp"

/* known working combinations with AnimFx:
   - RGB, BIT_PLANAR, Palette: 256 colors (512 bytes)
   - HAM, BIT_PLANAR, Palette: 16 colors (32 bytes)
   ? RGB, CHUNKY, Palette: 256 colors (not tested)

   Notes:
   audio data must be WORD aligned.
 */

enum CDXLFileType { CUSTOM=0, STANDARD=1, SPECIAL=2};
enum CDXLVideoEncoding { RGB=0, HAM=1, YUV=2, AVM_DCTV=3 };
enum CDXLPlaneArrangement { 
  BIT_PLANAR=0, // currently only supported mode
  BYTE_PLANAR=1, // not supported by AnimFx
  CHUNKY=2, // cannot be combined with HAM
  BIT_LINE=4, // not supported by AnimFx
  BYTE_LINE=6, // not supported by AnimFx
  PA_UNDEFINED=7
};

enum CDXLSoundMode { MONO=0, STEREO=1 };

struct CDXLInfo {
  UBYTE planeArrangement:4, stereo:1, encoding:3;
  string encodingToString();
  string stereoToString();
  string planeArrangementToString();
  string toString();
  UBYTE getUBYTE();
};

struct CDXLGfxModes {
  string colorBitsFlagToString();
  UBYTE getUBYTE();
  UBYTE paddingModes:3,colorBitsFlag:1,resolutionModes:4;
  string toString();
};

class CDXLHeader : public CDXLBlock {
 public:
  CDXLHeader();
  CDXLHeader(IffBMHDChunk* bmhdChunk, IffCMAPChunk* cmap, IffCAMGChunk* camg);
  void initialize(IffBMHDChunk* bmhdChunk, IffCMAPChunk* cmap, IffCAMGChunk* camg);
  void readChunk();
  void writeChunk();
  std::string toString();
  ULONG getLength();
  void setFileType(CDXLFileType fileType);
  string fileTypeToString();
  void setPreviousChunkSize(ULONG size);
  ULONG getPreviousFrameSize();
  void setCurrentChunkSize(ULONG size);
  ULONG getCurrentFrameSize();

  void setPlaneArrangement(CDXLPlaneArrangement planeArrangement);
  void setEncoding(CDXLVideoEncoding encoding);
  void setSoundMode(CDXLSoundMode mode);
  CDXLSoundMode getSoundMode();
  void setFrameNr(UWORD nr);
  UWORD getNumberOfBitplanes();
  
  UWORD getPaletteSize();
  void setChannelAudioSize(ULONG size);
  UWORD getChannelAudioSize();
  UWORD getTotalAudioSize();
  ULONG getVideoSize();

  UWORD getVideoWidth();
  UWORD getVideoHeight();

  UWORD getCurrentFrameNr();
  UWORD getFrequency();
  void setFrequency(UWORD frequency);
  void setFps(UBYTE fps);
  void setResolutionModes(UBYTE modes);
  void setColorBitsFlag(bool flag);
  bool getColorBitsFlag();
  void setPaddingModes(UBYTE modes);
  UBYTE getPaddingModes();
  ULONG getPaddingSize();
  void setPaddingSize(ULONG);
  bool isConsistent();
  ULONG getColorPaddingBytes();
  ULONG getAudioPaddingBytes();
  ULONG getVideoPaddingBytes();
  ULONG getTotalPaddingBytes();
 protected:
  UBYTE fileType=STANDARD; // CDXLFileType
  CDXLInfo info; // BYTE // TODO:initialize here
  ULONG currentChunkSize=0; // in bytes
  ULONG previousChunkSize=0; // in bytes
  UWORD reserved1=0; // upper word of FrameNumber
  UWORD currentFrameNumber=1; // starting at 1
  UWORD videoWidth=0; // in pixel
  UWORD videoHeight=0; // in pixel
  UWORD numberOfBitplanes=0;
  UWORD paletteSize=0; // in bytes (colors*2 or colors*3)
  UWORD channelAudioSize=0; // in bytes
  UWORD frequency=0; // in Hz (agablaster extension)
  //UWORD reserved2=0;
  UBYTE fps=0;
 public:
  CDXLGfxModes modes; // TODO: initialize here
 protected:
  UWORD padding=0;
  UWORD reserved3=0;
};

#endif
