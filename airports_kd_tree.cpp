#include "airports_kd_tree.h"

#include <algorithm>
#include <sstream>
#include <fstream>

#include <cmath>
#include <cstring>

static std::unique_ptr<AirportKDTree> kdTree;

// Helper to load airports from file. Throws on IO/parse error.
TAirportRecs loadAirports(const char* path);

void initKD(const char *airportsPath) {
  log_printf("Loading from file: %s.", airportsPath);

  // CS1 only supports c++11, can't use!
  //kdTree = std::make_unique<AirportKDTree>(loadAirports(airportsPath));

  kdTree = std::unique_ptr<AirportKDTree>(
    new AirportKDTree(loadAirports(airportsPath)));

  log_printf("Loaded %d airports.", (int)kdTree->size());
}

airport * kd5Closest(const location target) {
  static airports result;
  constexpr size_t resultArrSize = sizeof(result) / sizeof(result[0]);

  // Clear out any previous values
  std::memset(&result, 0, sizeof(airports));

  // Query KD-Tree and copy values and string pointers to static data
  auto closest = kdTree->kClosestLocations(target);
  const size_t nResults = std::min(closest.size(), resultArrSize);
  for (size_t i = 0; i < nResults; ++i) {
    result[i].dist = closest[i].dist;
    const auto &airp = *closest[i].airport;
    result[i].code = (char*)airp.code.c_str();
    result[i].name = (char*)airp.name.c_str();
    result[i].state = (char*)airp.state.c_str();
    result[i].loc = airp.loc;
  }
  
  return &result[0];
}

/**
 * \brief Constructs an AirportRecord from a data file line.
 * \param line airport-locations.txt file line
 * \return Constructed AirportRecord.
 */
AirportRecord airportFromLine(std::string &line) {
  constexpr double inf = std::numeric_limits<double>::max();

  std::string code;     ///< Airport code
  double latit = inf;   ///< Latitude
  double longit = inf;  ///< Longitude
  std::string name;     ///< Full airport name
  std::string state;    ///< Airport state

  std::istringstream strm{ line }; 
  strm >> code >> latit >> longit >> std::ws;
  name.assign(std::istreambuf_iterator<char>{strm}, {});
  const auto commaIdx = name.rfind(',');
  if (commaIdx != std::string::npos) {
    state = name.substr(commaIdx + 1);
    name.erase(commaIdx);
  }

  if (code.size() != 5 ||
      latit == inf || longit == inf ||
      name.empty() || state.empty()) {
    throw std::invalid_argument("Invalid airport record: " + line);
  }

  return AirportRecord{{latit, longit}, code.substr(1, 3), name, state};
}

TAirportRecs loadAirports(const char* path) {
  std::ifstream airFile(path);

  if (!airFile) 
    throw std::invalid_argument("Unable to open airports file for reading.");
    //exitMessage("Unable to open '" + std::string(path) + "' for reading.");


  // CS1 only supports c++11, can't use!
  //auto airRecs = std::make_unique<std::vector<AirportRecord>>();

  auto airRecs = std::unique_ptr<std::vector<AirportRecord>>(
    new std::vector<AirportRecord>());

  std::string line;
  std::getline(airFile, line);  // first line is header

  if (line.size() < 10 || line.substr(1, 3) != "air")
    throw std::invalid_argument("Airports file is in invalid format.");

  while (std::getline(airFile, line)) {
    if (line.empty()) continue;
    airRecs->emplace_back(airportFromLine(line));
  }
  airRecs->shrink_to_fit();
  return airRecs;
}


// Forward declaration of the helper routines + classes in this file
/******************************************************************************/

// Type of random-access iterator used to construct tree
using It = std::vector<AirportRecord>::iterator;

/**
 * \brief Constructs a KD subtree from the given range of records.
 * \param fm Start of records sequence
 * \param to End of records sequence
 * \param depth current depth of the subtree
 * \return Constructed subtree from the given range
 */
static std::unique_ptr<KDNode> construct(It fm, It to, int depth = 0);

/**
 * \brief Traverses the K-D Tree and collects k-closest points to the target.
 * \param node Current subtree node
 * \param target Target location to collect closest points to
 * \param k K number of closest points to collect
 * \param isEvenNodeLevel Current depth of the subtree rooted at node is even.
 * \param closest OUT of the collected closest points, ordered by dist
 */
void kClosestPimpl(const std::unique_ptr<KDNode> &node,
                   const location &target,
                   const size_t k, const bool isEvenNodeLevel,
                   std::vector<DistAirport> &closest);



KDNode::KDNode(AirportRecord pt,
               std::unique_ptr<KDNode> lft,
               std::unique_ptr<KDNode> rgt):
  airport(std::move(pt)), left(std::move(lft)), right(std::move(rgt)) { }

AirportKDTree::
AirportKDTree(TAirportRecs airRecs): airports(std::move(airRecs)) {
  root = construct(airports->begin(), airports->end());
}


std::vector<DistAirport> 
AirportKDTree::kClosestLocations(const location target, const size_t k) const {
  std::vector<DistAirport> results;
  kClosestPimpl(root, target, k, true, results);
  return results;
}

size_t AirportKDTree::size() const {
  return airports->size();
}

// Private implementation of the KD-Tree class below
/******************************************************************************/

static constexpr long double PI = 3.141592653589793238L;

static long double deg2rad(long double deg) {
  return deg * PI / 180.0L;
}

/**
 * \brief Computes great circle distance between two points. Note that long double
 * precision is here since this computation suffers from floating point epsilon
 * problems.
 * \param lat1 Point 1 latitude
 * \param lon1 Point 1 longitude
 * \param lat2 Point 2 latitude
 * \param lon2 Point 2 longitude
 * \return Great circle distance in statute miles
 */
static long double distance(long double lat1, long double lon1,
                            long double lat2, long double lon2) {
  lat1 = deg2rad(lat1);
  lat2 = deg2rad(lat2);
  lon1 = deg2rad(lon1);
  lon2 = deg2rad(lon2);
  const long double a = std::sin(lat1) * std::sin(lat2);
  const long double b = std::cos(lat1) * std::cos(lat2) * std::cos(lon2 - lon1);
  return 3959.0L * std::acos(a + b);
}

/**
 * \brief Computes great circle distance between two Locations. 
 * \param pt1 Location of point 1.
 * \param pt2 Location of point 2.
 * \return Great circle distance in statute miles
 */
static long double distance(const location& pt1, const location& pt2) {
  return distance(pt1.latitude, pt1.longitude, pt2.latitude, pt2.longitude);
}


static std::unique_ptr<KDNode> construct(It fm, It to, int depth) {
  // Base case
  if (fm >= to) return nullptr;

  // Comparator used on even depth of the tree. Splits plane by latitude.
  static const auto latComparator = 
    [](const AirportRecord &p1, const AirportRecord &p2) {
      return p1.loc.latitude < p2.loc.latitude;
  };

  // Comparator used on odd depth of the tree. Splits plane by longitude.
  static const auto longComparator = 
    [](const AirportRecord &p1, const AirportRecord &p2) {
      return p1.loc.longitude < p2.loc.longitude;
  };

  const int mid = (int)(std::distance(fm, to) / 2); // Offset to the med node

  // Partition around median node
  if ((depth & 1) == 0) {
    std::nth_element(fm, fm + mid, to, latComparator);
  } else {
    std::nth_element(fm, fm + mid, to, longComparator);
  }

  // Create a subtree with the value of median and children of partitions

  // CS1 only supports c++11, can't use!
  //return std::make_unique<KDNode>(*(fm + mid),
  //                                construct(fm, fm + mid, depth + 1),
  //                                construct(fm + mid + 1, to, depth + 1));

  return std::unique_ptr<KDNode>(
    new KDNode(*(fm + mid),
               construct(fm, fm + mid, depth + 1),
               construct(fm + mid + 1, to, depth + 1)));
}

void kClosestPimpl(const std::unique_ptr<KDNode> &node,
                   const location &target,
                   const size_t k, const bool isEvenNodeLevel,
                   std::vector<DistAirport> &closest) {
  if (!node) return;                      // Base case

  location nloc = node->airport.loc;              // Alias for readability
  double dist = (double)distance(nloc, target);   // Great circ dist in miles


  // Collect the current node when it belongs in closest k set
  // Note: Linear insertion here since objs are small
  if (closest.size() < k || dist < closest.back().dist) {
    closest.emplace(
      std::find_if(closest.begin(), closest.end(),
                   [dist](const DistAirport &rec) { return dist < rec.dist; }),
      node->airport, dist);
    if (closest.size() > k) {
      closest.pop_back();
    }
  }

  // Determine which side of the div plane faces the target
  const bool leftSubtreeCloser = isEvenNodeLevel
                                   ? target.latitude < nloc.latitude
                                   : target.longitude < nloc.longitude;

  // Visit the side closer to target
  kClosestPimpl(leftSubtreeCloser ? node->left : node->right,
                target, k, !isEvenNodeLevel, closest);

  // Move nodeLoc on split plane closest to target, check if intersects
  if (isEvenNodeLevel) {
    nloc.longitude = target.longitude;
  } else {
    nloc.latitude = target.latitude;
  }
  const double planeMinDist = (double)distance(nloc, target);
  const bool planeIntersects = planeMinDist < closest.back().dist;

  // Search farther side of div plane when intersects or have not filled k
  if (closest.size() < k || planeIntersects) {
    kClosestPimpl(leftSubtreeCloser ? node->right : node->left,
                  target, k, !isEvenNodeLevel, closest);
  }
}
