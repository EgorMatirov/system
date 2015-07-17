#include <bacs/system/process.hpp>

#include <yandex/contest/invoker/All.hpp>

#include <bunsan/config/cast.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <sstream>

namespace bacs {
namespace system {
namespace process {

using namespace yandex::contest::invoker;

#define RLIM_UPDATE(SRC, DST, TR) \
  if (resource_limits.has_##SRC()) rlimit.DST = TR(resource_limits.SRC())

void setup(const yandex::contest::invoker::ProcessGroupPointer &process_group,
           const yandex::contest::invoker::ProcessPointer &process,
           const bacs::process::ResourceLimits &resource_limits) {
  ProcessGroup::ResourceLimits rlimit = process_group->resourceLimits();
  RLIM_UPDATE(real_time_limit_millis, realTimeLimit, std::chrono::milliseconds);
  process_group->setResourceLimits(rlimit);

  setup(process, resource_limits);
}

void setup(const yandex::contest::invoker::ProcessPointer &process,
           const bacs::process::ResourceLimits &resource_limits) {
  Process::ResourceLimits rlimit = process->resourceLimits();
  RLIM_UPDATE(time_limit_millis, timeLimit, std::chrono::milliseconds);
  RLIM_UPDATE(memory_limit_bytes, memoryLimitBytes, );
  RLIM_UPDATE(output_limit_bytes, outputLimitBytes, );
  RLIM_UPDATE(number_of_processes, numberOfProcesses, );
  process->setResourceLimits(rlimit);
}

#undef RLIM_UPDATE

bool parse_result(const ProcessGroup::Result &process_group_result,
                  const Process::Result &process_result,
                  bacs::process::ExecutionResult &result) {
  switch (process_group_result.completionStatus) {
    case ProcessGroup::Result::CompletionStatus::OK:
    case ProcessGroup::Result::CompletionStatus::ABNORMAL_EXIT:
      // it may happen if Process::terminateGroupOnCrash is not set
      switch (process_result.completionStatus) {
        case Process::Result::CompletionStatus::OK:
          result.set_status(bacs::process::ExecutionResult::OK);
          break;
        case Process::Result::CompletionStatus::ABNORMAL_EXIT:
          result.set_status(bacs::process::ExecutionResult::ABNORMAL_EXIT);
          break;
        case Process::Result::CompletionStatus::MEMORY_LIMIT_EXCEEDED:
          result.set_status(
              bacs::process::ExecutionResult::MEMORY_LIMIT_EXCEEDED);
          break;
        case Process::Result::CompletionStatus::TIME_LIMIT_EXCEEDED:
          result.set_status(
              bacs::process::ExecutionResult::TIME_LIMIT_EXCEEDED);
          break;
        case Process::Result::CompletionStatus::OUTPUT_LIMIT_EXCEEDED:
          result.set_status(
              bacs::process::ExecutionResult::OUTPUT_LIMIT_EXCEEDED);
          break;
        case Process::Result::CompletionStatus::USER_TIME_LIMIT_EXCEEDED:
        case Process::Result::CompletionStatus::SYSTEM_TIME_LIMIT_EXCEEDED:
        case Process::Result::CompletionStatus::TERMINATED_BY_SYSTEM:
          result.set_status(
              bacs::process::ExecutionResult::TERMINATED_BY_SYSTEM);
          break;
        case Process::Result::CompletionStatus::START_FAILED:
        case Process::Result::CompletionStatus::STOPPED:
          result.set_status(bacs::process::ExecutionResult::FAILED);
          break;
      }
      break;
    case ProcessGroup::Result::CompletionStatus::REAL_TIME_LIMIT_EXCEEDED:
      result.set_status(
          bacs::process::ExecutionResult::REAL_TIME_LIMIT_EXCEEDED);
      break;
    case ProcessGroup::Result::CompletionStatus::STOPPED:
      result.set_status(bacs::process::ExecutionResult::FAILED);
      break;
  }
  if (process_result.exitStatus)
    result.set_exit_status(process_result.exitStatus.get());
  if (process_result.termSig) result.set_term_sig(process_result.termSig.get());
  {
    bacs::process::ResourceUsage &resource_usage =
        *result.mutable_resource_usage();
    resource_usage.set_time_usage_millis(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            process_result.resourceUsage.timeUsage).count());
    resource_usage.set_memory_usage_bytes(
        process_result.resourceUsage.memoryUsageBytes);
  }
  // full: dump all results here
  {
    boost::property_tree::ptree ptree;
    ptree.put_child("processGroupResult",
                    bunsan::config::save<boost::property_tree::ptree>(
                        process_group_result));
    ptree.put_child(
        "processResult",
        bunsan::config::save<boost::property_tree::ptree>(process_result));
    std::ostringstream buf;
    boost::property_tree::write_json(buf, ptree);
    result.set_full(buf.str());
  }
  return result.status() == bacs::process::ExecutionResult::OK;
}

}  // namespace process
}  // namespace system
}  // namespace bacs
