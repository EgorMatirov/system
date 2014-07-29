#include "native_compilable.hpp"

namespace bacs{namespace system{namespace builders
{
    executable_ptr native_compilable::create_executable(
        const ContainerPointer &container,
        bunsan::tempfile &&tmpdir,
        const name_type &name)
    {
        executable_ptr tmp(
            new native_compilable_executable(container, std::move(tmpdir), name)
        );
        return tmp;
    }

    ProcessPointer native_compilable_executable::create(
        const ProcessGroupPointer &process_group,
        const ProcessArguments &arguments)
    {
        const ProcessPointer process =
            process_group->createProcess(executable());
        process->setArguments(process->executable(), arguments);
        return process;
    }
}}}
