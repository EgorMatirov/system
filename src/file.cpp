#include <bacs/system/file.hpp>

#include <bunsan/filesystem/fstream.hpp>

namespace bacs{namespace system{namespace file
{
    std::string read(const boost::filesystem::path &path,
                     const bacs::file::Range &range)
    {
        std::string data;
        bunsan::filesystem::ifstream fin(path, std::ios::binary);
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
        {
            switch (range.whence())
            {
            case bacs::file::Range::BEGIN:
                fin.seekg(range.offset(), std::ios::beg);
                break;
            case bacs::file::Range::END:
                fin.seekg(range.offset(), std::ios::end);
                break;
            }
            char buf[4096];
            while (fin && data.size() < range.size())
            {
                fin.read(buf, std::min(sizeof(buf), range.size() - data.size()));
                data.insert(data.end(), buf, buf + fin.gcount());
            }
        }
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
        fin.close();
        return data;
    }
}}}
