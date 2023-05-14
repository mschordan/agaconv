#ifndef OS_LAYER_LINUX_HPP
#define OS_LAYER_LINUX_HPP

#ifdef __linux__

#include "OSLayer.hpp"
#include <string>
#include <filesystem>

namespace AGAConv {

class OSLayerLinux : public OSLayer {
public:
  bool isSupported() override;
  std::string redirectOutputToNullDevice() override;
  bool isInstalledTool(const std::string& toolName) override;
  std::string getPidString() override;
  std::filesystem::path getDefaultConfigFileName() override;
  std::string getHomeDirString() override;

protected:
  const std::string defaultLinuxConfigDir=".config";
  const std::string defaultConfigAgaConvDir="agaconv";
  const std::string defaultConfigFileName="default.config";
};

} // namespace AGAConv

#endif  // __linux__
#endif
