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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <algorithm>
#include <filesystem>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "OSLayer.hpp"
#include "Options.hpp"

namespace AGAConv {

class Configuration {
public:
  Configuration();
  void buildOptionsTable();
  void clearOptionsTable();
  std::filesystem::path getDefaultConfigFileName();
  void checkAndLoadConfigFile();
  // Loads config file (if available) and sets options in 'opt'. Returns true on success.
  bool loadConfigFile(std::filesystem::path configFile);
  void resetDefaultConfigFile();
  void saveDefaultConfigFile();
  void installDefaultConfigFile();  
  void uninstallDefaultConfigFile();
  void saveConfigFile(std::filesystem::path configFileName);
  std::string getValAsString(std::string var);

  // Returns number of parsed elements
  // 0: Not found
  // 1: Boolean option
  // 2: Option with one argument
  uint32_t getCLOptionNameArgs(std::string name);
  void processCLOption(std::string name, std::string value);
  static void checkAndSetOptions(Options& opt);
  void printCLOptions();
  void printCLOptionsAdvanced();
  void handleDefaultConfigFile();
  void setOptions(Options options);
  void resolveHcPath();
  void resolveTmpDir();
  Options getOptions();
  bool isSupportedOS();
  
  /* Allow class CommandLineParser access to private data member 'options',
     but not to any other private member. Checked at compile time.
     Attorney-Client design pattern
  */
  class PrivateOptionsRestrictedAccessClient {
  private:
    static Options& getOptionsRef(Configuration& config) {
      return config.getOptionsRef();
    }
    friend class CommandLineParser;
  };

private:
  Options options;
  friend class PrivateOptionsRestrictedAccessClient;
  Options& getOptionsRef();
  void appendCdxlConfigFileData(std::ofstream& configFile);
  enum ToolInterface { TI_UNSET, TI_CDXL, TI_CDXL_ADVANCED, TI_CL, TI_CF, TI_HIDDEN, TI_ANIM,
                       TI_ANIM_ALL, TI_ANIM_CL_ONLY };
  typedef std::set<ToolInterface> ToolInterfaceSet;
  static bool contains(ToolInterface ti, ToolInterfaceSet& set);
  static bool isSubSet(ToolInterfaceSet& s1, ToolInterfaceSet& s2);  
  void printCLOptionsSet(ToolInterfaceSet& select);
  void addOptionsBool0(std::string optName, bool& var, ToolInterfaceSet ti, std::string description);
  void addOptionsBool1(std::string optName, bool& var, ToolInterfaceSet ti, std::string description);  
  void addOptionsEntry(std::string optName, uint32_t& var, ToolInterfaceSet ti, uint32_t min, uint32_t max, std::string description);
  void addOptionsEntry(std::string optName, uint32_t& var, ToolInterfaceSet ti, uint32_t min, uint32_t max, std::string options, std::string description);  
  void addOptionsEntry(std::string optName, std::string& var, ToolInterfaceSet ti, std::string options, std::string description);
  void addOptionsEntry(std::string optName, double& var, ToolInterfaceSet ti, std::string options, std::string description);
  void addOptionsEntry(std::string optName, std::filesystem::path& var, ToolInterfaceSet ti, std::string options, std::string description);
  std::string replaceUnderscoreWithDash(std::string s);
  std::string replaceDashWithUnderscore(std::string s);

  void prepareDefaultConfigPath(std::filesystem::path defaultConfigPath);
  void saveDefaultConfigFileInternal();
  
  static bool isCommentLine(std::string& line);
  static bool isEmptyLine(std::string& line);
  static bool isCommentOrEmptyLine(std::string& line);
  void processConfigEntry(int lineNr, std::string var, std::string val);
  void processConfigEntryWithComment(int lineNr,  std::string var,  std::string val,  std::string comment);
  void processConfigEntryNoValueWithComment(int lineNr, std::string var, std::string commentBody);
  std::filesystem::path expandTilde(std::filesystem::path fspath);
  std::uint32_t convertStringToInt(int lineNr, std::string s);
  double convertStringToDouble(int lineNr, std::string s);  
  bool convertBoolStringToBool(int lineNr, std::string s);
  enum ParseType { PT_UNSET, PT_UINT32, PT_STRING, PT_BOOL0, PT_BOOL1, PT_DOUBLE, PT_PATH };
  std::string typeToCLDocString(ParseType pt);
  std::string formatError(int lineNr);
  struct OptionEntry {
    std::string name;
    ParseType parseType=PT_UNSET;
    void* valPtr=0;
    ToolInterfaceSet toolInterfaceSet;    
    uint32_t min=0;
    uint32_t max=std::numeric_limits<uint32_t>::max();
    std::string options;
    std::string description;
  };
  void writeConfigLine(std::ofstream& configFile, OptionEntry& el);
  void addRangesToOptionEntryDescriptions();
  std::map<std::string,uint32_t> optLookupMap;  
  std::vector<OptionEntry> optVector;
  uint32_t optId=0;
  std::unique_ptr<OSLayer> osLayer;
};

} // namespace AGAConv

#endif
