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


#include "CommandLineParser.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "Util.hpp"
#include <map>
#include "Options.hpp"

using namespace std;
using namespace Util;

void CommandLineParser::inc() {
  if(argi>=argc) {
    cerr<<"Error: command line error."<<endl;
    exit(1);
  } else {
    argi++;
  }
}

bool CommandLineParser::option(string option) {
  return argv[argi]==string("--")+option;
}

static bool isAnimFileName(string s) {
  string fileExtension=Util::fileNameExtension(s);
  return hasPrefix("anim",fileExtension)
    ||hasPrefix("sndanim",fileExtension)
    ||hasPrefix("fxanim",fileExtension)
    ;
}

static bool isCdxlFileName(string s) {
  string fileExtension=Util::fileNameExtension(s);
  return hasPrefix("cdxl",fileExtension)
    ||hasPrefix("xl",fileExtension)
    ;
}

void CommandLineParser::setVersion(std::string version) {
  this->version=version;
}

Options CommandLineParser::parse(int argc0, char **argv0) {
  if(version.size()==0) {
    cerr<<"Internal error: program version not set."<<endl;
    exit(1);
  }
  this->argc=argc0;
  this->argv=argv0;
  string commandName=argv[0];
  string helpText="Usage: "+commandName+" [--quiet] [--status] [--cdxl-info] [--anim-chunk-info] [--ilbm-info] [--detailed] ";
  helpText+="[--cdxl-encode] [--cdxl-info-all] ";
  helpText+="[--frequency NUMBER] [--color-bits NUMBER] [--anim-play-rate NUMBER] [--sndfile PCMFILE | --pcm-file PCMFILE] [--audio-data-type u8|s8] [--audio-mode mono|stereo] [--gfx-mode lores|hires|superhires] [--inject-dpan] [--no-anim-padding-fix] [--cdxl-padding-size NUMBER] [--optimize yes|no] [--std-cdxl] [--fixed-planes NUM] [--version] INFILE [OUTFILE]";
  if(argc <= 1) {
    cout << helpText <<endl;
    exit(0);
  }
  Options options;
  argi=1;
  int fileNames=0;
  while(argi<argc) {
    stringstream ss;
    if(option("help")) {
      cout << helpText <<endl;
      exit(0);
    } else if(option("version")) {
      cout<<"agaconv-encode version "<<version<<endl;
      cout<<"Copyright (C) 2019-2021 Markus Schordan"<<endl;
      cout<<"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>."<<endl;
      cout<<"This is free software: you are free to change and redistribute it."<<endl;
      cout<<"There is NO WARRANTY, to the extent permitted by law."<<endl;
      cout<<"Written by Markus Schordan."<<endl;
      exit(0);
    } else if(option("cdxl-info")) {
      options.cdxlInfo=true;
    } else if(option("cdxl-encode")) {
      options.cdxlEncode=true;
      options.writeCdxl=true;
    } else if(option("cdxl-info-all")) {
      options.cdxlDecode=true;
    } else if(option("color-bits")) {
      inc();
      int colBits=std::stoi(argv[argi]);
      switch(colBits) {
      case 12:
        options.colorSize=Options::COL_12BIT;
        break;
      case 24:
        options.colorSize=Options::COL_24BIT;
        break;
      default:
        cerr<<"Error: unsupported color format of "<<colBits<<" bits selected."<<endl;
        exit(1);
      }
    } else if(option("ilbm-info")) {
      options.ilbmInfo=true;
    } else if(option("detailed")) {
      options.detailed=true;
    } else if(option("status")) {
      options.status=true;
    } else if(option("quiet")) {
      options.quiet=true;
    } else if(option("anim-chunk-info")) {
      options.chunkInfo=true;
      options.readAnim=true;
    } else if(option("no-anim-padding-fix")) {
      options.paddingFix=false;
    } else if(option("optimize")) {
      inc();
      string option=argv[argi];
      if(option=="yes") {
        options.optimize=true;
      } else if (option=="no") {
        options.optimize=false;
      } else {
        cerr<<"Error: unknown selection "<<option<<" for --optimize."<<endl;
        exit(1);
      }
    } else if(option("fps")) {
      inc();
      options.fps=std::stol(argv[argi]);
    } else if(option("fixed-planes")) {
      inc();
      options.fixedPlanes=std::stoi(argv[argi]);
    } else if(option("cdxl-padding-size")) {
      inc();
      options.paddingSize=std::stol(argv[argi]);
      switch(options.paddingSize) {
      case 0: options.paddingMode=Options::PAD_UNSPECIFIED;break;
      case 1: options.paddingMode=Options::PAD_NONE;break;
      case 2: options.paddingMode=Options::PAD_16BIT;break;
      case 4: options.paddingMode=Options::PAD_32BIT;break;
      case 8: options.paddingMode=Options::PAD_64BIT;break;
      default:
        cerr<<"Error: unsupported padding size: "<<options.paddingSize<<endl;
        exit(1);
      }
    } else if(option("std-cdxl")) {
      options.stdCdxl=true;
    } else if(option("audio-mode")) {
      inc();
      string audioMode=argv[argi];
      if(audioMode=="stereo") {
        options.stereo=true;
      } else if(audioMode=="mono") {
        options.stereo=false;
      } else {
        cerr<<"Error: unknown audio mode: "<<audioMode<<endl;
        exit(1);
      }
    } else if(option("audio-mode")) {
      options.stereo=false;
    } else if(option("inject-dpan")) {
      options.injectDPANChunk=true;
    } else if(option("snd-file")||option("pcm-file")) {
      inc();
      options.sndFileName=argv[argi];
      options.audioDataType=AUDIO_DATA_TYPE_UBYTE;
      cout<<"Audio file name:"<<options.sndFileName<<endl;
    } else if(option("audio-data-type")) {
      inc();
      string adtOption=argv[argi];
      if(adtOption=="u8") {
        options.audioDataType=AUDIO_DATA_TYPE_UBYTE;
      } else if(adtOption=="s8") {
        options.audioDataType=AUDIO_DATA_TYPE_SBYTE;
      } else {
        cerr<<"Error: unknown audio data type option "<<adtOption<<endl;
        exit(1);
      }
    } else if(option("frequency")) {
      inc();
      options.frequency=std::stol(argv[argi]);
    } else if(option("anim-play-rate")) {
      inc();
      options.playRate=std::stol(argv[argi]);
    } else if(option("gfx-mode")) {
      inc();
      typedef std::map<string,Options::GFX_RESOLUTION> ResMapType;
      ResMapType resModeMap={
        {"unspecified",Options::GFX_UNSPECIFIED},
        {"lores",Options::GFX_LORES},
        {"hires",Options::GFX_HIRES},
        {"superhires",Options::GFX_SUPERHIRES},
        {"ultrahires",Options::GFX_ULTRAHIRES}
      };
      string modeName=argv[argi];
      ResMapType::iterator i=resModeMap.find(modeName);
      if(i!=resModeMap.end()) {
        options.resMode=(*i).second;
      } else {
        cerr<<"Error: unknown resolution mode: "<<modeName<<endl;
        exit(1);
      }
    } else if(option("debug")) {
      options.debug=true;
    } else {
      switch(fileNames) {
      case 0: options.inFileName=argv[argi];fileNames++;break;
      case 1: options.outFileName=argv[argi];fileNames++;break;
      default: 
        cerr<<"Error: too many file names on command line ("<<fileNames<<")"<<endl;
        cerr<<"inFile: "<<options.inFileName<<endl;
        cerr<<"outFile: "<<options.outFileName<<endl;
        cerr<<"current file name: "<<argv[argi]<<endl;
        exit(1);
      }
      if(options.inFileName[0]=='-') {
        cerr<<"Error: expected file name, option "<<options.inFileName<<" provided."<<endl;
        exit(1);
      }
    }
    inc();
  }

  // derive options from filenames
  if(options.hasInFile()) {
    if(isAnimFileName(options.inFileName)) {
      options.readAnim=true;
    } else if(isCdxlFileName(options.inFileName)) {
      options.readCdxl=true;
    }
  }
  if(options.hasOutFile()) {
    if(isAnimFileName(options.outFileName)) {
      options.writeAnim=true;
    } else if(isCdxlFileName(options.outFileName)) {
      options.writeCdxl=true;
    }
  }
  if(options.sndFileName) {
    options.writeAnim=true;
  }
  
  // perform consistency check of options
  if(!options.checkConsistency()) {
    cout<<"Error: inconsistent command line options."<<endl;
    exit(1);
  }
  if(options.optimize==false) {
    cout<<"Note: disabling empty bitplane filtering."<<endl;
  }
  return options;
}
