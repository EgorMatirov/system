#pragma once

#include "interpretable.hpp"

namespace bacs {
namespace system {
namespace builders {

class python : public interpretable {
 public:
  explicit python(const std::vector<std::string> &arguments);

 protected:
  ProcessPointer create_process(const ProcessGroupPointer &process_group,
                                const name_type &name) override;

  executable_ptr create_executable(const ContainerPointer &container,
                                   bunsan::tempfile &&tmpdir,
                                   const name_type &name) override;

 private:
  std::string m_lang;
  std::vector<std::string> m_flags;
};

}  // namespace builders
}  // namespace system
}  // namespace bacs
