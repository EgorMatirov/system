#pragma once

#include <bacs/system/system_verifier.hpp>

#include <turtle/mock.hpp>

namespace bacs {
namespace system {

MOCK_BASE_CLASS(mock_system_verifier, system_verifier) {
  MOCK_METHOD(verify, 2, bool(const problem::System &request,
                              problem::SystemResult &result))
};

}  // namespace system
}  // namespace bacs
