#pragma once

#include <bacs/problem/common.hpp>
#include <bacs/problem/problem.pb.h>
#include <bacs/problem/result.pb.h>

#include <bunsan/plugin.hpp>

namespace bacs {
namespace system {

class system_verifier {
  BUNSAN_PLUGIN_AUTO_BODY(system_verifier)
 public:
  virtual ~system_verifier();

  virtual bool verify(const problem::System &request,
                      problem::SystemResult &result) = 0;
};
BUNSAN_PLUGIN_TYPES(system_verifier)

}  // namespace system
}  // namespace bacs
