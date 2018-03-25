## Summary

Three-tiered client-server system used to lookup closest airports to the user's given city.

### `client`
- Used by client to query with a city and optional state. 
- Uses RPC to contact the `places_server` with location information.
- In case of ambiguity, an error with partial results is returned.  
- Returns 5 closest airports to the user's given location.

### `places_server`
- Uses a Trie constructed from the US Census location data file.
- Auto-completes the query by efficiently looking up a proper prefix.
- Uses user's state to resolve ambiguity (same city name in multiple states)
- Contacts `airports_server` with lat/lon of city to query closest airports.

### `airports_server`
- A balanced KD tree was bulk-constructed from data file.
- Efficiently queries K-Nearest-Neighbors by pruning tree branches. 
- Uses RPC to return 5 closest airports to the given lat/lon request

## Example Run

```bash
[yk@arch bin]$ ./airports_server &
[yk@arch bin]$ ./places_server localhost &
[yk@arch bin]$ ./client localhost seattle
```
```
Seattle, WA {   47.63, -122.33 }
#   Dist Code State Name
--+-----+----+-----+------------------------------------------
1.   6.8 BFI  WA    Seattle/Boeing
2.  10.2 RNT  WA    Renton
3.  12.3 SEA  WA    Seattle
4.  20.4 PAE  WA    Everet/Paine
5.  22.7 PWT  WA    Bremerton
```

## File Descriptions

### `places_airports.x / .h / _clnt.c / _xdr.c`

IDL program file and the generated common stubs to marchall and retrieve data.

### `common.h / .cpp`
Structures / routines common to all files to handle errors and console output.

### `airports_kd_tree.h / .cpp`
Implementation of the KD tree. Two public functions are used to init and query:
extern "C" void initKD(const char* airportsPath);
airport* kd5Closest(const location target);

### `airports_server.cpp`
Server stub implementation that interfaces with KD tree to retrieve records.

### `places_trie.h / .cpp`
Implementation of the Trie data structure. Module interface to init and query:
void initTrie(const char* placesPath);
TrieQueryResult queryPlace(const std::string &city, const std::string &state);

### `places_server.cpp`
Server stub implementation that initializes Trie and queries to find cities.

### `places_client.cpp`
Client for places_server.

### `Makefile`
Used to build the project.

### `run_all.sh`
Bash script to build, launch server instances, and perform a sample query.
