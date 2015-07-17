#include "xmlrpc.hpp"

#include <bunsan/enable_error_info.hpp>
#include <bunsan/static_initializer.hpp>

#define BUNSAN_EXCEPTIONS_WRAP_END_XMLRPC() \
  BUNSAN_EXCEPTIONS_WRAP_END_EXCEPT(::girerr::error)

namespace bacs {
namespace system {
namespace callback {
namespace callbacks {

BUNSAN_STATIC_INITIALIZER(bacs_system_callback_callbacks_xmlrpc, {
  BUNSAN_FACTORY_REGISTER_TOKEN(base, xmlrpc,
                                [](const std::vector<std::string> &arguments) {
                                  return base::make_shared<xmlrpc>(arguments);
                                })
})

xmlrpc::xmlrpc(const std::vector<std::string> &arguments) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    for (std::size_t i = 0; i < arguments.size(); ++i) {
      switch (i) {
        case 0:
          m_uri = arguments[i];
          break;
        case 1:
          m_method = arguments[i];
          break;
        default:
          m_arguments.addc(arguments[i]);
      }
    }
  } BUNSAN_EXCEPTIONS_WRAP_END_XMLRPC()
}

void xmlrpc::call(const data_type &data) {
  BUNSAN_EXCEPTIONS_WRAP_BEGIN() {
    xmlrpc_c::paramList argv(m_arguments);
    argv.addc(data);
    xmlrpc_c::value result;
    m_proxy.call(m_uri, m_method, argv, &result);
  } BUNSAN_EXCEPTIONS_WRAP_END_XMLRPC()
}

}  // namespace callbacks
}  // namespace callback
}  // namespace system
}  // namespace bacs
