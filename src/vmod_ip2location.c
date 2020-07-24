/**
 * Under testing...
 * Things to do:
*/

#include <stdlib.h>
#include <string.h>
#include <IP2Location.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include "cache/cache.h"

/* Varnish < 6.2 compat */
#ifndef VPFX
#  define VPFX(a) vmod_ ## a
#  define VARGS(a) vmod_ ## a ## _arg
#  define VENUM(a) vmod_enum_ ## a
#  define VEVENT(a) a
#else
#  define VEVENT(a) VPFX(a)
#endif

#ifndef VRT_H_INCLUDED
#include "vrt.h"
#endif

/* Defined options for querying IP2Location data */
#define query_COUNTRY_SHORT       1
#define query_COUNTRY_LONG        2
#define query_REGION              3
#define query_CITY                4
#define query_ISP                 5
#define query_LATITUDE            6
#define query_LONGITUDE           7
#define query_DOMAIN              8
#define query_ZIPCODE             9
#define query_TIMEZONE           10
#define query_NETSPEED           11
#define query_IDDCODE            12
#define query_AREACODE           13
#define query_WEATHERSTATIONCODE 14
#define query_WEATHERSTATIONNAME 15
#define query_MCC                16
#define query_MNC                17
#define query_MOBILEBRAND        18
#define query_ELEVATION          19
#define query_USAGETYPE          20

typedef struct vmod_ip2location_data {
  time_t		ip2l_db_ts;     /* timestamp of the database file */
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
	printf("The filename accepted is %s.\n", (char *) filename);	
	
	IP2Location *IP2LocationObj = IP2Location_open( (char *) filename);
	priv->priv = IP2LocationObj;
	// IP2Location_open_mem(priv->priv, IP2LOCATION_SHARED_MEMORY);
	if (strcmp(memtype, "IP2LOCATION_FILE_IO") == 0) {
		IP2Location_open_mem(priv->priv, IP2LOCATION_FILE_IO);
	} else if (strcmp(memtype, "IP2LOCATION_CACHE_MEMORY") == 0) {
		IP2Location_open_mem(priv->priv, IP2LOCATION_CACHE_MEMORY);
	} else if (strcmp(memtype, "IP2LOCATION_SHARED_MEMORY") == 0) {
		IP2Location_open_mem(priv->priv, IP2LOCATION_SHARED_MEMORY);
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
	
	char longitude[10];
	char latitude[10];
	char elevation[10];
	
	char *ip1 = (char *) ip;
	
	// printf("The IP address accepted is %s.\n", ip1);
	
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

    if (priv->priv != NULL) {
		handle = priv->priv;
        r = IP2Location_get_all(handle, ip1);

        if (r != NULL) {
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
					gcvt(r->latitude, 5, latitude); 
					result = WS_Copy(ctx->ws, latitude, -1);
					break;
				case query_LONGITUDE:
					gcvt(r->longitude, 5, longitude); 
					result = WS_Copy(ctx->ws, longitude, -1);
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
					gcvt(r->elevation, 5, elevation); 
					result = WS_Copy(ctx->ws, elevation, -1);
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
    }

    // VMOD_LOG("ERROR: IP2Location database failed to load");

    return WS_Copy(ctx->ws, "-", -1);
}

VCL_STRING
VPFX(country_short)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_COUNTRY_SHORT);
	return (result);
}

VCL_STRING
VPFX(country_long)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_COUNTRY_LONG);
	return (result);
}

VCL_STRING
VPFX(region)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_REGION);
	return (result);
}

VCL_STRING
VPFX(city)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_CITY);
	return (result);
}

VCL_STRING
VPFX(isp)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_ISP);
	return (result);
}

VCL_STRING
VPFX(latitude)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_LATITUDE);
	return (result);
}

VCL_STRING
VPFX(longitude)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_LONGITUDE);
	return (result);
}

VCL_STRING
VPFX(domain)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_DOMAIN);
	return (result);
}

VCL_STRING
VPFX(zipcode)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_ZIPCODE);
	return (result);
}

VCL_STRING
VPFX(timezone)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_TIMEZONE);
	return (result);
}

VCL_STRING
VPFX(netspeed)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_NETSPEED);
	return (result);
}

VCL_STRING
VPFX(iddcode)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_IDDCODE);
	return (result);
}

VCL_STRING
VPFX(areacode)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_AREACODE);
	return (result);
}

VCL_STRING
VPFX(weatherstationcode)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_WEATHERSTATIONCODE);
	return (result);
}

VCL_STRING
VPFX(weatherstationname)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_WEATHERSTATIONNAME);
	return (result);
}

VCL_STRING
VPFX(mcc)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_MCC);
	return (result);
}

VCL_STRING
VPFX(mnc)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_MNC);
	return (result);
}

VCL_STRING
VPFX(mobilebrand)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_MOBILEBRAND);
	return (result);
}

VCL_STRING
VPFX(elevation)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_ELEVATION);
	return (result);
}

VCL_STRING
VPFX(usagetype)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_USAGETYPE);
	return (result);
}

