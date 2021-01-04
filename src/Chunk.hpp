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

#ifndef CHUNK_H
#define CHUNK_H

using namespace std;

#include <fstream>
#include <vector>

#include "ByteSizeTypeDefs.hpp"

class Chunk {

 public:
  Chunk();
  virtual ~Chunk();
  // set the name of the file where the iff chunk is read from
  virtual void setFile(fstream* fstream);
  // set the name of the output file where the iff chunk is written to
  virtual void setOutFile(fstream* fstream);
  string getName();
  void setName(string);
  string getChunkName();
  uint32_t getDataSize() const;
  void setDataSize(uint32_t);
  virtual uint32_t getTotalChunkSize() const;
  virtual uint32_t getAdjustedTotalChunkSize() const;
  virtual void readChunk()=0;
  virtual void writeChunk()=0;
  virtual string toString()=0;
  virtual string toDetailedString();
  string indent();
  string peekName();
  bool isForm();
  bool getLongToString();
  void setLongToString(bool flag);
  static void setDebug(bool flag);
  static void msg(string s);
  static bool debug;
 protected:
  uint8_t readUBYTE();
  uint16_t readUWORD();
  uint32_t readULONG();
  int8_t readSBYTE();
  int16_t readSWORD();
  int32_t readSLONG();
  void writeUByte(uint8_t x);
  void writeUWord(uint16_t x);
  void writeULong(uint32_t x);
  void writeUBYTE(uint8_t x);
  void writeUWORD(uint16_t x);
  void writeULONG(uint32_t x);
  void writeSBYTE(int8_t x);
  void writeSWORD(int16_t x);
  void writeSLONG(int32_t x);
  // reads an additional byte if data size does not end on an even
  // address. Does not change the data size itself.
  void readAdjustPadding(uint32_t readDataSize);
  // writes an additional byte if data size does not end on an even
  // address.
  void writeAdjustPadding(uint32_t writeDataSize);
  // reads only the chunkname and its size (stored in a LONG word).
  void readChunkNameAndSize();
  // writes only the chunkname and its size.
  void writeChunkNameAndSize();
  // reads only the chunkname
  string readChunkName();
  // writes a name that is provided as parameter (chunk name)
  void writeName(string name);
  // writes the stored chunk name
  void writeChunkName();
  // reads only the chunk size (4 bytes)
  void readChunkSize();
  // writes only the chunk size (4 bytes)
  void writeChunkSize();
  string name; // 4 letters
  uint32_t dataSize=0;
  fstream* file=nullptr;
  fstream* outFile=nullptr;
  bool isFormFlag=false;
 private:
  static bool longToString;
};

#endif
