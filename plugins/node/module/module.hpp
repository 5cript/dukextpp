#ifndef REQUIRE_HPP_INCLUDED
#define REQUIRE_HPP_INCLUDED

#include "../../../core.hpp"
#include "../../../duktape.hpp"
#include "../../../object.hpp"
#include "../../../type_stubs.hpp"
#include "../../plugin.hpp"

namespace Duktape { namespace Node
{
    duk_ret_t require(duk_context *ctx);
    duk_ret_t mod_search(duk_context* ctx);

    class Module : public Plugin
    {
    public:
        Module(Context* context);
        void install() override;
    };

    struct ModuleObject : public Object <ModuleObject>
    {
        std::string id;
        std::string filename;
        bool loaded;
        EmptyObject exports;

        OBJECT_CONSTRUCTOR(ModuleObject)
    };

    struct Exports : public Object <Exports>
    {
        OBJECT_CONSTRUCTOR(Exports)
    };
}}

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::ModuleObject,
    (std::string, id)
    (std::string, filename)
    (bool, loaded)
    (Duktape::EmptyObject, exports)
)

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::Exports
)

#endif // REQUIRE_HPP_INCLUDED
