#include "kturtle.hpp"

#include <bunsan/static_initializer.hpp>

#include <boost/assert.hpp>

namespace bacs {
namespace system {
namespace builders {

BUNSAN_STATIC_INITIALIZER(bacs_system_builders_kturtle, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      builder, kturtle, [](const std::vector<std::string> &arguments) {
        return builder::make_shared<kturtle>(arguments);
      })
})

kturtle::kturtle(const std::vector<std::string> &arguments) {
  BOOST_ASSERT(arguments.empty());
  m_flags.push_back("--test");
}

ProcessPointer kturtle::create_process(const ProcessGroupPointer &process_group,
                                      const name_type &name) {
  const ProcessPointer process = process_group->createProcess("kturtle_interpreter");
  process->setArguments(process->executable(), "--check", name.source);
  return process;
}

executable_ptr kturtle::create_executable(const ContainerPointer &container,
                                         bunsan::tempfile &&tmpdir,
                                         const name_type &name) {
  return std::make_shared<interpretable_executable>(
      container, std::move(tmpdir), name, "kturtle_interpreter", m_flags);
}

}  // namespace builders
}  // namespace system
}  // namespace bacs
