#include "project.hpp"

#include <bacs/system/process.hpp>

#include <yandex/contest/system/unistd/Operations.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/utility/archiver.hpp>
#include <bunsan/utility/resolver.hpp>

#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>

namespace bacs{namespace system{namespace builders
{
    static const boost::filesystem::path project_path =
        "/builders_project";

    static const boost::filesystem::path source_path = "source";
    static const boost::filesystem::path source_archive_path = "source_archive";

    static bool extract(
        const bacs::process::Source &source,
        const boost::filesystem::path &root,
        const unistd::access::Id &owner_id,
        std::string &error)
    {
        using namespace bunsan::utility;
        using boost::filesystem::recursive_directory_iterator;

        if (!source.has_archiver())
            BOOST_THROW_EXCEPTION(
                incompatible_builder_error() <<
                incompatible_builder_error::message(
                    "Directory tree source is required"));

        boost::property_tree::ptree config;
        if (source.archiver().has_format())
            config.put("format", source.archiver().format());
        resolver rs;
        const archiver_ptr ar =
            archiver::instance(source.archiver().type(), rs);
        ar->setup(config);

        try
        {
            bunsan::filesystem::ofstream fout(root / source_archive_path);
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout)
            {
                fout << source.data();
            }
            BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
            fout.close();

            ar->unpack(
                root / source_archive_path,
                root / source_path
            );

            for (recursive_directory_iterator i(root / source_path),
                                              end;
                 i != end;
                 ++i)
            {
                unistd::lchown(i->path(), owner_id);
            }
        }
        catch (std::exception &e)
        {
            error = e.what();
            return false;
        }
        return true;
    }

    executable_ptr project::build(
        const ContainerPointer &container,
        const unistd::access::Id &owner_id,
        const bacs::process::Source &source,
        const bacs::process::ResourceLimits &resource_limits,
        bacs::process::BuildResult &result)
    {
        const boost::filesystem::path solutions =
            container->filesystem().keepInRoot(project_path);
        boost::filesystem::create_directories(solutions);
        bunsan::tempfile tmpdir =
            bunsan::tempfile::directory_in_directory(solutions);
        container->filesystem().setOwnerId(
            project_path / tmpdir.path().filename(), owner_id);
        BOOST_VERIFY(boost::filesystem::create_directory(
            tmpdir.path() / source_path
        ));
        std::string error;
        if (!extract(source, tmpdir.path(), owner_id, error))
        {
            result.set_status(bacs::process::BuildResult::FAILED);
            result.set_output(error);
            return executable_ptr();
        }
        return build_extracted(
            container,
            owner_id,
            std::move(tmpdir),
            project_path / tmpdir.path().filename() / source_path,
            resource_limits,
            result
        );
    }

    project_executable::project_executable(
        const ContainerPointer &container,
        bunsan::tempfile &&tmpdir,
        const boost::filesystem::path &executable):
            m_container(container),
            m_tmpdir(std::move(tmpdir)),
            m_executable(executable) {}

    ProcessPointer project_executable::create(
        const ProcessGroupPointer &process_group,
        const ProcessArguments &arguments)
    {
        const ProcessPointer process =
            process_group->createProcess(executable());
        process->setArguments(process->executable(), arguments);
        return process;
    }

    ContainerPointer project_executable::container()
    {
        return m_container;
    }

    boost::filesystem::path project_executable::dir() const
    {
        return project_path / m_tmpdir.path().filename();
    }

    boost::filesystem::path project_executable::source() const
    {
        return dir() / source_path;
    }

    boost::filesystem::path project_executable::executable() const
    {
        return dir() / m_executable;
    }
}}}
