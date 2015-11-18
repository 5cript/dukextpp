#ifndef ERRORS_HPP_INCLUDED
#define ERRORS_HPP_INCLUDED

#include "../../core.hpp"
#include "../plugin.hpp"

#include <string>

namespace Duktape
{
    class VerboseErrorReporter : public Plugin
    {
    public:
        VerboseErrorReporter(Context* context);
        void install() override;
    };

    struct Error {
        long long lineNumber;
        std::string fileName;
        std::string message;

        std::string toString() const;
    };

    Error decodeError(duk_context* ctx);
}

#endif // ERRORS_HPP_INCLUDED
