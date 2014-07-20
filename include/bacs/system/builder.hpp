#pragma once

#include <bacs/system/error.hpp>

#include <bacs/process.pb.h>

#include <yandex/contest/invoker/Forward.hpp>
#include <yandex/contest/system/unistd/access/Id.hpp>

#include <bunsan/factory_helper.hpp>

#include <boost/noncopyable.hpp>

namespace bacs{namespace system
{
    class executable: private boost::noncopyable
    {
    public:
        virtual ~executable() {}

        /// \return process with executable and arguments initialized
        virtual yandex::contest::invoker::ProcessPointer create(
            const yandex::contest::invoker::ProcessGroupPointer &process_group,
            const yandex::contest::invoker::ProcessArguments &arguments)=0;

        template <typename T>
        yandex::contest::invoker::ProcessPointer create(
            const yandex::contest::invoker::ProcessGroupPointer &process_group,
            const T &arguments)
        {
            return create(process_group, begin(arguments), end(arguments));
        }

        template <typename Iter>
        yandex::contest::invoker::ProcessPointer create(
            const yandex::contest::invoker::ProcessGroupPointer &process_group,
            const Iter &begin, const Iter &end)
        {
            return create(
                process_group,
                yandex::contest::invoker::ProcessArguments(begin, end)
            );
        }
    };
    typedef std::shared_ptr<executable> executable_ptr;

    class builder: private boost::noncopyable
    BUNSAN_FACTORY_BEGIN(builder, const std::vector<std::string> &/*arguments*/)
    public:
        static builder_ptr instance(const bacs::process::Builder &config);

    public:
        virtual ~builder() {}

        /// \return nullptr on fail
        virtual executable_ptr build(
            const yandex::contest::invoker::ContainerPointer &container,
            const yandex::contest::system::unistd::access::Id &owner_id,
            const bacs::process::Source &source,
            const bacs::process::ResourceLimits &resource_limits,
            bacs::process::BuildResult &result)=0;
    BUNSAN_FACTORY_END(builder)
}}
