#include "Configuration.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <list>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "AGAConvException.hpp"
#include "Util.hpp"

// For obtaining a PID in resolveTmpDir() 
#include "OSLayer.hpp"

using std::filesystem::path;
using namespace std;

namespace AGAConv {

Configuration::Configuration():osLayer(OSLayer::createOSLayer()) {
  buildOptionsTable();
}

void Configuration::setOptions(Options options) {
  this->options=options;
}

Options Configuration::getOptions() {
  return options;
}

Options& Configuration::getOptionsRef() {
  return options;
}

bool Configuration::isSupportedOS() {
  return osLayer->isSupported();
}

string Configuration::formatError(int lineNr) {
  if(lineNr==0)
    return "in command line option.";
  else
    return "in line "+std::to_string(lineNr)+" in config file.";
}

uint32_t Configuration::convertStringToInt(int lineNr, string s) {
  // c++17
  uint32_t number;
  if(s=="auto")
    return Options::autoValue;
  auto [ptr, ec] = std::from_chars(s.data(), s.data()+s.size(), number);
  if(ec != std::errc{})
    throw AGAConvException(15,"incorrect number format "+formatError(lineNr));
  return number;
}

double Configuration::convertStringToDouble(int lineNr, string s) {
  double number;
  try {
    number = std::stod(s);
  } catch (std::invalid_argument const& ex) {
    throw AGAConvException(16,"incorrect floating point number format "+formatError(lineNr));
  }
  return number;
}

bool Configuration::convertBoolStringToBool(int lineNr, string s) {
  if(s=="true") return true;
  else if(s=="false") return false;
  else throw AGAConvException(17,"incorrect bool value format "+formatError(lineNr));
  assert(false);
}

std::string Configuration::replaceUnderscoreWithDash(string s) {
  std::replace(s.begin(), s.end(), '_', '-');
  return s;
}
std::string Configuration::replaceDashWithUnderscore(string s) {
  std::replace(s.begin(), s.end(), '-', '_');
  return s;
}

void Configuration::addOptionsBool0(string optName, bool& var, ToolInterfaceSet ti, string description) {
  auto e=OptionEntry{optName,PT_BOOL0,&var,ti};
  e.description=description;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}
void Configuration::addOptionsBool1(string optName, bool& var, ToolInterfaceSet ti, string description) {
  auto e=OptionEntry{optName,PT_BOOL1,&var,ti};
  e.description=description;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}
void Configuration::addOptionsEntry(std::string optName, uint32_t& var, ToolInterfaceSet ti, uint32_t min, uint32_t max, std::string description) {
  auto e=OptionEntry{optName,PT_UINT32,&var,ti,min,max};
  e.description=description;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}
void Configuration::addOptionsEntry(std::string optName, uint32_t& var, ToolInterfaceSet ti, uint32_t min, uint32_t max, std::string options, std::string description) {
  auto e=OptionEntry{optName,PT_UINT32,&var,ti,min,max};
  e.description=description;
  e.options=options;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}
void Configuration::addOptionsEntry(std::string optName, std::string& var, ToolInterfaceSet ti, std::string options, std::string description) {
  auto e=OptionEntry{optName,PT_STRING,&var,ti};
  e.options=options;
  e.description=description;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}
void Configuration::addOptionsEntry(std::string optName, double& var, ToolInterfaceSet ti, std::string options, std::string description) {
  auto e=OptionEntry{optName,PT_DOUBLE,&var,ti};
  e.options=options;
  e.description=description;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}
void Configuration::addOptionsEntry(std::string optName, std::filesystem::path& var, ToolInterfaceSet ti, std::string options, std::string description) {
  auto e=OptionEntry{optName,PT_PATH,&var,ti};
  e.options=options;
  e.description=description;
  optLookupMap[optName]=optId++;
  optVector.push_back(e);
}

void Configuration::handleDefaultConfigFile() {
  std::filesystem::path defaultConfigFileName=osLayer->getDefaultConfigFileName();
  if(osLayer->isSupported()) {
    if(defaultConfigFileName.string().size()==0) {
      // Issue warning, do not report error which would disable the application. The config file is a convenience feature.
      cerr<<"WARNING: default config file path is empty on supported OS. Not loading default config file.";
      return;
    }
    if(Util::fileExists(defaultConfigFileName)) {
      bool success=loadConfigFile(defaultConfigFileName);
      if(options.verbose>=3 && success) {
        cerr<<"Loaded default config file "<<defaultConfigFileName<<"."<<endl;
      }
      if(!success) {
        cerr<<"WARNING: could not load existing default config file "<<defaultConfigFileName<<". Using default values."<<endl;
      }
    }
  } else {
    // Nothing to be done. Unsupported OS is only reported when change of default config file is requested. Otherwise
    // default values are exactly the same as from a loaded (unchanged) default config file.
  }
}

void Configuration::checkAndLoadConfigFile() {
  if(options.inConfigFileName.string().size()>0) {
    // Copy verbose value, allowing to specify verbose level before the load-config command for
    // the 'Loaded config file' message below.
    bool verboseCopy=options.verbose;
    bool fileFound=loadConfigFile(options.inConfigFileName);
    if(!fileFound) {
      throw AGAConvException(4, "could not find config file "+options.inConfigFileName.string());
    }
    if(verboseCopy>=1) cout<<"Loaded config file "<<options.inConfigFileName<<endl;
    options.inConfigFileName="";
    // After loading the config file the verbose level from this loaded config file is used
  }
}

void Configuration::prepareDefaultConfigPath(std::filesystem::path defaultConfigPath) {
  if (std::filesystem::exists(defaultConfigPath))
    return;
  std::error_code err;
  bool success=std::filesystem::create_directories(defaultConfigPath, err);
  if(!success)
    throw AGAConvException(32,"could not create default config path "+defaultConfigPath.string());
}
    
void Configuration::resetDefaultConfigFile() {
  auto hcPathCopy=options.hcPath; // Keep hc_path
  Options defaultOptions;
  setOptions(defaultOptions); // Reset options
  options.hcPath=hcPathCopy;
  options.checkAndSetOptions();
  saveDefaultConfigFileInternal();
  if(options.verbose>=1) {
    cout<<"Reset default config file "<<osLayer->getDefaultConfigFileName()<<endl;
    cout<<"with hc_path = "<<options.hcPath<<endl;
  }
}

void Configuration::saveDefaultConfigFile() {
  saveDefaultConfigFileInternal();
  if(options.verbose>=1) cout<<"Saved default config file "<<osLayer->getDefaultConfigFileName()<<endl;
}

void Configuration::installDefaultConfigFile() {
  saveDefaultConfigFileInternal();
  if(options.verbose>=1) cout<<"Installed default config file "<<osLayer->getDefaultConfigFileName()<<endl;
}

void Configuration::uninstallDefaultConfigFile() {
  std::filesystem::path defaultConfigFileName=osLayer->getDefaultConfigFileName();
  if(!osLayer->isSupported()) {
    throw AGAConvException(190, "unsupported OS. Cannot uninstall default config file.");
  }
  if (!std::filesystem::exists(defaultConfigFileName)) {
    throw AGAConvException(191, "default config file does not exist, cannot be uninstalled.");
  }
  bool success=std::filesystem::remove(defaultConfigFileName );
  if(!success) {
    throw AGAConvException(192, "default config file could not be uninstalled, remains as "+defaultConfigFileName.string());
  } else {
    if(options.verbose>=1) cout<<"Uninstalled agaconv default config file "<<defaultConfigFileName<<endl;
  }

  std::filesystem::path defaultConfigPath=defaultConfigFileName.parent_path();
  // only removes directory if empty, throws filesystem exception when not empty
  success=std::filesystem::remove(defaultConfigPath);
  if(options.verbose>=1) {
    cout<<"Removed agaconv default config directory "<<defaultConfigPath<<endl;
    cout<<"Uninstall config finished."<<endl;
  }
}

void Configuration::saveDefaultConfigFileInternal() {
  std::filesystem::path defaultConfigFileName=osLayer->getDefaultConfigFileName();
  if(!osLayer->isSupported()) {
    throw AGAConvException(35, "unsupported OS. Cannot save default config file.");
  }
  if(defaultConfigFileName.string().size()==0) {
    throw AGAConvException(36, "cannot save default config file, could not determine location.");
  }
  prepareDefaultConfigPath(defaultConfigFileName.parent_path());  
  saveConfigFile(defaultConfigFileName);
}

void Configuration::saveConfigFile(std::filesystem::path configFileName) {
  assert(optLookupMap.size()>0);
  ofstream configFile;
  configFile.open(configFileName, ios::out | ios::trunc |  ios::binary);
  if(!configFile.is_open()) {
    throw AGAConvException(18,"could not save config file "+configFileName.string());
  }
  appendCdxlConfigFileData(configFile);
  configFile.close();
}  

bool Configuration::contains(ToolInterface ti, ToolInterfaceSet& set) {
  return set.find(ti)!=set.end();
}

bool Configuration::isSubSet(ToolInterfaceSet& s1, ToolInterfaceSet& s2) {
  for (auto el: s1) 
    if(!contains(el,s2))
      return false;
  return true;
}

void Configuration::writeConfigLine(ofstream& configFile, OptionEntry& o) {
  configFile<<std::left<<std::setw(20)<<o.name
            <<std::setw(2)<<"="
            <<std::setw(20)<<getValAsString(o.name)
            <<" ; "<<o.description;
  auto pt=o.parseType;
  if(pt==PT_UINT32) {
    configFile<<" (range: "<<o.min<<".."<<o.max<<")";
  }
  configFile<<endl;
}

void Configuration::appendCdxlConfigFileData(ofstream& configFile) {
  configFile<<"; Basic options"<<endl;
  for(auto el : optVector) {
    auto s1=ToolInterfaceSet{TI_CDXL, TI_CF};
    if(isSubSet(s1, el.toolInterfaceSet)) {
      writeConfigLine(configFile, el);
    }
  }
  configFile<<endl;
  configFile<<"; Advanced options"<<endl;
  for(auto el : optVector) {
    auto s1=ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF};
    if(isSubSet(s1,el.toolInterfaceSet))
      writeConfigLine(configFile, el);
  }
}

void Configuration::printCLOptions() {
  ToolInterfaceSet select={TI_CDXL, TI_CL};
  printCLOptionsSet(select);
}
void Configuration::printCLOptionsAdvanced() {
  ToolInterfaceSet select={TI_CDXL_ADVANCED, TI_CL};
  printCLOptionsSet(select);
}

void Configuration::printCLOptionsSet(ToolInterfaceSet& tiSet) {
  assert(optVector.size()>0);
  for(auto o : optVector) {
    if(isSubSet(tiSet, o.toolInterfaceSet)) {
      string s1="  [--"+replaceUnderscoreWithDash(o.name);
      auto pt=o.parseType;
      if(pt!=PT_BOOL0 && pt!=PT_BOOL1) {
        if(pt==PT_STRING) {
          s1+=" "+o.options;
        } else {
          s1+=" ";
          if(o.options.size()>0)
            s1+=o.options;
          else
            s1+=typeToCLDocString(o.parseType);
        }
      }
      s1+="]";
      cout<<std::left<<std::setw(32)<<s1<<o.description;
      auto defVal=getValAsString(o.name);
      bool defPrint=pt!=PT_BOOL1 && o.options!="FILENAME";
      if(defPrint) {
        cout<<" (default: "<<(defVal==""?"UNSET":defVal);
      }
      if(pt==PT_UINT32) {
        cout<<", range: "<<o.min<<".."<<o.max;
      }
      if(defPrint)
        cout<<")";
      cout<<endl;
    }
  }
}

void Configuration::buildOptionsTable() {
  Options& opt=this->options;
  stringstream adjAspectVal;
  adjAspectVal<<std::setprecision(3)<<setw(4)<<opt.adjustAspectSelectorValue1;
  addOptionsEntry("color_mode",opt.colorMode, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},"STRING","color mode where STRING = aga8|..|aga2|ham8|ham6|ocs5|..|ocs2|ehb"); // => colorModeEnum, several vals (aga8, etc.)
  addOptionsEntry("fps",opt.fps, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},1,60,"frames per second");
  addOptionsEntry("width",opt.width, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},128,3840,"width of video in pixels");
  addOptionsEntry("height",opt.height, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF}, 1, 2160,"auto|NUMBER","height of video - 'auto' derives height for proper aspect ratio");
  addOptionsEntry("frequency",opt.frequency, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},1,AGAConv::maxAmigaFrequency,"audio frequency");
  addOptionsEntry("audio_mode",opt.audioMode, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},"mono|stereo","audio mode"); // mono|stereo => stereo:bool
  addOptionsEntry("adjust_aspect",opt.adjustAspectMode, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},"VALUE","adjust ascpect ratio, where VALUE=FLOAT|"+opt.adjustAspectSelectorName1+"(="+adjAspectVal.str()+")");
  addOptionsBool1("change_config",opt.saveDefaultConfigFile, ToolInterfaceSet{TI_CDXL, TI_CL}, "change default configuration - to be used with other options, e.g. --hc-path=PATH.");
  addOptionsEntry("load_config",opt.inConfigFileName, ToolInterfaceSet{TI_CDXL, TI_CL}, "FILE", "load user configuration file");
  addOptionsEntry("save_config",opt.outConfigFileName, ToolInterfaceSet{TI_CDXL, TI_CL}, "FILE", "save user configuration file");
  addOptionsBool1("fixed_frames",opt.fixedFrames, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},"all frames have the same length");
  addOptionsEntry("hc_path",opt.hcPath, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF},"PATH", "absolute file path to ham_convert");
  addOptionsBool1("cdxl_info",opt.cdxlInfo,ToolInterfaceSet{TI_CDXL, TI_CL}, "show all info of frame 1 of given CDXL video");
  addOptionsBool1("cdxl_info_all",opt.cdxlDecode,ToolInterfaceSet{TI_CDXL, TI_HIDDEN},"display info about all frames in given CDXL video");  
  addOptionsEntry("verbose",opt.verbose, ToolInterfaceSet{TI_CDXL, TI_CL, TI_CF}, 0,3, "select how verbose output is during conversion");
  addOptionsBool1("version",opt.showVersion, ToolInterfaceSet{TI_CDXL, TI_CL}, "display program version and copyright");
  addOptionsBool1("help",opt.showHelpText, ToolInterfaceSet{TI_CDXL, TI_CL},"show basic command line options");
  addOptionsBool1("help_advanced",opt.showHelpExtraText, ToolInterfaceSet{TI_CDXL, TI_CL}, "show advanced command line options");

  // Advanced options
  addOptionsBool1("black_and_white",opt.blackAndWhite, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL, TI_CF},"convert video to black-and-white colors");
  addOptionsBool1("black_background",opt.reserveBlackBackgroundColor, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},"reserve black background color (only relevant on OCS systems)");
  addOptionsEntry("dither",opt.ditherMode, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL, TI_CF},"STRING","ffmpeg dithering mode when rescaling video, where STRING =floyd_steinberg|bayer:bayer_scale=X|sierra2");
  addOptionsEntry("screen_mode",opt.screenMode, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},"STRING","screen (resolution) mode, where STRING = auto|unspecified|lores|hires|superhires"); // => resMode:GFX_RESOLUTION, implicit with 'auto'
  addOptionsEntry("force_color_depth",opt.colorDepthBits, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL, TI_CF},12,24,"allows to force color depth in palette where NUMBER = auto|12|24");
  addOptionsBool1("install_config",opt.installDefaultConfigFile, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL}, "install default config file");
  addOptionsBool1("uninstall_config",opt.uninstallDefaultConfigFile, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL}, "uninstall default config file");
  addOptionsBool1("reset_config",opt.resetDefaultConfigFile, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL}, "reset default config file to original values (keeps hc_path)");
  addOptionsEntry("tmp_dir_prefix",opt.tmpDir, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL}, "DIRNAME", "prefix of temporary directory name.");
  addOptionsBool1("keep_tmp_dir",opt.keepTmpFiles, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL, TI_CF},"keep temporary directory (temporary dir is removed by default)");
  addOptionsEntry("hc_ham_quality",opt.hcHamQuality, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL}, 0, 3,"ham_convert HAM conversion quality"); // ham8: 1-3, ham6 1-7
  addOptionsEntry("hc_dither",opt.hcDither, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},"STRING","ham_convert dither mode where STRING=auto|none|fs|bayer8x8");   // dither_X, X=fs|bayer8x8
  addOptionsEntry("hc_propagation",opt.hcPropagation, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},0,100,"ham_convert error propagation factor, requires hc_dither = fs");
  addOptionsEntry("hc_diversity",opt.hcDiversity, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},0,9,"ham_convert diversity X=0-6 for ehb, X=0-9 for other modes, not supported in ham8"); // ehb: 0-6, default 3
  addOptionsEntry("hc_quant",opt.hcQuant, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},"STRING","ham_convert quantisation algorithm where STRING=wu|neuquant");
  addOptionsBool1("iff_info",opt.ilbmInfo, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CL},"show IFF info for given IFF file.");
  addOptionsEntry("in_file",opt.inFileName, ToolInterfaceSet{TI_ANIM, TI_CDXL_ADVANCED, TI_CF, TI_CL}, "FILE", "set input file (available for tool generated config)");
  addOptionsEntry("out_file",opt.outFileName, ToolInterfaceSet{TI_ANIM, TI_CDXL_ADVANCED, TI_CF, TI_CL}, "FILE", "set output file (available for tool generated config)");
  addOptionsEntry("pcm_file",opt.sndFileName, ToolInterfaceSet{TI_ANIM, TI_CL}, "FILE","name of audio PCM file");
  addOptionsBool1("anim_chunk_info",opt.chunkInfo, ToolInterfaceSet{TI_ANIM, TI_CL}, "show ANIM chunck info for entire ANIM video"); // 2 vals
  addOptionsBool0("no_anim_padding_fix",opt.animPaddingFix, ToolInterfaceSet{TI_ANIM, TI_CL, TI_CF},"do not apply padding fix in ANIM file"); // bool0 
  addOptionsBool1("inject_dpan",opt.injectDPANChunk, ToolInterfaceSet{TI_ANIM, TI_CL, TI_CF}, "inject DPAN chunk in ANIM file");  
  addOptionsEntry("anim_play_rate",opt.playRate, ToolInterfaceSet{TI_ANIM, TI_CL, TI_CF}, 1, 255, "play rate. Must be set explicitly in ANIM files");
  addOptionsEntry("alignment",opt.alignmentString, ToolInterfaceSet{TI_CDXL_ADVANCED, TI_CF, TI_CL},"STRING","alignment of all data chunks, where STRING=auto|none|16bit|32bit|64bit.");  // => paddingSize => paddingMode

  // Hidden options
  addOptionsBool1("debug",opt.debug, ToolInterfaceSet{TI_CDXL, TI_HIDDEN}, "debug mode, very slow");
  addOptionsEntry("audio_data_type",opt.audioDataTypeString, ToolInterfaceSet{TI_CDXL, TI_HIDDEN}, "u8|s8", "set audio data type."); // => audioDataType: AudioDataType  
  // should be always true, therefore hidden. False only relevant for testing.
  addOptionsBool1("optimize_palette",opt.optimizePngPalette, ToolInterfaceSet{TI_HIDDEN, TI_CF},"eliminate empty bit planes and remap colors");
  addOptionsEntry("reserved_colors",opt.reservedColorsNum, ToolInterfaceSet{TI_HIDDEN, TI_CF, TI_CL},0,256,"number of colors to be reserved");
  addOptionsBool1("fixed_planes",opt.fixedPlanesFlag, ToolInterfaceSet{TI_CDXL, TI_HIDDEN, TI_CF},"select fixed number of planes in all frames according to color mode");
  addOptionsEntry("extraction_tool",opt.extractionTool, ToolInterfaceSet{TI_CDXL, TI_HIDDEN}, "", "name of extraction tool");
  addOptionsEntry("conversion_tool",opt.conversionTool, ToolInterfaceSet{TI_CDXL, TI_HIDDEN}, "", "name of conversion tool");
  addOptionsEntry("fixed_frame_digits",opt.fixedFrameDigits, ToolInterfaceSet{TI_CDXL, TI_HIDDEN}, 3, 20,"number of digits used for frame numbers in tmp files");
  addOptionsBool0("no_32bit_check",opt.enabled32BitCheck, ToolInterfaceSet{TI_CDXL, TI_HIDDEN}, "disable 32bit alignment check");  

  addRangesToOptionEntryDescriptions();
}

void Configuration::addRangesToOptionEntryDescriptions() {
  assert(optVector.size()>0);
  for (auto e : optVector) {
    e.description+=" RANGE";
  }
}

void Configuration::clearOptionsTable() {
  optLookupMap.clear();
  optVector.clear();
}

string Configuration::typeToCLDocString(ParseType pt) {
  switch(pt) {
  case PT_UNSET: throw AGAConvException(19,"unknown type in configuration.");
  case PT_UINT32: return "NUMBER";
  case PT_STRING: return "STRING";
  case PT_BOOL1:
  case PT_BOOL0: return "";
  case PT_DOUBLE: return "FLOAT";
  case PT_PATH: return "PATH";
  }
  return "";
}

void Configuration::processConfigEntryNoValueWithComment(int lineNr, string var, string comment) {
  auto it=optLookupMap.find(var);
  if(it==optLookupMap.end())
    throw AGAConvException(34,"unknown config option \""+var+"\" with no value in line "+std::to_string(lineNr)+" in config file.");
  // only check, keep default comment, do not store in optVector[(*it).second].description=comment;
}

void Configuration::processConfigEntryWithComment(int lineNr, string var, string val, string comment) {
  processConfigEntry(lineNr,var,val);
  auto it=optLookupMap.find(var);
  assert(it!=optLookupMap.end()); // must hold here
  // only check, keep default comment, do not store in optVector[(*it).second].description=comment;
}

void Configuration::processConfigEntry(int lineNr, string var, string val) {
  auto it=optLookupMap.find(var);
  if(it!=optLookupMap.end()) {
    auto& entry=optVector[(*it).second];
    switch(entry.parseType) {
    case PT_UNSET:
      throw AGAConvException(20,string("config entry: lines "+std::to_string(lineNr)+" accessing unset value of "+(*it).first));
    case PT_UINT32: {
      uint32_t valx = *(static_cast<uint32_t*>(entry.valPtr)) = convertStringToInt(lineNr,val);
      if(valx!=Options::autoValue && (valx<entry.min || valx>entry.max)) {
        string s;
        if(lineNr!=0)
          s+=("config file: line "+std::to_string(lineNr)+": ");
        s+=("value of "+var+" = "+std::to_string(valx)+" outside required range of "+std::to_string(entry.min)+" .. "+std::to_string(entry.max));
        throw AGAConvException(21,s);
      }
      return;
    }
    case PT_STRING: { *(static_cast<string*>(entry.valPtr)) = val; return; }
    case PT_BOOL0:
    case PT_BOOL1: { *(static_cast<bool*>(entry.valPtr)) = convertBoolStringToBool(lineNr,val); return; }
    case PT_DOUBLE: { *(static_cast<double*>(entry.valPtr)) = convertStringToDouble(lineNr,val); return; }
    case PT_PATH: { *(static_cast<std::filesystem::path*>(entry.valPtr)) = val; return; }
    }
  } else {
    throw AGAConvException(22,"unknown config option \""+var+"\" in line "+std::to_string(lineNr)+" in config file.");
  }
}

string Configuration::getValAsString(string var) {
  auto it=optLookupMap.find(var);
  if(it!=optLookupMap.end()) {
    auto& entry=optVector[(*it).second];
    stringstream ss;
    switch(entry.parseType) {
    case PT_UNSET: throw AGAConvException(23,"config entry: "+var+": accessing unset value in configuration data."); break;
    case PT_UINT32: {
      uint32_t val=*(static_cast<uint32_t*>(entry.valPtr));
      if(val==Options::autoValue)
        return "auto";
      else
        return std::to_string(val);
      break;
    }
    case PT_STRING: ss<<*(static_cast<string*>(entry.valPtr));break;
    case PT_BOOL0:
    case PT_BOOL1: if(*(static_cast<bool*>(entry.valPtr))) ss<<"true";else ss<<"false";break;
    case PT_DOUBLE: ss.precision(2);ss<<fixed<<*(static_cast<double*>(entry.valPtr));break;
    case PT_PATH: ss<<*(static_cast<std::filesystem::path*>(entry.valPtr));break;
    }
    return ss.str();
  } else {
    return "";
  }
}

// Allows for white-space before ';' and empty line
bool Configuration::isCommentOrEmptyLine(std::string& line) {
  size_t pos=0;
  if(line.size()>0) {
    while(line[pos]==' ')
      pos++;
  }
  if(pos<line.size()) {
    if(line[pos]==';') {
      return true;
    }
  }
  return line.size()==pos;
}

// Allows for white-space before required ';'
bool Configuration::isCommentLine(std::string& line) {
  size_t pos=0;
  if(line.size()>0) {
    while(line[pos]==' ')
      pos++;
  }
  if(pos<line.size())
    return line[pos]==';';
  else
    return false;
}

// Empty line can consist of arbitrary many white space characters (including 0).
bool Configuration::isEmptyLine(std::string& line) {
  size_t pos=0;
  if(line.size()>0) {
    while(line[pos]==' ')
      pos++;
  }
  return pos==line.size();
}

void checkValidComment(const string& s, uint32_t lineNr) {
  if(s.size()>=2 && s[1]!=' ') {
    throw AGAConvException(33, "config file format: line "+std::to_string(lineNr)+":"
                           +" a comment must begin with \"; \", not with \""+s+"\""
                           +" The space character is mandatory.");
  }
}

bool Configuration::loadConfigFile(std::filesystem::path configFile) {
  if(Util::fileExists(configFile)) {
    std::ifstream infile(configFile);
    std::string line;
    uint32_t lineNr=1;
    while (std::getline(infile, line)) {
      std::istringstream iss(line);
      // Check for comment
      if(isCommentOrEmptyLine(line)) {
        continue;
      }
      string var, sep;
      iss >> var >> sep;
      if (iss.fail()||sep!="=") {
        stringstream ss;
        ss<<"Error: config file "<<configFile<<": incorrect format in line "<<lineNr;
        throw AGAConvException(24,ss.str());
      }
      string val;
      iss >> val;
      // Remove double quotes from path name values
      if(val.size()>=2) {
        if(val.front()=='\"' && val.back()=='\"') {
          val.erase(0,1);
          val.erase(val.size()-1);
        }
      }
      // Line with no value, but comment
      if(val.size()>=1 && val[0]==';') {
        checkValidComment(val, lineNr);
        string commentBody; std::getline(iss, commentBody);
        assert(commentBody[0]==' ');
        commentBody.erase(0,1);
        processConfigEntryNoValueWithComment(lineNr,var, commentBody);
        continue;
      }
      if(iss.fail()) {
        // No value provided, ignore, keep default value.
      } else {
        string optionalComment;
        iss >> optionalComment;
        if(isEmptyLine(optionalComment)) {
          // Line with value (no comment)
          processConfigEntry(lineNr,var,val);
          continue;
        } else {
          // Line with value and omment
          checkValidComment(optionalComment, lineNr);
          string commentBody; std::getline(iss, commentBody);
          assert(commentBody[0]==' ');
          commentBody.erase(0,1);
          processConfigEntryWithComment(lineNr,var,val,commentBody);
        }
      }
      lineNr++;
    }
    return true;
  }
  return false;
}

uint32_t Configuration::getCLOptionNameArgs(std::string name0) {
  string name=replaceDashWithUnderscore(name0);
  auto iter=optLookupMap.find(name);
  if(iter!=optLookupMap.end()) {
    auto pt=optVector[(*iter).second].parseType;
    assert(pt!=PT_UNSET);
    if( pt==PT_BOOL0||pt==PT_BOOL1 )
      return 1;
    else
      return 2;
  }
  return 0;
}

void Configuration::processCLOption(std::string name, std::string value) {
  processConfigEntry(0,replaceDashWithUnderscore(name),value);
}

std::filesystem::path Configuration::expandTilde(std::filesystem::path fspath) {
  string path=fspath.string();
  string homePath=osLayer->getHomeDirString();
  if(homePath.empty()) {
    // If no path was returned (unknown OS fallback), leave string unchanged.
    return path;
  }
  if (!path.empty() && path[0] == '~') {
    if(path.size() == 1 || path[1] == '/') {
      path.replace(0, 1, homePath);
    }
  }
  return path;
}

// Search for ham_convert jar file inside hc_path, if set
void Configuration::resolveHcPath() {
  std::filesystem::path hcPath=options.hcPath;
  hcPath=expandTilde(hcPath);
  if(hcPath.string().size()>0) {
    if(std::filesystem::is_directory(hcPath)) {
      // Iterate through file in hc_path, skip dirs, non-recursive
      list<std::filesystem::path> jarFiles;
      for (auto const& dir_entry : std::filesystem::directory_iterator{hcPath}) {
        if(std::filesystem::is_regular_file(dir_entry)) {
          std::filesystem::path filePath=dir_entry.path();
          string fileName=filePath.stem();
          string fileExt=filePath.extension();
          std::regex hamConvertRegex("ham_convert_[0-9]+(\\.[0-9])*");
          if(std::regex_match(fileName, hamConvertRegex) && fileExt==".jar") {
            jarFiles.push_back(hcPath/filePath);
          }
        }
      }
      if(jarFiles.size()==1) {
        options.hcPathResolved=jarFiles.front();
        // Found jar file, record path
        return;
      } else if(jarFiles.size()>1) {
        stringstream ss;
        ss<<"Error: found several ham_convert jar files:"<<endl;
        for (auto file:jarFiles) {
          ss<<file<<endl;
        }
        ss<<"Ensure there exists only one jar file. Did you install a new ham_convert version in the same directory?";
        throw AGAConvException(25, ss.str());
      }
      // No jar file found
      throw AGAConvException(27,"could not find ham_convert's jar file in hc_path = "+hcPath.string());
    } else if(std::filesystem::is_regular_file(hcPath)) {
      if(!(hcPath.extension().string()==".jar")) {
        throw AGAConvException(28, "provided hc_path = "+hcPath.string()+" is not a Java jar file.");
      }
      options.hcPathResolved=hcPath;
    } else {
      if(!Util::fileExists(hcPath.string()))
        throw AGAConvException(29, "provided hc_path = "+hcPath.string()+" does not exist.");
      else
        throw AGAConvException(30, "provided hc_path = "+hcPath.string()+" is not a directory or file.");
    }
  }
  // Empty hcPath
}

void Configuration::resolveTmpDir() {
  path tmpPath(options.getTmpDirName());
  if(tmpPath.string().length()==0)
    throw AGAConvException(31,"temporary working directory name is not set. Suggestion: use option --reset-config.");

  // Append PID to tmp dir name to ensure multiple instances of agaconv
  // can be run in parallel, in case the other properties are not sufficient
  // Fallback for unknown OS uses pid = 0.
  string pid = osLayer->getPidString();
  options.setTmpDirName(tmpPath.string()
                        +"-"+options.inFileName.stem().string()
                        +"-"+options.colorMode
                        +"-"+std::to_string(options.width)
                        +"-"+pid);
}

} // namespace AGAConv
