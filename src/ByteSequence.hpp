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

#ifndef BYTE_SEQUENCE_HPP
#define BYTE_SEQUENCE_HPP

#include <fstream>
#include <vector>

#include "AmigaTypeDefs.hpp"

namespace AGAConv {

class ByteSequence {
 public:
  ByteSequence();
  ByteSequence(ULONG size);
  ~ByteSequence() = default;
  void add(UBYTE byte);
  void readData(ULONG dataSize0);
  void readDataAdjusted(ULONG dataSize0);
  void writeData();
  void writeDataAdjusted();
  void printData();
  uint32_t getDataSize() const;
  void removeData();
  void setInFile(std::fstream* inFile);
  void setOutFile(std::fstream* inFile);
  UBYTE* address(ULONG offset);
  UBYTE getByte(ULONG offset);
  void setByte(ULONG offset, UBYTE byte);

 private:
  void readAdjustPadding(uint32_t readDataSize);
  void writeAdjustPadding(uint32_t writeDataSize);
  bool debug;
  std::size_t dataSize;
  std::fstream* inFile;
  std::fstream* outFile;
  std::vector<UBYTE> data;
};

} // namespace AGAConv

#endif
