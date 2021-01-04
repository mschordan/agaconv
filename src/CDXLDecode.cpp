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

#include "CDXLDecode.hpp"
#include <iostream>
#include <cstdlib>
#include "CommandLineParser.hpp"
#include "Stage.hpp"
#include "AGAConvException.hpp"
#include "Util.hpp"

using namespace std;

CDXLDecode::CDXLDecode() {
}

void CDXLDecode::run(Options& options) {
  if(options.hasInFile()) {
    cout << "Reading cdxl file \"" << options.inFileName << "\"."<<endl;
    //open file
    long fileSize=Util::fileSize(options.inFileName);
    if(fileSize==-1) {
      cerr<<"Error: File not found."<<endl;
      exit(1);
    }
    fstream inFile0;
    fstream* inFile=&inFile0;
    inFile->open(options.inFileName, ios::in | ios::binary);
    if(inFile->is_open() == false) {        
      cerr<<"Error: cannot open cdxl file "<<options.inFileName<<endl;
      throw AGAConvException();
    }
    ULONG i=0;
    ULONG readLen=0;
    if(fileSize>0) {
      while(1) {
        cout<<"DECODING frame "<<i+1<<endl;
        CDXLFrame frame;
        frame.setFile(inFile);
        frame.readChunk();
        cout<<frame.toString()<<endl;
        ULONG frameLen=frame.getLength();
        if(readLen+frameLen==fileSize) {
          break;
        }
        if(readLen+frameLen>fileSize) {
          cerr<<"Error: incomplete frame data: readLen+frameLen: "<<readLen+frameLen<<" vs files size:"<<fileSize<<endl;
          exit(1);
        }
        readLen+=frameLen;
        i++;
      }
    }
    inFile->close();

    cout<<"End of cdxl file "<<options.inFileName<<endl;
  }
}
