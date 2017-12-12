/*
 * =====================================================================================
 *
 *       Filename:  net_toml.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11.12.2017 12:38:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "net_toml.hpp"
#include <ossia/network/common/complex_type.hpp>

void wpn114::net::parse_toml(ossia::net::node_base &unit_root, std::string path)
{
    std::shared_ptr<cpptoml::table> config;

    try     { config = cpptoml::parse_file(path); }
    catch   ( const cpptoml::parse_exception& e )
    {
        std::cerr << "Failed to parse file: " << path
                  << " - " << e.what() << std::endl;
        return;
    }

    auto tarr = config->get_table_array("parameters");

    for(const auto& table : tarr)
    {
        auto address    = table->get_as<std::string>("address");
        auto type_str   = table->get_as<std::string>("type");

        auto node       = ossia::net::create_node(unit_root, address);
        auto parameter  = ossia::try_setup_parameter(node, type_str);
    }
}
