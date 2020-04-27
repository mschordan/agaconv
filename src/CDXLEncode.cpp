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
    //cout<<"DEBUG1: _resolutionModes="<<_resolutionModes<<endl;
    _paddingModes=(UBYTE)options.paddingMode;
    
    _frequency=options.frequency;
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

int CDXLEncode::getMonoAudioDataLength(int frameNr) {
  // available are: _currentFrameNr, _totalAudioDataLength, _fps, _frequency
  // imprecise:  _monoAudioDataLength=options.frequency/options.fps;  
  int totalMonoAudioDataLength=_totalAudioDataLength/_audioMode;
  // need total number of frames, then it's simple; or even it out within each second
  int frameNrWithinSec=((_currentFrameNr-1) % options.fps)+1;
  double dFrameLen=(double)options.frequency/(double)options.fps;
  if(options.debug) {
    cout<<"FrameNrSec: "<<frameNrWithinSec<<" TotalAudioLen: "<<totalMonoAudioDataLength<<" fps: "<<options.fps<<" dFrameLen: "<< dFrameLen<<endl;
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
      if(frameNr%2==1)
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
    cout<<"DEBUG: Frame: "<<_currentFrameNr<<" length: "<<frameLen<<" [frameLenSum: "<<_frameLenSum<<"totalLenSum: "<<_totalMonoAudioDataLengthSum<<"]"<<endl;
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
        //DEL:cout<<"("<<(int)audioByte0<<","<<(int)audioByte1<<","<<(unsigned int)audioByte2<<")";
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

void CDXLEncode::importILBMChunk(CDXLFrame& frame, IffILBMChunk* ilbmChunk) {
  // (i) convert iff file into a CDXL block
  IffBMHDChunk* bmhdChunk=dynamic_cast<IffBMHDChunk*>(ilbmChunk->getChunkByName("BMHD"));
  IffCAMGChunk* camgChunk=dynamic_cast<IffCAMGChunk*>(ilbmChunk->getChunkByName("CAMG"));
  IffCMAPChunk* cmapChunk=dynamic_cast<IffCMAPChunk*>(ilbmChunk->getChunkByName("CMAP"));
  IffBODYChunk* bodyChunk=dynamic_cast<IffBODYChunk*>(ilbmChunk->getChunkByName("BODY"));
  if(!(bmhdChunk&&camgChunk&&cmapChunk&&bodyChunk)) {
    cerr<<"Error: missing chunk."<<endl;
    exit(1);
  }
  ilbmChunk->uncompressBODYChunk();

  frame.header.initialize(bmhdChunk,cmapChunk,camgChunk);
  //cout<<"DEBUG1: modes.resolutionModes"<<+frame.header.modes.resolutionModes<<endl;

  //cout<<"CDXLHeader: "<<frame.header.toString()<<endl;
  if((!options.quiet)||options.debug) {
    cout<<".. encoding CDXL frame "<<frame.header.getCurrentFrameNr();
    cout<<" ("<<cmapChunk->numberOfColors()<<" colors, "<<+bmhdChunk->getNumPlanes()<<" bitplanes)";
    cout<<endl;
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
}

// set all options in CDXL header
void CDXLEncode::importOptions(CDXLFrame& frame) {
  frame.header.setColorBitsFlag(_24BitColors);
  
  // padding size 2 to reproduce results
  frame.setPaddingSize(options.paddingSize);
  
  if(_24BitColors) {
    // use CUSTOM file type if 24 bit color mode is used, otherwise keep STANDARD (initialized)
    frame.header.setFileType(CUSTOM);
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
  //cout<<"DEBUG2: modes.resolutionModes"<<+frame.header.modes.resolutionModes<<":"<<+frame.header.modes.getUBYTE()<<endl;
  //frame.header.setResolutionModes(_resolutionModes);
  //cout<<"DEBUG3: modes.resolutionModes"<<+frame.header.modes.resolutionModes<<":"<<+frame.header.modes.getUBYTE()<<endl;
  frame.header.setPaddingModes(_paddingModes);
  //cout<<"DEBUG4: modes.resolutionModes"<<+frame.header.modes.resolutionModes<<":"<<+frame.header.modes.getUBYTE()<<endl;

}

void CDXLEncode::importAudio(CDXLFrame& frame) {
  // audio mode is set in run method
  frame.audio=readAudioData();

  // is overridden later if stereo (div 2)
  //frame.header.setAudioSize(frame.audio->getDataSize());

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

  IffILBMChunk* ilbmChunk=pngLoader.createILBMChunk();
  //cout<<"DEBUG: next: visitILBMChunk."<<endl;
  if(ilbmChunk) {
    visitILBMChunk(ilbmChunk);
    //cout<<"DEBUG: visitPngFile done."<<endl;
    delete ilbmChunk;
  }
}


void CDXLEncode::visitILBMChunk(IffILBMChunk* ilbmChunk) {
  // (ii) insert block into CDXL file.
  // (iii) write CDXL block
  CDXLFrame& frame=*new CDXLFrame();
  importOptions(frame); // sets values in header from command line options
  frame.header.setFrameNr(_currentFrameNr);
  importILBMChunk(frame,ilbmChunk);
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
    frame.header.setPreviousChunkSize(_previousFrameSize); // TODO: not the previous frame size
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
