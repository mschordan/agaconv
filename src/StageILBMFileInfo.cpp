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

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "AGAConvException.hpp"
#include "IffILBMChunk.hpp"
#include "Options.hpp"
#include "StageILBMFileInfo.hpp"

using namespace std;

namespace AGAConv {

void StageILBMFileInfo::run(Options& options) {
  if(options.ilbmInfo) {
    // Read ilbm file only
    if(options.debug) cout << "DEBUG: Reading ilbm file \"" << options.inFileName << "\"."<<endl;
    // Open file
    fstream inFile0;
    fstream* inFile=&inFile0;
    inFile->open(options.inFileName, ios::in | ios::binary);
    if(inFile->is_open() == false) {        
      throw AGAConvException(151, string("Error: cannot open file ")+string(options.inFileName));
      return;
    }
    IffILBMChunk ilbm;
    ilbm.setFile(inFile);
    ilbm.readChunk();
    if(IffChunk::debug) cout<<"DEBUG: entire file successfully read."<<endl;
    if(options.debug) {
      cout<<ilbm.toDetailedString();
    } else {
      cout<<ilbm.toString();
    }
    if(IffChunk::debug) cout<<"DEBUG: all info printed."<<endl;
    if(ilbm.hasBODYChunk()) {
      bool result=ilbm.uncompressBODYChunk();
      if(result) {
        // Not supported, is reported earlier
        cout<<"Uncompressed BODY chunk."<<endl;
      } else {
        cout<<"BODY chunk not compressed."<<endl;
      }
    }
  }
}

} // namespace AGAConv
