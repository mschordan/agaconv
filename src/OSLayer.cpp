
#include "OSLayer.hpp"
#include "OSLayerFallback.hpp"
#include "OSLayerLinux.hpp"
#include "OSLayerMacOs.hpp"

namespace AGAConv {

OSLayer* OSLayer::createOSLayer() {
#ifdef __linux__
  return new OSLayerLinux();
#elif __MACH__
  return new OSLayerMacOs();
#else
  return new OSLayerFallback();
#endif
}

} // namespace AGAConv
