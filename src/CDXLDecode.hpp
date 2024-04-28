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

#ifndef CDXL_DECODE_HPP
#define CDXL_DECODE_HPP

#include "Stage.hpp"
#include "CDXLFrame.hpp"

namespace AGAConv {

class CDXLDecode : public Stage {
 public:
  CDXLDecode();
  // uses options: inFile
  // sets inFile in CDXLFrame
  void run(Options& options);
};

} // namespace AGAConv

#endif
