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

#include "CDXLEncode.hpp"
#include "CDXLHeader.hpp"
#include "IffBMHDChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffBODYChunk.hpp"
#include <cassert>
#include "Util.hpp"
#include <cmath>
#include "PngLoader.hpp"
#include "Options.hpp"

void CDXLEncode::checkFrequencyForStdCdxl() {
  // ensure that frequency/fps is divisble by 4
  if(_frequency % _fps != 0) {
    cout<<"Frequency is not divisble by fps ("<<_frequency<<"/"<<+_fps<<"="<<((double)_frequency/_fps)<<"). Audio data cannot be devided into chunks of equal size. ... bailing out."<<endl;
    cout<<"STANDARD CDXL OPTIMIZATION CHECK: FAIL"<<endl;
    exit(1);
  }
  // to be 32-bit aligned, it must be a multiple 4 for mono, a multiple of 2 for stereo
  ULONG audioChannelDataSize=_frequency/_fps; // guaranteed to be divisible here
  assert(_audioMode==1||_audioMode==2);
  if( (audioChannelDataSize*_audioMode) % 4 != 0 ) {
    cout<<"Total audio data per frame not 32-bit aligned ("<<_frequency<<"Hz/"<<+_fps<<"FPS*"<<_audioMode<<" = "<<audioChannelDataSize*_audioMode<<" not a multiple of 4) ... bailing out."<<endl;
    cout<<"STANDARD CDXL OPTIMIZATION CHECK: FAIL"<<endl;
    exit(1);
  } 
  cout<<"STANDARD CDXL OPTIMIZATION CHECK: PASS (frequency is divisible by FPS and total audio data is 32-bit aligned)"<<endl;
  cout<<"Frame audio data details: "<<_frequency<<"Hz/"<<+_fps<<"FPS = "<<audioChannelDataSize<<" bytes, total audio data: "<<audioChannelDataSize*_audioMode<<" bytes per frame)"<<endl;
}

void CDXLEncode::run(Options& options) {
  if(options.writeCdxl && options.hasOutFile()) {
    _outFile.open(options.outFileName, ios::out | ios::binary);
    if(_outFile.is_open() == false) {        
      cerr<<"Error: cannot open output CDXL file "<<options.outFileName<<endl;
      exit(1);
    }
    _writeFile=true;
  }
  // audio data setup of global parameters for audio data. Actual
  // conversion and audio-chunk size is computed in
  // FileSequenceConversion::run
  if(options.hasSndFile()) {
    _sndFile.open(options.sndFileName, ios::in | ios::binary);
    if(_sndFile.is_open() == false) {        
      cerr<<"Error: cannot open snd file "<<options.sndFileName<<endl;
      exit(1);
    }
    _totalAudioDataLength=Util::fileSize(options.sndFileName);
    cout<<"Found audio file of length "<<_totalAudioDataLength<<"."<<endl;

    // compute snd data size per frame
    if(options.frequency<=0) {
      cerr<<"Error: no frequency for audio file provided."<<endl;
      exit(1);
    }
    if(options.fps==0) {
      cerr<<"Error: no fps provided."<<endl;
      exit(1);
    }
    if(options.stereo) {
      _audioMode=2;
    } else {
      _audioMode=1;
    }

    switch(options.colorSize) {
    case Options::COL_12BIT:
      _24BitColors=false;
      break;
    case Options::COL_24BIT:
      _24BitColors=true;
      break;
    default:
      cerr<<"Error: unsupported color size: "<<options.colorSize<<endl;
      exit(1);
    }
	_fps=(UBYTE)options.fps;
    _resolutionModes=(UBYTE)options.resMode;
    _paddingModes=(UBYTE)options.paddingMode;
    _frequency=options.frequency;
    if(options.stdCdxl) {
      checkFrequencyForStdCdxl();
    }
    if(options.playRate==0) {
      options.playRate=(ULONG)(options.systemTimingConstant/options.frequency);
    }
    cout<<"AUDIO frequency:"<<options.frequency<<", playrate:"<<options.playRate<<", fps: "<<options.fps<<", stereo: "<<options.stereo<<endl;
  }
  if(!options.quiet) {
    cout<<"Encoding started ... "<<endl;
  }
  FileSequenceConversion::run(options);
}

void CDXLEncode::preVisitFirstFrame() {
  _currentFrameNr=1;
  _previousFrameSize=0;
}

int CDXLEncode::getMonoAudioDataLength() {
  // available are: _currentFrameNr, _totalAudioDataLength, _fps, _frequency
  // imprecise:  _monoAudioDataLength=options.frequency/options.fps;  
  int totalMonoAudioDataLength=_totalAudioDataLength/_audioMode;
  // need total number of frames, then it's simple; or even it out within each second
  int frameNrWithinSec=((_currentFrameNr-1) % options.fps)+1;
  double dFrameLen=(double)options.frequency/(double)options.fps;
  if(options.debug) {
    cout<<"FrameNrSec: "<<frameNrWithinSec<<" TotalMonoAudioLen: "<<totalMonoAudioDataLength<<" fps: "<<options.fps<<" dFrameLen: "<< dFrameLen<<endl;
  }
  int frameLen = (int)std::round(dFrameLen*frameNrWithinSec-_frameLenSum);
  // frameLen must be multiple of 2 because the Amiga takes the length in number of words
  if(frameLen%2 == 1) {
    frameLen--;
  }
  if(frameNrWithinSec==options.fps) {
    // correct for last frame within second
    frameLen = options.frequency-_frameLenSum ;
    if(frameLen%2 == 1) {
      // this means an odd frequency is used. Use alternating +/- 1 correction based on frame nr.
      if(_currentFrameNr%2==1)
        frameLen++;
      else
        frameLen--;
      //cerr<<"CDXL Encoding leads to odd audio length in last frame of second."<<endl;
      //cerr<<"Frame Nr         : "<<frameNr<<endl;
      //cerr<<"Audio Frequency  : "<<options.frequency<<endl;
      //cerr<<"Audio FrameLenSum: "<<_frameLenSum<<endl;
      //cerr<<"Audio FrameLen   : "<<frameLen<<endl;
      //exit(1);
    }
    _frameLenSum=0; // reset to init value
  } else {
    _frameLenSum+=frameLen;
  }
  _totalMonoAudioDataLengthSum+=frameLen;
  if(options.debug) {
    cout<<"DEBUG: Frame: "<<_currentFrameNr<<" length: "<<frameLen<<" [ frameLenSum: "<<_frameLenSum<<" totalLenSum: "<<_totalMonoAudioDataLengthSum<<"]"<<endl;
  }
  return frameLen;
}

// mode: 1 mono, 2: stereo
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
      /* convert from signed byte -128 .. 127
         to unsigned byte 0 .. 255
      */
      int8_t audioByte0=(int8_t)_sndFile.get();
      int8_t audioByte1=(int8_t)audioByte0+128;
      UBYTE audioByte2=(UBYTE)audioByte1;
      audioByteSequence->add(audioByte2);
    }
    return audioByteSequence;
  }
  case 2: {
    ByteSequence* audioByteSequence=new ByteSequence();
    // reshuffle bytes for stero (ABABAB.. => AAA..BBB..)
    list<UBYTE> tmp;
    int dataLen=getMonoAudioDataLength();
    for(int j=0;j<dataLen*_audioMode;j++) {
      /* convert from signed byte -128 .. 127
	 to unsigned byte 0 .. 255
      */
      int8_t audioByte0=(int8_t)_sndFile.get();
      int8_t audioByte1=(int8_t)audioByte0+128;
      UBYTE audioByte2=(UBYTE)audioByte1;
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
    cerr<<"Error: unsupported audio mode in CDXL generation (mode:"<<_audioMode<<")"<<endl;
    exit(1);
  }
  // not reachable
  assert(false);
}

void CDXLEncode::addColorsForTargetPlanes(int targetPlanes, IffCMAPChunk* cmapChunk) {
  // fill up colors to 2^fixedPlanes
  ULONG targetColors=Util::ULONGPow(2,static_cast<ULONG>(targetPlanes));
  ULONG currentNumColors=cmapChunk->numberOfColors();
  if(targetColors>currentNumColors) {
    cout<<"[colors:"<<currentNumColors<<"->"<<targetColors<<"] ";
    ULONG colorsToAdd=targetColors-currentNumColors;
    for(ULONG i=0;i<colorsToAdd;i++) {
      cmapChunk->addColor(RGBColor(0,0,0));
    }
  }
}

void CDXLEncode::addColorsForTargetPlanes(int targetPlanes, CDXLPalette& palette) {
  // fill up colors to 2^fixedPlanes
  ULONG targetColors=Util::ULONGPow(2,static_cast<ULONG>(targetPlanes));
  ULONG currentNumColors=palette.numberOfColors();
  if(targetColors>currentNumColors) {
    cout<<" [colors:"<<currentNumColors<<"->"<<targetColors<<"] ";
    ULONG colorsToAdd=targetColors-currentNumColors;
    for(ULONG i=0;i<colorsToAdd;i++) {
      palette.addColor(RGBColor(0,0,0));
    }
  }
}


void CDXLEncode::importILBMChunk(CDXLFrame& frame, IffILBMChunk* ilbmChunk) {
  // (i) convert iff file into a CDXL block
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
    cerr<<"Error: "<<errorChunk<<" chunk is missing."<<endl;
    exit(1);
  }

  frame.header.initialize(bmhdChunk,cmapChunk,camgChunk);
  if((!options.quiet)||options.debug) {
    cout<<".. encoding CDXL frame "<<frame.header.getCurrentFrameNr();
    cout<<" ("<<cmapChunk->numberOfColors()<<" colors, "<<+bmhdChunk->getNumPlanes()<<" bitplanes)";
  }

  if(options.debug) {
    cout<<"DEBUG: Before importing colors for frame "<<_currentFrameNr<<endl;
    cout<<"DEBUG: ILBM Palette size: "<<cmapChunk->getDataSize()<<endl;
    cout<<"DEBUG: CDXL Palette size: "<<frame.palette.getLength()<<endl;
  }

  frame.palette.importColors(cmapChunk);
  //cout<<"ILBM Palette: "<<cmapChunk->paletteToString()<<endl;
  //cout<<"CDXL Palette: "<<frame.palette.toString()<<endl;
  if(options.debug) {
    cout<<"DEBUG: After importing colors for frame "<<_currentFrameNr<<endl;
    cout<<"DEBUG: ILBM Palette size: "<<cmapChunk->getDataSize()<<endl;
    cout<<"DEBUG: CDXL Palette size: "<<frame.palette.getLength()<<endl;
  }

  assert(ilbmChunk);
  frame.importVideo(ilbmChunk);
  assert(frame.video);

  // fill color palette and video data with fill data ensure fixed frame size, update header
  if(options.fixedPlanes>0) {
    if(options.debug) cout<<"DEBUG: fixed planes: "<<options.fixedPlanes<<endl;
    uint32_t oldNumPlanes=(uint32_t)frame.header.getNumberOfBitplanes();
    if(options.fixedPlanes<oldNumPlanes) {
      cout<<"\n\nError: fixed number of "<<options.fixedPlanes<<" planes requested is lower than "<<oldNumPlanes<<" planes in input frame. '--optimize no' with ffmpeg generated frames selected? Bailing out. "<<endl;
      exit(1);
    }
    // add colors may also complete a plane's colors up to 2^fixedPlanes
    addColorsForTargetPlanes(options.fixedPlanes,frame.palette); // does not update numPlanes in BMHD
    // update palettesize in header
    frame.header.setPaletteSize(frame.palette.numberOfColors()*frame.header.getColorBytes());
    if(options.fixedPlanes>oldNumPlanes) {
      cout<<"[planes:"<<oldNumPlanes<<"->"<<options.fixedPlanes<<"] ";
      // determine plane size
      UWORD lineLengthInBytes=Util::wordAlignedLengthInBytes(frame.header.getVideoWidth());
      UWORD height=frame.header.getVideoHeight();
      ULONG planeSize=(ULONG)lineLengthInBytes*(ULONG)height;
      if(oldNumPlanes<options.fixedPlanes) {
        ULONG planesToAdd=options.fixedPlanes-oldNumPlanes;
        // add all required empty planes at once
        if(options.debug) cout<<"\nDEBUG: ADDING: planes:"<<planesToAdd<<" planesize: "<<planeSize<<" total: "<<planeSize*planesToAdd<<endl;
        ULONG zeroDataToAdd=planeSize*planesToAdd;
        for(ULONG i=0;i<zeroDataToAdd;i++) {
          frame.video->add(0);
        }
        // update numberOfBitplanes
        assert(frame.header.getNumberOfBitplanes()+planesToAdd==options.fixedPlanes);
        frame.header.setNumberOfBitplanes(options.fixedPlanes);
        assert(frame.video->getDataSize()==frame.header.getNumberOfBitplanes()*planeSize);
      }
    }
  }
  if((!options.quiet)||options.debug) {
    cout<<endl;
  }
}

// set all options in CDXL header
void CDXLEncode::importOptions(CDXLFrame& frame) {
  frame.header.setColorBitsFlag(_24BitColors);
  
  // padding size 2 to reproduce results
  frame.setPaddingSize(options.paddingSize);
  
  if(_24BitColors) {
    // use CUSTOM file type if 24 bit color mode is used, otherwise keep STANDARD (initialized)
    //frame.header.setFileType(CUSTOM);
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
    cerr<<"Internal error: wrong audio mode: "<<_audioMode<<endl; 
    exit(1);
  }
  // AnimFx only supports: BIT_PLANAR
  frame.header.setPlaneArrangement(BIT_PLANAR);
  // set frequency (CDXL extension)
  frame.header.setFrequency(_frequency);
  frame.header.setFps(_fps);
  frame.header.setPaddingModes(_paddingModes);
  if(options.stdCdxl)
    frame.header.setFileType(STANDARD);
  else
    frame.header.setFileType(CUSTOM);
}

void CDXLEncode::importAudio(CDXLFrame& frame) {
  // audio mode is set in run method
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
  if(!options.quiet) {
    cout<<"Loading: png file "<<pngFileName;
    cout<<" ";
  }
  PngLoader pngLoader;
  pngLoader.readFile(pngFileName);

  if(options.optimize) {
    pngLoader.optimizePngPalette();
  }

  IffILBMChunk* ilbmChunk=pngLoader.createILBMChunk();
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

  // special case: KILL EHB flag, can only be set now, after ILBM has been imported
  if(IffCAMGChunk* iffCAMGChunk=ilbmChunk->getCAMGChunk()) {
    if(frame.header.getNumberOfBitplanes()==6 && !iffCAMGChunk->isHalfBrite() && !iffCAMGChunk->isHam()) {
      // required for AGA with 6 planes to not display EHB mode by default
      frame.header.setKillEHBFlag(true); 
    }
  } else {
    cout<<"Error: no CAMG chunk found. Bailing out."<<endl;
    exit(1);
  }

  // set all audio relevant values in CDXL frame
  importAudio(frame);
  
  // compute frame size and set (check if 1st frame)
  ULONG frameSize=frame.getLength();
  frame.header.setCurrentChunkSize(frameSize);
  if(options.debug) {
    cout<<"DEBUG: frame "<<_currentFrameNr<<": setting current header chunksize = "<<frameSize<<endl;
  }
  if(_currentFrameNr==1) {
    frame.header.setPreviousChunkSize(0);
  } else {
    frame.header.setPreviousChunkSize(_previousFrameSize);
    _previousFrameSize=frameSize;
  }

  if(_writeFile) {
    frame.setOutFile(&_outFile);
    frame.writeChunk();
  }
  delete &frame;
  _currentFrameNr++;
}

void CDXLEncode::postVisitLastILBMChunk(IffILBMChunk* ilbmChunk) {
  // close CDXL file
  if(!options.quiet) {
    cout<<"Encoding finished."<<endl;
  }
  FileSequenceConversion::postVisitLastILBMChunk(ilbmChunk);
  _outFile.close();
  if(!options.quiet) {
    cout<<"Generated file "<<options.outFileName<<endl;
  }
}
