#include "mono.hpp"

#include <bunsan/static_initializer.hpp>

#include <boost/assert.hpp>
#include <boost/regex.hpp>

namespace bacs {
namespace system {
namespace builders {

BUNSAN_STATIC_INITIALIZER(bacs_system_builders_mono, {
  BUNSAN_FACTORY_REGISTER_TOKEN(builder, mono,
                                [](const std::vector<std::string> &arguments) {
                                  return builder::make_shared<mono>(arguments);
                                })
})

static const boost::regex positional("[^=]+"), key_value("([^=]+)=(.*)");

mono::mono(const std::vector<std::string> &arguments) {
  for (const std::string &arg : arguments) {
    boost::smatch match;
    if (boost::regex_match(arg, match, positional)) {
      BOOST_ASSERT(match.size() == 1);
      m_flags.push_back("-" + arg);
    } else if (boost::regex_match(arg, match, key_value)) {
      BOOST_ASSERT(match.size() == 3);
      const std::string key = match[1].str(), value = match[2].str();
      if (key == "lang") {
        m_lang = value;
      } else {
        BOOST_THROW_EXCEPTION(invalid_argument_error()
                              << invalid_argument_error::argument(arg));
      }
    } else {
      BOOST_THROW_EXCEPTION(invalid_argument_error()
                            << invalid_argument_error::argument(arg));
    }
  }
}

compilable::name_type mono::name(const bacs::process::Source & /*source*/) {
  return {.source = "source.cs", .executable = "source.exe"};
}

ProcessPointer mono::create_process(const ProcessGroupPointer &process_group,
                                    const name_type &name) {
  const ProcessPointer process = process_group->createProcess(m_lang + "mcs");
  process->setArguments(process->executable(),
                        "-out:" + name.executable.string(), name.source);
  return process;
}

executable_ptr mono::create_executable(const ContainerPointer &container,
                                       bunsan::tempfile &&tmpdir,
                                       const name_type &name) {
  return std::make_shared<interpretable_executable>(
      container, std::move(tmpdir), name, "mono", m_flags);
}

}  // namespace builders
}  // namespace system
}  // namespace bacs
