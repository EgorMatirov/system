#pragma once

#include <bacs/process.pb.h>

#include <yandex/contest/invoker/Process.hpp>
#include <yandex/contest/invoker/ProcessGroup.hpp>

namespace bacs {
namespace system {
namespace process {

void setup(const yandex::contest::invoker::ProcessPointer &process,
           const bacs::process::ResourceLimits &resource_limits);

void setup(const yandex::contest::invoker::ProcessGroupPointer &process_group,
           const yandex::contest::invoker::ProcessPointer &process,
           const bacs::process::ResourceLimits &resource_limits);

/// \return true if process has completed successfully
bool parse_result(
    const yandex::contest::invoker::ProcessGroup::Result &process_group_result,
    const yandex::contest::invoker::Process::Result &process_result,
    bacs::process::ExecutionResult &result);

}  // namespace process
}  // namespace system
}  // namespace bacs
