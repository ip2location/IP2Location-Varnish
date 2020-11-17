# IP2Location Varnish Module

| | |
|-|-|
| Author:         | IP2Location |
| Date:           | 2020-11-17 |
| Version:        | 1.0.3      |
| Manual section: | 3           |

An Varnish module that enables the website or server admins to find the country, region, city, latitude, longitude, zip code, time zone, ISP, domain name, connection type, area code, weather, mobile network, elevation, usage type by IP address. The module reads the geo location information from **IP2Location BIN data** file. 

This module currently only support Varnish version 6.5.

Required [IP2Location C Library](https://github.com/chrislim2888/IP2Location-C-Library) to work.



# Installation

Before install and use this module, you have to install:

- IP2Location C Library. You can get IP2Location C Library from <https://github.com/chrislim2888/IP2Location-C-Library> 

- libvarnishapi-dev and python-docutils package. Install the require packages using following command:

  ```bash
  apt-get install -y libvarnishapi-dev python-docutils autoconf libtool make
  ```

  

- And of course, the **Varnish**.

Then, clone this repo into your local, and run following commands to install:

```bash
./autogen.sh
./configure
make
make install
```



# Usage

1. Import the IP2Location VMod in `default.vcl`.

   ```
   import ip2location;
   ```

   

2. Initialize and load the database you downloaded from IP2Location like this:

   ```c
   sub vcl_init {
   	ip2location.init_db("/usr/share/ip2location/DB1.BIN", "IP2LOCATION_FILE_IO");
   }
   ```



### Custom Headers

To use custom headers in the backed, add the following context.

```
sub vcl_recv {
    set req.http.X-Country-Code = ip2location.country_short(client.ip);
	set req.http.X-Region = ip2location.region(client.ip);
}
```



### Block Countries

To block visitors from some countries to access your website, you may use the example below.

```
sub vcl_recv {
    set req.http.X-Country-Code = ip2location.country_short(client.ip);
    
    if (req.http.X-Country-Code ~ "(CN|RU)" ) {
    	return (synth(403, "Forbidden"));
    }
}
```





# Functions

The table below listed down the functionality of available functions. For more information on returned data from IP2Location BIN database file, visit here: <https://www.ip2location.com/database/db24-ip-country-region-city-latitude-longitude-zipcode-timezone-isp-domain-netspeed-areacode-weather-mobile-elevation-usagetype>

### init\_db

Initialize and load database. The first argument indicates the path of the database, and the second argument indicates the mode used to open the database. Modes available are: IP2LOCATION_FILE_IO, IP2LOCATION_CACHE_MEMORY and IP2LOCATION_SHARED_MEMORY.

#### Example

```c
ip2location.init_db("PATH_TO_IP2LOCATION_BIN_DATABASE", "IP2LOCATION_FILE_IO");
```

### country\_short

Returns two-letter country code based on ISO 3166.

#### Example

```c
set req.http.X-Country-Code = ip2location.country_short(cleipt.ip);
```

### country\_long

Returns country name based on ISO 3166.

#### Example

```c
set req.http.X-Country-Name = ip2location.country_long(cleipt.ip);
```

### region 

Returns region or state name.

#### Example

```c
set req.http.X-Region = ip2location.region(cleipt.ip);
```

### city 

Returns city name.

#### Example

```c
set req.http.X-City = ip2location.city(cleipt.ip);
```

### isp 

Returns Internet Service Provider or company's name of the IP Address.

#### Example

```c
set req.http.X-ISP = ip2location.isp(cleipt.ip);
```

### latitude 

Returns latitude of the IP Address.

#### Example

```c
set req.http.X-Latitude = ip2location.latitude(cleipt.ip);
```

### longitude 

Returns longitude of the IP Address.

#### Example

```c
set req.http.X-Longitude = ip2location.longitude(cleipt.ip);
```

### domain 

 Returns Internet domain name associated with IP address range.

#### Example

```c
set req.http.X-Domain = ip2location.domain(cleipt.ip);
```

### zipcode 

Returns ZIP/Postal code.

#### Example

```c
set req.http.X-Zipcode = ip2location.zipcode(cleipt.ip);
```

### timezone 

Returns UTC time zone (with DST supported).

#### Example

```c
set req.http.X-Timezone = ip2location.timezone(cleipt.ip);
```

### netspeed 

Returns Internet connection type.

#### Example

```c
set req.http.X-Netspeed = ip2location.netspeed(cleipt.ip);
```

### iddcode 

Returns IDD prefix.

#### Example

```c
set req.http.X-Iddcode = ip2location.iddcode(cleipt.ip);
```

### areacode 

Returns Area Code of the IP Address.

#### Example

```c
set req.http.X-Areacode = ip2location.areacode(cleipt.ip);
```

### weatherstationcode 

Returns code to identify the nearest weather observation station

#### Example

```c
set req.http.X-Weatherstationcode = ip2location.weatherstationcode(cleipt.ip);
```

### weatherstationname 

Returns  name of the nearest weather observation st#### Example

#### Example

```c
set req.http.X-Weatherstationname = ip2location.weatherstationname(cleipt.ip);
```

### mcc 

Returns Mobile Country #### ExampleMCC).

#### Example

```c
set req.http.X-MCC = ip2location.mcc(cleipt.ip);
```

### mnc 

Returns Mobile Ne#### Exampleode (MNC).

#### Example

```c
set req.http.X-MNC = ip2location.mnc(cleipt.ip);
```

### mobilebrand 

 Returns Commercial brand associated wi#### Examplemobile carrier.

#### Example

```c
set req.http.X-Mobilebrand = ip2location.mobilebrand(cleipt.ip);
```

### elevation 

Return elevation in meters (m).

#### Example

```c
set req.http.X-Elevation = ip2location.elevation(cleipt.ip);
```

### usagetype 

 Returns Usage type classification of ISP or company.

#### Example

```c
set req.http.X-Usagetype = ip2location.usagetype(cleipt.ip);
```



# FAQ 

1.  "Where can I get the database to use?" 

   Answer: You can get free IP2Location LITE databases from [https://lite.ip2location.com](https://lite.ip2location.com/), or purchase an IP2Location commercial database from <https://www.ip2location.com/database/ip2location>.

2.  "I can't install the module. Please help me."

   Answer: Once again, before you install the package, please make sure that you have installed autoconf, libtool and make packages first. Those packages are necessary to compile and install this module.

3.  "Why am I getting an error message said that Package varnishapi was not found?"

   Answer: In order to use Varnish vmod, you have to install varnishapi package before hand. Please refer to the [Installation](#installation) section first before install this module.

# Support

Email: [support@ip2location.com](mailto:support@ip2location.com).
URL: [https://www.ip2location.com](https://www.ip2location.com/)
