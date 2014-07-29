#pragma once

#include <bacs/system/builder.hpp>

#include <yandex/contest/invoker/All.hpp>

#include <bunsan/tempfile.hpp>

namespace bacs{namespace system{namespace builders
{
    using namespace yandex::contest::invoker;
    namespace unistd = yandex::contest::system::unistd;

    class compilable: public builder
    {
    public:
        struct name_type
        {
            boost::filesystem::path source, executable;
        };

    public:
        executable_ptr build(
            const ContainerPointer &container,
            const unistd::access::Id &owner_id,
            const bacs::process::Source &source,
            const bacs::process::ResourceLimits &resource_limits,
            bacs::process::BuildResult &result) override;

    protected:
        virtual name_type name(const bacs::process::Source &source);

        virtual ProcessPointer create_process(
            const ProcessGroupPointer &process_group,
            const name_type &name)=0;

        virtual executable_ptr create_executable(
            const ContainerPointer &container,
            bunsan::tempfile &&tmpdir,
            const name_type &name)=0;
    };

    class compilable_executable: public executable
    {
    public:
        compilable_executable(
            const ContainerPointer &container,
            bunsan::tempfile &&tmpdir,
            const compilable::name_type &name);

    protected:
        ContainerPointer container();
        const compilable::name_type &name() const;
        boost::filesystem::path dir() const;
        boost::filesystem::path source() const;
        boost::filesystem::path executable() const;

    private:
        const ContainerPointer m_container;
        const bunsan::tempfile m_tmpdir;
        const compilable::name_type m_name;
    };
}}}
