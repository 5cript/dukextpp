#ifndef OBJECT_HPP_INCLUDED
#define OBJECT_HPP_INCLUDED

#include "core.hpp"
#include "object_fwd.hpp"
#include "push_value.hpp"

#include <boost/fusion/mpl.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

#include <string>
#include <functional>
#include <boost/type_index.hpp>

namespace Duktape
{
    namespace Internal
    {
        template <typename T>
        class ObjectPusher
        {
        public:
            duk_idx_t operator()(duk_context* ctx, T const& object) const
            {
                //! If you get an Error here, you likely forgot to use BOOST_FUSION_ADAPT_STRUCT / BOOST_FUSION_DECLARE_STRUCT !

                typedef boost::mpl::range_c<
                    int,
                    0,
                    boost::fusion::result_of::size<T>::type::value
                > range;

                auto relpos = duk_push_object(ctx);
                boost::mpl::for_each<range> (std::bind<void>(_helper(boost::fusion::result_of::size<T>::type::value), std::placeholders::_1, std::ref(ctx), std::cref(object), std::ref(relpos)));
                return relpos;
            }
        private:
            class _helper
            {
            public:
                template<class Index>
                void operator()(Index, duk_context* ctx, T const& object, duk_idx_t& relpos) const
                {
                    std::string typeName = boost::typeindex::type_id<decltype(boost::fusion::at<Index>(object))>().pretty_name();
                    push(ctx, boost::fusion::at<Index>(object));
                    duk_put_prop_string(ctx, relpos, boost::fusion::extension::struct_member_name<T, Index::value>::call());
                }
                _helper(int len) : len(len) {}
            private:
                int len;
            };
        };
    }

    template <typename Derived>
    class Object : public IObject
    {
    public:
        Object(std::string name)
            : name_(std::move(name))
        {
        }

        void putGlobal(duk_context* ctx) override
        {
            push(ctx);
            duk_put_global_string(ctx, name_.c_str());
        }

        duk_idx_t push(duk_context* ctx) const override
        {
            return Internal::ObjectPusher <Derived>{}(ctx, *static_cast <Derived const*> (this));
        }

        virtual void pushMember(duk_context* ctx) const override
        {
            duk_put_prop_string(ctx, push(ctx), name_.c_str());
        }

        std::string getName() const override
        {
            return name_;
        }

    private:
        std::string name_;
    };

    #define OBJECT_CONSTRUCTOR(NAME) \
    NAME(std::string name) \
        : Object(std::move(name)) \
    { \
    }

    #define OBJECT_CONSTRUCTOR_STATIC_NAME(NAME, STATIC_NAME) \
    NAME() \
        : Object(STATIC_NAME) \
    { \
    }
}

#endif // OBJECT_HPP_INCLUDED
