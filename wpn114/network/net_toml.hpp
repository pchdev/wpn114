/*
 * =====================================================================================
 *
 *       Filename:  net_toml.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11.12.2017 12:37:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <ossia/ossia.hpp>
#include <cpptoml.h>

namespace wpn114 {
namespace net {

void toml_netparse(ossia::net::node_base& unit_root, std::string path);
}
}
