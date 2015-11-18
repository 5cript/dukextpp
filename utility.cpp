#include "utility.hpp"

#include <iostream>

namespace Duktape
{
    int countObjectProperties(duk_context* ctx, duk_uint_t enum_flags, duk_idx_t obj_index)
    {
        int count = 0;
        duk_enum(ctx, obj_index, enum_flags);

        while(duk_next(ctx, -1, 0)) {
            duk_pop(ctx);
            ++count;
        }

        duk_pop(ctx);
        return count;
    }
}
