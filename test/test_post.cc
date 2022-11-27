#include <post.h>

#include <orchis.h>

namespace post {

    void single_station(orchis::TC)
    {
	const char ref[] =
	    "POST /v2/data.xml HTTP/1.1\r\n"
	    "Host: example.org\r\n"
	    "User-Agent: weather/4.0\r\n"
	    "Accept: */*\r\n"
	    "Content-Type: application/xml\r\n"
	    "Content-Length: 313\r\n"
	    "Connection: close\r\n"
	    "\r\n"
	    "<?xml version='1.0' encoding='utf-8' ?>\n"
	    "<request>\n"
	    "  <login authenticationkey='xyzzy' />\n"
	    "  <query objecttype='WeatherObservation' schemaversion='2' limit='48'>\n"
	    "  <filter><and>\n"
	    "    <eq name='Measurepoint.Id' value='4711' />\n"
	    "    <gt name='Sample' value='$dateadd(-0.08:00)' />\n"
	    "  </and></filter>\n"
	    "  </query>\n"
	    "</request>";

	orchis::assert_eq(post::req("example.org", "xyzzy",
				    {"4711"}, Duration{"8h"}),
			  ref);
    }
}
