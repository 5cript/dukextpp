#ifndef PUSH_VALUE_HPP_INCLUDED
#define PUSH_VALUE_HPP_INCLUDED

#include "core.hpp"
#include "type_stubs.hpp"
#include "object_fwd.hpp"

#include <type_traits>
#include <tuple>
#include <array>
#include <unordered_map>

/*
    Supports:

    integers, floats, bool, std::array, container<T, AllocatorT>, std::string, chars
*/

namespace Duktape
{
    namespace Internal
    {
        struct runner {
            template <typename T, typename... List>
            static void run(T t, List&&... params) { t(std::forward <List&&> (params)...); }
        };

        struct nop_runner {
            template <typename T, typename... List>
            static void run(T t, List&&... params) {}
        };

        #define DO_IF(CONDITION, CALLABLE, ...) \
        std::conditional <CONDITION, Internal::runner, Internal::nop_runner>::type::run(CALLABLE, __VA_ARGS__)
    }

    // using char_types = std::tuple <char, wchar_t, char16_t, char32_t, unsigned char>;
    // no support for utf yet. -> printing numbers instead. sry
    using char_types = std::tuple <char>;

    //####################################################################################################
    // std::arrays
    template <typename T, std::size_t N>
    void push(duk_context* ctx, std::array<T, N> const& values);

    // all sorts of integers
    template <typename T>
    void push(duk_context* ctx, T value, typename std::enable_if <std::is_integral <T>::value && !std::is_same <T, char>::value>::type* = nullptr);

    // all floating point
    template <typename T,
              class = typename std::enable_if <std::is_floating_point <T>::value>::type>
    void push(duk_context* ctx, T value, typename std::enable_if <std::is_floating_point <T>::value>::type* = nullptr);

    // bool
    void push(duk_context* ctx, bool value);

    // std::arrays
    template <typename T, std::size_t N>
    void push(duk_context* ctx, std::array<T, N> const& values);

    // std::vector, std::deque
    template <typename T, template <typename, class = std::allocator <T> > class ContainerT>
    void push(duk_context* ctx, ContainerT <T> const& values);

    // function
    void push(duk_context* ctx, duk_c_function func, duk_idx_t nargs);

    // strings
    void push(duk_context* ctx, std::string const& value);

    // characters as strings
    template <typename T>
    void push(duk_context* ctx, T value, typename std::enable_if <std::is_same <T, char>::value>::type* = nullptr);

    void push(duk_context* ctx, std::nullptr_t);

    void push(duk_context* ctx, EmptyObject value);
    void push(duk_context* ctx, Undefined value);
    void push(duk_context* ctx, IObject const& object);

    // this is treated like an object!
    void push(duk_context* ctx, std::unordered_map <std::string, std::string> const& map);
    //####################################################################################################

    template <typename T>
    void push(duk_context* ctx, T value, typename std::enable_if <std::is_integral <T>::value && !std::is_same <T, char>::value>::type*)
    {
        DO_IF(std::is_signed <T>::value, duk_push_int, ctx, value);
        DO_IF(std::is_unsigned <T>::value, duk_push_uint, ctx, value);
    }

    template <typename T>
    void push(duk_context* ctx, T value, typename std::enable_if <std::is_floating_point <T>::value>::type*)
    {
        duk_push_number(ctx, value);
    }

    template <typename T, std::size_t N>
    void push(duk_context* ctx, std::array<T, N> const& values)
    {
        auto arr = duk_push_array(ctx);
        for (std::size_t i = 0; i != N; ++i)
        {
            push(ctx, values[i]);
            duk_put_prop_index(ctx, arr, i);
        }

        // duk_pop(ctx);
    }

    template <typename T, template <typename, class = std::allocator <T> > class ContainerT>
    void push(duk_context* ctx, ContainerT <T> const& values)
    {
        auto arr = duk_push_array(ctx);
        std::size_t index = 0;
        for (auto const& i : values) {
            push(ctx, i);
            duk_put_prop_index(ctx, arr, index);
            ++index;
        }

        // duk_pop(ctx);
    }

    template <typename T>
    void push(duk_context* ctx, T value, typename std::enable_if <std::is_same <T, char>::value>::type*)
    {
        std::string str;
        str.push_back(value);
        push(ctx, str);
    }
}

#endif // PUSH_VALUE_HPP_INCLUDED
