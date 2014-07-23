#pragma once

#include <bacs/system/builder.hpp>

#include <yandex/contest/invoker/All.hpp>

#include <bunsan/tempfile.hpp>

namespace bacs{namespace system{namespace builders
{
    using namespace yandex::contest::invoker;
    namespace unistd = yandex::contest::system::unistd;

    class project: public builder
    {
    public:
        executable_ptr build(
            const ContainerPointer &container,
            const unistd::access::Id &owner_id,
            const bacs::process::Source &source,
            const bacs::process::ResourceLimits &resource_limits,
            bacs::process::BuildResult &result) override;

    protected:
        virtual executable_ptr build_extracted(
            const ContainerPointer &container,
            const unistd::access::Id &owner_id,
            bunsan::tempfile &&tmpdir,
            const boost::filesystem::path &source,
            const bacs::process::ResourceLimits &resource_limits,
            bacs::process::BuildResult &result)=0;
    };

    class project_executable: public executable
    {
    public:
        project_executable(
            const ContainerPointer &container,
            bunsan::tempfile &&tmpdir,
            const boost::filesystem::path &executable);

        ProcessPointer create(
            const ProcessGroupPointer &process_group,
            const ProcessArguments &arguments) override;

    protected:
        ContainerPointer container();
        boost::filesystem::path dir() const;
        boost::filesystem::path source() const;
        boost::filesystem::path executable() const;

    private:
        const ContainerPointer m_container;
        const bunsan::tempfile m_tmpdir;
        const boost::filesystem::path m_executable;
    };
}}}
