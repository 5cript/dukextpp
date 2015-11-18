#include "push_value.hpp"

namespace Duktape
{
    void push(duk_context* ctx, bool value)
    {
        duk_push_boolean(ctx, value/* ? 1 : 0 */);
    }

    void push(duk_context* ctx, duk_c_function func, duk_idx_t nargs)
    {
        duk_push_c_function(ctx, func, nargs);
    }

    void push(duk_context* ctx, std::string const& value)
    {
        duk_push_lstring(ctx, value.c_str(), value.length());
    }

    void push(duk_context* ctx, std::nullptr_t)
    {
        duk_push_undefined(ctx);
    }

    void push(duk_context* ctx, EmptyObject value)
    {
        duk_push_object(ctx);
    }

    void push(duk_context* ctx, Undefined value)
    {
        duk_push_undefined(ctx);
    }

    void push(duk_context* ctx, IObject const& object)
    {
        object.push(ctx);
    }

    void push(duk_context* ctx, std::unordered_map <std::string, std::string> const& map)
    {
        auto id = duk_push_object(ctx);

        for (auto const& i : map)
        {
            duk_push_string(ctx, i.second.c_str());
            duk_put_prop_string(ctx, id, i.first.c_str());
        }
    }
}
