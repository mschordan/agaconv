#include "OSLayerLinux.hpp"
#include <filesystem>

#ifdef __linux__

#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::filesystem::path;

namespace AGAConv {

bool OSLayerLinux::isSupported() {
  return true;
}

std::string OSLayerLinux::redirectOutputToNullDevice() {
  return ">/dev/null";
}

bool OSLayerLinux::isInstalledTool(const std::string& toolName) {
   std::string testCommand="which "+toolName+" > /dev/null 2>&1";
   return std::system(testCommand.c_str())==0;
}

string OSLayerLinux::getPidString() {
  pid_t pid = getpid();
  return std::to_string(pid);
}

string OSLayerLinux::getHomeDirString() {
  return getenv("HOME");
}

path OSLayerLinux::getDefaultConfigFileName() {
  path filePath;
  path homePath=getHomeDirString();
  if(!homePath.empty()) {
    filePath=homePath;
    filePath/=defaultLinuxConfigDir;
    filePath/=defaultConfigAgaConvDir;
    filePath/=defaultConfigFileName;
  }
  return filePath;
}

}

#endif  // __linux__
