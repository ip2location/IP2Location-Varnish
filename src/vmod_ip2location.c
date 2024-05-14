#ifdef __FreeBSD__
#include <stdio.h>
#define gcvt(v, d, c) sprintf(c, "%*g", d, v);
#endif
#include <string.h>

#include <IP2Location.h>

#include "cache/cache.h"

#ifndef VRT_H_INCLUDED
#include "vrt.h"
#endif

void
ip2l_free(VRT_CTX, void *obj)
{
	AN(obj);
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	IP2Location_close((IP2Location *)obj);
}

static const struct vmod_priv_methods ip2l_methods[1] = {{
	.magic = VMOD_PRIV_METHODS_MAGIC,
	.type = "vmod_std_ip2location",
	.fini = ip2l_free
}};

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
	priv->methods = ip2l_methods;
}

static VCL_STRING
copy(VRT_CTX, VCL_STRING s)
{
	VCL_STRING r = WS_Copy(ctx->ws, s, -1);

	if (!r)
		VRT_fail(ctx, "IP2Location: insufficient workspace");
	return (r);
}

static VCL_STRING
convert(VRT_CTX, float f)
{
	char buf[10];

	(void)! gcvt(f, 5, buf);
	return (copy(ctx, buf));
}

#define FUNC(name, render)						\
	VCL_STRING 							\
	vmod_ ## name(VRT_CTX, struct vmod_priv *priv, char * ip)	\
	{								\
		IP2LocationRecord *r;					\
		IP2Location *handle;					\
		VCL_STRING result;					\
									\
		CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);			\
		AN(priv);						\
									\
		handle = priv->priv;					\
		if (!handle) {						\
			VRT_fail(ctx, "IP2Location: uninitialized db");	\
			return (NULL);					\
		}							\
									\
		r = IP2Location_get_ ## name(handle, ip); 		\
		if (r == NULL)						\
			return ("????");				\
		result = render(ctx, r->name);				\
		IP2Location_free_record(r);				\
		return (result);					\
	}

FUNC(country_short,       copy);
FUNC(country_long,        copy);
FUNC(region,              copy);
FUNC(city,                copy);
FUNC(isp,                 copy);
FUNC(domain,              copy);
FUNC(zipcode,             copy);
FUNC(timezone,            copy);
FUNC(netspeed,            copy);
FUNC(iddcode,             copy);
FUNC(areacode,            copy);
FUNC(weatherstationcode,  copy);
FUNC(weatherstationname,  copy);
FUNC(mcc,                 copy);
FUNC(mnc,                 copy);
FUNC(mobilebrand,         copy);
FUNC(usagetype,           copy);

FUNC(latitude,            convert);
FUNC(longitude,           convert);
FUNC(elevation,           convert);
