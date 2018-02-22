# Y.Kamenski
# Project 2

LINK.o = $(LINK.cc)
CFLAGS += -g
CXXFLAGS= -g -Wall -pedantic -std=c++1y

PROJ_OBJECTS = $(addsuffix .o,$(basename $(wildcard *.c *.cpp)))
PROJ_HEADERS = $(wildcard *.h)

all : client places_server airports_server
all : CXXFLAGS += -O3
all : CFLAGS += -O3

client : test-main.o common.o places_trie.o airports_kd_tree.o -lnsl

# places_server : listdir_client.o listdir_clnt.o listdir_xdr.o -lnsl

# airports_server : listdir_server.o listdir_svc.o listdir_xdr.o -lnsl

# Header dependencies in the project
common.o : common.h
airports_kd_tree.o : airports_kd_tree.h
places_trie.o : places_trie.h
airports_server.o places_server.o places-airports_clnt.o : places-airports.h
places-airports_xdr.o airports_server_svc.o places_server_svc.o : places-airports.h

test-main.o : $(PROJ_PEADERS)


clean:
	$(RM) client places_server airports_server $(PROJ_OBJECTS)





# client_main.o test-main.o


