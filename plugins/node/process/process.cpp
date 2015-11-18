#include "process.hpp"
#include "../../../push_value.hpp"
#include "internals/objects.hpp"

#include <exception>
#include <string>
#include <vector>
#include <cstring>
#include <utility>
#include <chrono>

#include <cstdio>
#include <cerrno>
#include <csignal>

#include <unistd.h>
#include <sys/types.h>

#ifdef __WIN32__
#include <windows.h>
#endif // __WIN32__

extern char ** environ;

namespace Duktape { namespace Node
{
//####################################################################################################
    std::unordered_map <std::string, std::string> loadEnvironmentMember();
//####################################################################################################
    Process::Process(Context* context, int argc, char** argv)
        : Plugin(context)
        , args()
        , execPath(argv[0])
    {
        for (int i = 1; i != argc; ++i)
            args.push_back(argv[i]);
    }
//----------------------------------------------------------------------------------------------------
    void Process::install()
    {
        auto* ctx = getContext(context_);
        createObject(ctx);

        duk_put_global_string(ctx, "process");
    }
//----------------------------------------------------------------------------------------------------
    void Process::createObject(duk_context* ctx)
    {
        NodeProcess process;

        process.argv.push_back(execPath); // there is no node executable behind this. Might break things, but I cannot do anything about it.
        process.argv.push_back(""); // running script
        for (auto const& i : args)
            process.argv.push_back(i);

        NodeProcessConfig& cfg = process.config;
        cfg.target_defaults.default_configuration = "Release";

#ifdef __arm__
        cfg.variables.host_arch = "arm";
        cfg.variables.target_arch = "arm"
#elif defined(__i686__) || defined(__i586__) || defined(__i486__) || defined(__i386__)
        cfg.variables.host_arch = "ia32";
        cfg.variables.target_arch = "ia32"
#elif defined(__x86_64__) || defined(__ia64__)
        cfg.variables.host_arch = "x64";
        cfg.variables.target_arch = "x64";
#else
        cfg.variables.host_arch = "x64";
        cfg.variables.target_arch = "x64";
#endif

        cfg.variables.node_install_npm = "true";
        cfg.variables.node_prefix = "";
        cfg.variables.node_shared_cares = "false";
        cfg.variables.node_shared_http_parser = "false";
        cfg.variables.node_shared_libuv = "false";
        cfg.variables.node_shared_zlib = "false";
        cfg.variables.node_use_dtrace = "false";
        cfg.variables.node_use_openssl = "true";
        cfg.variables.node_shared_openssl = "false";
        cfg.variables.strict_aliasing = "true";
        cfg.variables.v8_use_snapshot = "false";

#ifdef __arm__
        process.arch = "arm";
#elif defined(__i686__) || defined(__i586__) || defined(__i486__) || defined(__i386__)
        process.arch = "ia32";
#elif defined(__x86_64__) || defined(__ia64__)
        process.arch = "x64";
#else
        process.arch = "x64";
#endif

        process.env = loadEnvironmentMember();

        for (auto const& i : args)
            process.execArgv.push_back(i);

        process.execPath = execPath;

#ifdef __WIN32__
        process.pid = GetCurrentProcessId();
        process.platform = "win32";
#else
        process.pid = getpid();
        process.platform = "linux";
#endif

        process.release.name = "duktape";

        process.push(ctx);

        // all functions

        duk_push_c_function(ctx, process_abort, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "abort");

        duk_push_c_function(ctx, process_chdir, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "chdir");

        duk_push_c_function(ctx, process_connected, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "connected");

        duk_push_c_function(ctx, process_cwd, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "cwd");

        duk_push_c_function(ctx, process_exit, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "exit");

        duk_push_c_function(ctx, process_hrtime, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "hrtime");

        duk_push_c_function(ctx, process_kill, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "kill");

        duk_push_c_function(ctx, process_memory_usage, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "memoryUsage");

        duk_push_c_function(ctx, process_next_tick, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, "nextTick");
    }
//####################################################################################################
    std::unordered_map <std::string, std::string> loadEnvironmentMember()
    {
        auto split = [](char* line) -> std::pair <std::string, std::string> {
            std::pair <std::string, std::string> result;

            bool swit = false;
            for (char* c = line; *c != '\0'; ++c) {
                if (*c == '=')
                    swit = true;
                else if (!swit)
                    result.first.push_back(*c);
                else
                    result.second.push_back(*c);
            }
            return result;
        };

        std::unordered_map <std::string, std::string> mapping;
        for (char **env = environ; *env; ++env)
        {
            mapping.insert(split(*env));
        }

        return mapping;
    }
//----------------------------------------------------------------------------------------------------
#ifdef __WIN32__
    BOOL TerminateProcessByPID(DWORD dwProcessId, UINT uExitCode)
    {
        DWORD dwDesiredAccess = PROCESS_TERMINATE;
        BOOL  bInheritHandle  = FALSE;
        HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
        if (hProcess == NULL)
            return FALSE;

        BOOL result = TerminateProcess(hProcess, uExitCode);

        CloseHandle(hProcess);

        return result;
    }
#endif // __WIN32__
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_abort(duk_context* ctx)
    {
        std::terminate();
        //abort();
        return 0;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_chdir(duk_context* ctx)
    {
        const char* dir = duk_require_string(ctx, 0);

        if (chdir(dir) != 0) {
            duk_push_string(ctx, std::strerror(errno));
            duk_throw(ctx);
        }
        return 1;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_connected(duk_context* ctx)
    {
        duk_push_boolean(ctx, false);
        return 1;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_cwd(duk_context* ctx)
    {
        char buffer[4096];
        getcwd(buffer, 4096);
        duk_push_string(ctx, buffer);
        return 1;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_exit(duk_context* ctx)
    {
        duk_int_t code = 0;

        int nargs = duk_get_top(ctx);
        if (nargs > 0)
            code = duk_get_int(ctx, 0);

        exit(code);

        return 0;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_hrtime(duk_context* ctx)
    {
        duk_int_t prev_s = 0;
        duk_int_t prev_ns = 0;

        int nargs = duk_get_top(ctx);
        if (nargs > 0)
        {
            prev_s = duk_get_prop_index(ctx, 0, 0);
            prev_ns = duk_get_prop_index(ctx, 0, 1);
        }

        auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
        auto ns = std::chrono::duration_cast <std::chrono::nanoseconds> (now).count();
        auto s = std::chrono::duration_cast <std::chrono::seconds> (now).count();

        ns = ns - prev_ns;
        s = s - prev_s;

        auto arr = duk_push_array(ctx);

        push(ctx, s);
        duk_put_prop_index(ctx, arr, 0);
        push(ctx, ns);
        duk_put_prop_index(ctx, arr, 1);

        return 1;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_kill(duk_context* ctx)
    {
        int nargs = duk_get_top(ctx);

        auto pid = duk_require_int(ctx, 0);

        // second parameter only supported on linux
        std::string signal = "SIGTERM";
        if (nargs >= 2)
            signal = {duk_get_string(ctx, 1)};

#ifdef __WIN32__
        TerminateProcessByPID(pid, 0);
#else
        if (signal == "SIGTERM")
            kill(pid, SIGTERM);
        else if (signal == "SIGABRT")
            kill(pid, SIGABORT);
        else if (signal == "SIGKILL")
            kill(pid, SIGKILL);
        else if (signal == "SIGQUIT")
            kill(pid, SIGQUIT);
        else if (signal == "SIGSTOP")
            kill(pid, SIGSTOP)
        else
            kill(pid, SIGTERM);
#endif
        return 1;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_memory_usage(duk_context* ctx)
    {
        NodeMemoryUsage usage;
        usage.push(ctx);

        return 1;
    }
//----------------------------------------------------------------------------------------------------
    duk_ret_t process_next_tick(duk_context* ctx)
    {
        return 1;
    }
//----------------------------------------------------------------------------------------------------
} // namespace Node
} // namespace Duktape
