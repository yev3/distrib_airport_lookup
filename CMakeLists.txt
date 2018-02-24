cmake_minimum_required(VERSION 3.9)
project(distrib_rpc)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(COMMON_FUNCT
    common.cpp
    common.h
        places_airports_clnt.c
        places_airports_xdr.c
        places_airports.h
)

add_executable(client
    client_main.cpp
    ${COMMON_FUNCT}
)

add_executable(places_server
        places_server.cpp
    places_trie.cpp
    places_trie.h
    ${COMMON_FUNCT}
)

add_executable(airports_server
        airports_server.cpp
        airports_kd_tree.cpp
    airports_kd_tree.h
    ${COMMON_FUNCT}
)
