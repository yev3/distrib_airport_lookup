cmake_minimum_required(VERSION 3.9)
project(distrib_rpc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

add_executable(test_main
        common.cpp
        common.h
        airports_kd_tree.cpp
        airports_kd_tree.h
        places_trie.cpp
        places_trie.h

        places-airports.h
        places-airports_xdr.c

#        places-airports_clnt.c

#        airports_server.cpp
#        airports_server_svc.c

#        places_server.cpp
#        places_server_svc.c

#        client_main.cpp

        test-main.cpp)

#add_executable(distrib_rpc
#        airports_kd_tree.cpp
#        airports_kd_tree.h
#        airports_server.cpp
#        airports_server_svc.c
#        client_main.cpp
#        common.cpp
#        common.h
#        places-airports.h
#        places-airports_clnt.c
#        places-airports_xdr.c
#        places_server.cpp
#        places_server_svc.c
#        places_trie.cpp
#        places_trie.h
#        test-main.cpp)
