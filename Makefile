# Y.Kamenski
# Project 2

VPATH = src

LINK.o =$(LINK.cc)
CFLAGS += -g -Iinclude
CXXFLAGS = -g -Wall -pedantic -std=c++1y -Iinclude
LDLIBS += -lnsl

PROJ_OBJECTS = $(addsuffix .o,$(basename $(wildcard *.c *.cpp)))
PROJ_HEADERS = $(wildcard *.h)
PROJ_BIN_NAMES = client places_server airports_server
PROJ_BIN_PATHS = $(addprefix bin/,$(PROJ_BIN_NAMES))
COMMON_FUNCT = common.o places_airports_clnt.o places_airports_xdr.o


PHONY: help
help: ; $(info $$var is [${var}])

all : $(PROJ_BIN_PATHS)
all : CXXFLAGS += -O3 -DNDEBUG
all : CFLAGS += -O3 -DNDEBUG

debug: $(PROJ_BIN_PATHS)

bin/client : places_client.o $(COMMON_FUNCT)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

bin/places_server : places_server.o places_trie.o $(COMMON_FUNCT)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

bin/airports_server : airports_server.o airports_kd_tree.o $(COMMON_FUNCT)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(PROJ_OBJECTS): $(PROJ_HEADERS)

clean:
	$(RM) $(PROJ_BIN_PATHS) $(PROJ_OBJECTS)

