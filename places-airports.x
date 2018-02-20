/* Y.Kamenski */
/* Project 2 - client, places, airports */
/* To generate: $rpcgen -a -C client-places-airports.x */

/******************************************************************************/
/* Program constants */
/******************************************************************************/

const NRESULTS    = 5;      /* Number of closest airports */
const NAME_MAX    = 65;     /* Max len of place name */
const STATE_MAX   = 3;      /* Max len of state */
const AIRCODE_MAX = 4;      /* Max len of air code */
const ERRMSG_MAX  = 384;    /* Max len of an error message */
/* Note: strings do not include null term char in len */

/******************************************************************************/
/* PLACE DATA STRUCTURES */
/******************************************************************************/

/* Location on a map */
struct location {
    double latitude;    /* Lat in decimal point format */
    double longitude;   /* Long in decimal point format */
};

/* Place on a map */
struct place {
    string name<NAME_MAX>;      /* City / place name */
    string state<STATE_MAX>;    /* US state abbrev. */
    location loc;               /* lat/long location */
};

/******************************************************************************/
/* AIRPORTS DATA STRUCTURES */
/******************************************************************************/

/* An airport record used for replies */
struct airport {
    location loc;               /* Lat/long location */
    double dist;                /* Dist from search target in stat. miles */
    string code<AIRCODE_MAX>;   /* airport code */
    string name<NAME_MAX>;      /* full airport name */
    string state<STATE_MAX>;    /* airport state abbrev. */
};

/* Resulting list of closest airports */
typedef airports airport[NRESULTS];

/* REQUEST + REPLY DATA STRUCTURES */
/******************************************************************************/

/* City + state based request from the client*/
struct name_state {
    string name<NAME_MAX>;      /* Name of place to lookup */
    string state<STATE_MAX>;    /* US State abbreviation to lookup */
};

/* Union representing the type of client request - by name or by lat / long */
union places_req switch (int req_type) {
  case 0:
    name_state named;           /* Named request by place/US state */
  case 1:
    location loc;               /* Request using lat /long */
  default:
    void;
};

/* Reply data to client with a name record and closest airports */
struct place_airports {
    place request;              /* Actual place record used to lookup */
    airports results;           /* List of resulting airports */
};

/* Reply from places to client with an optional errno */
union places_ret switch (int err) {
  case 0:
    place_airports results;         /* Resulting user query result */
  default:
    string error_msg<ERRMSG_MAX>;   /* Error message in case of failure */
};

/* Reply from airports to places with an optional errno */
union airports_ret switch (int err) {
  case 0:
    airports results;               /* Found airport records */
  default:
    string error_msg<ERRMSG_MAX>;   /* Error message in case of failure */
};

/* PLACES + AIRPORTS IDL programs */
/******************************************************************************/
program AIRPORTS_PLACES_PROG {
    version AIRPORTS_PLACES_VERS {
        /* Query for a place and return place and list of closest airports */
        places_ret PLACES_QRY(places_req) = 1;

        /* Query for a lat / long and return closest airports */
        airports_ret AIRPORTS_QRY(location) = 2;
    } = 1;
} = 0x3AB0B041;
/* Note: Program number must start from 2 or 3          */
/*       To gen a random: $head -c 4 /dev/urandom | xxd */
