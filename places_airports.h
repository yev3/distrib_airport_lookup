/*****************************************************************************
 * Yevgeni Kamenski
 * CPSC 5520 Distributed Systems, Seattle University, 2018
 * Project #2: A Three-Tiered Airport Lookup System
 *****************************************************************************/
#pragma once

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NRESULTS 5
#define PLACE_NAME_MAX 65
#define STATE_MAX 3
#define AIRCODE_MAX 4
#define ERRMSG_MAX 384

#define REQ_NAMED 0
#define REQ_LAT_LONG 1

struct location {
	double latitude;
	double longitude;
};
typedef struct location location;

struct place {
	char *name;
	char *state;
	location loc;
};
typedef struct place place;

struct airport {
	location loc;
	double dist;
	char *code;
	char *name;
	char *state;
};
typedef struct airport airport;

typedef airport airports[NRESULTS];

struct name_state {
	char *name;
	char *state;
};
typedef struct name_state name_state;

struct places_req {
	int req_type;
	union {
		name_state named;
		location loc;
	} places_req_u;
};
typedef struct places_req places_req;

struct place_airports {
	place request;
	airports results;
};
typedef struct place_airports place_airports;

struct places_ret {
	int err;
	union {
		place_airports results;
		char *error_msg;
	} places_ret_u;
};
typedef struct places_ret places_ret;

struct airports_ret {
	int err;
	union {
		airports results;
		char *error_msg;
	} airports_ret_u;
};
typedef struct airports_ret airports_ret;

#define PLACES_PROG 0x3AB0B041
#define PLACES_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define PLACES_QRY 1
extern  places_ret * places_qry_1(places_req *, CLIENT *);
extern  places_ret * places_qry_1_svc(places_req *, struct svc_req *);
extern int places_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define PLACES_QRY 1
extern  places_ret * places_qry_1();
extern  places_ret * places_qry_1_svc();
extern int places_prog_1_freeresult ();
#endif /* K&R C */

#define AIRPORTS_PROG 0x3AB0B042
#define AIRPORTS_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define AIRPORTS_QRY 2
extern  airports_ret * airports_qry_1(location *, CLIENT *);
extern  airports_ret * airports_qry_1_svc(location *, struct svc_req *);
extern int airports_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define AIRPORTS_QRY 2
extern  airports_ret * airports_qry_1();
extern  airports_ret * airports_qry_1_svc();
extern int airports_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_location (XDR *, location*);
extern  bool_t xdr_place (XDR *, place*);
extern  bool_t xdr_airport (XDR *, airport*);
extern  bool_t xdr_airports (XDR *, airports);
extern  bool_t xdr_name_state (XDR *, name_state*);
extern  bool_t xdr_places_req (XDR *, places_req*);
extern  bool_t xdr_place_airports (XDR *, place_airports*);
extern  bool_t xdr_places_ret (XDR *, places_ret*);
extern  bool_t xdr_airports_ret (XDR *, airports_ret*);

#else /* K&R C */
extern bool_t xdr_location ();
extern bool_t xdr_place ();
extern bool_t xdr_airport ();
extern bool_t xdr_airports ();
extern bool_t xdr_name_state ();
extern bool_t xdr_places_req ();
extern bool_t xdr_place_airports ();
extern bool_t xdr_places_ret ();
extern bool_t xdr_airports_ret ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

