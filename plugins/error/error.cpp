#include "error.hpp"

namespace Duktape
{
    const char errCreate[] =
    "Duktape.errCreate = function (err) {"                                                              "\n"
        "try {"                                                                                         "\n"
            "if (typeof err === 'object' &&"                                                            "\n"
                "typeof err.message !== 'undefined' &&"                                                 "\n"
                "typeof err.lineNumber === 'number' &&"                                                 "\n"
                "typeof err.fileName !== 'undefined') "                                                 "\n"
            "{"                                                                                         "\n"
                "err.message = err.fileName + ':' + err.lineNumber + '\n' + err.message;"               "\n"
            "}"                                                                                         "\n"
        "} catch (e) {}"                                                                                "\n"
        "return err;"                                                                                   "\n"
    "}"                                                                                                 "\n"
    ;

    //--------------------------------------------------------------------------------------
    VerboseErrorReporter::VerboseErrorReporter(Context* context)
        : Plugin(context)
    {

    }
    //--------------------------------------------------------------------------------------
    void VerboseErrorReporter::install()
    {
        duk_peval_string(getContext(context_), errCreate);
    }
    //--------------------------------------------------------------------------------------
    std::string Error::toString() const
    {
        return fileName + ":" + std::to_string(lineNumber) + "\n" + message;
    }
    //--------------------------------------------------------------------------------------
    Error decodeError(duk_context* ctx)
    {
        Error err;

        duk_get_prop_string(ctx, -1, "lineNumber");
        err.lineNumber = duk_to_int(ctx, -1);
        duk_get_prop_string(ctx, -2, "fileName");
        err.fileName = {duk_to_string(ctx, -1)};
        duk_pop_2(ctx);
        err.message = {duk_to_string(ctx, -1)};
        duk_pop(ctx);

        return err;
    }
    //--------------------------------------------------------------------------------------
}
