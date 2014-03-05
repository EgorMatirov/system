#pragma once

#include <bacs/system/error.hpp>

#include <bacs/callback.pb.h>

#include <bunsan/factory_helper.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>

#include <functional>
#include <iterator>
#include <string>
#include <vector>

namespace bacs{namespace system{namespace callback
{
    struct error: virtual system::error {};
    struct serialization_error: virtual error {};

    class base: private boost::noncopyable
    BUNSAN_FACTORY_BEGIN(base, const std::vector<std::string> &/*arguments*/)
    public:
        virtual ~base() {}

    public:
        static base_ptr instance(const Callback &config);

    public:
        typedef std::vector<unsigned char> data_type;

    public:
        virtual void call(const data_type &data)=0;

        template <typename T>
        void call(const T &obj)
        {
            call(begin(obj), end(obj));
        }

        template <typename Iter>
        void call(const Iter &begin, const Iter &end)
        {
            typedef std::iterator_traits<Iter> traits;
            typedef typename traits::value_type value_type;
            static_assert(
                sizeof(value_type) == sizeof(data_type::value_type),
                "Sizes should be equal."
            );
            const std::function<data_type::value_type (const value_type)> to_uc =
                [](const value_type c)
                {
                    return data_type::value_type(c);
                };
            call(data_type(boost::make_transform_iterator(begin, to_uc),
                           boost::make_transform_iterator(end, to_uc)));
        }
    BUNSAN_FACTORY_END(base)

    template <typename T>
    class message_interface
    {
    public:
        message_interface()=default;
        message_interface(const message_interface &)=default;
        message_interface &operator=(const message_interface &)=default;

        explicit message_interface(const base_ptr &base_): m_base(base_) {}

        explicit message_interface(const Callback &config):
            message_interface(base::instance(config)) {}

        template <typename Y>
        void assign(Y &&y)
        {
            message_interface(y).swap(*this);
        }

        void swap(message_interface &iface) noexcept
        {
            using boost::swap;
            swap(m_base, iface.m_base);
        }

        void call(const T &obj) const
        {
            call_(obj);
        }

    private:
        void call_(const google::protobuf::MessageLite &message) const
        {
            if (m_base)
                m_base->call(message.SerializeAsString());
        }

    private:
        base_ptr m_base;
    };

    template <typename T>
    void swap(message_interface<T> &a, message_interface<T> &b) noexcept
    {
        a.swap(b);
    }
}}}
