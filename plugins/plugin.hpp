#ifndef PLUGIN_HPP_INCLUDED
#define PLUGIN_HPP_INCLUDED

#include "../core.hpp"

namespace Duktape
{
    class Plugin
    {
    public:
        Plugin(Context* context)
            : context_(context)
        {
        }

        virtual void install() = 0;

    protected:
        Context* context_;
    };


}

#endif // PLUGIN_HPP_INCLUDED
