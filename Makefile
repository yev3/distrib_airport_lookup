# Y.Kamenski
# Project 2

LINK.o = $(LINK.cc)
CFLAGS += -g
CXXFLAGS= -g -Wall -pedantic -std=c++1y

PROJ_OBJECTS = $(addsuffix .o,$(basename $(wildcard *.c *.cpp)))
PROJ_HEADERS = $(wildcard *.h)

all : client places_server airports_server
all : CXXFLAGS += -O3 -DNDEBUG
all : CFLAGS += -O3 -DNDEBUG

debug: client places_server airports_server

# client : test-main.o common.o places_trie.o airports_kd_tree.o -lnsl

client : client_main.o common.o places_trie.o airports_kd_tree.o places-airports_clnt.o places-airports_xdr.o -lnsl
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

places_server : places_server.o places_trie.o common.o places_server_svc.o places-airports_clnt.o places-airports_xdr.o -lnsl
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

airports_server : airports_server.o airports_server_svc.o airports_kd_tree.o common.o places-airports_xdr.o -lnsl
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

# Header dependencies in the project
common.o : common.h
airports_kd_tree.o : airports_kd_tree.h
places_trie.o : places_trie.h
airports_server.o places_server.o places-airports_clnt.o : places-airports.h
places-airports_xdr.o airports_server_svc.o places_server_svc.o : places-airports.h

test-main.o : $(PROJ_HEADERS)


clean:
	$(RM) client places_server airports_server $(PROJ_OBJECTS)





# client_main.o test-main.o


