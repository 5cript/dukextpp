#include "core.hpp"
#include "duktape.hpp"

#include <iostream>
#include <mutex>

namespace Duktape
{
    std::unordered_map <duk_context*, Duktape::Context*> contextMap;
    std::mutex mapProtector;

    void registerContext(duk_context* ctx, Duktape::Context* context)
    {
        std::lock_guard <std::mutex> guard (mapProtector);
        contextMap[ctx] = context;
    }
    void deregisterContext(duk_context* ctx)
    {
        std::lock_guard <std::mutex> guard (mapProtector);
        contextMap.erase(ctx);
    }
    duk_context* getContext(Duktape::Context* context)
    {
        std::lock_guard <std::mutex> guard (mapProtector);
        return context->getContext();
    }
    Duktape::Context* getContext(duk_context* context)
    {
        std::lock_guard <std::mutex> guard (mapProtector);
        return contextMap[context];
    }
    void dump_context(duk_context* ctx)
    {
        duk_push_context_dump(ctx);
        std::cout << duk_to_string(ctx, -1) << std::endl;
        duk_pop(ctx);
    }
}
