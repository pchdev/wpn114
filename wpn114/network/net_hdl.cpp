/*
 * =====================================================================================
 *
 *       Filename:  net_hdl.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  26.10.2017 07:19:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <wpn114/network/net_hdl.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

using namespace wpn114::net;
using namespace ossia::net;

net_hdl::net_hdl(const char *user_namespace)
{
    auto mpx    = std::make_unique<ossia::net::multiplex_protocol>();
    m_device    = std::make_unique<ossia::net::generic_device>(std::move(mpx), user_namespace);
    m_protocol  = mpx.get();
}

net_hdl::~net_hdl() {}

node_base &net_hdl::get_application_node()
{
    return m_device->get_root_node();
}

void net_hdl::expose_oscquery_server(uint32_t udp_port, uint32_t tcp_port)
{
    auto server = std::make_unique<ossia::oscquery::oscquery_server_protocol>(udp_port, tcp_port);
    m_protocol->expose_to(std::move(server));
}

void net_hdl::expose_osc(const char* remote_addr, uint32_t remote_port, uint32_t local_port)
{
    auto osc_protocol = std::make_unique<ossia::net::osc_protocol>(remote_addr, remote_port, local_port);
    m_protocol->expose_to(std::move(osc_protocol));
}

void net_hdl::expose_minuit(const char* remote_addr, uint32_t remote_port, uint32_t local_port)
{
    auto minuit = std::make_unique<minuit_protocol>(m_device->get_name(), remote_addr,
                                                    remote_port, local_port);
    m_protocol->expose_to((std::move(minuit)));
}
