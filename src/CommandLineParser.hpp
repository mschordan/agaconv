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

#ifndef COMMAND_LINE_PARSER_HPP
#define COMMAND_LINE_PARSER_HPP

#include <memory>
#include <string>
#include <vector>

#include "Configuration.hpp"
#include "Options.hpp"

namespace AGAConv {

class CommandLineParser {
public:
  // return value of true signals to continue processing, false means to exit.
  void parse(int argc, char **argv, Configuration& config);
  void setVersion(std::string version);
  void printOptionsList();
  ~CommandLineParser();
  bool done();
private:
  void printVersion();
  void checkInOutFileOptions(Options& options);
  void splitArgvOnEqualSign(int argc, char** argv);
  void inc();
  size_t argc;
  std::vector<std::string> argv;
  size_t argi=0;
  std::string version;
  bool doneFlag=false;
};

}

#endif
