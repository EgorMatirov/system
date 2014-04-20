#pragma once

#include <bacs/file.pb.h>

#include <boost/filesystem/path.hpp>

#include <string>

namespace bacs{namespace system{namespace file
{
    std::string read(const boost::filesystem::path &path,
                     const bacs::file::Range &range);
}}}
