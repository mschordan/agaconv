#ifndef OS_LAYER_MACOS_HPP
#define OS_LAYER_MACOS_HPP

#ifdef __MACH__

#include "OSLayer.hpp"
#include <string>
#include <filesystem>

namespace AGAConv {

class OSLayerMacOs : public OSLayer {
public:
  bool isSupported() override;
  std::string redirectOutputToNullDevice() override;
  bool isInstalledTool(const std::string& toolName) override;
  std::string getPidString() override;
  std::filesystem::path getDefaultConfigFileName() override;
  std::string getHomeDirString() override;

protected:
  const std::string defaultAgaConvConfigDir=".agaconv";
  const std::string defaultConfigFileName="default.config";
};

} // namespace AGAConv

#endif  // __MACH__
#endif
