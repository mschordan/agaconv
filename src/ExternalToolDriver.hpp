#ifndef EXTERNAL_TOOL_DRIVER_HPP
#define EXTERNAL_TOOL_DRIVER_HPP

#include <string>
#include "CDXLEncode.hpp"
#include "Options.hpp"
#include "OSLayer.hpp"

namespace AGAConv {

class ExternalToolDriver {
public:
  ExternalToolDriver();
  void checkCommandProcessor();
  void runFFMPEGExtraction(const Options& options);
  void runHamConvert(Options& options);
  void prepareTmpDir(const Options& options); // Modifies tmpDir if necessary
  void finalizeTmpDir(const Options& options);
  void runTool(const Options& options, const std::string& tool, const std::string& commandLine, const std::string& info);
private:
  void runFFMPEGAudioExtraction(const Options& options);
  void runFFMPEGVideoExtraction(const Options& options);
  void runFFMPEG(const Options& options, const std::string& ffmpegCommandLine, const std::string& info);
  void removeTmpDir(const Options& options, bool strict);
  std::uintmax_t removeFrameFiles(const Options& options, std::string extension);
  const std::string _quietOptions="-y -hide_banner";
  const std::string _allQuietOptions=_quietOptions+" "+"-loglevel fatal -nostats";
  const std::string _frameInfoOptions=_quietOptions+" "+"-loglevel info -nostats";
  std::unique_ptr<OSLayer> _osLayer;
};

} // namespace AGAConv

#endif
