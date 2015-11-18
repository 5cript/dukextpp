#ifndef CORE_HPP_INCLUDED
#define CORE_HPP_INCLUDED

#include "duktape/src/duktape.h"

#include <unordered_map>

namespace Duktape
{
    class Context;

    duk_context* getContext(Duktape::Context* context);
    Duktape::Context* getContext(duk_context* context);

    void registerContext(duk_context* ctx, Duktape::Context* context);
    void deregisterContext(duk_context* ctx);
    void dump_context(duk_context* ctx);
}
#define PRIVATE_GLOBAL "\xff" "\xff"

#endif // CORE_HPP_INCLUDED
