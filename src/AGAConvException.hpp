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

#ifndef AGACONV_EXCEPTION_HPP
#define AGACONV_EXCEPTION_HPP

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

namespace AGAConv {

class AGAConvException : public std::exception {
public:
  AGAConvException(std::uint32_t, std::string);
  const char* what() const throw();
protected:
  std::uint32_t errorNr;
  std::string errorMessage;
  std::string fullErrorMessage;
};

} // namespace AGAConv

#endif
