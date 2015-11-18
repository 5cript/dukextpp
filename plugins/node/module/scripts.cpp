#include "helper.hpp"

namespace Duktape
{
    const char* require_init() { return
        JS("global.__moduleStack = [];")
    ;}

    const char* require_front() { return
        JS("global.__moduleStack.push({exports: {}});")
        JS("function __module_wrap (module, exports) {")
    ;}

    const char* require_back() { return
        JS("}")
        JS("__module_wrap(global.__moduleStack[global.__moduleStack.length - 1], global.__moduleStack[global.__moduleStack.length - 1].exports);")
    ;}

    const char* require_clean() { return
        JS("global.__moduleStack.pop();")
    ;}
}
