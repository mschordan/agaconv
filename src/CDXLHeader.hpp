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

#ifndef CDXL_HEADER_HPP
#define CDXL_HEADER_HPP

#include "CDXLBlock.hpp"
#include "IffBMHDChunk.hpp"
#include "IffCAMGChunk.hpp"
#include "IffCMAPChunk.hpp"

namespace AGAConv {

/* Known working combinations with AnimFx:
   - RGB, BIT_PLANAR, Palette: 256 colors (512 bytes)
   - HAM, BIT_PLANAR, Palette: 16 colors (32 bytes)
   ? RGB, CHUNKY, Palette: 256 colors (not tested)

   Notes:
   Audio data must be WORD aligned.
 */

enum CDXLFileType { CUSTOM=0, STANDARD=1, SPECIAL=2};
enum CDXLVideoEncoding { RGB=0, HAM=1, YUV=2, AVM_DCTV=3 };
enum CDXLPlaneArrangement
  { 
   BIT_PLANAR=0,  // Currently only supported mode
   BYTE_PLANAR=1, // Not supported by AnimFx
   CHUNKY=2,      // Cannot be combined with HAM
   BIT_LINE=4,    // Not supported by AnimFx
   BYTE_LINE=6,   // Not supported by AnimFx
   PA_UNDEFINED=7
  };

enum CDXLSoundMode { MONO=0, STEREO=1 };

struct CDXLInfo {
  UBYTE planeArrangement:4, stereo:1, encoding:3;
  std::string encodingToString();
  std::string stereoToString();
  std::string planeArrangementToString();
  std::string toString();
  UBYTE getUBYTE();
};

struct CDXLGfxModes {
  std::string colorDepthFlagToString();
  std::string killEHBFlagToString();
  std::string resolutionModesToString();
  UBYTE getUBYTE();
  UBYTE reserved:2,killEHBFlag:1,colorDepthFlag:1,resolutionModes:4;
  std::string toString();
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
  std::string fileTypeToString();
  void setPreviousChunkSize(ULONG size);
  ULONG getPreviousFrameSize();
  void setCurrentChunkSize(ULONG size);
  ULONG getCurrentFrameSize();

  void setPlaneArrangement(CDXLPlaneArrangement planeArrangement);
  void setEncoding(CDXLVideoEncoding encoding);
  void setSoundMode(CDXLSoundMode mode);
  CDXLSoundMode getSoundMode();
  void setFrameNr(ULONG nr);
  ULONG getFrameNr();
  UWORD getNumberOfBitplanes();
  void setNumberOfBitplanes(UWORD);
  
  UWORD getPaletteSize();
  void setPaletteSize(UWORD);
  void setChannelAudioSize(ULONG size);
  UWORD getChannelAudioSize();
  UWORD getTotalAudioSize();
  ULONG getVideoSize();

  UWORD getVideoWidth();
  UWORD getVideoHeight();

  ULONG getCurrentFrameNr();
  UWORD getFrequency();
  void setFrequency(UWORD frequency);
  void setFps(UBYTE fps);
  void setResolutionModes(UBYTE modes);
  void setColorBitsFlag(bool flag);
  bool getColorBitsFlag();
  void setKillEHBFlag(bool flag);
  bool getKillEHBFlag();
  void setPaddingModes(UBYTE modes);
  UBYTE getPaddingModes();
  ULONG getPaddingSize();
  std::string paddingModesToString();
  void setPaddingSize(ULONG);
  bool isConsistent();
  ULONG getColorPaddingBytes(); // Computed, not extracted from padding field
  ULONG getAudioPaddingBytes(); // Computed, not extracted from padding field
  ULONG getVideoPaddingBytes(); // Computed, not extracted from padding field
  ULONG getTotalPaddingBytes(); // Computed, not extracted from padding field

  UWORD getNumberOfColors();    // Computed based on colorBytes and paletteSize.
  UWORD getColorBytes();        // 2 or 3

protected:
  UBYTE fileType=STANDARD;      // CDXLFileType
  CDXLInfo info;                // Initialized in constructor (bitfields)
  ULONG currentChunkSize=0;     // In bytes
  ULONG previousChunkSize=0;    // In bytes
  ULONG currentFrameNumber=1;   // Starting at 1
  UWORD videoWidth=0;           // In pixel
  UWORD videoHeight=0;          // In pixel
  UWORD numberOfBitplanes=0;
  UWORD paletteSize=0;          // In bytes (colors*2 or colors*3)
  UWORD channelAudioSize=0;     // In bytes
  UWORD frequency=0;            // In Hz (agablaster extension)
  UBYTE fps=0;
 public:
  CDXLGfxModes modes;           // Initialized in constructor (bitfields)
 protected:
  UBYTE paddingModes=0;         // Extra field
  UWORD padding=0;
  UWORD reserved3=0;
};

} // namespace AGAConv

#endif
