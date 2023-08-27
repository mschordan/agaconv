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

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include "Util.hpp"

namespace AGAConv {

enum AudioDataType {
  AUDIO_DATA_TYPE_UNDEFINED,
  AUDIO_DATA_TYPE_UBYTE,
  AUDIO_DATA_TYPE_SBYTE,
  AUDIO_DATA_TYPE_UWORD, // Not used
  AUDIO_DATA_TYPE_SWORD  // Not used
};

enum ColorModeEnum {CM_UNKNOWN, CM_HAM, CM_EHB, CM_AGA, CM_OCS};

const uint32_t minPlanes=2;  
const uint32_t minColors=Util::ULONGPow(2,minPlanes);
const uint32_t maxAmigaFrequency=28836;
const uint32_t maxLoresWidth=320+64;       // With overscan
const uint32_t maxHiresWidth=640+64;       // With overscan
const uint32_t maxSuperHiresWidth=1280+64; // With overscan

struct Options {
  Options();
  static const uint32_t autoValue=std::numeric_limits<uint32_t>::max();
  
  ///////////////////////////////
  // Additional script options
  ///////////////////////////////

  ColorModeEnum colorModeEnum=ColorModeEnum::CM_UNKNOWN;
  std::string colorModeEnumToString() const;
  std::string colorMode="aga8";
  std::string screenMode="auto";
  std::string adjustAspectMode="1.00";

  // Not supported but already tested in resizing calculations
  bool screenModeLace=false;
  uint32_t width=320;
  uint32_t height=autoValue;
  double yScaleFactor=1.0;
  uint8_t numPlanes=1; // Internal, set by colorMode

  std::string ditherMode="floyd_steinberg"; // floyd_steinberg|bayer:bayer_scale=2
  
  uint32_t ffDitherBayerScale=3;
  bool showVersion=false;
  bool showHelpText=false;
  bool showHelpExtraText=false;
  
  // Generate standard CDXL file with 24-bit RGB888 color palette and fixed frame size
  uint32_t ffBayerScale=4; // --ff-bayer-scale=NUMBER] 0..5 (default:4)
  std::string extractionTool="ffmpeg";
  std::string conversionTool="ffmpeg"; // + ham_convert
  uint32_t fixedFrameDigits=4; // Used with ffmpeg
  std::filesystem::path tmpDir=std::string("tmp-agaconv");
  std::string hamConvertBatchFileName="ham_convert_batch_file.txt";

  // ham_convert options
  std::filesystem::path hcPath="";
  std::filesystem::path hcPathResolved="";
  uint32_t hcHamQuality=1;
  std::string hcDither="fs";
  uint32_t hcPropagation=autoValue;
  uint32_t hcDiversity=autoValue;
  std::string hcQuant="auto"; // "wu"
  
  ///////////////////////////////////
  // Original agaconv-encode options
  ///////////////////////////////////
  uint32_t verbose=1;
  bool cdxlInfo=false;
  bool cdxlEncode=true;
  bool cdxlDecode=false;
  bool ilbmInfo=false;
  bool chunkInfo=false;
  bool firstChunkInfo=false;
  bool injectDPANChunk=false;
  bool paddingFix=true;
  bool readAnim=false;
  bool readCdxl=false;
  bool readIlbm=false;
  bool writeAnim=false;
  bool writeCdxl=true;
  bool writeIlbm=false;
  std::string audioMode="stereo";
  bool stereo=true; // audioMode
  uint32_t colorDepthBits=autoValue;
  enum COLOR_DEPTH { COL_12BIT, COL_24BIT } colorDepth=COL_24BIT;
  uint32_t fps=24;
  // Default, irrelevant for CDXL, only relevant for ANIM
  uint32_t playRate=162;
  bool installDefaultConfigFile=false;
  bool uninstallDefaultConfigFile=false;
  bool resetDefaultConfigFile=false;
  bool saveDefaultConfigFile=false;
  bool reserveBlackBackgroundColor=false;
  
  std::filesystem::path inFileName;
  std::filesystem::path outFileName;
  std::filesystem::path sndFileName="audio_track.pcm";
  std::filesystem::path inConfigFileName;
  std::filesystem::path outConfigFileName;  

  std::string audioDataTypeString="u8";
  AudioDataType audioDataType=AUDIO_DATA_TYPE_UNDEFINED;
  uint32_t frequency=28000;
  bool hasInFile() const;
  bool hasOutFile() const;
  bool hasSndFile() const;
  bool checkConsistency() const;
  uint64_t systemTimingConstant=3546895; // PAL system
  enum GFX_RESOLUTION { GFX_UNSPECIFIED=0, GFX_LORES=1, GFX_HIRES=2, GFX_SUPERHIRES=3, GFX_ULTRAHIRES=4, GFX_AUTO=5 };
  GFX_RESOLUTION resMode=GFX_AUTO;
  enum PADDING_TYPE { PAD_UNSPECIFIED=0, PAD_NONE=1, PAD_16BIT=2, PAD_32BIT=3, PAD_64BIT=4 };
  PADDING_TYPE paddingMode=PAD_32BIT;
  uint32_t paddingSize=4;
  bool debug=false;
  // Eliminates empty bitplanes, remaps colors. Must also be on for fixedPlanes (which are filled after all empty ones are removed)
  bool optimizePngPalette=true;
  bool stdCdxl=false;
  bool stdCdxl24=false;  // Depricated. Not required anymore.
  bool fixedPlanes=false;
  uint32_t reservedColorsNum=0;
  // If different to 0, then fixed number of planes is requested. Controlled by fixedPlanes flag.
  uint32_t fixedPlanesNum=0;
  uint32_t maxColors() const;
  uint32_t maxColorsCorrected() const;
  std::string audioModeToString() const;
  std::string ffmpegFrameNameSuffix() const;
  // Returns 000..0 for the number of frame zero digits
  std::string fixedFrameZeroDigitsToString() const; 
  std::string firstFrameNumberToString() const;
  std::filesystem::path getTmpDirName() const;
  void setTmpDirName(std::filesystem::path name);
  std::filesystem::path getTmpDirSndFileName() const;
  void checkAndSetOptions();
  bool keepTmpFiles=false;
  bool blackAndWhite=false;
  std::string adjustAspectSelectorName1="hdstretched";
  double adjustAspectSelectorValue1=1.35;
  bool hasInputFile() const;
  bool hasOutputFile() const;
  
private:
  void valueConsistencyChecks();
  void checkAndSetScreenMode();
  void checkAndSetAudioMode();
  void checkAndSetAudioDataType();
  void checkAndSetColorDepthBasedOnColorMode();
  void checkAndSetColorDepthBasedOnColorDepthBits();
  void checkAndSetPaddingSize(std::string paddingSize);
  void checkAndSetPaddingSize();
  void checkAndSetColorMode();
  void checkAndSetAdjustAspect();
  void checkVideoDimensionStride();
  void handleAutoScreenMode();
  void checkAndConfigureStdCdxl();   // Overrides multiple options
  void checkAndConfigureStdCdxlFrequency();
  void checkAndSetFixedPlanes();
  void checkImpossibleCombinations();
};

} // namespace AGAConv

#endif
