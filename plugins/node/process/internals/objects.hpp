#ifndef OBJECTS_HPP_INCLUDED
#define OBJECTS_HPP_INCLUDED

#include "../../../../core.hpp"
#include "../../../../object.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace Duktape { namespace Node
{
    struct NodeProcessConfig_Variables : public Object <NodeProcessConfig_Variables>
    {
        std::string host_arch;
        std::string node_install_npm;
        std::string node_prefix;
        std::string node_shared_cares;
        std::string node_shared_http_parser;
        std::string node_shared_libuv;
        std::string node_shared_zlib;
        std::string node_use_dtrace;
        std::string node_use_openssl;
        std::string node_shared_openssl;
        std::string strict_aliasing;
        std::string target_arch;
        std::string v8_use_snapshot;

        OBJECT_CONSTRUCTOR_STATIC_NAME(NodeProcessConfig_Variables, "variables");
    };

    struct NodeProcessConfig_TargetDefaults : public Object <NodeProcessConfig_TargetDefaults>
    {
        std::vector <std::string> cflags;
        std::string default_configuration;
        std::vector <std::string> defines;
        std::vector <std::string> include_dirs;
        std::vector <std::string> libraries;

        OBJECT_CONSTRUCTOR_STATIC_NAME(NodeProcessConfig_TargetDefaults, "target_defaults");
    };

    struct NodeProcessConfig : public Object <NodeProcessConfig>
    {
        NodeProcessConfig_TargetDefaults target_defaults;
        NodeProcessConfig_Variables variables;

        OBJECT_CONSTRUCTOR_STATIC_NAME(NodeProcessConfig, "config");
    };

    struct NodeMemoryUsage : public Object <NodeMemoryUsage>
    {
        duk_int_t rss;
        duk_int_t heapTotal;
        duk_int_t heapUsed;

        OBJECT_CONSTRUCTOR_STATIC_NAME(NodeMemoryUsage, "memoryUsage");
    };

    struct NodeProcessRelease : public Object <NodeProcessRelease>
    {
        std::string name;
        std::string sourceUrl;
        std::string headersUrl;
        std::string libUrl;

        OBJECT_CONSTRUCTOR_STATIC_NAME(NodeProcessRelease, "release");
    };

    struct NodeProcess : public Object <NodeProcess>
    {
        NodeProcessConfig config;
        std::vector <std::string> argv;
        std::string arch;
        std::unordered_map <std::string, std::string> env;
        std::vector <std::string> execArgv;
        std::string execPath;
        duk_int_t exitCode;
        Undefined mainModule;
        duk_int_t pid;
        std::string platform;
        NodeProcessRelease release;

        OBJECT_CONSTRUCTOR_STATIC_NAME(NodeProcess, "process");
    };

}}

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::NodeProcessConfig_Variables,
    (std::string, host_arch)
    (std::string, node_install_npm)
    (std::string, node_prefix)
    (std::string, node_shared_cares)
    (std::string, node_shared_http_parser)
    (std::string, node_shared_libuv)
    (std::string, node_shared_zlib)
    (std::string, node_use_dtrace)
    (std::string, node_use_openssl)
    (std::string, node_shared_openssl)
    (std::string, strict_aliasing)
    (std::string, target_arch)
    (std::string, v8_use_snapshot)
)

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::NodeProcessConfig_TargetDefaults,
    (std::vector <std::string>, cflags)
    (std::string, default_configuration)
    (std::vector <std::string>, defines)
    (std::vector <std::string>, include_dirs)
    (std::vector <std::string>, libraries)
)

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::NodeProcessConfig,
    (Duktape::Node::NodeProcessConfig_TargetDefaults, target_defaults)
    (Duktape::Node::NodeProcessConfig_Variables, variables)
)

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::NodeProcessRelease,
    (std::string, name)
    (std::string, sourceUrl)
    (std::string, headersUrl)
    (std::string, libUrl)
)

BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::NodeMemoryUsage,
    (duk_int_t, rss)
    (duk_int_t, heapTotal)
    (duk_int_t, heapUsed)
)

namespace Internal {
    using stringMap = std::unordered_map <std::string, std::string>;
}
BOOST_FUSION_ADAPT_STRUCT
(
    Duktape::Node::NodeProcess,
    (std::vector <std::string>, argv)
    (std::string, arch)
    (Duktape::Node::NodeProcessConfig, config)
    (Internal::stringMap, env)
    (std::vector <std::string>, execArgv)
    (std::string, execPath)
    (duk_int_t, exitCode)
    (Duktape::Undefined, mainModule)
    (duk_int_t, pid)
    (std::string, platform)
    (Duktape::Node::NodeProcessRelease, release)
)


#endif // OBJECTS_HPP_INCLUDED
