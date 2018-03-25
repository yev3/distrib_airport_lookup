Team members and their respective contributions.
================================================================================
Y. Kamenski wrote all the code.


Data structures and algorithms to meet performance and usability requirements.
================================================================================
airports_server:
- A balanced KD tree was bulk-constructed from airports file.
- Lookup is performed by keeping a set of closest 5 records and continually
  pruning tree branches that fall outside the closest 5 range.

places_server:
- A Trie was bulk-constructed from the places2k.txt file.
- Lookup is performed by traversing the trie with given prefix, then attempting
  to complete the query to the closest match.
- User's provided state is used to resolve ambiguity in city's results.
- When city couldn't be completed to closest match, ambiguous error is returned.


Third-party code (name and reference, if any)
================================================================================
No libraries other than STL were used.


Strengths and weakness
================================================================================
+ C++ containers and managed pointers are used to simplify code
+ Programs are robust and fail at startup in case of data file failure

- Only bulk-loading are implemented for simplicity in KD & Trie data structures
- Only query are implemented in KD & Trie data structures


Instructions to run
================================================================================

run_all bash script is provided to execute the commands below.

Execute the following commands in the project directory:

$ make

$ ./airports_server &

$ ./places_server [airports_server_host] &

$ ./client [places_server_host] seattle wa


Files summary
================================================================================

*  places_airports.x / .h / _clnt.c / _xdr.c
--------------------------------------------------------------------------------
IDL program file and the generated common stubs to marchall and retrieve data.

*  common.h / .cpp
--------------------------------------------------------------------------------
Structures / routines common to all files to handle errors and console output.

*  airports_kd_tree.h / .cpp
--------------------------------------------------------------------------------
Implementation of the KD tree. Two public functions are used to init and query:
extern "C" void initKD(const char* airportsPath);
airport* kd5Closest(const location target);

*  airports_server.cpp
--------------------------------------------------------------------------------
Server stub implementation that interfaces with KD tree to retrieve records.

*  places_trie.h / .cpp
--------------------------------------------------------------------------------
Implementation of the Trie data structure. Module interface to init and query:
void initTrie(const char* placesPath);
TrieQueryResult queryPlace(const std::string &city, const std::string &state);

*  places_server.cpp
--------------------------------------------------------------------------------
Server stub implementation that initializes Trie and queries to find cities.

*  places_client.cpp
--------------------------------------------------------------------------------
Client for places_server.

*  Makefile
--------------------------------------------------------------------------------
Used to build the project.

*  run_all
--------------------------------------------------------------------------------
Bash script to build, launch server instances, and perform a sample query.