#include "duktape.hpp"
#include "plugins/error/error.hpp"

#include <boost/filesystem.hpp>

#include <stdexcept>
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace fs = boost::filesystem;

//####################################################################################################
Duktape::Context::Context(std::string const& baseDir)
    : context_(duk_create_heap_default())
    , baseDir_([&]() {
        std::string temp;
        if (baseDir.length() >= 2 && (baseDir.substr(0, 2) == "./" || baseDir.substr(0, 2) == ".\\"))
            temp = (fs::current_path() / fs::path(baseDir.substr(2, baseDir.length() - 2))).string();
        else
            temp = baseDir;
        return temp;
    }())
    , invalidated_(false)
{
    if (context_ == nullptr)
        throw std::runtime_error("could not create duktape context");

    // create the global object
    duk_push_global_object(context_);
    duk_dup(context_, -1);
    duk_put_prop_string(context_, -2, "global");

    // lastly register this:
    registerContext(context_, this);
}
//----------------------------------------------------------------------------------------------------
Duktape::Context::~Context()
{
    deregisterContext(context_);
    duk_destroy_heap(context_);
}
//----------------------------------------------------------------------------------------------------
std::string Duktape::Context::getBaseDir() const
{
    return baseDir_;
}
//----------------------------------------------------------------------------------------------------
duk_context* Duktape::Context::getContext()
{
    return context_;
}
//----------------------------------------------------------------------------------------------------
void Duktape::Context::runFile(std::string const& file)
{
    if (invalidated_)
        throw std::runtime_error ("this context was invalidated - create a new one");

    auto p = fs::path(baseDir_) / fs::path(file);
    p = p.make_preferred().string();

    std::ifstream reader{file, std::ios_base::binary};
    std::stringstream sstr;
    sstr << reader.rdbuf();

    if (duk_peval_string(context_, sstr.str().c_str()) != 0)
    {
        auto error = decodeError(context_);

        invalidated_ = true;
        throw std::runtime_error (error.toString().c_str());
    }
}
//####################################################################################################
