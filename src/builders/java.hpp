#pragma once

#include "interpretable.hpp"

#include <memory>

namespace bacs{namespace system{namespace builders
{
    class java: public interpretable
    {
    public:
        explicit java(
            const std::vector<std::string> &arguments,
            const bool parse_name=true);

        executable_ptr build(
            const ContainerPointer &container,
            const unistd::access::Id &owner_id,
            const std::string &source,
            const bacs::process::ResourceLimits &resource_limits,
            bacs::process::BuildResult &result) override;

    protected:
        name_type name(const std::string &source);

        ProcessPointer create_process(
            const ProcessGroupPointer &process_group,
            const name_type &name) override;

        executable_ptr create_executable(
            const ContainerPointer &container,
            bunsan::tempfile &&tmpdir,
            const name_type &name) override;

    private:
        std::unique_ptr<java> m_java;
        std::string m_class = "Main";
        std::string m_lang;
        std::vector<std::string> m_flags;

    private:
        static const bool factory_reg_hook;
    };

    class java_executable: public interpretable_executable
    {
    public:
        using interpretable_executable::interpretable_executable;

    protected:
        std::vector<std::string> arguments() const override;
    };
}}}
