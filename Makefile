# Y.Kamenski
# Project 2

LINK.o =$(LINK.cc)
CFLAGS += -g
CXXFLAGS = -g -Wall -pedantic -std=c++1y
LDLIBS += -lnsl

PROJ_OBJECTS = $(addsuffix .o,$(basename $(wildcard *.c *.cpp)))
PROJ_HEADERS = $(wildcard *.h)
COMMON_FUNCT = common.o places_airports_clnt.o places_airports_xdr.o -lnsl

all : client places_server airports_server
all : CXXFLAGS += -O3 -DNDEBUG
all : CFLAGS += -O3 -DNDEBUG

debug: client places_server airports_server

client : client_main.o $(COMMON_FUNCT)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

places_server : places_server.o places_trie.o $(COMMON_FUNCT)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

airports_server : airports_server.o airports_server_svc.o airports_kd_tree.o $ (COMMON_FUNCT)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

# Header dependencies in the project

common.o: common.h
airports_kd_tree.o: airports_kd_tree.h
places_trie.o: places_trie.h
airports_server.o places_server.o places_airports_clnt.o: places_airports.h
places_airports_xdr.o airports_server_svc.o : places_airports.h

clean:
	$(RM) client places_server airports_server $(PROJ_OBJECTS)

