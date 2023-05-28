#include "ExternalToolDriver.hpp"

#include <cassert>
#include <cctype>
#include <exception>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>

#include "AGAConvException.hpp"

using namespace std;

namespace AGAConv {

ExternalToolDriver::ExternalToolDriver():_osLayer(OSLayer::createOSLayer()) {
}

void ExternalToolDriver::checkCommandProcessor() {
  if (!system(nullptr)) {
    throw AGAConvException(79, "Command processor does not exist. Cannot run any external tool.");
  }
}

void ExternalToolDriver::runFFMPEGExtraction(const Options& options) {
  runFFMPEGAudioExtraction(options);
  runFFMPEGVideoExtraction(options);  
}

void ExternalToolDriver::runFFMPEGAudioExtraction(const Options& options) {
  string audioModeFFMPEGOption="unset";
  std::filesystem::path audioFileName=options.getTmpDirSndFileName();
  if(options.stereo) {
    audioModeFFMPEGOption="";
  } else {
    audioModeFFMPEGOption="-ac 1";
  }
  assert(audioModeFFMPEGOption!="unset");
  assert(options.hasInFile());
  assert(options.hasSndFile());
  stringstream audioCommand;
  audioCommand<<"-i "<<options.inFileName
              <<" "<<_allQuietOptions
              <<" -ar "<<options.frequency
              <<" "<<"-f u8 -acodec pcm_u8"
              <<" "<<audioModeFFMPEGOption
              <<" "<<audioFileName
    ;
  // Extract audio data from input video file
  runFFMPEG(options, audioCommand.str(), "ffmpeg (extracting audio data)");
  if(options.verbose>=2) {
    cout<<"Extracted audio file "<<audioFileName<<" (sound mode:"<<options.audioModeToString()<<")"<<endl;
  }
}

void ExternalToolDriver::prepareTmpDir(const Options& options) {
  namespace fs = std::filesystem;
  fs::path tmpPath(options.getTmpDirName());
  // Checked in configuration setup, must be true here
  assert(tmpPath.string().length()>0);
  if(fs::exists(tmpPath)) {
    // Reuse existing tmp dir, clean it up first
    removeTmpDir(options,false);
  }
  // Create temporary directory  mytmpdir/b/c
  std::error_code ec;
  bool res=create_directories(tmpPath,ec);
  if(!res) {
    throw AGAConvException(70,"could not create temporary working directory "+tmpPath.string());
  }
  if(options.verbose>=3) cout<<"Using temporary directory "<<tmpPath<<endl;
}

std::uintmax_t ExternalToolDriver::removeFrameFiles(const Options& options, string extension) {
  std::uintmax_t numFiles=0;
  if(std::filesystem::is_directory(options.getTmpDirName())) {
    for (auto const& dir_entry : std::filesystem::directory_iterator{options.getTmpDirName()}) {
      if(std::filesystem::is_regular_file(dir_entry)) {
        std::filesystem::path filePath=dir_entry.path();
        string fileName=filePath.stem();
        string fileExt=filePath.extension();
        std::regex hamConvertRegex("frame[0-9]+(_output)?");
        if(std::regex_match(fileName, hamConvertRegex) && fileExt==("."+extension)) {
          bool success=std::filesystem::remove(filePath);
          if(success)
            numFiles++;
        }
      }
    }
    if(options.verbose>=2 && numFiles>0) cout<<"- Removed "<<numFiles<<" "<<extension<<" frame files."<<endl;
  } else {
    throw AGAConvException(71,"temporary directory path is not a directory.");
  }
  return numFiles;
}

// For strict=true removeTmpDir fails if audio file cannot be removed (or does not exist) since this indicates
// that something went wrong in the conversion.
void ExternalToolDriver::removeTmpDir(const Options& options, bool strict) {
  if(options.verbose>=2) cout<<"Removing files in tmp dir "<<options.getTmpDirName()<<endl;
  // 1) Delete audio track file (this must succeed, otherwise something is very wrong and exit)
  bool success=std::filesystem::remove(options.getTmpDirSndFileName());
  if(success) {
    if(options.verbose>=2)
      cout<<"- Removed 1 temporary audio file "<<options.getTmpDirSndFileName()<<endl;
  } else {
    if(strict) {
      cerr<<"Failed to remove audio file "<<options.getTmpDirSndFileName()<<endl;
      throw AGAConvException(72, "Did not remove tmp directory "+options.getTmpDirName().string());
    }
  }
  
  // Only to be removed when ham_convert is used
  std::filesystem::path hcBatchFile=options.getTmpDirName()/options.hamConvertBatchFileName;
  success=std::filesystem::remove(hcBatchFile);
  if(success && options.verbose==2)
    cout<<"- Removed 1 batch file "<<hcBatchFile<<endl;
  
  std::uintmax_t num1=removeFrameFiles(options,"png");
  std::uintmax_t num2=removeFrameFiles(options,"iff");
  if(strict && num1>0 && num2>0 && num1!=num2) {
    throw AGAConvException(73, "Number of png files is differnt to iff files ("+std::to_string(num1)+" vs "+std::to_string(num2)+" - something went wrong in the conversion.");
  }

  auto tmpDir=options.getTmpDirName();
  success=std::filesystem::remove(tmpDir);
  if(success) {
    if(options.verbose==2)
      cout<<"- Removed empty tmp dir "<<tmpDir<<endl;
  } else {
    throw AGAConvException(74, "Failed to remove tmp dir "+tmpDir.string());
  }
  if(options.verbose>=1) cout<<"Removed all temporary files and tmp dir "<<options.getTmpDirName()<<endl;

}

void ExternalToolDriver::finalizeTmpDir(const Options& options) {
  namespace fs = std::filesystem;
  fs::path tmpPath(options.getTmpDirName());
  if(options.keepTmpFiles) {
    if(options.verbose>=1) cout<<"Keeping files in tmp dir "<<tmpPath<<endl;
    return;
  }
  bool strict=true;
  removeTmpDir(options,strict);
}

void ExternalToolDriver::runFFMPEGVideoExtraction(const Options& options) {
  string heightString;
  if(options.height==Options::autoValue) {
    // Height 'auto' mode
    // ffmpeg vars: iw, ih; yscale: ratio=iw/width; height=ih/ratio*yscale (explicit values with iw, ih for ffmpeg)
    string divString="unset";
    if(options.resMode==Options::GFX_UNSPECIFIED) {
      divString="";
    } else {
      if(!options.screenModeLace) {
        if(options.resMode==Options::GFX_SUPERHIRES)
          divString="/4";
        else if(options.resMode==Options::GFX_HIRES)
          divString="/2";
        else
          divString="";
        assert(options.resMode!=Options::GFX_AUTO);
      } else {
        divString="";
      }
    }
    assert(divString!="unset");
    heightString="(ih"+divString+")"+"/(iw/"+std::to_string(options.width)+")*"+std::to_string(options.yScaleFactor);
  } else {
    // Height 'auto' mode off. Height is explicitly defined by user or in config file.
    // Consequently, screen-resolution and scale-ratio are not considered in height computation.
    heightString=std::to_string(options.height);
  }

  string ffmpegBlackAndWhiteOption;
  if(options.blackAndWhite)
    ffmpegBlackAndWhiteOption=",format=gray";

  stringstream videoCommand;
  string ffmpegVerbosity;
  if(options.verbose<=1)
    ffmpegVerbosity=this->_allQuietOptions;
  else if(options.verbose<=2)
    ffmpegVerbosity=this->_frameInfoOptions;
  else
    ffmpegVerbosity="";
  
  if(options.conversionTool=="ffmpeg") {
    videoCommand
      <<"-i "<<options.inFileName
      <<" "<<ffmpegVerbosity
      <<" "<<"-filter_complex \"[0:v] fps="<<options.fps
      <<ffmpegBlackAndWhiteOption
      <<",scale=w="<<options.width
      <<":h="<<heightString<<":sws_flags=lanczos:param0=3:sws_dither=none,split [a][b];[a] palettegen=max_colors="<<options.maxColorsCorrected()
      <<":stats_mode=single:reserve_transparent=false [p];[b][p] paletteuse=new=1:dither="<<options.ditherMode<<"\""
      <<" "<<(options.getTmpDirName()/(options.ffmpegFrameNameSuffix()+".png"))
      ;
  } else {
    // For all other conversion tools only extract frames and resize, don't change color format (except for black-and-white option)
    videoCommand
      <<"-i "<<options.inFileName
      <<" "<<ffmpegVerbosity
      <<" "<<"-filter_complex \"[0:v] fps="<<options.fps
      <<ffmpegBlackAndWhiteOption
      <<",scale=w="<<options.width<<":h="+heightString<<":sws_flags=lanczos:param0=3:sws_dither=none\""
      <<" "<<(options.getTmpDirName()/(options.ffmpegFrameNameSuffix()+".png"))
      ;
  }
  runFFMPEG(options,videoCommand.str(), "ffmpeg (extracting frames as PNG files)");
}

void ExternalToolDriver::runFFMPEG(const Options& options, const string& ffmpegCommandLine, const string& info) {
  runTool(options, "ffmpeg",ffmpegCommandLine, info);
}

void ExternalToolDriver::runTool(const Options& options, const string& tool, const string& commandLine, const string& info) {
  if(options.verbose>=1) cout<<"Running external tool "<<info<<endl;
  string cl=tool+" "+commandLine;
  // Verbose is reversed, because output is turned off up to this level, hence, turned on above it
  if(options.verbose<=1) {
    cl+=(" "+_osLayer->redirectOutputToNullDevice());
  }
  if(options.verbose>=3) {
    cout<<"TOOL COMMAND: "<<cl<<endl;
  }
  if(!_osLayer->isInstalledTool(tool)) {
    // Command doesn't exist
    throw AGAConvException(78, "Tool "+tool+" is not installed");
  }
  int systemCode=std::system((const char *) (cl.c_str()));
  if(systemCode!=0) {
    throw AGAConvException(75, "Invocation of "+tool+" failed with system code "+std::to_string(systemCode));
  }
}

void ExternalToolDriver::runHamConvert(Options& options) {
  if(options.hcPath=="") {
    throw AGAConvException(76, options.colorMode+" conversion requested, but 'hc_path' not set. Cannot find ham_convert. Exiting.");
  }
  const std::filesystem::path tmpDir(options.getTmpDirName());
  std::list<std::string> batchFileLines;
  for (auto const& dir_entry : std::filesystem::directory_iterator{options.getTmpDirName()})  {
    stringstream ss;
    std::filesystem::path filePath=dir_entry.path();
    string fileName=filePath.stem();
    string fileExt=filePath.extension();
    std::regex frameFileRegex("frame[0-9]+");
    string extension="png";
    if(std::regex_match(fileName, frameFileRegex) && fileExt==("."+extension)) {
      ss<<dir_entry;
    }
    batchFileLines.push_back(ss.str());
  }
  batchFileLines.sort();
  // Open file
  ofstream batchFile;
  std::filesystem::path batchFileName=options.getTmpDirName()/options.hamConvertBatchFileName;
  batchFile.open(batchFileName, ios::out | ios::binary);
  if(!batchFile.is_open()) {
    throw AGAConvException(77, "could not open batch file "+batchFileName.string());
  }
  for(auto line : batchFileLines) {
    batchFile<<line<<endl;
  }
  batchFile.close();
  const string jvmMemoryOptions="-Xms500m -Xmx2g";
  const string hamQuality="q0"; // ham6:q0-7, ham8:q0-4
  string hcColorMode="";
  if(options.colorMode=="ehb")
    hcColorMode="ehb";
  else
    hcColorMode=options.colorMode+"_"+"q"+std::to_string(options.hcHamQuality);
  string hamConvertCommand=
    jvmMemoryOptions+" -jar "+options.hcPathResolved.string()
    +" "+batchFileName.string()
    +" "+hcColorMode
    +" "+(options.reserveBlackBackgroundColor?"black_bkd":"")
    +" norle nopng";
  if(options.hcDither!="auto")
    hamConvertCommand+=+" dither_"+options.hcDither;
  if(options.hcDither=="fs" && options.hcPropagation!=Options::autoValue)
    hamConvertCommand+=(" propagation_"+std::to_string(options.hcPropagation)); // requires dither_fs
  if(options.colorMode!="ham8" && options.hcDiversity!=Options::autoValue)
    hamConvertCommand+=(" diversity_"+std::to_string(options.hcDiversity));
  if(options.hcQuant!="auto")
    hamConvertCommand+=(" quant_"+options.hcQuant);

  const string xvfbTool="xvfb-run";
  const string javaTool="java";
  // check xvfb-run (only relevant for MS Ubuntu app). If available use it, otherwise not.
  if(_osLayer->isInstalledTool(xvfbTool) && _osLayer->isInstalledTool(javaTool)) {
    // with xvfb-run
    ExternalToolDriver::runTool(options, xvfbTool ,string("-a ")+javaTool+" "+hamConvertCommand, "ham_convert with xvfb-run (converting PNG frames to IFF/ILBM frames)");
  } else {
    // with java only (reports error if not installed)
    ExternalToolDriver::runTool(options, javaTool, hamConvertCommand, "ham_convert (converting PNG frames to IFF/ILBM frames)");
  }

  CDXLEncode stage;
  std::filesystem::path inFileWithPath=options.getTmpDirName()/
    ("frame"
     +options.firstFrameNumberToString()
     +"_output.iff");
  stage.setInFileWithPath(inFileWithPath);
  stage.run(options);
}

} // namespace AGAConv
