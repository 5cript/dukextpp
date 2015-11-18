#ifndef UTILITY_HPP_INCLUDED
#define UTILITY_HPP_INCLUDED

#include "core.hpp"

namespace Duktape
{
    int countObjectProperties(duk_context* ctx, duk_uint_t enum_flags = 0, duk_idx_t obj_index = -1);
}

#endif // UTILITY_HPP_INCLUDED
