
#include "OSLayer.hpp"
#include "OSLayerFallback.hpp"
#include "OSLayerLinux.hpp"

namespace AGAConv {

OSLayer* OSLayer::createOSLayer() {
#ifdef __linux__
  return new OSLayerLinux();
#else
  return new OSLayerFallback();
#endif
}

} // namespace AGAConv
