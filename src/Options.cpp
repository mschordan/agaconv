/*
    AGAConv - CDXL video converter for Commodore-Amiga computers
    Copyright (C) 2019-2024 Markus Schordan

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

#include "Options.hpp"

#include <vector>
#include <cassert>
#include <unordered_map>

#include "AGAConvException.hpp"
#include "Util.hpp"

using namespace std;

namespace AGAConv {

Options::Options()
{}

bool Options::hasInFile() const {
  return inFileName.string().size()>0;
}

bool Options::hasOutFile() const {
  return outFileName.string().size()>0;
}

bool Options::hasSndFile() const {
  return sndFileName.string().size()>0;
}

bool Options::checkConsistency() const {
  bool check=true;
  if(injectDPANChunk)
    check=check||(hasSndFile());
  return check;
}

std::string Options::audioModeToString() const {
  return (stereo? "stereo":"mono");
}

std::string Options::colorModeEnumToString() const {
  switch(colorModeEnum) {
  case CM_AGA: return "aga";
  case CM_HAM: return "ham";
  case CM_EHB: return "ehb";
  case CM_OCS: return "ocs";
  case CM_UNKNOWN: return "unknown";
  }
  throw AGAConvException(301,"unknown color mode enum "+std::to_string(colorModeEnum)+".");
}

uint32_t Options::maxColors() const {
  if(fixedPlanesNum>0)
    return Util::ULONGPow(2,fixedPlanesNum);
  else
    return Util::ULONGPow(2,numPlanes);
}

uint32_t Options::maxColorsCorrected() const {
  uint32_t maxCol=maxColors();
  if(maxCol<minColors) {
    throw AGAConvException(40, "max colors is "+std::to_string(maxCol)+", but must be at least "+std::to_string(minColors)+".");
  }
  uint32_t reserveBlackBackground=(this->reserveBlackBackgroundColor?1:0);
  uint32_t sum=reserveBlackBackground+this->reservedColorsNum;
  if(sum+minColors>maxCol) {
    throw AGAConvException(53, "requested "+std::to_string(sum)+" to be reserved, but only "+std::to_string(maxCol)+" are available. At least "+std::to_string(minColors)+" colors must remain available.");
  }
  return maxCol-sum;
}

std::string Options::ffmpegFrameNameSuffix() const {
  if(fixedFrameDigits==0) {
    return "frame%d";
  } else {
    return "frame%0"+std::to_string(fixedFrameDigits)+"d";
  }
}

std::string Options::firstFrameNumberToString() const {
  if(fixedFrameDigits==0) {
    return "1";
  } else {
    // e.g. "0001"
    string s=fixedFrameZeroDigitsToString();
    if(s.length()>0)
      s.pop_back();
    return s+"1";
  }
}

std::string Options::fixedFrameZeroDigitsToString() const {
  string stringOfZeros(fixedFrameDigits,'0');
  return stringOfZeros;
}

bool Options::hasInputFile() const {
  return inFileName.string().size()!=0;
}

bool Options::hasOutputFile() const {
  return outFileName.string().size()!=0;
}

std::filesystem::path Options::getTmpDirName() const {
  return tmpDir;
}

void Options::setTmpDirName(std::filesystem::path name) {
  tmpDir=name;
}

std::filesystem::path Options::getTmpDirSndFileName() const {
  return getTmpDirName()/sndFileName;
}

void Options::valueConsistencyChecks() {
  if(colorMode=="auto"||fps==autoValue||frequency==autoValue||hcHamQuality==autoValue) {
    throw AGAConvException(55, "color_mode, fps, frequency, or hc_ham_quality is set to 'auto'. Not supported.");
  }
}

void Options::checkImpossibleCombinations() {
  if(colorDepth==COL_24BIT && colorModeEnum==CM_EHB) {
    throw AGAConvException(59, "EHB mode with 24 bit color depth is not allowed.");
  }
  if(fixedFrames&&getPaddingMode()!=PAD_UNSPECIFIED) {
    throw AGAConvException(200, "Options --fixed-frames and alignment mode "+paddingModeToString(getPaddingMode())+" are not compatible.");
  }
}

void Options::checkVideoDimensionStride() {
  if(width%16!=0) {
    throw AGAConvException(50, "width of "+std::to_string(width)+" is not divisible by 16. Not supported."); 
  }
}

void Options::checkAndSetScreenMode() {
  typedef std::map<string,Options::GFX_RESOLUTION> ResMapType;
  ResMapType resModeMap={
                         {"unspecified",Options::GFX_UNSPECIFIED},
                         {"lores",Options::GFX_LORES},
                         {"hires",Options::GFX_HIRES},
                         {"superhires",Options::GFX_SUPERHIRES},
                         {"ultrahires",Options::GFX_ULTRAHIRES},
                         {"auto",Options::GFX_AUTO}
  };
  ResMapType::iterator i=resModeMap.find(screenMode);
  if(i!=resModeMap.end()) {
    resMode=(*i).second;
  } else {
    throw AGAConvException(41,"unknown resolution mode: "+screenMode);
  }
  handleAutoScreenMode();
}

void Options::handleAutoScreenMode() {
  if(resMode==Options::GFX_AUTO) {
    if(width<=AGAConv::maxLoresWidth)
      resMode=Options::GFX_LORES;
    else if(width<=AGAConv::maxHiresWidth)
      resMode=Options::GFX_HIRES;
    else if(width<=AGAConv::maxSuperHiresWidth)
      resMode=Options::GFX_SUPERHIRES;
    else
      resMode=Options::GFX_UNSPECIFIED;
  }
}

void Options::checkAndSetAdjustAspect() {
  if(adjustAspectMode==adjustAspectSelectorName1) {
    yScaleFactor=adjustAspectSelectorValue1;
  } else {
    // parse float value and set yScaleFactor
    try {
      yScaleFactor=std::stod(adjustAspectMode);
    } catch (std::invalid_argument const& ex) {
      throw AGAConvException(54,"incorrect floating point number format");
    }
  }
}

void Options::checkAndSetAudioDataType() {
  if(audioDataTypeString=="u8") {
    audioDataType=AUDIO_DATA_TYPE_UBYTE;
  } else if(audioDataTypeString=="s8") {
    audioDataType=AUDIO_DATA_TYPE_SBYTE;
  } else {
    throw AGAConvException(42,"unknown audio data type option "+audioDataTypeString);
  }
}

void Options::checkAndSetAudioMode() {
  if(audioMode=="stereo") {
    stereo=true;
  } else if(audioMode=="mono") {
    stereo=false;
  } else {
    throw AGAConvException(43,"unknown audio mode: "+audioMode);
  }
}

void Options::checkAndSetColorDepthBasedOnColorDepthBits() {
  switch(colorDepthBits) {
  case autoValue:
    // default, keep as-is
    break;
  case 12:
    // override other modes
    colorDepth=Options::COL_12BIT;
    break;
  case 24:
    // override other modes
    colorDepth=Options::COL_24BIT;
    break;
  default:
    throw AGAConvException(45, "unsupported color depth of "+std::to_string(colorDepthBits)+" bits selected.");
  }
}

void Options::checkAndSetColorDepthBasedOnColorMode() {
  switch(colorModeEnum) {
  case CM_OCS:
  case CM_EHB:
    colorDepth=Options::COL_12BIT;break;
  case CM_HAM:
    switch(numPlanes) {
    case 6:
      colorDepth=Options::COL_12BIT;break;
    case 8:
      colorDepth=Options::COL_24BIT;break;
    default:
      throw AGAConvException(44, "HAM mode with wrong number of "+std::to_string(numPlanes)+" planes.");
    }
    break;
  case CM_AGA:
    colorDepth=Options::COL_24BIT;break;
  case CM_UNKNOWN:
    throw AGAConvException(58, "unknown color mode.");
    // Intentionally no default case
  }
}

std::string Options::paddingSizeToString() const {
  if(paddingSize==autoValue)
    return std::to_string(0);
  else
    return std::to_string(paddingSize);
}

uint32_t Options::getPaddingSize() const {
  return paddingSize;
}

Options::PADDING_MODE Options::getPaddingMode() const {
  switch(paddingSize) {
  case autoValue:
  case 0: return Options::PAD_UNSPECIFIED;break;
  case 2: return Options::PAD_16BIT;break;
  case 4: return Options::PAD_32BIT;break;
  case 8: return Options::PAD_64BIT;break;
  default:
    throw AGAConvException(46, "unsupported padding size: "+std::to_string(paddingSize)+". Supported sizes: 0, 2, 4, 8.");
  }
}

std::string Options::paddingModeToString(Options::PADDING_MODE mode) const {
  switch(mode) {
  case Options::PAD_UNSPECIFIED: return "unspecified";
  case Options::PAD_16BIT: return "16bit";
  case Options::PAD_32BIT: return "32bit";
  case Options::PAD_64BIT: return "64bit";
  }
  throw AGAConvException(303, "unknown padding mode "+std::to_string(mode));
};

void Options::checkAndSetPadding() {
  static const std::unordered_map<std::string, uint32_t> mapping={
    {"auto",std::numeric_limits<uint32_t>::max()},
    {"none",0},
    {"16bit",2},
    {"32bit",4},
    {"64bit",8}
  };
  auto iter=mapping.find(alignmentString);
  if(iter!=mapping.end()) {
    paddingSize=(*iter).second;
  } else {
    throw AGAConvException(201, "unsupported alignment mode: "+alignmentString);    
  }
      
  // paddingSize=0 forces padding to be off
  if(width%4==0 || paddingSize==0) {
    return;
  } else if(paddingSize==autoValue) {
    paddingSize=4;
  }
}

void Options::checkAndSetColorMode() {
  bool foundValidConfig=false;
  uint8_t numPlanes=0;
  if(colorMode.size()==4) {
    struct ColorModeConfig { string prefix; uint8_t start; uint8_t end; ColorModeEnum colorModeEnum; };
    vector<ColorModeConfig> supportedColorModes={{"aga", 2, 8, ColorModeEnum::CM_AGA},
                                                 {"ocs", 2, 5, ColorModeEnum::CM_OCS},
                                                 {"ham", 6, 8, ColorModeEnum::CM_HAM}
    };
    for(auto config : supportedColorModes) {
      if(!(colorMode[3]>='0'&&colorMode[3]<='9')) {
        throw AGAConvException(56, "Incorrect specification of planes in color mode: "+this->colorMode);
      }
      numPlanes=(uint8_t)(colorMode[3]-'0');
      if(colorMode.compare(0, config.prefix.size(), config.prefix)==0 && numPlanes>=config.start && numPlanes<=config.end) {
        if(config.prefix=="ham") {
          if(numPlanes==6) {
            foundValidConfig=true;
            colorModeEnum=config.colorModeEnum;
            conversionTool="ham_convert"; // HAM can only be converted with ham_convert
          } else if(numPlanes==8) {
            foundValidConfig=true;
            colorModeEnum=config.colorModeEnum;
            conversionTool="ham_convert"; // HAM can only be converted with ham_convert
          } else {
            foundValidConfig=false;
          }
        }
      }
    }
  }
  // EHB must be exact match
  if(colorMode=="ehb") {
    foundValidConfig=true;
    numPlanes=6;
    colorModeEnum=CM_EHB;
    // EHB can only be converted with ham_convert
    conversionTool="ham_convert";
  }
  if(numPlanes==0) {
    throw AGAConvException(57, "wrong number of planes requested in color mode "+colorMode+".");
  }
  if(Util::hasPrefix("aga",colorMode) && numPlanes<=8) {
    if(numPlanes<minPlanes) {
      throw AGAConvException(47, "color mode "+colorMode+" not supported. At least "+std::to_string(minPlanes)+" planes are required.");
    }
    foundValidConfig=true;
    colorModeEnum=CM_AGA;
    conversionTool="ffmpeg";
  }
  if(Util::hasPrefix("ocs",colorMode) && numPlanes<=5) {
    if(numPlanes<minPlanes) {
      throw AGAConvException(48, "color mode "+colorMode+" not supported. At least "+std::to_string(minPlanes)+" planes are required.");
    }
    foundValidConfig=true;
    colorModeEnum=CM_OCS;
    conversionTool="ffmpeg";
  }
  if(!foundValidConfig) {
    throw AGAConvException(49, "wrong color-mode selected: "+colorMode);
  }
  this->numPlanes=numPlanes;
  checkAndSetColorDepthBasedOnColorMode();
  checkAndSetColorDepthBasedOnColorDepthBits(); // overrides 'BasedOnColorMode' if not 0=auto
}

void Options::checkAndSetFixedPlanes() {
  if(fixedPlanesFlag) {
    fixedPlanesNum=numPlanes;
  }
}

void Options::checkAndAdjustFrequencyFor32BitAlignedAudioChunk() {
  if(!adjustFrequency)
    return;
  uint32_t oldFrequency=frequency;
  uint32_t channelAdj=stereo?2:1;
  uint32_t adj=fps*(4/(channelAdj));
  if(frequency%adj!=0) {
    frequency=(frequency/adj)*adj;
  }
  // check and present results
  uint32_t totalAudioSize=(frequency/fps)*(channelAdj);
  bool frequencyDivPass=(frequency%fps==0);
  bool audioSize32BitAlignedPass=(totalAudioSize%4==0);
  if(!frequencyDivPass || !audioSize32BitAlignedPass) {
    throw AGAConvException(51, "Standard CDXL adjustment of frequency failed for 32bit audio data alignment = "+std::to_string(frequency));
  }
  if(verbose>=1) {
    if(oldFrequency!=frequency)
      cout<<"Adjusted frequency from "<<oldFrequency<<" to "<<frequency<<" (Standard CDXL)."<<endl;
  }
  if(verbose>=2) {
    cout<<"Standard CDXL: Frequency divisibile by fps      : "<<(frequencyDivPass?"PASS":"FAIL")<<" ("<<frequency<<"="<<frequency/fps<<"*"<<fps<<"+"<<frequency%fps<<")"<<endl;
    cout<<"Standard CDXL: Frame audio data 32-bit aligned  : "<<(audioSize32BitAlignedPass?"PASS":"FAIL")<<" ("<<totalAudioSize<<"="<<totalAudioSize/4<<"*4+"<<totalAudioSize%4<<")"<<endl;
  }
}

bool Options::isStdCdxl() const {
  return fixedFrames
    ||(getPaddingMode()==PAD_UNSPECIFIED)
    ;
}

void Options::checkAndSetFormat() {
  if(formatName=="std-opt")
    format=FMT_STD_OPT;
  else if(formatName=="std-fixed")
    format=FMT_STD_FIXED;
  else if(formatName=="ctm-opt")
    format=FMT_CTM_OPT;
  else if(formatName=="unspecified")
    format=FMT_UNSPECIFIED;
  else {
    throw AGAConvException(202,"unknown format specified: "+formatName+".");
  }
}

void Options::processFormatOptions() {
  switch(format) {
  case FMT_CTM_OPT:
    alignmentString="32bit";
    fixedFrames=false;
    fillPaletteToMaxColorsOfPlanes=false;
    adjustFrequency=false;
    fixedPlanesFlag=false;
    adjustHeight=false;
    break;
  case FMT_STD_OPT:
    // default STD CXDL with variable chunk size
    alignmentString="none";
    // allows a player to detect 12/24 bit colors without extension and ensure 32-bit alignment for #24bit colors and planes>=2.
    fillPaletteToMaxColorsOfPlanes=true;
    adjustFrequency=true; // only needs to be adjust to 32bit, but is currently adjusted for fixed-frame size
    fixedPlanesFlag=false;
    // without padding this may be necessary to align for 32bit if width is not a multiple of 4
    adjustHeight=true;
    break;
  case FMT_STD_FIXED:
    // STD CDXL with fixed frames supported by old players
    alignmentString="none";
    fixedFrames=true;
    fillPaletteToMaxColorsOfPlanes=true;
    adjustFrequency=true; // adjusted for 32bit alignment and fixed size within 1 sec (i.e. divisible by fps)
    fixedPlanesFlag=true;
    adjustHeight=true;
    break;
  case FMT_UNSPECIFIED:
      // do not set any options
      break;
  default:
    throw AGAConvException(203,"undefined CDXL file format: "+to_string(format)+".");
  }
}

std::string Options::conversionFlagsToString() {
  stringstream ss;
  ss<<"paddingSize: "<<paddingSize
    <<", fixed frames: "<<fixedFrames
    <<", fillPaletteToMaxColorsOfPlanes: "<<fillPaletteToMaxColorsOfPlanes
    <<", adjustFrequency: "<<adjustFrequency
    <<", fixedPlanesFlag: "<<fixedPlanesFlag
    <<", fixedPlanesNum: "<<fixedPlanesNum
    <<", numPlanes: "<<+numPlanes
    <<endl;
  return ss.str();
}

void Options::checkAndSetOptions() {
  checkAndSetFormat();
  processFormatOptions();
  valueConsistencyChecks();
  
  checkAndSetPadding();

  // Handle graphics
  checkAndSetColorMode();
  checkAndSetFixedPlanes(); // requires checkAndSetColorMode
  checkAndSetScreenMode();
  checkVideoDimensionStride();
  checkAndSetAdjustAspect();

  // Handle audio
  checkAndSetAudioDataType();
  checkAndSetAudioMode();
  // Only adjusts frequency if flag adjustFrequency is set
  checkAndAdjustFrequencyFor32BitAlignedAudioChunk();

  checkImpossibleCombinations();
}

} // namespace AGAConv
