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

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>

#include "Options.hpp"
#include "FileSequenceConversion.hpp"
#include "IffILBMChunk.hpp"
#include "IffUnknownChunk.hpp"
#include "IffCMAPChunk.hpp"
#include "IffCAMGChunk.hpp"
#include <cassert>
#include "Util.hpp"

using namespace std;

FileSequenceConversion::FileSequenceConversion()
{
}

FileSequenceConversion::~FileSequenceConversion()
{
}

size_t FileSequenceConversion::initFileName(string inFilePattern) {

  // search for number in reverse from last char of string
  if(inFilePattern.size()==0)
    throw AGAConvException("Error: no number pattern in file name.");
  size_t found=inFilePattern.size()-1;
  while(!isdigit(inFilePattern[found])) found--;
  size_t endPos=found;
  if (!(endPos!=std::string::npos)) {
    throw AGAConvException("Detection of number pattern in file name failed.");
  }
  found=endPos;
  while(isdigit(inFilePattern[found])) {
    found--;
  }
  size_t startPos=found+1;
  if(startPos>endPos) {
    throw AGAConvException("Detection of number pattern in file name failed.");
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

void FileSequenceConversion::run(Options& optionsIn) {
  options=optionsIn;
  inFileName=options.inFileName;
  firstInFileName=inFileName;
  initFileName(inFileName);
  enum FileType fileType=determineFileType(inFileName);
  frames=0;
  preVisitFirstFrame();
  IffILBMChunk* lastILBMChunk=nullptr;
  while(true) {
    if(options.debug) cout<<"DEBUG: Reading "<<inFileName<<endl;
    // TODO generic: test if file exists (exit based on this property)
    // then check file and dispatch to iff or png
    if(!Util::fileExists(inFileName))
      break;
    if(lastILBMChunk) {
      delete lastILBMChunk;
    }
    switch(fileType) {
    case FILE_IFF: {
      if(!options.quiet) {
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
      throw AGAConvException(string("Error: unknown file typ of "+inFileName));
    }
    frames++;
    inFileName=nextFileName();
  };

  // the number of frames must be >0
  if(frames==0) {
    throw AGAConvException(string("Error: Could not find file ")+inFileName);
  }
  postVisitLastILBMChunk(lastILBMChunk);
  delete lastILBMChunk;
}

void FileSequenceConversion::preVisitFirstFrame() {
  // empty by default
}

void FileSequenceConversion::visitILBMChunk(IffILBMChunk* ilbmChunk) {
  if(options.status) {
    cout<<inFileName<<" frame "<<frames+1;
    cout<<endl;
  }
}

void FileSequenceConversion::visitPngFile(std::string pngFileName) {
  if(options.status) {
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

      cout<<"First frame file: "<<firstInFileName<<endl;
      cout<<"Last frame file: "<<inFileName<<endl;
      cout<<"Processed "<<frames+1<<" frames."<<endl;
      cout<<"Last frame format: ";
      if(IffCAMGChunk* camgChunk=dynamic_cast<IffCAMGChunk*>(ilbmChunk->getChunkByName("CAMG"))) {
        bool ham=camgChunk->isHam();
        bool hires=camgChunk->isHires();
        bool superhires=camgChunk->isSuperHires();
        if(ham)
          cout<<"HAM";
        else
          cout<<"AGA";
        cout<<+numPlanes<<" ";
        if(superhires)
          cout<<"SUPERHIRES";
        else if(hires)
          cout<<"HIRES";
        else
          cout<<"LORES";
      } else {
        cout<<+numPlanes<<" planes";
        cout<<" "<<xdim<<"x"<<ydim<<" (no CAMG chunk found)"<<endl;
      }
      cout<<endl;
    } else {
      cerr<<endl<<"Error: no BMHD chunk found in ILBM IFF file."<<endl;
      exit(1);
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
