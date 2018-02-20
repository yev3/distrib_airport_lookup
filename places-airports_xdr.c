/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "places-airports.h"

bool_t
xdr_location_s (XDR *xdrs, location_s *objp)
{
	register int32_t *buf;

	 if (!xdr_double (xdrs, &objp->latitude))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->longitude))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_airport_record_s (XDR *xdrs, airport_record_s *objp)
{
	register int32_t *buf;

	 if (!xdr_location_s (xdrs, &objp->loc))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->code, 4))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->name, 128))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->state, 3))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_airp_dist_t (XDR *xdrs, airp_dist_t *objp)
{
	register int32_t *buf;

	 if (!xdr_airport_record_s (xdrs, &objp->airport))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->distance))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_airp_dist_recs_t (XDR *xdrs, airp_dist_recs_t *objp)
{
	register int32_t *buf;

	 if (!xdr_array (xdrs, (char **)&objp->airp_dist_recs_t_val, (u_int *) &objp->airp_dist_recs_t_len, 10,
		sizeof (airp_dist_t), (xdrproc_t) xdr_airp_dist_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_airports_ret (XDR *xdrs, airports_ret *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->err))
		 return FALSE;
	switch (objp->err) {
	case 0:
		 if (!xdr_airp_dist_recs_t (xdrs, &objp->airports_ret_u.airports))
			 return FALSE;
		break;
	default:
		 if (!xdr_string (xdrs, &objp->airports_ret_u.error_msg, 256))
			 return FALSE;
		break;
	}
	return TRUE;
}

bool_t
xdr_places_result_s (XDR *xdrs, places_result_s *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->place, 128))
		 return FALSE;
	 if (!xdr_airp_dist_recs_t (xdrs, &objp->airports))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_places_ret (XDR *xdrs, places_ret *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->err))
		 return FALSE;
	switch (objp->err) {
	case 0:
		 if (!xdr_places_result_s (xdrs, &objp->places_ret_u.result))
			 return FALSE;
		break;
	default:
		 if (!xdr_string (xdrs, &objp->places_ret_u.error_msg, 256))
			 return FALSE;
		break;
	}
	return TRUE;
}

bool_t
xdr_client_city_state_req_t (XDR *xdrs, client_city_state_req_t *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->city, 65))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->state, 3))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_client_req_t (XDR *xdrs, client_req_t *objp)
{
	register int32_t *buf;

	 if (!xdr_client_city_state_req_t (xdrs, objp))
		 return FALSE;
	return TRUE;
}
