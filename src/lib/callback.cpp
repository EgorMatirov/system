#include <bacs/system/callback.hpp>

namespace bacs {
namespace system {
namespace callback {

BUNSAN_FACTORY_DEFINE(base)

base_ptr base::instance(const Callback &config) {
  const std::vector<std::string> arguments(config.argument().begin(),
                                           config.argument().end());
  return instance(config.type(), arguments);
}

}  // namespace callback
}  // namespace system
}  // namespace bacs
