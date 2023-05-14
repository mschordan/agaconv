#include "OSLayerFallback.hpp"

using std::string;

namespace AGAConv {

OSLayerFallback::~OSLayerFallback() {
}

bool OSLayerFallback::isSupported() {
  // Reports not to be a support OS. Used to trigger some warnings.
  return false;
}

std::string OSLayerFallback::redirectOutputToNullDevice() {
  // Simply do not provide feature as fallback. It is used for verbose <= 1 mode.
  return "";
}

bool OSLayerFallback::isInstalledTool(const std::string& toolName) {
  // Return true by default. If it is installed, it will execute the command with the std::system call. If this test is
  // not performed, the tool will be attempted to run and eventually fail, if not installed and the error message will
  // be less clear. However, by returning true it does not prevent it from executing if installed.
  return true;
}

string OSLayerFallback::getPidString() {
  // Return "0" string, not guaranteed to be unique, but agaconv still uses some properties of the video such as
  // filename, colormode, width, to name the tmp dir.
  return "0";
}

std::filesystem::path OSLayerFallback::getDefaultConfigFileName() {
  // Empty string triggers not to generate default config file and to issue warning.
  return "";
}

string OSLayerFallback::getHomeDirString() {
  // Return empty string, supposed to be used by getDefaultConfigFileName.
  return "";
}

} // namespace AGAConv
