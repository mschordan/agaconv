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

#include "AGAConvException.hpp"
#include <string>

using namespace std;

namespace AGAConv {

AGAConvException::AGAConvException(std::uint32_t nr, std::string s) {
  errorNr=nr;
  errorMessage=s;
  fullErrorMessage=string("Error ")+std::to_string(errorNr)+": "+errorMessage;
}

const char* AGAConvException::what () const throw() {
  return fullErrorMessage.c_str();
}

}
