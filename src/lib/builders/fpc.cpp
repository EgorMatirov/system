#include "fpc.hpp"

#include <bunsan/static_initializer.hpp>

#include <boost/assert.hpp>
#include <boost/regex.hpp>

namespace bacs {
namespace system {
namespace builders {

BUNSAN_STATIC_INITIALIZER(bacs_system_builders_fpc, {
  BUNSAN_FACTORY_REGISTER_TOKEN(builder, fpc,
                                [](const std::vector<std::string> &arguments) {
                                  return builder::make_shared<fpc>(arguments);
                                })
})

static const boost::regex positional("[^=]+"), key_value("([^=]+)=(.*)");

fpc::fpc(const std::vector<std::string> &arguments) {
  for (const std::string &arg : arguments) {
    boost::smatch match;
    if (boost::regex_match(arg, match, positional)) {
      BOOST_ASSERT(match.size() == 1);
      m_flags.push_back("-" + arg);
    } else if (boost::regex_match(arg, match, key_value)) {
      BOOST_ASSERT(match.size() == 3);
      const std::string key = match[1].str(), value = match[2].str();
      if (key == "optimize") {
        m_flags.push_back("-O" + value);
      } else if (key == "lang") {
        m_flags.push_back("-M" + value);
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

ProcessPointer fpc::create_process(const ProcessGroupPointer &process_group,
                                   const name_type &name) {
  const ProcessPointer process = process_group->createProcess("fpc");
  process->setArguments(process->executable(), m_flags, name.source,
                        "-o" + name.executable.string());
  return process;
}

}  // namespace builders
}  // namespace system
}  // namespace bacs
