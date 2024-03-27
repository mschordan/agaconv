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

#include "StageAnimEdit.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "AGAConvException.hpp"
#include "CommandLineParser.hpp"
#include "IffANIMForm.hpp"
#include "IffDPANChunk.hpp"
#include "IffILBMChunk.hpp"
#include "IffSBDYChunk.hpp"
#include "IffSXHDChunk.hpp"
#include "StageAnimEdit.hpp"

using namespace std;

namespace AGAConv {

StageAnimEdit::StageAnimEdit() {
}

void StageAnimEdit::run(Options& options) {
  int numOfSBDYInserts=0;
  if(options.hasOutFile()) {
    cout << "Reading ANIM file \"" << options.inFileName << "\"."<<endl;
    // Open file
    fstream inFile0;
    fstream* inFile=&inFile0;
    inFile->open(options.inFileName, ios::in | ios::binary);
    if(inFile->is_open() == false) {        
      throw AGAConvException(160, "cannot open file "+options.inFileName.string());
    }
    fstream outFile0;
    fstream* outFile=&outFile0;
    outFile->open(options.outFileName, ios::out | ios::binary);
    if(outFile->is_open() == false) {        
      throw AGAConvException(161, "cannot open output file "+options.outFileName.string());
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
        throw AGAConvException(162, "no ILBM frame found.");
      }
      bool success=firstFrame->insertBeforeBODYChunk(dpanChunk);
      if(success) {
        anim.setDataSize(anim.getDataSize()+dpanChunk->getTotalChunkSize());
        dpanChunk->setLongToString(true);
        cout<<"Inserted DPAN chunk:"<<endl;
        cout<<dpanChunk->toString();
      } else {
        throw AGAConvException(163, "could not find BODY chunk.");
      }
    }
    if(options.hasSndFile()) {
      if(options.verbose>=1) cout<<"Adding sound data to ANIM file."<<endl;
      fstream sndFile0;
      fstream* sndFile=&sndFile0;
      sndFile->open(options.sndFileName.c_str(), ios::in | ios::binary);
      if(sndFile->is_open() == false) {        
        throw AGAConvException(164, "cannot open snd file "+options.sndFileName.string());
      }
      // Compute snd data size per frame (stereo)
      int mode=(options.stereo?2:1); // stereo=2, mono=1
      int SXHDLength=options.frequency/options.fps;
      if(options.playRate==0) {
        options.playRate=(ULONG)(options.systemTimingConstant/options.frequency);
      }
      if(options.verbose>=1) cout<<"AnimEdit: SOUND: frequency="<<options.frequency<<" playrate:"<<options.playRate<<" fps="<<options.fps<<" sounddata per frame: "<<SXHDLength*mode<<" stereo"<<endl;
      // Create SXHD
      IffSXHDChunk* sxhd=new IffSXHDChunk(8,SXHDLength,options.playRate,3,mode,options.frequency);
      sxhd->setOutFile(outFile);
      IffILBMChunk* firstFrame=dynamic_cast<IffILBMChunk*>(anim.getFirstChunk());
      if(!firstFrame) {
        throw AGAConvException(165, "no ILBM frame found.");
      }
      bool success=firstFrame->insertBeforeBODYChunk(sxhd);
      if(success) {
        anim.setDataSize(anim.getDataSize()+sxhd->getTotalChunkSize());
        sxhd->setLongToString(true);
        if(options.verbose>=2) cout<<sxhd->toString()<<endl;
        if(options.verbose>=1) cout<<"Inserted SXHD chunk."<<endl;
      } else {
        throw AGAConvException(166, "could not find BODY chunk.");
      }
      // Generate sound data in each frame
      if(options.verbose>=1) cout<<"Found "<<anim.numberOfChunks()<<" ILBM frames."<<endl;
      for(size_t i=0;i<anim.numberOfChunks();i++) {
        //cout<<"Adding sound data to frame "<<i<<"."<<endl;
        IffSBDYChunk* sndBody=new IffSBDYChunk();
        sndBody->setOutFile(outFile);
        switch(options.audioDataType) {
          // https://trac.ffmpeg.org/wiki/audio%20types
        case AUDIO_DATA_TYPE_UBYTE: {
          // Reshuffle bytes for stero (ABABAB.. => AAA..BBB..)
          list<UBYTE> tmp;
          for(int j=0;j<SXHDLength*mode;j++) {
            // Convert from signed byte -128 .. 127 (reading unsigned bytes)
            // to unsigned byte 0 .. 255
            int8_t soundByte0=(int8_t)sndFile->get();
            int8_t soundByte1=(int8_t)soundByte0+128;
            UBYTE soundByte2=(UBYTE)soundByte1;
            if(j%2==0) {
              sndBody->add(soundByte2);
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
          // Reshuffle bytes for stero (ABABAB.. => AAA..BBB..)
          list<UBYTE> tmp;
          for(int j=0;j<SXHDLength*mode;j++) {
            // Convert from signed byte -128 .. 127
            // to unsigned byte 0 .. 255
            int8_t soundByte0=(int8_t)sndFile->get();
            UBYTE soundByte2=(UBYTE)soundByte0;
            if(j%2==0) {
              sndBody->add(soundByte2);
            } else {
              tmp.push_back(soundByte2);
            }
          }
          for(UBYTE soundByte : tmp) {
            sndBody->add(soundByte);
          }
          break;
#else
          // Currently not used
          throw AGAConvException(167, "PCM with signed bytes not supported.")
#endif
        }
        default:
          throw AGAConvException(168, "unsupported PCM type"+std::to_string(options.audioDataType)+".");
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
        } else {
          stringstream ss;
          ss<<"\nError: could not find "<<searchfor<<" chunk in frame"<<i<<"."<<endl;
          throw AGAConvException(169, ss.str());
        }
      }
      // Finished with reading sound data
      sndFile->close();
    }
    if(options.hasSndFile()) {
      cout<<"Inserted "<<numOfSBDYInserts<<" SBDY chunks in "<<anim.numberOfChunks()<<" ILBM chunks."<<endl;
    }
    anim.writeChunk();

    inFile->close();
    outFile->flush();
    outFile->close();

    cout<<"Generated ANIM file "<<options.outFileName<< " with "<<anim.numberOfChunks()<<" ILBM chunks."<<endl;
  }
}

} // namespace AGAConv
