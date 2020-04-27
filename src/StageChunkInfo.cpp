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

#include "StageChunkInfo.hpp"
#include <iostream>

#include "IffANIMForm.hpp"
#include <fstream>
#include "AGAConvException.hpp"

using namespace std;

void StageChunkInfo::run(Options& options) {
  cout << "Opening animation file \"" << options.inFileName << "\" ... "<<endl;
  //open file
  fstream file0;
  fstream* file=&file0;
  file->open(options.inFileName, ios::in | ios::binary);
  if(file->is_open() == false) {        
    throw AGAConvException(string("Error: cannot open file ")+options.inFileName);
  }
  IffANIMForm anim;
  anim.setFile(file);
  anim.readChunk();
  IffChunk::msg("entire file successfully read.");
  cout<<anim.toString()<<endl;
  IffChunk::msg("all info printed.");
}
