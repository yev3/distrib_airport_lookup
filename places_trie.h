#pragma once

#include "common.h"

// Public interface functions
/*****************************************************************************/

/** Found places collection with references to the indexed city entries */
using TFoundPlaces = std::vector<std::reference_wrapper<const CityRecord>>;

/** Result of a user's query */
struct TrieQueryResult {
  TFoundPlaces places;  // Empty when not found, partial when ambiguous
  bool isAmbiguous;     // Flag to indicate when query is ambiguous
};

/**
 * \brief Initializes the trie lookup data structure with given data. 
 * Throws on IO/file format error.
 * \param placesPath Path to the places file to load
 * \param recordCountHint Approx #records, shrinks to fit at end.
 */
void initTrie(const char* placesPath, const int recordCountHint);

/**
 * \brief Performs an efficient prefix completion lookup using a Trie data
 * structure. Uses state to filter ambiguous entries. Returns ref to 
 * stored records and flag if lookup was not found or is ambiguous.
 * \param city City name to lookup
 * \param state State to use when ambiguous entries
 * \return A list of references to city records, and a flag if ambiguous
 */
TrieQueryResult queryPlace(const std::string &city, const std::string &state);

// Trie class that is used to perform an efficient lookup
/*****************************************************************************/

/** Type of collection of cities loaded from the places file */
using TPlaceRecs = std::unique_ptr<std::vector<CityRecord>>;

/**
 * \brief Trie data structure to hold place information
 */
class PlacesTrie {
public:

  /**
   * \brief Initializes and builds an internal index of the city records
   * \param cityRecords Reference to the city records
   */
  explicit PlacesTrie(TPlaceRecs cityRecords);


  /**
   * \brief Perform a query on the trie, case-insensitive
   * \param cityName City name of the places
   * \return Trie traversal results
   */
  TrieQueryResult query(const std::string &cityName) const;

  /**
   * \brief Get the size of the underlying container constructed with.
   * \return Number of records including duplicates.
   */
  size_t size() const;

private:
  struct TrieNode {
    const char c;                           // Char associated with node
    std::pair<int, int> idxRange{-1, -1};   // Range of entries that match
    std::vector<TrieNode> next;             // Sorted list of next nodes

    explicit TrieNode(const char ch);
  };

  /**
   * \brief Helper to traverse the tree for the given query
   * \param cname City name
   * \param node Current node
   * \param depth Current subtree depth
   * \return Result of the query search
   */
  TrieQueryResult query(const std::string &cname, const TrieNode &node,
                    const int depth) const;

  /**
   * \brief Attempts to traverse the remaining tree to find the first valid 
   *        prefix completion. When shortest prefix search fails, 
   *        returns an ambiguous set of results.
   * \param node Current subtree being searched
   * \return Result of the completion attempt
   */
  TrieQueryResult getFirstCompletion(const TrieNode &node) const;

  TrieQueryResult getAmbiguousHints(const TrieNode &node) const;

  /**
   * \brief Helper to construct a trie subtree from a given subrange of records
   * \param begin First index of the subrange constructed
   * \param end Last exclusive index of the subrange constructed
   * \param depth Current depth of the subtree
   * \param node Current root node of the subree
   */
  void construct(const int begin, const int end,
                 const int depth, TrieNode &node) const;

  /**
   * \brief Helper to find the subrange where all chars at this range are same
   * \param fm Beginning index of the subrange
   * \param to End index of the subrange (exclusive)
   * \param depth Current depth of the subree
   * \return First index where the character is different from first index 
   */
  int endOfSameLetterRange(const int fm, const int to,
                           const size_t depth) const;

  /**
   * \brief Place records owned by the trie. Freed in dtor.
   */
  TPlaceRecs places;

  /**
   * \brief Root node of the Trie
   */
  TrieNode root;
};

