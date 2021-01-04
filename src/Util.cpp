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

#include "Util.hpp"
#include <iostream>
#include <sys/stat.h>
#include <cassert>

using namespace std;

bool Util::hasPrefix(string prefix, string s) {
  // size and mismatch have both linear complexity
  if(prefix.size()<=s.size()) {
    auto res = std::mismatch(prefix.begin(), prefix.end(), s.begin());
    return res.first == prefix.end();
  } else {
    return false;
  }
}

bool Util::hasSuffix(string postfix, string s) {
  // size and mismatch have both linear complexity
  if(postfix.size()<=s.size()) {
    return std::equal(postfix.rbegin(), postfix.rend(), s.rbegin());
  } else {
    return false;
  }
}

string Util::fileNameExtension(string fileName) {
  std::string::size_type idx;
  idx = fileName.rfind('.');

  if(idx != std::string::npos) {
    return fileName.substr(idx+1);
  }  
  return "";
}

UWORD Util::wordAlignedLengthInBytes(UWORD width) {
  return ((width+15)/16)<<1;
}

string Util::audioModeToString(int audioMode) {
  switch(audioMode) {
  case 1: return "mono";
  case 2: return "stereo";
  default:
    cerr<<"Error: unknown audio mode "<<audioMode<<endl;
    exit(1);
  }
}

long Util::fileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool Util::fileExists(std::string name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

// power function for integers
ULONG Util::ULONGPow(ULONG base, ULONG exp) {
  ULONG result = 1;
  while (exp) {
    if (exp&1)
      result*=base;
    exp>>=1;
    base*=base;
  }
  return result;
}

ULONG Util::ULONGLog2(ULONG number) {
  assert(number>0);
  ULONG result = 0;
  while (number >>= 1) ++result;
  return result;
}
