set(WPN114_NETWORK_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/net_hdl.hpp)

set(WPN114_NETWORK_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/net_hdl.cpp)

add_library(wpn114net SHARED
    ${WPN114_NETWORK_HEADERS}
    ${WPN114_NETWORK_IMPLEMENTATIONS})

target_include_directories(wpn114net PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(wpn114net /usr/local/lib/libossia.dylib)
