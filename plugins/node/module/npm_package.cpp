#include "npm_package.hpp"
#include <string>
#include <fstream>
#include <streambuf>

bool readPackageJson(std::string const& fileName, Package& pack)
{
    std::ifstream reader (fileName);
    if (!reader.good())
        return false;

    std::string content((std::istreambuf_iterator<char>(reader)),
                         std::istreambuf_iterator<char>());

    try
    {
        auto tree = JSON::parse_json(content);
        JSON::parse(pack, "", tree, {
            JSON::InvalidPropertyHandlingBehaviour::IGNORE_ALL_ERROR,
            JSON::InvalidPathHandlingBehaviour::TAG
        });
    }
    catch (boost::property_tree::ptree_error& exc)
    {
        throw exc;
    }
    catch (...)
    {
        return false;
    }

    return true;
}
