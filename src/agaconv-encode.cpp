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

#include <iostream>
#include <cstdlib>
#include "IffANIMForm.hpp"
#include "IffILBMChunk.hpp"
#include "IffDPANChunk.hpp"
#include "IffSXHDChunk.hpp"
#include "IffSBDYChunk.hpp"
#include "CommandLineParser.hpp"

#include "StageAnimEdit.hpp"
#include "StageILBMFileInfo.hpp"
#include "StageChunkInfo.hpp"

#include "CDXLEncode.hpp"
#include "CDXLDecode.hpp"
#include "CDXLHeader.hpp"

using namespace std;

int main(int argc, char **argv) {
  string version="0.9.5";
  try {
    CommandLineParser clp;
    clp.setVersion(version);
    // options is a global variable
    options=clp.parse(argc,argv);
    Chunk::setDebug(options.verbose);

    ////////////////
    // CDXL files //
    ////////////////

    // CDXL encoding
    if(options.writeCdxl && options.cdxlEncode) {
      CDXLEncode stage;
      stage.run(options);
      return 0;
    }

    // cdxl file info for one file
    if(options.cdxlInfo) {
      // read just first frame's header
      // open file
      string fileName=string(options.inFileName);
      fstream inFile;
      inFile.open(fileName, ios::in | ios::binary);
      if(inFile.is_open() == false) {        
        cerr<<"Error: "<<fileName<<" not found."<<endl;
        return 1;
      }
      CDXLHeader cdxlHeader;
      cdxlHeader.setFile(&inFile);
      cdxlHeader.readChunk();
      cout<<cdxlHeader.toString();
      inFile.close();
      return 0;
    }

    // CDXL file header printing
    if(options.cdxlDecode) {
      CDXLDecode stage;
      stage.run(options);
      return 0;
    }

    ////////////////
    // ANIM files //
    ////////////////

    // regenerate original file (for testing) or also add sound data if present
    if(options.writeAnim && options.hasOutFile() &&!options.writeCdxl) {
      StageAnimEdit stage;
      stage.run(options);
      return 0;
    }

    // ilbm file info for one file
    if(options.ilbmInfo) {
      StageILBMFileInfo stage;
      stage.run(options);
      return 0;
    }

    // anim chunk info option
    if(options.chunkInfo) {
      StageChunkInfo stage;
      stage.run(options);
      return 0;
    }

  } catch(AGAConvException e) {
    cerr<<e.what()<<endl;
    return 1;
  } catch(std::exception e) {
    cerr<<e.what()<<endl;
    return 1;
  }
  return 0;
}  
