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

#include "StageAnimEdit.hpp"
#include <iostream>
#include <cstdlib>
#include "IffANIMForm.hpp"
#include "IffILBMChunk.hpp"
#include "IffDPANChunk.hpp"
#include "IffSXHDChunk.hpp"
#include "IffSBDYChunk.hpp"
#include "CommandLineParser.hpp"
#include "Stage.hpp"
#include "StageAnimEdit.hpp"
#include "AGAConvException.hpp"

using namespace std;

StageAnimEdit::StageAnimEdit() {
}

void StageAnimEdit::run(Options& options) {
  int numOfSBDYInserts=0;
  if(options.hasOutFile()) {
    cout << "Reading ANIM file \"" << options.inFileName << "\"."<<endl;
    //open file
    fstream inFile0;
    fstream* inFile=&inFile0;
    inFile->open(options.inFileName, ios::in | ios::binary);
    if(inFile->is_open() == false) {        
      cerr<<"Error: cannot open file "<<options.inFileName<<endl;
      throw AGAConvException();
    }
    fstream outFile0;
    fstream* outFile=&outFile0;
    outFile->open(options.outFileName, ios::out | ios::binary);
    if(outFile->is_open() == false) {        
      cerr<<"Error: cannot open output file "<<options.outFileName<<endl;
      throw AGAConvException();
    }
    IffANIMForm anim;
    anim.setFile(inFile);
    anim.setOutFile(outFile);
    anim.readChunk();
    if(IffChunk::debug) cout<<"DEBUG: entire file successfully read."<<endl;
    if(IffChunk::debug) cout<<"DEBUG: all info printed."<<endl;
    if(options.injectDPANChunk) {
      UWORD nframes=anim.numberOfChunks();
      IffChunk* dpanChunk=new IffDPANChunk(nframes);
      dpanChunk->setOutFile(outFile);
      IffILBMChunk* firstFrame=dynamic_cast<IffILBMChunk*>(anim.getFirstChunk());
      if(!firstFrame) {
        cerr<<"Error: no ILBM frame found."<<endl;
        exit(1);
      }
      bool success=firstFrame->insertBeforeBODYChunk(dpanChunk);
      if(success) {
        anim.setDataSize(anim.getDataSize()+dpanChunk->getTotalChunkSize());
        dpanChunk->setLongToString(true);
        cout<<"Inserted DPAN chunk:"<<endl;
        cout<<dpanChunk->toString();
      } else {
        cout<<"Error: could not find BODY chunk."<<endl;
        exit(1);
      }
    }
    if(options.hasSndFile()) {
      cout<<"Adding sound data."<<endl;
      fstream sndFile0;
      fstream* sndFile=&sndFile0;
      sndFile->open(options.sndFileName, ios::in | ios::binary);
      if(sndFile->is_open() == false) {        
        cerr<<"Error: cannot open snd file "<<options.sndFileName<<endl;
        throw AGAConvException();
      }
      // compute snd data size per frame (stereo)
      int mode=2; // stereo=2, mono=1
      int SXHDLength=options.frequency/options.fps;
      // create sound for stereo 22050, fps 25
      if(options.playRate==0) {
        options.playRate=(ULONG)(options.systemTimingConstant/options.frequency); // PAL
      }
      cout<<"SOUND: frequency="<<options.frequency<<" playrate:"<<options.playRate<<" fps="<<options.fps<<" sounddata per frame: "<<SXHDLength*mode<<" stereo"<<endl;
      // create SXHD
      IffSXHDChunk* sxhd=new IffSXHDChunk(8,SXHDLength,options.playRate,3,mode,options.frequency);
      sxhd->setOutFile(outFile);
      IffILBMChunk* firstFrame=dynamic_cast<IffILBMChunk*>(anim.getFirstChunk());
      if(!firstFrame) {
        cerr<<"Error: no ILBM frame found."<<endl;
        exit(1);
      }
      bool success=firstFrame->insertBeforeBODYChunk(sxhd);
      if(success) {
        anim.setDataSize(anim.getDataSize()+sxhd->getTotalChunkSize());
        sxhd->setLongToString(true);
        cout<<sxhd->toString()<<endl;
        cout<<"Inserted SXHD chunk."<<endl;
      } else {
        cout<<"Error: could not find BODY chunk."<<endl;
        exit(1);
      }
      // generate sound data in each frame
      cout<<"Found "<<anim.numberOfChunks()<<" ILBM frames."<<endl;
      for(size_t i=0;i<anim.numberOfChunks();i++) {
        //cout<<"Adding sound data to frame "<<i<<"."<<endl;
        IffSBDYChunk* sndBody=new IffSBDYChunk();
        sndBody->setOutFile(outFile);
        switch(options.audioDataType) {
          // https://trac.ffmpeg.org/wiki/audio%20types
        case AUDIO_DATA_TYPE_UBYTE: {
          // reshuffle bytes for stero (ABABAB.. => AAA..BBB..)
          list<UBYTE> tmp;
          for(int j=0;j<SXHDLength*mode;j++) {
            /* convert from signed byte -128 .. 127 (reading unsigned bytes)
               to unsigned byte 0 .. 255
            */
            int8_t soundByte0=(int8_t)sndFile->get();
            int8_t soundByte1=(int8_t)soundByte0+128;
            UBYTE soundByte2=(UBYTE)soundByte1;
            if(j%2==0) {
              sndBody->add(soundByte2);
              //DEL:cout<<"("<<(int)soundByte0<<","<<(int)soundByte1<<","<<(unsigned int)soundByte2<<")";
            } else {
              tmp.push_back(soundByte2);
            }
          }
          for(UBYTE soundByte : tmp) {
            sndBody->add(soundByte);
          }
          break;
        }
        case AUDIO_DATA_TYPE_SBYTE: {
#if 1
          // reshuffle bytes for stero (ABABAB.. => AAA..BBB..)
          list<UBYTE> tmp;
          for(int j=0;j<SXHDLength*mode;j++) {
            /* convert from signed byte -128 .. 127
               to unsigned byte 0 .. 255
            */
            int8_t soundByte0=(int8_t)sndFile->get();
            UBYTE soundByte2=(UBYTE)soundByte0;
            if(j%2==0) {
              sndBody->add(soundByte2);
              //DEL:cout<<"("<<(int)soundByte0<<","<<(int)soundByte1<<","<<(unsigned int)soundByte2<<")";
            } else {
              tmp.push_back(soundByte2);
            }
          }
          for(UBYTE soundByte : tmp) {
            sndBody->add(soundByte);
          }
          break;
#else
          cerr<<"Error: PCM with signed bytes not supported."<<endl;
          exit(1);
#endif
        }
        default:
          cerr<<"Error: unsupported PCM type"<<options.audioDataType<<"."<<endl;
          exit(1);
        }

        IffILBMChunk* ilbm=anim.getChunk(i);
        bool success;
        string searchfor;
        if(i==0) {
          searchfor="BODY";
          success=ilbm->insertBeforeChunk(sndBody,searchfor);
          numOfSBDYInserts++;
        } else {
          searchfor="DLTA";
          success=ilbm->insertBeforeChunk(sndBody,searchfor);
          numOfSBDYInserts++;
        }
        if(success) {
          anim.setDataSize(anim.getDataSize()+sndBody->getTotalChunkSize());
          //cout<<"done."<<endl;
        } else {
          cout<<"\nError: could not find "<<searchfor<<" chunk in frame"<<i<<"."<<endl;
          exit(1);
        }
      }
      // finished with reading sound data
      sndFile->close();
    }
    if(options.hasSndFile()) {
      cout<<"Inserted "<<numOfSBDYInserts<<" SBDY chunks in "<<anim.numberOfChunks()<<" ILBM chunks."<<endl;
    }
    //cout<<"DEBUG: num ILBM2: "<<anim.numberOfChunks()<<endl;
    //anim.setDebug(true);
    anim.writeChunk();

    inFile->close();
    outFile->flush();
    outFile->close();

    cout<<"Generated ANIM file "<<options.outFileName<< " with "<<anim.numberOfChunks()<<" ILBM chunks."<<endl;
  }
}
