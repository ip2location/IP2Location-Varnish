#include <stdlib.h>
#include <string.h>
#include <IP2Location.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#ifdef __FreeBSD__
#include <stdio.h>
#define gcvt(v, d, c) sprintf(c, "%*g", d, v);
#endif

#include "cache/cache.h"

/* Varnish < 6.2 compat */
#ifndef VPFX
#	define VPFX(a) vmod_ ## a
#	define VARGS(a) vmod_ ## a ## _arg
#	define VENUM(a) vmod_enum_ ## a
#	define VEVENT(a) a
#else
#	define VEVENT(a) VPFX(a)
#endif

#ifndef VRT_H_INCLUDED
#include "vrt.h"
#endif

/* Defined options for querying IP2Location data */
#define query_COUNTRY_SHORT			1
#define query_COUNTRY_LONG			2
#define query_REGION				3
#define query_CITY					4
#define query_ISP					5
#define query_LATITUDE				6
#define query_LONGITUDE				7
#define query_DOMAIN				8
#define query_ZIPCODE				9
#define query_TIMEZONE				10
#define query_NETSPEED				11
#define query_IDDCODE				12
#define query_AREACODE				13
#define query_WEATHERSTATIONCODE	14
#define query_WEATHERSTATIONNAME	15
#define query_MCC					16
#define query_MNC					17
#define query_MOBILEBRAND			18
#define query_ELEVATION				19
#define query_USAGETYPE				20

typedef struct vmod_ip2location_data {
	time_t			ip2l_db_ts;	 /* timestamp of the database file */
	IP2Location		*ip2l_handle;
	pthread_mutex_t	lock;
} ip2location_data_t;

void
i2pl_free(void *d)
{
	ip2location_data_t *data = d;

	if (data->ip2l_handle != NULL) {
		IP2Location_close(data->ip2l_handle);
	}
}

VCL_VOID
VPFX(init_db)(VRT_CTX, struct VPFX(priv) *priv, char *filename, char *memtype)
{
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (priv->priv != NULL)
		return;

	IP2Location *IP2LocationObj = IP2Location_open((char *) filename);

	if (IP2LocationObj == NULL) {
		printf("Not able to load IP2Location Database \"%s\".\n", (char *) filename);

		exit(0);
	}

	printf("IP2Location Database %s is loaded.\n", (char *) filename);

	priv->priv = IP2LocationObj;

	if (strcmp(memtype, "IP2LOCATION_FILE_IO") == 0) {
		IP2Location_set_lookup_mode(priv->priv, IP2LOCATION_FILE_IO);
	} else if (strcmp(memtype, "IP2LOCATION_CACHE_MEMORY") == 0) {
		IP2Location_set_lookup_mode(priv->priv, IP2LOCATION_CACHE_MEMORY);
	} else if (strcmp(memtype, "IP2LOCATION_SHARED_MEMORY") == 0) {
		IP2Location_set_lookup_mode(priv->priv, IP2LOCATION_SHARED_MEMORY);
	}

	AN(priv->priv);
	priv->free = i2pl_free;
	
}

// Use this function to query result, and then extract the field based on user selection
void *
query_all(VRT_CTX, struct VPFX(priv) *priv, char * ip, int option)
{
	IP2LocationRecord *r;
	IP2Location *handle;
	char *result = NULL;
	char buf[10];

	printf("The client IP is %s.\n", (char *) ip);
	
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (priv->priv == NULL)
		return result;

	handle = priv->priv;
	r = IP2Location_get_all(handle, (char *) ip);

	if (r == NULL)
		return ("????");

	switch (option) {
		case query_COUNTRY_SHORT:
			result = WS_Copy(ctx->ws, r->country_short, -1);
			break;
		case query_COUNTRY_LONG:
			result = WS_Copy(ctx->ws, r->country_long, -1);
			break;
		case query_REGION:
			result = WS_Copy(ctx->ws, r->region, -1);
			break;
		case query_CITY:
			result = WS_Copy(ctx->ws, r->city, -1);
			break;
		case query_ISP:
			result = WS_Copy(ctx->ws, r->isp, -1);
			break;
		case query_LATITUDE:
			gcvt(r->latitude, 5, buf) ;
			result = WS_Copy(ctx->ws, buf, -1);
			break;
		case query_LONGITUDE:
			gcvt(r->longitude, 5, buf);
			result = WS_Copy(ctx->ws, buf, -1);
			break;
		case query_DOMAIN:
			result = WS_Copy(ctx->ws, r->domain, -1);
			break;
		case query_ZIPCODE:
			result = WS_Copy(ctx->ws, r->zipcode, -1);
			break;
		case query_TIMEZONE:
			result = WS_Copy(ctx->ws, r->timezone, -1);
			break;
		case query_NETSPEED:
			result = WS_Copy(ctx->ws, r->netspeed, -1);
			break;
		case query_IDDCODE:
			result = WS_Copy(ctx->ws, r->iddcode, -1);
			break;
		case query_AREACODE:
			result = WS_Copy(ctx->ws, r->areacode, -1);
			break;
		case query_WEATHERSTATIONCODE:
			result = WS_Copy(ctx->ws, r->weatherstationcode, -1);
			break;
		case query_WEATHERSTATIONNAME:
			result = WS_Copy(ctx->ws, r->weatherstationname, -1);
			break;
		case query_MCC:
			result = WS_Copy(ctx->ws, r->mcc, -1);
			break;
		case query_MNC:
			result = WS_Copy(ctx->ws, r->mnc, -1);
			break;
		case query_MOBILEBRAND:
			result = WS_Copy(ctx->ws, r->mobilebrand, -1);
			break;
		case query_ELEVATION:
			gcvt(r->elevation, 5, buf);
			result = WS_Copy(ctx->ws, buf, -1);
			break;
		case query_USAGETYPE:
			result = WS_Copy(ctx->ws, r->usagetype, -1);
			break;
		default:
			result = WS_Copy(ctx->ws, "-", -1);
			break;
	}

	IP2Location_free_record(r);

	return (result);
}

#define FUNC(UPPER, lower)						\
	VCL_STRING 							\
	vmod_ ## lower(VRT_CTX, struct vmod_priv *priv, char * ip)	\
	{								\
		return (query_all(ctx, priv, ip, 			\
		    query_ ## COUNTRY_SHORT));				\
	}

FUNC(country_short, COUNTRY_SHORT)
FUNC(country_long , COUNTRY_LONG);
FUNC(region, REGION);
FUNC(city, CITY);
FUNC(isp, ISP);
FUNC(latitude, LATITUDE);
FUNC(longitude, LONGITUDE);
FUNC(domain, DOMAIN);
FUNC(zipcode, ZIPCODE);
FUNC(timezone, TIMEZONE);
FUNC(netspeed, NETSPEED);
FUNC(iddcode, IDDCODE);
FUNC(areacode, AREACODE);
FUNC(weatherstationcode, WEATHERSTATIONCODE);
FUNC(weatherstationname, WEATHERSTATIONNAME);
FUNC(mcc, MCC);
FUNC(mnc, MNC);
FUNC(mobilebrand, MOBILEBRAND);
FUNC(elevation, ELEVATION);
FUNC(usagetype, USAGETYPE);
