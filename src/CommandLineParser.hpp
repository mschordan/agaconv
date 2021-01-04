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

#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H
#include "Options.hpp"
#include <string>

class CommandLineParser {
 public:
  Options parse(int argc, char **argv);
  void setVersion(std::string version);
 private:
  bool option(std::string option);
  void inc();
  int argc=0;
  char** argv=nullptr;
  int argi=0;
  std::string version;
};

#endif
