set(WPN114_NETWORK_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/net_hdl.hpp)

set(WPN114_NETWORK_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/net_hdl.cpp)

if(WPN_CONTROL_OSSIA_TOML)
        set(WPN114_NETWORK_HEADERS
            ${WPN114_NETWORK_HEADERS}
            ${CMAKE_CURRENT_SOURCE_DIR}/net_toml.hpp)
        set(WPN114_NETWORK_IMPLEMENTATIONS
            ${WPN114_NETWORK_IMPLEMENTATIONS}
            ${CMAKE_CURRENT_SOURCE_DIR}/net_toml.cpp)
endif()

add_library(wpn114net SHARED
    ${WPN114_NETWORK_HEADERS}
    ${WPN114_NETWORK_IMPLEMENTATIONS})

if(WPN_CONTROL_OSSIA_TOML)
    target_include_directories(wpn114net PRIVATE ${CPPTOML_INCLUDE_DIR})
endif()

target_include_directories(wpn114net PUBLIC ${OSSIA_INCLUDE_DIR})
target_include_directories(wpn114net PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(wpn114net ossia)
