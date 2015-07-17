#pragma once

#include "compilable.hpp"

namespace bacs {
namespace system {
namespace builders {

class native_compilable : public compilable {
 protected:
  executable_ptr create_executable(const ContainerPointer &container,
                                   bunsan::tempfile &&tmpdir,
                                   const name_type &name) override;
};

class native_compilable_executable : public compilable_executable {
 public:
  using compilable_executable::compilable_executable;

  ProcessPointer create(const ProcessGroupPointer &process_group,
                        const ProcessArguments &arguments) override;
};

}  // namespace builders
}  // namespace system
}  // namespace bacs
