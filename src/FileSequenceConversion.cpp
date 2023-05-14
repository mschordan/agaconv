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

#include "FileSequenceConversion.hpp"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "AGAConvException.hpp"
#include "IffCAMGChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffILBMChunk.hpp"
#include "IffUnknownChunk.hpp"
#include "Options.hpp"
#include "Util.hpp"

using namespace std;

namespace AGAConv {

FileSequenceConversion::FileSequenceConversion()
{
}

FileSequenceConversion::~FileSequenceConversion()
{
}

size_t FileSequenceConversion::initFileName(string inFilePattern) {

  // search for number in reverse from last char of string
  if(inFilePattern.size()==0)
    throw AGAConvException(60, "no number pattern in file name.");
  size_t found=inFilePattern.size()-1;
  while(!isdigit(inFilePattern[found])) found--;
  size_t endPos=found;
  if (!(endPos!=std::string::npos)) {
    throw AGAConvException(61, "detection of number pattern in file name failed.");
  }
  found=endPos;
  while(isdigit(inFilePattern[found])) {
    found--;
  }
  size_t startPos=found+1;
  if(startPos>endPos) {
    throw AGAConvException(62, "Detection of number pattern in file name failed.");
  }

  // use found number as starting number
  string numberString=inFilePattern.substr(startPos,endPos-startPos+1);

  // since it is a fixed number length string, numberStart and numberEnd don't change
  startNumber=std::stoi(numberString);
  currentNumber=startNumber;
  fileNamePattern=inFilePattern;
  fileNamePatternReplacePos=startPos;
  length=endPos-startPos+1;
  return startNumber;
}

string FileSequenceConversion::nextFileName() {
  string newNumber=std::to_string(++currentNumber);
  size_t newNumberLength=newNumber.size();
  while(newNumberLength>length) {
    fileNamePattern.insert(fileNamePatternReplacePos,"_");
    length++;
    if(options.debug) cout<<"DEBUG: added one character to fileNamePattern: "<<fileNamePattern<<endl;
  }
  size_t offset=length-newNumberLength;
  fileNamePattern.replace(fileNamePatternReplacePos+offset,newNumberLength,newNumber);
  if(options.debug) cout<<"DEBUG: updated fileNamePattern: "<<fileNamePattern<<endl;
  return fileNamePattern;
}

IffILBMChunk* FileSequenceConversion::readIffFile(string fileName) {
  fstream inFile0;
  fstream* inFile=&inFile0;
  inFile->open(fileName, ios::in | ios::binary);
  if(inFile->is_open() == false) {        
    return 0;
  }
  IffILBMChunk* ilbm=new IffILBMChunk();
  ilbm->setFile(inFile);
  ilbm->readChunk();
  inFile->close();
  return ilbm;
}

void FileSequenceConversion::setInFileWithPath(std::string inFileNameWithPath) {
  inFileName=inFileNameWithPath;
  firstInFileName=inFileName;
  initFileName(inFileName);
}

void FileSequenceConversion::run(Options& optionsIn) {
  options=optionsIn;
  assert(inFileName!=""); // required to be set before run is called
  enum FileType fileType=determineFileType(inFileName);
  frames=0;
  preVisitFirstFrame();
  IffILBMChunk* lastILBMChunk=nullptr;
  while(true) {
    if(options.debug) cout<<"DEBUG: Reading "<<inFileName<<endl;
    if(!Util::fileExists(inFileName))
      break;
    if(lastILBMChunk) {
      delete lastILBMChunk;
    }
    switch(fileType) {
    case FILE_IFF: {
      if(options.verbose>=2) {
        cout<<"Loading: iff file "<<inFileName;
        cout<<" ";
      }
      IffILBMChunk* ilbmChunk=readIffFile(inFileName);
      visitILBMChunk(ilbmChunk);
      lastILBMChunk=ilbmChunk; // also used to deallocate after each iteration, except the very last iteration
      break;
    }
    case FILE_PNG: {
      visitPngFile(inFileName);
      break;
    }
    default:
      throw AGAConvException(63, "unknown file type: "+inFileName);
    }
    frames++;
    lastInFileName=inFileName; // remember previous file name for info message
    inFileName=nextFileName();
  };

  // the number of frames must be >0
  if(frames==0) {
    throw AGAConvException(64, "could not find file "+inFileName);
  }
  postVisitLastILBMChunk(lastILBMChunk);
  delete lastILBMChunk;
}

void FileSequenceConversion::preVisitFirstFrame() {
  // empty by default
}

void FileSequenceConversion::visitILBMChunk(IffILBMChunk* ilbmChunk) {
  if(options.debug) {
    cout<<inFileName<<" frame "<<frames+1;
    cout<<endl;
  }
}

void FileSequenceConversion::visitPngFile(std::string pngFileName) {
  if(options.debug) {
    cout<<inFileName<<" frame "<<frames+1;
    cout<<endl;
  }
}

void FileSequenceConversion::postVisitLastILBMChunk(IffILBMChunk* ilbmChunk) {
  if(ilbmChunk) {
    if(IffBMHDChunk* bmhdChunk=dynamic_cast<IffBMHDChunk*>(ilbmChunk->getChunkByName("BMHD"))) {
      UWORD xdim=bmhdChunk->getWidth();
      UWORD ydim=bmhdChunk->getHeight();
      UBYTE numPlanes=bmhdChunk->getNumPlanes();

      if(options.verbose>=1) {
        cout<<"Processed "<<frames<<" frames."<<endl;
      }
      if(options.verbose>=2) {
        cout<<"First frame file: "<<firstInFileName<<endl;
        cout<<"Last frame file: "<<lastInFileName<<endl;
        cout<<"Last frame format: ";
        if(IffCAMGChunk* camgChunk=dynamic_cast<IffCAMGChunk*>(ilbmChunk->getChunkByName("CAMG"))) {
          if(camgChunk->isHam())
            cout<<"HAM"<<+numPlanes<<" ";
          else if(camgChunk->isHalfBrite())
            cout<<"EHB ";
          else
            cout<<"Planes: "<<+numPlanes<<" ";;
          if(camgChunk->isSuperHires())
            cout<<"SUPERHIRES";
          else if(camgChunk->isHires())
            cout<<"HIRES";
          else
            cout<<"LORES";
          if(camgChunk->isLace())
            cout<<" LACE";
        } else {
          cout<<+numPlanes<<" planes";
          cout<<" "<<xdim<<"x"<<ydim<<" (no CAMG chunk found)"<<endl;
        }
        cout<<endl;
      }
    } else {
      throw AGAConvException(65, "\n No BMHD chunk found in ILBM IFF file.");
    }
  }
}

FileSequenceConversion::FileType FileSequenceConversion::determineFileType(std::string inFileName) {
  string ext=Util::fileNameExtension(inFileName);
  if(ext=="iff"||ext=="ilbm") {
    return FILE_IFF;
  } else if(ext=="png") {
    return FILE_PNG;
  } else {
    return FILE_UNKNOWN;
  }
}

} // namespace AGAConv
