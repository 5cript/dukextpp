#ifndef PROCESS_HPP_INCLUDED
#define PROCESS_HPP_INCLUDED

#include "../../../core.hpp"
#include "../../plugin.hpp"

#include <vector>
#include <string>

namespace Duktape { namespace Node
{
    class Process : public Plugin
    {
    public:
        Process(Context* context, int argc, char** argv);

        void install() override;

    private:
        void createObject(duk_context* ctx);
        std::vector <std::string> args;
        std::string execPath;
    };

    duk_ret_t process_abort(duk_context* ctx);
    duk_ret_t process_chdir(duk_context* ctx);
    duk_ret_t process_connected(duk_context* ctx);
    duk_ret_t process_cwd(duk_context* ctx);
    duk_ret_t process_exit(duk_context* ctx);
    duk_ret_t process_hrtime(duk_context* ctx);
    duk_ret_t process_kill(duk_context* ctx);
    duk_ret_t process_memory_usage(duk_context* ctx);
    duk_ret_t process_next_tick(duk_context* ctx);

    // not supported yet
    // process.send(message[, sendHandle][, callback])
    //
    // process.disconnect()
    // process.getegid()
    // process.getgid()
    // process.getgroups()
    // process.getuid()
    // process.initgroups()
    // process.setegid()
    // process.seteuid()
    // process.setgid()
    // process.setgroups()
    // process.setuid()
    // process.stderr
    // process.stdin
    // process.stdout
    // process.title
    // process.umask([mask])
    // process.uptime()
    // process.version
    // process.versions
}}

#endif // PROCESS_HPP_INCLUDED
