#ifndef DUKTAPE_HPP_INCLUDED
#define DUKTAPE_HPP_INCLUDED

#include "core.hpp"
#include "object.hpp"
#include "plugins/plugin.hpp"

#include <vector>
#include <string>
#include <memory>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

namespace Duktape
{
    class Context
    {
    public:
        // ctor / dtor
        Context(std::string const& baseDir = "");
        ~Context();

        // noncopyable
        Context(Context const&) = delete;
        Context& operator= (Context const&) = delete;

        // run script from file
        void runFile(std::string const& file);

        // getters
        std::string getBaseDir() const;
        duk_context* getContext();

        // install plugins
        template <typename PluginT, typename... List>
        PluginT* installPlugin(List&&... constructorParameters)
        {
            std::unique_ptr <Plugin> plug (new PluginT {this, std::forward <List> (constructorParameters)...});
            plugins.push_back(std::move(plug));
            plugins.back()->install();
            return static_cast <PluginT*> (plugins.back().get());
        }

    private:
        duk_context* context_;
        std::string baseDir_;
        bool invalidated_;

        std::vector <std::unique_ptr <Plugin>> plugins;
    };

}

#endif // DUKTAPE_HPP_INCLUDED
