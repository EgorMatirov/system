#include "interpretable.hpp"

namespace bacs{namespace system{namespace builders
{
    compilable::name_type interpretable::name(
        const bacs::process::Source &/*source*/)
    {
        static const std::string script = "script";
        return {.source = script, .executable = script};
    }

    interpretable_executable::interpretable_executable(
        const ContainerPointer &container,
        bunsan::tempfile &&tmpdir,
        const compilable::name_type &name,
        const boost::filesystem::path &executable,
        const std::vector<std::string> &flags):
            compilable_executable(container, std::move(tmpdir), name),
            m_executable(executable), m_flags(flags) {}

    ProcessPointer interpretable_executable::create(
        const ProcessGroupPointer &process_group,
        const ProcessArguments &arguments)
    {
        const ProcessPointer process =
            process_group->createProcess(m_executable);
        process->setArguments(
            process->executable(),
            this->arguments(),
            arguments
        );
        return process;
    }

    std::vector<std::string> interpretable_executable::arguments() const
    {
        std::vector<std::string> arguments_ = flags();
        arguments_.push_back(executable().string());
        return arguments_;
    }

    std::vector<std::string> interpretable_executable::flags() const
    {
        return m_flags;
    }
}}}
