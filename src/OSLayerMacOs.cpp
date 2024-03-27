#include "OSLayerMacOs.hpp"
#include <filesystem>

#ifdef __MACH__

#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::filesystem::path;

namespace AGAConv {

bool OSLayerMacOs::isSupported() {
  return true;
}

std::string OSLayerMacOs::redirectOutputToNullDevice() {
  return ">/dev/null";
}

bool OSLayerMacOs::isInstalledTool(const std::string& toolName) {
   std::string testCommand="which "+toolName+" > /dev/null 2>&1";
   return std::system(testCommand.c_str())==0;
}

string OSLayerMacOs::getPidString() {
  pid_t pid = getpid();
  return std::to_string(pid);
}

string OSLayerMacOs::getHomeDirString() {
  return getenv("HOME");
}

path OSLayerMacOs::getDefaultConfigFileName() {
  path filePath;
  path homePath=getHomeDirString();
  if(!homePath.empty()) {
    filePath=homePath;
    filePath/=defaultAgaConvConfigDir;
    filePath/=defaultConfigFileName;
  }
  return filePath;
}

}

#endif  // __MACH__
