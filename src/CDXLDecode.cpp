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

#include "CDXLDecode.hpp"

#include <cstdlib>
#include <iostream>

#include "AGAConvException.hpp"
#include "CommandLineParser.hpp"
#include "Stage.hpp"
#include "Util.hpp"

using namespace std;

namespace AGAConv {

CDXLDecode::CDXLDecode() {
}

void CDXLDecode::run(Options& options) {
  if(options.hasInFile()) {
    cout << "Reading cdxl file \"" << options.inFileName << "\"."<<endl;
    //open file
    long fileSize=Util::fileSize(options.inFileName);
    if(fileSize==-1) {
      throw AGAConvException(110, "File not found: "+options.inFileName.string());
    }
    fstream inFile0;
    fstream* inFile=&inFile0;
    inFile->open(options.inFileName, ios::in | ios::binary);
    if(!inFile->is_open()) {        
      throw AGAConvException(111, "cannot open cdxl file "+options.inFileName.string());
    }
    ULONG i=0;
    ULONG readLen=0;
    if(fileSize>0) {
      while(1) {
        if(options.verbose>=3) cout<<"DECODING frame "<<i+1<<endl;
        CDXLFrame frame;
        frame.setFile(inFile);
        frame.readChunk();
        cout<<frame.toString()<<endl;
        ULONG frameLen=frame.getLength();
        if(readLen+frameLen==fileSize) {
          break;
        }
        if(readLen+frameLen>fileSize) {
          throw AGAConvException(112, "incomplete frame data: readLen+frameLen: "+std::to_string(readLen+frameLen)+" vs files size:"+std::to_string(fileSize));
        }
        readLen+=frameLen;
        i++;
      }
    }
    inFile->close();

    cout<<"End of cdxl file "<<options.inFileName<<endl;
  }
}

} // namespace AGAConv
