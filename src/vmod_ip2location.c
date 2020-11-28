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

#ifndef VRT_H_INCLUDED
#include "vrt.h"
#endif

void
i2pl_free(void *obj)
{
	AN(obj);
	IP2Location_close((IP2Location *)obj);
}

VCL_VOID
vmod_init_db(VRT_CTX, struct vmod_priv *priv, char *filename, char *memtype)
{
	enum IP2Location_lookup_mode mtype;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(memtype);
	AN(priv);

	if (strcmp(memtype, "IP2LOCATION_FILE_IO") == 0)
		mtype = IP2LOCATION_FILE_IO;
	else if (strcmp(memtype, "IP2LOCATION_CACHE_MEMORY") == 0)
		mtype = IP2LOCATION_CACHE_MEMORY;
	else if (strcmp(memtype, "IP2LOCATION_SHARED_MEMORY") == 0)
		mtype = IP2LOCATION_SHARED_MEMORY;
	else {
		VRT_fail(ctx, "IP2Location: invalid memtype (%s)", memtype);
		return;
	}

	if (priv->priv != NULL)
		IP2Location_close((IP2Location *)priv->priv);;

	IP2Location *IP2LocationObj = IP2Location_open(filename);
	if (!IP2LocationObj) {
		VRT_fail(ctx, "IP2Location: can't open database (%s)", filename);
		return;
	}

	IP2Location_set_lookup_mode(IP2LocationObj, mtype);

	priv->priv = IP2LocationObj;
	priv->free = i2pl_free;
}

// Use this function to query result, and then extract the field based on user selection
IP2LocationRecord *
query_all(VRT_CTX, struct vmod_priv *priv, char * ip)
{
	IP2Location *handle;

	printf("The client IP is %s.\n", ip);
	
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (priv->priv == NULL)
		return (NULL);

	handle = priv->priv;
	return (IP2Location_get_all(handle, ip));
}

#define FUNC(lower)						\
	VCL_STRING 							\
	vmod_ ## lower(VRT_CTX, struct vmod_priv *priv, char * ip)	\
	{								\
		IP2LocationRecord *r;					\
		char *result;						\
									\
		CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);			\
									\
		r = query_all(ctx, priv, ip); 				\
		if (r == NULL)						\
			return ("????");				\
		result = WS_Copy(ctx->ws, r->lower, -1);		\
		IP2Location_free_record(r);				\
		return (result);					\
	}

FUNC(country_short);
FUNC(country_long );
FUNC(region);
FUNC(city);
FUNC(isp);
FUNC(domain);
FUNC(zipcode);
FUNC(timezone);
FUNC(netspeed);
FUNC(iddcode);
FUNC(areacode);
FUNC(weatherstationcode);
FUNC(weatherstationname);
FUNC(mcc);
FUNC(mnc);
FUNC(mobilebrand);
FUNC(usagetype);

/* same as FUNC, but with the gcvt() call */
#define FUNC_GCVT(lower)						\
	VCL_STRING 							\
	vmod_ ## lower(VRT_CTX, struct vmod_priv *priv, char * ip)	\
	{								\
		IP2LocationRecord *r;					\
		char *result;						\
		char buf[10];						\
									\
		CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);			\
									\
		r = query_all(ctx, priv, ip); 				\
		if (r == NULL)						\
			return ("????");				\
									\
		gcvt(r->lower, 5, buf);					\
		result = WS_Copy(ctx->ws, buf, -1);			\
		IP2Location_free_record(r);				\
		return (result);					\
	}

FUNC_GCVT(latitude);
FUNC_GCVT(longitude);
FUNC_GCVT(elevation);
