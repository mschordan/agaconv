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

#include "Options.hpp"

using namespace std;

Options options;

Options::Options()
{}

bool Options::hasInFile() {
  return inFileName!=0;
}

bool Options::hasOutFile() {
  return outFileName!=0;
}

bool Options::hasSndFile() {
  return sndFileName!=0;
}

bool Options::checkConsistency() {
  bool check=true;
  if(injectDPANChunk)
    check=check||(sndFileName!=nullptr);
  return check;
}
