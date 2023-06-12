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

#include <filesystem>
#include <iostream>

#include "CDXLDecode.hpp"
#include "CDXLEncode.hpp"
#include "CommandLineParser.hpp"
#include "Configuration.hpp"
#include "ExternalToolDriver.hpp"
#include "StageAnimEdit.hpp"
#include "StageChunkInfo.hpp"
#include "StageILBMFileInfo.hpp"

using namespace std;
using namespace AGAConv;

void runCDXLEncode(Options& options) {
  CDXLEncode stage;
  std::filesystem::path inFileWithPath=options.getTmpDirName()/("frame"+options.firstFrameNumberToString()+".png");
  stage.setInFileWithPath(inFileWithPath);
  stage.run(options);
}

int main(int argc, char **argv) {
  string version="1.0.2";
  try {
    Configuration config;
    CommandLineParser clp;
    clp.setVersion(version);
    clp.parse(argc,argv,config);
    if(clp.done())
      return 0;
    Options options=config.getOptions();
    
    ////////////////
    // CDXL files //
    ////////////////
    
    // required
    Chunk::paddingFix=options.paddingFix;
    Chunk::setDebug(options.debug);
    
    // CDXL encoding
    if(options.writeCdxl && options.cdxlEncode && !options.cdxlInfo && !options.cdxlDecode) {
      // Extraction of png files
      ExternalToolDriver etd;
      etd.checkCommandProcessor();
      if(options.verbose>=1)
        cout<<"Conversion started."<<endl;
      etd.prepareTmpDir(options);
      if(options.verbose>=1)
        cout<<"Converting video file "<<options.inFileName<<endl;
      etd.runFFMPEGExtraction(options);

      // Conversion
      if(options.conversionTool=="ffmpeg") {
        runCDXLEncode(options);
      } else if(options.conversionTool=="ham_convert") {
        etd.runHamConvert(options);
      } else {
        throw AGAConvException(1,"unknown conversion tool "+options.conversionTool);
      }
      etd.finalizeTmpDir(options);
      if(options.verbose>=1)
        cout<<"Conversion finished."<<endl;
      if(options.verbose<=1 && !config.isSupportedOS() && options.conversionTool=="ham_convert") {
        cout<<"WARNING: unsupported OS. verbose level <= 1 not fully supported. Could not suppress output of external tools."<<endl;
      }
      return 0;
    }

    // CDXL file info for one file
    if(options.cdxlInfo) {
      // Read just first frame's header
      // Open file
      auto fileName=options.inFileName;
      fstream inFile;
      inFile.open(fileName, ios::in | ios::binary);
      if(inFile.is_open() == false) {        
        throw AGAConvException(2,"File not found: "+fileName.string());
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

    // ILBM/IFF file info printing
    if(options.ilbmInfo) {
      StageILBMFileInfo stage;
      stage.run(options);
      return 0;
    }

    ////////////////
    // ANIM files //
    ////////////////

    // Regenerate original file (for testing) or also add sound data if present
    if(options.writeAnim && options.hasOutFile() &&!options.writeCdxl) {
      StageAnimEdit stage;
      stage.run(options);
      return 0;
    }

    // Anim chunk info option
    if(options.chunkInfo) {
      StageChunkInfo stage;
      stage.run(options);
      return 0;
    }

  } catch(AGAConvException& e) {
    cerr<<e.what()<<endl;
    cerr<<"Exiting."<<endl;
    return 1;
  } catch(std::exception& e) {
    cerr<<e.what()<<endl;
    return 1;
  }
  return 0;
}  
