#include <bacs/system/builder.hpp>

namespace bacs {
namespace system {

BUNSAN_FACTORY_DEFINE(builder)

builder_ptr builder::instance(const bacs::process::BuilderConfig &config) {
  return instance(config.type(),
                  std::vector<std::string>{begin(config.argument()),
                                           end(config.argument())});
}

}  // namespace system
}  // namespace bacs
