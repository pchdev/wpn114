/*
 * =====================================================================================
 *
 *       Filename:  net_hdl.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  26.10.2017 07:19:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once
#include <ossia/ossia.hpp>

namespace wpn114 {
namespace net {

class net_hdl
{
public:
    net_hdl(const char* user_namespace);
    ~net_hdl();

    void expose_oscquery_server(uint32_t udp_port, uint32_t tcp_port);
    void expose_osc(const char *remote_addr, uint32_t remote_port, uint32_t local_port);
    void expose_minuit(const char *remote_addr, uint32_t remote_port, uint32_t local_port);

    ossia::net::node_base& get_application_node() const;

private:
    std::unique_ptr<ossia::net::device_base> m_device;
};
}
}
