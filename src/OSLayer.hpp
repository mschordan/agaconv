#ifndef OS_LAYER_HPP
#define OS_LAYER_HPP

#include <string>
#include <filesystem>

namespace AGAConv {

class OSLayer {
public:
  // Determines OS and creates required system-dependent layer
  static OSLayer* createOSLayer();
  // True for all layers supporting an OS, false for fallback (allows to issue warning)
  virtual bool isSupported()=0;
  // OS string that is appended to a command to redirect output not to be printed
  virtual std::string redirectOutputToNullDevice()=0;
  // Checks if tool is installed.
  virtual bool isInstalledTool(const std::string& toolName)=0;
  // Used to make tmp dir unique for parallel instances of agaconv
  virtual std::string getPidString()=0;
  // Used for determinig location of default config file
  virtual std::string getHomeDirString()=0;
  virtual std::filesystem::path getDefaultConfigFileName()=0;
  virtual ~OSLayer() = default;
private:
};

} // namespace AGAConv

#endif
