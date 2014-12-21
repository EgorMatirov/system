#pragma once

#include <bunsan/error.hpp>

namespace bacs{namespace system
{
    struct error: virtual bunsan::error {};

    struct invalid_argument_error: virtual error
    {
        typedef boost::error_info<struct tag_argument, std::string> argument;
    };

    struct builder_error: virtual error {};
    struct builder_build_error: virtual builder_error {};
}}
