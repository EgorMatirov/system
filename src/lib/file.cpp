#include <bacs/system/file.hpp>

#include <bunsan/filesystem/fstream.hpp>

namespace bacs {
namespace system {
namespace file {

std::string read(const boost::filesystem::path &path,
                 const bacs::file::Range &range) {
  std::string data;
  bunsan::filesystem::ifstream fin(path, std::ios::binary);
  BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
    switch (range.whence()) {
      case bacs::file::Range::BEGIN:
        fin.seekg(range.offset(), std::ios::beg);
        break;
      case bacs::file::Range::END:
        fin.seekg(range.offset(), std::ios::end);
        break;
    }
    char buf[4096];
    while (fin && data.size() < range.size()) {
      fin.read(buf, std::min(sizeof(buf), range.size() - data.size()));
      data.insert(data.end(), buf, buf + fin.gcount());
    }
  } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
  fin.close();
  return data;
}

static std::string read(const boost::filesystem::path &path,
                        const bacs::file::Range::Whence whence,
                        const std::int64_t offset, const std::uint64_t size) {
  bacs::file::Range range;
  range.set_whence(whence);
  range.set_offset(offset);
  range.set_size(size);
  return read(path, range);
}

std::string read_first(const boost::filesystem::path &path,
                       const std::uint64_t size) {
  return read(path, bacs::file::Range::BEGIN, 0, size);
}

std::string read_last(const boost::filesystem::path &path,
                      const std::uint64_t size) {
  return read(path, bacs::file::Range::END, 0, size);
}

}  // namespace file
}  // namespace system
}  // namespace bacs
