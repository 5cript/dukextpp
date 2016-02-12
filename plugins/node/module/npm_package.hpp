#ifndef NPM_PACKAGE_HPP_INCLUDED
#define NPM_PACKAGE_HPP_INCLUDED

#ifndef Q_MOC_RUN // A Qt workaround, for those of you who use Qt
#   include "../../../SimpleJSON/parse/jsd.h"
#   include "../../../SimpleJSON/parse/jsd_convenience.h"
#   include "../../../SimpleJSON/stringify/jss.h"
#   include "../../../SimpleJSON/stringify/jss_fusion_adapted_struct.h"
#endif

#include <string>
#include <vector>
#include <map>

// only reads most important stuff

struct PackageRepository : public JSON::Stringifiable <PackageRepository>
                         , public JSON::Parsable <PackageRepository>
{
    std::string type;
    std::string url;
};

struct Package : public JSON::Stringifiable <Package>
               , public JSON::Parsable <Package>
{
    std::string name;
    std::string version;
    std::string description;
    std::string homepage;
    std::string icon;
    std::string license;
    std::string main;
    //std::vector <std::string> keywords;
    std::map <std::string, std::string> dependencies;
    std::map <std::string, std::string> devDependencies;
};

BOOST_FUSION_ADAPT_STRUCT
(
    PackageRepository, type, url
)

BOOST_FUSION_ADAPT_STRUCT
(
    Package, name, version, description, homepage, icon, license, main, dependencies, devDependencies
)

bool readPackageJson(std::string const& fileName, Package& pack);

#endif // NPM_PACKAGE_HPP_INCLUDED
