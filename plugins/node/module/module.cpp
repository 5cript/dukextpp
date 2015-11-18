#include "module.hpp"
#include "npm_package.hpp"
#include "../../error/error.hpp"
#include "../../../utility.hpp"

#include "scripts.hpp"

#include <iostream>
#include <stack>
#include <string>
#include <boost/filesystem.hpp>

using namespace std::string_literals;
namespace fs = boost::filesystem;

namespace Duktape { namespace Node
{
    //----------------------------------------------------------------------------------------------------
    std::stack<std::string> paths;
    unsigned int moduleStackSize; // 0 by default, because it does not infer any cost
    //----------------------------------------------------------------------------------------------------
    Module::Module(Context* context)
        : Plugin(context)
    {

    }
    //----------------------------------------------------------------------------------------------------
    void Module::install()
    {
        auto* ctx = getContext(context_);

        duk_push_c_function(ctx, require, 1);
        duk_put_global_string(ctx, "require");

        duk_peval_string(ctx, require_init());
    }
    //----------------------------------------------------------------------------------------------------
    std::string dirName(std::string const& path)
    {
        std::size_t pos;

        pos = path.find_last_of('\\');
        if (pos == std::string::npos) {
            pos = path.find_last_of('/');
        }

        if (pos == std::string::npos) {
            return path;
        }

        return path.substr(0, pos);
    }
    //----------------------------------------------------------------------------------------------------
    std::string loadFile(std::string const& path)
    {
        std::ifstream reader (path);
        if (!reader.good())
            throw std::runtime_error ("file not found");

        std::string content((std::istreambuf_iterator<char>(reader)),
                             std::istreambuf_iterator<char>());
        return content;
    }
    //----------------------------------------------------------------------------------------------------
    duk_ret_t mod_search(duk_context* ctx)
    {
        duk_push_string(ctx, loadFile(duk_require_string(ctx, 0)).c_str());
        return 1;
    }
    //----------------------------------------------------------------------------------------------------
    duk_ret_t require(duk_context *ctx)
    {
        auto* contextClass = getContext(ctx);

        const char *id = duk_require_string(ctx, 0);

        /*
         * 1. Determine if we are top-level require or not, in that case
         * push "." as the current file path.
         */
        duk_get_global_string(ctx, "\xff""\xff""require-is-top");
        bool top = duk_get_type(ctx, -1) == DUK_TYPE_UNDEFINED;
        duk_pop(ctx);

        if (top) {
            paths.push(contextClass->getBaseDir() + "/");
        }

        /* Now set the require-top field to false */
        duk_push_boolean(ctx, false);
        duk_put_global_string(ctx, "\xff""\xff""require-is-top");

        auto hasEnding = [](std::string const &fullString, std::string const &ending) -> bool {
            if (fullString.length() >= ending.length()) {
                return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
            } else {
                return false;
            }
        };

        /*
         * 2. Search for the file relatively or absolutely to the global
         * directories.
         */
        std::string path;
        if (id[0] == '.') {
            std::string sid(id);
            path = paths.top() + id;
            if (!hasEnding(sid, ".js"))
                path += ".js";
        } else {
            auto tryReadPackage = [&path, &contextClass, &id, &hasEnding](std::string baseDir) -> bool {
                Package pack;
                if (readPackageJson((baseDir / fs::path("package.json")).string(), pack))
                {
                    // package json present
                    auto modulePackagePath = baseDir / fs::path("node_modules") / id / fs::path("package.json");

                    if (pack.dependencies.find(id) != std::end(pack.dependencies) || pack.dependencies.find(id) != std::end(pack.devDependencies))
                    {
                        // is an installed package - find in node_modules
                        Package modulePackage;
                        if (readPackageJson(modulePackagePath.string(), modulePackage))
                        {
                            if (modulePackage.main=="<undefined>")
                                modulePackage.main="index.js";

                            if (!hasEnding(modulePackage.main, ".js"))
                                modulePackage.main += ".js";

                            // package found, get main
                            auto main = baseDir / fs::path("node_modules") / id / fs::path(modulePackage.main);

                            if (fs::exists(main))
                            {
                                path = main.string();
                                return true;
                            }
                        }
                    }
                }
                return false;
            };

            bool loadedAsPackage = false;

            if (!tryReadPackage(paths.top()))
                loadedAsPackage = tryReadPackage(contextClass->getBaseDir());
            else
                loadedAsPackage = true;

            if (!loadedAsPackage)
            {
                auto fpath = fs::path(paths.top()) / fs::path(id);
                path = fpath.string();
                if (fpath.extension() == "")
                    path += ".js"s;
            }
        }

        // path fixup:
        path = fs::canonical(path).make_preferred().string();

        /*
         * 3. Push the current path for next require invocation.
         */
        paths.push(dirName(path) + "/");

        /*
         * 4. Calls the file, this will recursively go into that function
         * and use paths we have pushed before.
         */
        bool failed = false;

        //Module mod("module");
        //mod.putGlobal(ctx);

        auto source = loadFile(path.c_str());
        source = std::string(require_front()) + source + std::string(require_back());
        moduleStackSize++;

        //std::cout << "-------------------------------------------------------------" << std::endl;
        //std::cout << source << std::endl;
        //std::cout << "-------------------------------------------------------------" << std::endl;

        if (duk_peval_string(ctx, source.c_str()) != 0)
        {
            auto error = decodeError(ctx);

            duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, "failed to parse module: %s: %s", path.c_str(), error.toString().c_str());
            failed = true;
        }
        else
        {
            duk_pop(ctx); // remove duk_peval_string result

            duk_get_global_string(ctx, "__moduleStack");

            duk_get_prop_index(ctx, -1, moduleStackSize - 1); // get last array element
            duk_remove(ctx, -2); // array

            duk_get_prop_string(ctx, -1, "exports");
            duk_remove(ctx, -2); // module

            // removes temporaries and pop module stack
            duk_peval_string(ctx, require_clean());
            duk_pop(ctx);

            moduleStackSize--;
        }

        if (!failed)
            paths.pop();

        /* If top, we are at the absolute end of require() call, empty the paths */
        if (top || failed) {
            while (paths.size() > 0) {
                paths.pop();
            }

            /* Unset the require-is-top bit */
            duk_push_undefined(ctx);
            duk_put_global_string(ctx, "\xff""\xff""require-is-top");
        }

        /* If failed, stop immediately */
        if (failed) {
            duk_throw(ctx);
        }
        return 1;
    }
    //----------------------------------------------------------------------------------------------------
}}
