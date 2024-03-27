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

#include "CommandLineParser.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "AGAConvException.hpp"
#include "Configuration.hpp"
#include "Util.hpp"

using namespace std;

namespace AGAConv {

void CommandLineParser::inc() {
  if(argi>=argc) {
    throw AGAConvException(3,"command line: wrong number of arguments for option");
  } else {
    argi++;
  }
}

static bool isAnimFileName(string s) {
  string fileExtension=Util::fileNameExtension(s);
  return Util::hasPrefix("anim",fileExtension)
    ||Util::hasPrefix("sndanim",fileExtension)
    ||Util::hasPrefix("fxanim",fileExtension)
    ;
}

static bool isCdxlFileName(string s) {
  string fileExtension=Util::fileNameExtension(s);
  return Util::hasPrefix("cdxl",fileExtension)
    ||Util::hasPrefix("xl",fileExtension)
    ;
}

void CommandLineParser::setVersion(std::string version) {
  this->version=version;
}

// Search for '=' and split if present
void CommandLineParser::splitArgvOnEqualSign(int argc0, char** argv0) {
  for(int i=0;i<argc0;i++) {
    string s0=argv0[i];
    // Check for special case in dither ffmpeg option 
    if(Util::hasPrefix("bayer:bayer_scale=",s0)) {
      argv.push_back(string(argv0[i]));
      continue;
    }
    std::size_t found = s0.find("=");
    if(found!=std::string::npos) {
      string s1=s0.substr(0,found);
      // Skip '=' and copy rest
      string s2=s0.substr(found+1,s0.size()-(found+1));
      argv.push_back(s1);
      argv.push_back(s2);
    } else {
      argv.push_back(string(argv0[i]));
    }
  }
  argc=argv.size();
  for (auto s:argv) {
  }
}

bool CommandLineParser::done() {
  return doneFlag;
}

CommandLineParser::~CommandLineParser() {
  // All elements are deallocated automatically
}

void CommandLineParser::checkInOutFileOptions(Options& options) {
  bool optionWithOneInputFile
    = options.cdxlInfo
    || options.cdxlDecode // cdxl-info-all, only for 24bit videos (hidden)
    || options.ilbmInfo
    || options.chunkInfo
    || options.firstChunkInfo
    ;
  
  if(!options.hasInputFile()) {
    throw AGAConvException(11,"No input file name provided on command line");
  }
  if(!optionWithOneInputFile && !options.hasOutputFile()) {
    throw AGAConvException(12,"No output file name provided on command line");
  }
  if(optionWithOneInputFile && options.hasOutputFile()) {
    throw AGAConvException(13,"Output file name "
                        +options.outFileName.string()
                        +" provided, but option does not require it");
  }
}

void CommandLineParser::printVersion() {
  cout<<"agaconv version "<<version<<endl;
  cout<<"Copyright (C) 2019-2023 Markus Schordan"<<endl;
  cout<<"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>."<<endl;
  cout<<"This is free software: you are free to change and redistribute it."<<endl;
  cout<<"There is NO WARRANTY, to the extent permitted by law."<<endl;
  cout<<"Written by Markus Schordan."<<endl;
}

void CommandLineParser::parse(int argc0, char **argv0, Configuration& config) {
  
  if(version.size()==0) {
    throw AGAConvException(300, "Internal: program version not set.");
  }
  if(!(argc0==2 && (string(argv0[1])=="--install-config"||string(argv0[1])=="--uninstall-config"))) {
    // Load default config file, exept when the only provided option is --init-config
    // This is to allow cases where the config file contains errors, cannot be read,
    // needs to be regenerated.
    config.handleDefaultConfigFile();
  }
  if(argc0 <= 1) {
    config.printCLOptions();
    doneFlag=true;
    return;
  }

  Options& options=Configuration::PrivateOptionsRestrictedAccessClient::getOptionsRef(config);

  // Recognizes '=', splits where necessary and returns (newargc,newargv)
  splitArgvOnEqualSign(argc0, argv0);
  string commandName=argv[0];
  string helpTextHeader="Usage: "+commandName+" [OPTIONS] INPUTVIDEO OUTPUTVIDEO";
  string helpExtraTextHeader="Advanced options:";
  
  argi=1;
  int fileNames=0;
  while(argi<argc) {
    // Special case of provided config file. Allows to override config file with commamd line options.
    stringstream ss;
    if(argv[argi].size()>=2 && argv[argi][0]=='-' && argv[argi][1]=='-') {
      string optionName=argv[argi].substr(2);
      if(uint32_t res=config.getCLOptionNameArgs(optionName)) {
        if(res>0) {
          if(res==1) {
            if(optionName.size()>=4 && Util::hasPrefix("no-",optionName))
              config.processCLOption(optionName,"false"); // bool0
            else
              config.processCLOption(optionName,"true"); // bool1
            inc();
            if(options.installDefaultConfigFile) {
              if(argc>2) {
                throw AGAConvException(38, "option --install-config cannot be combined with other options.");
              }
              config.installDefaultConfigFile();
              options.installDefaultConfigFile=false;
              doneFlag=true;
              return;
            }
            if(options.uninstallDefaultConfigFile) {
              if(argc>2) {
                throw AGAConvException(193, "option --uninstall-config cannot be combined with other options.");
              }
              config.uninstallDefaultConfigFile();
              options.uninstallDefaultConfigFile=false;
              doneFlag=true;
              return;
            }
            if(options.resetDefaultConfigFile) {
              if(argc>2) {
                throw AGAConvException(39, "option --reset-config cannot be combined with other options.");
              }
              config.resetDefaultConfigFile();
              options.resetDefaultConfigFile=false;
              doneFlag=true;
              return;
            }
            continue;
          }
          if(res==2) {
            inc();
            if(argi>=argc) {
              throw AGAConvException(37,"missing argument in command line option --"+optionName+"=ARG");
            }
            config.processCLOption(optionName,argv[argi]);
            inc();
            config.checkAndLoadConfigFile();
            continue;
          }
        }
      }
    }
    if(argv[argi].size()>=1 && argv[argi][0]=='-') {
      throw AGAConvException(5,"unknown option "+string(argv[argi]));
    }
    switch(fileNames) {
    case 0: options.inFileName=string(argv[argi]);fileNames++;break;
    case 1: options.outFileName=string(argv[argi]);fileNames++;break;
    default:
      stringstream ss;
      ss<<"too many file names on command line ("<<fileNames<<")"<<endl;
      ss<<"inFile: "<<options.inFileName<<endl;
      ss<<"outFile: "<<options.outFileName<<endl;
      ss<<"current file name: "<<argv[argi]<<endl;
      throw AGAConvException(6,ss.str());
    }
    if(options.inFileName.string()[0]=='-') {
      throw AGAConvException(7,"expected file name, option "+options.inFileName.string()+" provided.");
    }
    inc();
  }

  if(options.showHelpText) {
    cout<<helpTextHeader<<endl;
    cout<<"Options:"<<endl;
    config.printCLOptions();
    doneFlag=true;
    return;
  }
  if(options.showHelpExtraText) {
    cout << helpExtraTextHeader <<endl;
    config.printCLOptionsAdvanced();
    doneFlag=true;
    return;
  }
  if(options.showVersion) {
    printVersion();
    doneFlag=true;
    return;
  }

  config.resolveHcPath();

  // All options not requiring any input files are handled above

  options.checkAndSetOptions();
  
   if(options.outConfigFileName.string().size()>0) {
    config.saveConfigFile(options.outConfigFileName);
    if(options.verbose>=1)
      cout<<"Saved config file: "<<options.outConfigFileName<<endl;
    doneFlag=true;
    return;
  }
  
  if(options.conversionTool=="ham_convert") {
    if(options.hcPath=="") {
      throw AGAConvException(8,options.colorMode+" conversion requested, but hc_path is not set. Cannot find ham_convert. Use option --hc-path PATH or set hc_path = PATH in config file.");
    }
  }
  
  // Derive options from filenames
  if(options.hasInFile()) {
    if(isAnimFileName(options.inFileName)) {
      options.readAnim=true;
    } else if(isCdxlFileName(options.inFileName)) {
      options.readCdxl=true;
    }
  }
  if(options.hasOutFile()) {
    if(isAnimFileName(options.outFileName)) {
      options.writeAnim=true;
    } else if(isCdxlFileName(options.outFileName)) {
      options.cdxlEncode=true;
      options.writeCdxl=true;
    }
  }
  if(options.hasSndFile()) {
    options.writeAnim=true;
  }
  
  // Perform consistency check of options
  if(!options.checkConsistency()) {
    throw AGAConvException(9,"inconsistent command line options.");
  }
  if(options.optimizePngPalette==false && options.verbose>=1) {
    if(options.stdCdxl)
      cout<<"STD-CDXL: ";
    else
      cout<<"Note: ";
    cout<<"Optimizing palette is off - not eliminating empty bitplanes."<<endl;
  }

  if(options.resetDefaultConfigFile) {
    if(argc!=2) {
      throw AGAConvException(10,"option reset-config must be used as the only option.");
    }
    config.resetDefaultConfigFile();
    doneFlag=true;
    return;
  }
  if(options.saveDefaultConfigFile) {
    config.saveDefaultConfigFile();
    doneFlag=true;
    return;
  }

  checkInOutFileOptions(options);

  // Resolving tmpDir here ensures that --help-advanced does not print the PID
  config.resolveTmpDir();

  // Check input file
  string inFileName=options.inFileName.string();
  if(inFileName.length()>0) {
    if(!Util::fileExists(inFileName)) {
      throw AGAConvException(14,"file "+inFileName+" does not exist.");
    }
  }
}

} // namespace AGAConv
