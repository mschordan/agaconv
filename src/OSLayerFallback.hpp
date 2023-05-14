#ifndef OS_LAYER_FALLBACK_HPP
#define OS_LAYER_FALLBACK_HPP

#include "OSLayer.hpp"
#include <string>

namespace AGAConv {

class OSLayerFallback : public OSLayer {
public:
  bool isSupported() override;
  std::string redirectOutputToNullDevice() override;
  bool isInstalledTool(const std::string& toolName) override;
  std::string getPidString() override;
  std::filesystem::path getDefaultConfigFileName() override;
  ~OSLayerFallback();
protected:
  std::string getHomeDirString() override;
};

} // namespace AGAConv

#endif
