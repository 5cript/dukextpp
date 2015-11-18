#ifndef OBJECT_FWD_HPP_INCLUDED
#define OBJECT_FWD_HPP_INCLUDED

#include "core.hpp"

namespace Duktape
{
    class IObject
    {
    public:
        virtual void putGlobal(duk_context* ctx) = 0;
        virtual duk_idx_t push(duk_context* ctx) const = 0;
        virtual void pushMember(duk_context* ctx) const = 0;
        virtual std::string getName() const = 0;
        virtual ~IObject() = default;
    };
}

#endif // OBJECT_FWD_HPP_INCLUDED
