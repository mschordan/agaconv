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

#ifndef FILE_SEQUENCE_CONVERSION_H
#define FILE_SEQUENCE_CONVERSION_H

#include <string>
#include <map>
#include "Stage.hpp"
#include "Options.hpp"
#include "AGAConvException.hpp"
#include "IffILBMChunk.hpp"

/* Read a sequence of iff files and allow to operate on each
   file. Each file is read in as ILBM chunk data structure with access
   functions to each chunk's information. This allows to implement
   stream based conversions. If a conversion requires to store
   previous frames (e.g. ANIM format) then the ownership is transfered
   to the conversion object (the use of C++ smart points simplifies
   this task).
 */
class FileSequenceConversion : public Stage {

 public:
  FileSequenceConversion();
  virtual ~FileSequenceConversion();
  // control the reading of all frames (usually not adapted)
  virtual void run(Options& opt);
  // called before first frame is read. Can be used for initialization.
  virtual void preVisitFirstFrame();
  // for processing ILBM chunk (frame). returns false if chunk cannot be read.
  virtual void visitILBMChunk(IffILBMChunk* ilbmChunk);
  // for printing infos after processing
  virtual void postVisitLastILBMChunk(IffILBMChunk* ilbmChunk);
  
  virtual void visitPngFile(std::string inFileName);

  enum FileType { FILE_UNKNOWN, FILE_PNG, FILE_IFF };
  FileType determineFileType(std::string inFileName);
 protected:
  Options options;
  IffILBMChunk* readIffFile(std::string fileName);
  std::size_t initFileName(std::string inFilePattern);
  int frames=0;
  /* replaces number at the end of filename
     (e.g. myname.0001). Replaces only relevant digits. Number must be
     at the end of the filename.
  */
  std::string nextFileName();
  std::string inFileName; // state variable
  std::string firstInFileName;

 private:
  std::size_t startNumber;
  std::size_t currentNumber; // state variable
  std::string fileNamePattern;
  std::size_t fileNamePatternReplacePos;
  std::size_t length; // length of number string in filename
};

#endif
