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

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include "ByteSizeTypeDefs.hpp"

namespace Util {
  // true if s has prefix p, otherwise false
  bool hasPrefix(std::string p, std::string s);
  // true if s has postfix p, otherwise false
  bool hasSuffix(std::string p, std::string s);
  
  // returns file extension after the last period '.' in name
  // if name contains no '.' then the string is empty
  std::string fileNameExtension(std::string s);
  // computes a word aligned value of a given integer. This is useful
  // for computing the number of bytes (word aligned) for a given
  // width of a display
  UWORD wordAlignedLengthInBytes(UWORD width);

  // prints mono for 1, stereo for 2.
  std::string audioModeToString(int audioMode);

  // determines file size for a given file name
  long fileSize(std::string filename);

  // checks if file exists (using POSIX stats)
  bool fileExists(std::string name);

  // pow for LONG (instead of using pow for floats)
  ULONG ULONGPow(ULONG base, ULONG exp);

  // log for LONG (instead of using log for floats)
  ULONG ULONGLog2(ULONG number);
}

#endif
