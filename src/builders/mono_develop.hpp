#pragma once

#include "project.hpp"

namespace bacs{namespace system{namespace builders
{
    class mono_develop: public project
    {
    public:
        explicit mono_develop(const std::vector<std::string> &arguments);

    protected:
        executable_ptr build_extracted(
            const ContainerPointer &container,
            const unistd::access::Id &owner_id,
            bunsan::tempfile &&tmpdir,
            const boost::filesystem::path &source,
            const bacs::process::ResourceLimits &resource_limits,
            bacs::process::BuildResult &result) override;

    private:
        std::string m_configuration;

    private:
        static const bool factory_reg_hook;
    };
}}}
