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

#include "CDXLEncode.hpp"

#include <cassert>
#include <cmath>
#include <memory>
#include <sstream>

#include "AGAConvException.hpp"
#include "CDXLEncode.hpp"
#include "CDXLHeader.hpp"
#include "IffBMHDChunk.hpp"
#include "IffBODYChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "Options.hpp"
#include "PngLoader.hpp"
#include "Util.hpp"

using namespace std;

namespace AGAConv {

// This check should always pass because the frequency is adjusted in Options::checkAndConfigureStdCdxl()
void CDXLEncode::checkFrequencyForStdCdxl(Options& options) {
  // Ensure that frequency/fps is divisble by 4
  if(_frequency % _fps != 0) {
    stringstream ss;
    ss<<"Frequency is not divisble by fps ("<<_frequency<<"/"<<+_fps<<"="<<((double)_frequency/_fps)<<"). Audio data cannot be devided into chunks of equal size. ... bailing out."<<endl;
    ss<<"CDXL 32bit alignment check: FAIL";
    throw AGAConvException(90, ss.str());
  }
  // To be 32-bit aligned, it must be a multiple 4 for mono, a multiple of 2 for stereo
  ULONG audioChannelDataSize=_frequency/_fps; // guaranteed to be divisible here
  assert(_audioMode==1||_audioMode==2);
  if( (audioChannelDataSize*_audioMode) % 4 != 0 ) {
    stringstream ss;
    ss<<"Total audio data per frame not 32-bit aligned ("<<_frequency<<"Hz/"<<+_fps<<"FPS*"<<_audioMode<<" = "<<audioChannelDataSize*_audioMode<<" not a multiple of 4) ... bailing out."<<endl;
    ss<<"32bit CDXL check: FAIL"<<endl;
    throw AGAConvException(91, ss.str());
  } 
  if(options.verbose>=2) {
    cout<<"Standard CDXL: PASS (frequency is divisible by FPS and total audio data is 32-bit aligned)"<<endl;
    cout<<"Standard CDXL: Frame audio data details: "<<_frequency<<"Hz/"<<+_fps<<"FPS = "<<audioChannelDataSize<<" bytes, total audio data: "<<audioChannelDataSize*_audioMode<<" bytes per frame)"<<endl;
  }
}

void CDXLEncode::run(Options& options) {
  if(options.writeCdxl && options.hasOutFile()) {
    _outFile.open(options.outFileName, ios::out | ios::binary);
    if(_outFile.is_open() == false) {        
      throw AGAConvException(92, "cannot open output CDXL file "+options.outFileName.string());
    }
    _writeFile=true;
  }
  // Audio data setup of global parameters for audio data. Actual
  // conversion and audio-chunk size is computed in
  // FileSequenceConversion::run
  if(options.hasSndFile()) {
    _sndFile.open(options.getTmpDirSndFileName().c_str(), ios::in | ios::binary);
    if(_sndFile.is_open() == false) {        
      throw AGAConvException(93, "cannot open snd file "+options.getTmpDirSndFileName().string());
    }
    _totalAudioDataLength=Util::fileSize(options.getTmpDirSndFileName().c_str());
    if(options.verbose>=2) cout<<"Found audio file of length "<<_totalAudioDataLength<<"."<<endl;

    // Compute snd data size per frame
    if(options.frequency<=0) {
      throw AGAConvException(94, "no frequency for audio file provided.");
    }
    if(options.fps==0) {
      throw AGAConvException(95, "no fps provided.");
    }
    if(options.stereo) {
      _audioMode=2;
    } else {
      _audioMode=1;
    }

    switch(options.colorDepth) {
    case Options::COL_12BIT:
      _24BitColors=false;
      break;
    case Options::COL_24BIT:
      _24BitColors=true;
      break;
      // Intentionally no default case
    }
    _fps=(UBYTE)options.fps;
    _resolutionModes=(UBYTE)options.resMode;
    _paddingModes=(UBYTE)options.getPaddingMode();
    _frameLengthMode=options.fixedFrames?FLM_FIXED:FLM_VARIABLE;
    _frequency=options.frequency;
    if(options.fixedFrames) {
      checkFrequencyForStdCdxl(options);
    }
  }
  if(options.verbose>=1) {
    cout<<"Running internal CDXL encoder (fps: "<<options.fps<<", frequency:"<<options.frequency<<", audio mode: "<<(options.stereo?"stereo":"mono")<<")"<<endl;
  }
  FileSequenceConversion::run(options);
}

void CDXLEncode::preVisitFirstFrame() {
  _currentFrameNr=1;
  _previousFrameSize=0;
}

int CDXLEncode::getMonoAudioDataLength() {
  // Available are: _currentFrameNr, _totalAudioDataLength, _fps, _frequency
  // Imprecise:  _monoAudioDataLength=options.frequency/options.fps;  
  int totalMonoAudioDataLength=_totalAudioDataLength/_audioMode;
  // Need total number of frames, then it's simple; or even it out within each second
  uint32_t frameNrWithinSec=((_currentFrameNr-1) % options.fps)+1;
  double dFrameLen=(double)options.frequency/(double)options.fps;
  if(options.debug) {
    cout<<"FrameNrSec: "<<frameNrWithinSec<<" TotalMonoAudioLen: "<<totalMonoAudioDataLength<<" fps: "<<options.fps<<" dFrameLen: "<< dFrameLen<<endl;
  }
  uint32_t frameLen = (uint32_t)std::round(dFrameLen*frameNrWithinSec-_frameLenSum);
  // frameLen must be multiple of 2 because the Amiga requires the length
  // in number of words (16 bit aligned)
  if(frameLen%2 == 1) {
    frameLen--;
    if(options.fixedFrames) {
      throw AGAConvException(310, "Internal: fixed frames requested but audio data requires adjustment. Inconsistent data size.");
    }
  }
  if(frameNrWithinSec==options.fps) {
    // Correct for last frame within second
    frameLen = options.frequency-_frameLenSum ;
    if(frameLen%2 == 1) {
      if(options.fixedFrames) {
        throw AGAConvException(311, "Internal: fixed frames requested but audio data requires adjustment at last frame in second. Inconsistent data size.");
      }
      // This means an odd frequency is used. Use alternating +/- 1 correction based on frame nr.
      if(options.verbose>=3) {
        cout<<"CDXL Encoding leads to odd total audio length in last frame of second - applying adjustment:"<<endl;
        cout<<"  Frame Nr           : "<<_currentFrameNr<<endl;
        cout<<"  Audio Frequency    : "<<options.frequency<<endl;
        cout<<"  Audio FrameLenSum  : "<<_frameLenSum<<endl;
        cout<<"  Audio FrameLen     : "<<frameLen<<endl;
      }
      if(_currentFrameNr%2==1)
        frameLen++;
      else
        frameLen--;
      if(options.verbose>=3)
        cout<<"  Audio FrameLen adj : "<<frameLen<<endl;
      }
    _frameLenSum=0; // Reset to initial value
  } else {
    _frameLenSum+=frameLen;
  }
  _totalMonoAudioDataLengthSum+=frameLen;
  if(options.debug) {
    cout<<"DEBUG: Frame: "<<_currentFrameNr<<" length: "<<frameLen<<" [ frameLenSum: "<<_frameLenSum<<" totalLenSum: "<<_totalMonoAudioDataLengthSum<<"]"<<endl;
  }
  return frameLen;
}

// Mode: 1 mono, 2: stereo
ByteSequence* CDXLEncode::readAudioData() {
  if(_sndFile.is_open() == false) {
    cerr<<"Error: no audio file."<<endl;
    return 0;
  }
  switch (_audioMode) {
  case 1: {
    ByteSequence* audioByteSequence=new ByteSequence();
    int dataLen=getMonoAudioDataLength();
    for(int j=0;j<dataLen*_audioMode;j++) {
      /* Convert from signed byte -128 .. 127
         to unsigned byte 0 .. 255
      */
      UBYTE audioByte2;
      if (!_sndFile.eof()) {
        int8_t audioByte0=(int8_t)_sndFile.get();
        int8_t audioByte1=(int8_t)audioByte0+128;
        audioByte2=(UBYTE)audioByte1;
        // Make sure the eof bit is set before we try to read again
        _sndFile.peek();
      } else {
        audioByte2 = 0;
      }
      audioByteSequence->add(audioByte2);
    }
    return audioByteSequence;
  }
  case 2: {
    ByteSequence* audioByteSequence=new ByteSequence();
    // Reshuffle bytes for Amiga stero format (ABABAB.. => AAA..BBB..)
    list<UBYTE> tmp;
    int dataLen=getMonoAudioDataLength();
    for(int j=0;j<dataLen*_audioMode;j++) {
      /* Convert from signed byte -128 .. 127
	 to unsigned byte 0 .. 255
      */
      UBYTE audioByte2;
      if (!_sndFile.eof()) {
        int8_t audioByte0=(int8_t)_sndFile.get();
        int8_t audioByte1=(int8_t)audioByte0+128;
        audioByte2=(UBYTE)audioByte1;
        // Make sure the eof bit is set before we try to read again
        _sndFile.peek();
      } else {
        audioByte2 = 0;
      }
      if(j%2==0) {
        audioByteSequence->add(audioByte2);
      } else {
        tmp.push_back(audioByte2);
      }
    }
    for(UBYTE audioByte : tmp) {
      audioByteSequence->add(audioByte);
    }
    return audioByteSequence;
  }
  default:
    throw AGAConvException(96, "unsupported audio mode in CDXL generation (mode: "+std::to_string(_audioMode)+")");
  }
  assert(false);
}

void CDXLEncode::addColorsForTargetPlanes(int targetPlanes, IffCMAPChunk* cmapChunk) {
  // Fill up colors to 2^fixedPlanesNum
  ULONG targetColors=Util::ULONGPow(2,static_cast<ULONG>(targetPlanes));
  ULONG currentNumColors=cmapChunk->numberOfColors();
  if(targetColors>currentNumColors) {
    if(options.debug) cout<<"DEBUG: [colors:"<<currentNumColors<<"->"<<targetColors<<"] ";
    ULONG colorsToAdd=targetColors-currentNumColors;
    for(ULONG i=0;i<colorsToAdd;i++) {
      cmapChunk->addColor(RGBColor(0,0,0));
    }
  }
}

void CDXLEncode::addColorsForTargetPlanes(int targetPlanes, CDXLPalette& palette) {
  // Fill up colors to 2^fixedPlanesNum
  ULONG targetColors=Util::ULONGPow(2,static_cast<ULONG>(targetPlanes));
  ULONG currentNumColors=palette.numberOfColors();
  if(targetColors>currentNumColors) {
    if(options.debug) cout<<" [colors:"<<currentNumColors<<"->"<<targetColors<<"] ";
    ULONG colorsToAdd=targetColors-currentNumColors;
    for(ULONG i=0;i<colorsToAdd;i++) {
      palette.addColor(RGBColor(0,0,0));
    }
  }
}

void CDXLEncode::fillPaletteToMaxColorsOfPlanes(int targetPlanes, CDXLFrame& frame) {
  auto palette = frame.palette;

  if(options.colorDepth==Options::COL_24BIT && targetPlanes<2) {
    // special case: for 24bit colors 1 plane has 2 colors with 2*3=6 bytes.
    // Not multiple of 4, therefore fill for at least 2 planes's colors.
    // Let p be the number of planes. Then 2^p mod 4 == 0 with p>=2.
    targetPlanes=2;
  }
  
  // Add colors may also complete a plane's colors up to 2^fixedPlanesNum
  addColorsForTargetPlanes(targetPlanes,frame.palette); // does not update frame header
  // Update palettesize in header
  frame.header.setPaletteSize(frame.palette.numberOfColors()*frame.header.getColorBytes());
}

void CDXLEncode::importILBMChunk(CDXLFrame& frame, IffILBMChunk* ilbmChunk) {
  uint32_t verbosityLevel=2;
  // (i) Convert IFF file into a CDXL block
  IffBMHDChunk* bmhdChunk=ilbmChunk->getBMHDChunk();
  IffCAMGChunk* camgChunk=ilbmChunk->getCAMGChunk();
  IffCMAPChunk* cmapChunk=dynamic_cast<IffCMAPChunk*>(ilbmChunk->getChunkByName("CMAP"));
  ilbmChunk->uncompressBODYChunk();
  IffBODYChunk* bodyChunk=ilbmChunk->getBODYChunk();
  string errorChunk="unknown";
  if(!bmhdChunk)
    errorChunk="BMHD";
  if(!camgChunk)
    errorChunk="CAMG";
  if(!cmapChunk)
    errorChunk="CMAP";
  if(!bodyChunk)
    errorChunk="BODY";
  if(!(bmhdChunk&&camgChunk&&cmapChunk&&bodyChunk)) {
    throw AGAConvException(97, errorChunk+" chunk is missing.");
    
  }

  frame.header.initialize(bmhdChunk,cmapChunk,camgChunk);
  if(options.resMode==Options::GFX_UNSPECIFIED) {
    // clear Lores flag in case it has been set
    frame.header.setResolutionModes(static_cast<UBYTE>(Options::GFX_UNSPECIFIED));
  }
  if((options.verbose>=verbosityLevel)||options.debug) {
    cout<<".. encoding CDXL frame "<<frame.header.getCurrentFrameNr();
    cout<<" ("<<cmapChunk->numberOfColors()<<" colors, "<<+bmhdChunk->getNumPlanes()<<" bitplanes)";
  }

  if(options.debug) {
    cout<<"DEBUG: Before importing colors for frame "<<_currentFrameNr<<endl;
    cout<<"DEBUG: ILBM Palette size: "<<cmapChunk->getDataSize()<<endl;
    cout<<"DEBUG: CDXL Palette size: "<<frame.palette.getLength()<<endl;
  }

  frame.palette.importColors(cmapChunk);
  if(options.debug) {
    //cout<<"ILBM Palette: "<<cmapChunk->paletteToString()<<endl;
    //cout<<"CDXL Palette: "<<frame.palette.toString()<<endl;
    cout<<"DEBUG: After importing colors for frame "<<_currentFrameNr<<endl;
    cout<<"DEBUG: ILBM Palette size: "<<cmapChunk->getDataSize()<<endl;
    cout<<"DEBUG: CDXL Palette size: "<<frame.palette.getLength()<<endl;
    cout<<"DEBUG: num fixed planes : "<<options.fixedPlanesNum<<endl;
  }

  assert(ilbmChunk);
  frame.importVideo(ilbmChunk);
  assert(frame.video);

  // Fill color palette and video data with fill data ensure fixed frame size, update header
  // except it is a HAM6, HAM8, or EHB frame, in which case the planes are fixed anyways,
  // but filling up would make it wrong, because these modes have extra planes.
  if(!camgChunk->isHam() && !camgChunk->isHalfBrite()) {
    if(options.fixedPlanesNum==0) {
      // only check for palette option
      if(options.fillPaletteToMaxColorsOfPlanes) {
        uint32_t numPlanes=frame.header.getNumberOfBitplanes();
        fillPaletteToMaxColorsOfPlanes(numPlanes,frame);
      }
    } else {
      if(options.debug) cout<<"DEBUG: fixed planes: "<<options.fixedPlanesNum<<endl;
      uint32_t oldNumPlanes=(uint32_t)frame.header.getNumberOfBitplanes();
      if(options.fixedPlanesNum<oldNumPlanes) {
        stringstream ss;
        ss<<"\n\nError: fixed number of "<<options.fixedPlanesNum<<" planes requested is lower than "<<oldNumPlanes<<" planes in input frame. '--optimize no' with ffmpeg generated frames selected? Bailing out. "<<endl;
        throw AGAConvException(98, ss.str());
      }
      fillPaletteToMaxColorsOfPlanes(options.fixedPlanesNum, frame);
      if(options.fixedPlanesNum>oldNumPlanes) {
        if(options.verbose>=2) cout<<"[planes:"<<oldNumPlanes<<"->"<<options.fixedPlanesNum<<"] ";
        // Determine plane size
        UWORD lineLengthInBytes=Util::wordAlignedLengthInBytes(frame.header.getVideoWidth());
        UWORD height=frame.header.getVideoHeight();
        ULONG planeSize=(ULONG)lineLengthInBytes*(ULONG)height;
        if(oldNumPlanes<options.fixedPlanesNum) {
          ULONG planesToAdd=options.fixedPlanesNum-oldNumPlanes;
          // Add all required empty planes at once
          if(options.debug) cout<<"\nDEBUG: ADDING: planes:"<<planesToAdd<<" planesize: "<<planeSize<<" total: "<<planeSize*planesToAdd<<endl;
          ULONG zeroDataToAdd=planeSize*planesToAdd;
          for(ULONG i=0;i<zeroDataToAdd;i++) {
            frame.video->add(0);
          }
          // Update numberOfBitplanes
          assert(frame.header.getNumberOfBitplanes()+planesToAdd==options.fixedPlanesNum);
          frame.header.setNumberOfBitplanes(options.fixedPlanesNum);
          assert(frame.video->getDataSize()==frame.header.getNumberOfBitplanes()*planeSize);
        }
      }
    }
  }
  if((options.verbose>=verbosityLevel)||options.debug) {
    cout<<endl;
  }
}

// Set all options in CDXL header
void CDXLEncode::importOptions(CDXLFrame& frame) {
  frame.header.setColorBitsFlag(_24BitColors);
  
  // Padding size 2 to reproduce results
  frame.setPaddingSize(options.paddingSize);
  
  if(_24BitColors) {
    frame.palette.setColorMode(CDXLPalette::COL_24BIT);
  } else {
    frame.palette.setColorMode(CDXLPalette::COL_12BIT);
  }
  switch(_audioMode) {
  case 1:
    frame.header.setSoundMode(MONO);
    break;
  case 2:
    frame.header.setSoundMode(STEREO);
    break;
  default:
    throw AGAConvException(302, "Internal: wrong audio mode: "+std::to_string(_audioMode));
  }
  // AnimFx only supports: BIT_PLANAR
  frame.header.setPlaneArrangement(BIT_PLANAR);
  // Set frequency (CDXL extension)
  frame.header.setFrequency(_frequency);
  frame.header.setFps(_fps);
  frame.header.setFrameLengthMode(_frameLengthMode);
  frame.header.setPaddingModes(_paddingModes);
  if(options.isStdCdxl())
    frame.header.setFileType(STANDARD);
  else
    frame.header.setFileType(CUSTOM);
}

void CDXLEncode::importAudio(CDXLFrame& frame) {
  // Audio mode is set in run method
  frame.audio=readAudioData();

  switch(frame.header.getSoundMode()) {
  case STEREO:
    frame.header.setChannelAudioSize(frame.audio->getDataSize()/2);
    break;
  case MONO:
    frame.header.setChannelAudioSize(frame.audio->getDataSize());
    break;
  }
}

void CDXLEncode::visitPngFile(string pngFileName) {
  if(options.verbose>=2) {
    cout<<"Loading: png file "<<pngFileName;
    cout<<" ";
  }
  PngLoader pngLoader;
  pngLoader.readFile(pngFileName);

  if(options.optimizePngPalette) {
    // Uses several other options for optimization
    pngLoader.optimizePngPalette(options);
  }

  IffILBMChunk* ilbmChunk=pngLoader.createILBMChunk(options);
  if(options.debug)
    cout<<"DEBUG: next: visitILBMChunk."<<endl;

  if(ilbmChunk) {
    visitILBMChunk(ilbmChunk);
    if(options.debug)
      cout<<"DEBUG: visitPngFile done."<<endl;
    delete ilbmChunk;
  }
}


void CDXLEncode::visitILBMChunk(IffILBMChunk* ilbmChunk) {
  CDXLFrame& frame=*new CDXLFrame();
  importOptions(frame); // sets values in header from command line options
  frame.header.setFrameNr(_currentFrameNr);
  importILBMChunk(frame,ilbmChunk);

  // Special case: KILL EHB flag, can only be set now, after ILBM has been imported
  if(IffCAMGChunk* iffCAMGChunk=ilbmChunk->getCAMGChunk()) {
    if(frame.header.getNumberOfBitplanes()==6 && !iffCAMGChunk->isHalfBrite() && !iffCAMGChunk->isHam()) {
      // Required for AGA with 6 planes to not display EHB mode by default
      frame.header.setKillEHBFlag(true); 
    }
  } else {
    // Manual clean up in lack of ref-counted pointers
    delete ilbmChunk;
    delete &frame;
    throw AGAConvException(99, "no CAMG chunk found. Bailing out.");
  }

  // Set all audio relevant values in CDXL frame
  importAudio(frame);
  
  // Compute frame size and set (check if 1st frame)
  ULONG frameSize=frame.getLength();
  if(options.enabled32BitCheck && frame.getLength()%4!=0) {
    auto len=std::to_string(frame.getLength());
    // Manual clean up in lack of ref-counted pointers
    delete ilbmChunk;
    delete &frame;
    if(options.adjustHeight) {
      if(options.width%4!=0 && (options.getPaddingMode()==Options::PAD_32BIT || options.getPaddingMode()==Options::PAD_64BIT))
        throw AGAConvException(101,"Video width of "
                               +std::to_string(options.width)
                               +" is not 32bit aligned and padding not enabled. This can reduce I/O speed by up to 50%. "
                               +"Not generating CDXL file. Change video width by 16 pixel (plus or minus).");
    }
    if(options.fixedFrames) {
      throw AGAConvException(101,"CDXL: frame size of "+len+" is not 32bit aligned. This can reduce I/O speed by up to 50%. Not generating CDXL file. Change video width by 16 pixel (plus or minus).");
    } else {
      throw AGAConvException(102,"CDXL: frame size of "+len+" is not 32bit aligned. Inconsistent user configuration. Not generating CDXL file."
                             +" Padding mode: "+std::to_string(options.getPaddingMode())+" padding size: "+options.paddingSizeToString()+" Add option --ctm-opt.");      
    }
  }
  frame.header.setCurrentChunkSize(frameSize);
  if(options.debug) {
    cout<<"DEBUG: frame "<<_currentFrameNr<<": setting current header chunksize = "<<frameSize<<endl;
    cout<<"DEBUG: "<<frame.header.toString()<<endl;
  }

  // _previousFrameSize is init to 0, and threfore 0 for first frame
  frame.header.setPreviousChunkSize(_previousFrameSize);
  _previousFrameSize=frameSize;

  if(_writeFile) {
    frame.setOutFile(&_outFile);
    frame.writeChunk();
  }

  delete &frame;
  _currentFrameNr++;
}

void CDXLEncode::postVisitLastILBMChunk(IffILBMChunk* ilbmChunk) {
  // Close CDXL file
  FileSequenceConversion::postVisitLastILBMChunk(ilbmChunk);
  _outFile.close();
  if(options.verbose>=1) {
    cout<<"All frame chunks 32-bit aligned "
        <<"("<<(options.isStdCdxl()?"Standard":"Custom")<<" CDXL check)"
        <<endl;
    cout<<"Generated CDXL file "<<options.outFileName<<endl;
  }
}

} // namespace AGAConv
