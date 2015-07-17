#include "compilable.hpp"

#include <bacs/system/process.hpp>

#include <bunsan/filesystem/fstream.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

namespace bacs {
namespace system {
namespace builders {

static const boost::filesystem::path compilable_path = "/builders_compilable";

executable_ptr compilable::build(
    const ContainerPointer &container, const unistd::access::Id &owner_id,
    const bacs::process::Source &source,
    const bacs::process::ResourceLimits &resource_limits,
    bacs::process::BuildResult &result) {
  try {
    const boost::filesystem::path solutions =
        container->filesystem().keepInRoot(compilable_path);
    boost::filesystem::create_directories(solutions);
    bunsan::tempfile tmpdir =
        bunsan::tempfile::directory_in_directory(solutions);
    container->filesystem().setOwnerId(
        compilable_path / tmpdir.path().filename(), owner_id);
    const name_type name_ = name(source);
    if (source.has_archiver()) {
      result.set_status(bacs::process::BuildResult::FAILED);
      result.set_output("Directory tree source is not supported");
      return nullptr;
    }
    bunsan::filesystem::ofstream fout(tmpdir.path() / name_.source);
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
      fout << source.data();
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
    fout.close();
    container->filesystem().setOwnerId(
        compilable_path / tmpdir.path().filename() / name_.source, owner_id);
    const ProcessGroupPointer process_group = container->createProcessGroup();
    const ProcessPointer process = create_process(process_group, name_);
    process::setup(process_group, process, resource_limits);
    process->setCurrentPath(compilable_path / tmpdir.path().filename());
    process->setOwnerId(owner_id);
    process->setStream(2, FdAlias(1));
    process->setStream(1, File("log", AccessMode::WRITE_ONLY));
    const ProcessGroup::Result process_group_result =
        process_group->synchronizedCall();
    const Process::Result process_result = process->result();
    const bool success = process::parse_result(
        process_group_result, process_result, *result.mutable_execution());
    bunsan::filesystem::ifstream fin(tmpdir.path() / "log");
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin) {
      std::string &output = *result.mutable_output();
      char buf[4096];
      fin.read(buf, sizeof(buf));
      output.assign(buf, fin.gcount());
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
    fin.close();
    if (success) {
      result.set_status(bacs::process::BuildResult::OK);
      return create_executable(container, std::move(tmpdir), name_);
    } else {
      result.set_status(bacs::process::BuildResult::FAILED);
      return nullptr;
    }
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(builder_build_error()
                          << bunsan::enable_nested_current());
  }
}

compilable::name_type compilable::name(
    const bacs::process::Source & /*source*/) {
  return {.source = "source", .executable = "executable"};
}

compilable_executable::compilable_executable(const ContainerPointer &container,
                                             bunsan::tempfile &&tmpdir,
                                             const compilable::name_type &name)
    : m_container(container), m_tmpdir(std::move(tmpdir)), m_name(name) {}

ContainerPointer compilable_executable::container() { return m_container; }

const compilable::name_type &compilable_executable::name() const {
  return m_name;
}

boost::filesystem::path compilable_executable::dir() const {
  return compilable_path / m_tmpdir.path().filename();
}

boost::filesystem::path compilable_executable::source() const {
  return dir() / m_name.source;
}

boost::filesystem::path compilable_executable::executable() const {
  return dir() / m_name.executable;
}

}  // namespace builders
}  // namespace system
}  // namespace bacs
