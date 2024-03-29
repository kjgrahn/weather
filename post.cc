/*
 * Copyright (c) 2018, 2020 J�rgen Grahn
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "post.h"

#include <algorithm>
#include <sstream>

namespace {
    template <class C>
    std::string join(const C& cont)
    {
	std::ostringstream oss;
	const char* sep = "";
	for(const auto& val: cont) {
	    oss << sep << val;
	    sep = ", ";
	}
	return oss.str();
    }
}


/**
 * Form a post request suitable for asking for weather data, something
 * like this.  Also see RFC 2616.
 *
 *     POST /v1.3/data.xml HTTP/1.1
 *     Host: localhost:4000
 *     User-Agent: curl/7.52.1
 *     Accept: *|*
 *     Content-Type: application/xml
 *     Content-Length: 304
 *
 *     <?xml version='1.0' encoding='utf-8' ?>
 *     <request>
 *       <login authenticationkey='...' />
 *       <query objecttype='WeatherObservation' limit='10'>
 *         <filter>
 *           <any>
 *             <in name='Measurepoint.Id' value='1433, 1434' />
 *             <gt name='Sample' value='$dateadd(-0.01:00)' />
 *           </any>
 *         </filter>
 *       </query>
 *     </request>
 *
 * <in> can be used instead of <eq> to match several values; we do this
 * when asking for data from multiple stations.
 */
std::string post::req(const std::string& host,
		      const std::string& key,
		      const std::vector<std::string>& stations,
		      const Duration& duration)
{
    std::ostringstream body;
    body << "<?xml version='1.0' encoding='utf-8' ?>\n"
	 << "<request>\n"
	 << "  <login authenticationkey='" << key << "' />\n"
	 << "  <query objecttype='WeatherObservation'"
	 << " schemaversion='2'"
	 << " limit='" << stations.size() * duration.samples() << "'>\n"
	 << "  <filter><and>\n";
    if(stations.size()==1) {
	const auto& station = stations.front();
	body << "    <eq name='Measurepoint.Id' value='" << station << "' />\n";
    }
    else {
	body << "    <in name='Measurepoint.Id' value='" << join(stations) << "' />\n";
    }
    body << "    <gt name='Sample' value='$dateadd(" << duration << ")' />\n"
	 << "  </and></filter>\n"
	 << "  </query>\n"
	 << "</request>";

    const auto bodys = body.str();
    const char crlf[] = "\r\n";

    std::ostringstream req;
    req << "POST /v2/data.xml HTTP/1.1" << crlf
	<< "Host: " << host << crlf
	<< "User-Agent: weather/4.0" << crlf
	<< "Accept: */*" << crlf
	<< "Content-Type: application/xml" << crlf
	<< "Content-Length: " << bodys.size() << crlf
	<< "Connection: close" << crlf
	<< crlf
	<< bodys;

    return req.str();
}

namespace {
    std::string::const_iterator crlf(const std::string& s)
    {
	static const char needle[] = "\r\n";
	return std::search(begin(s), end(s),
			   needle, needle + 2);
    }

    std::string::const_iterator crlfcrlf(const std::string& s)
    {
	static const char needle[] = "\r\n\r\n";
	auto i = std::search(begin(s), end(s),
			     needle, needle + 4);
	if(i!=end(s)) i += 4;
	return i;
    }

    bool starts_with(const std::string& haystack,
		     const std::string& needle)
    {
	auto i = std::search(begin(haystack), end(haystack),
			     begin(needle), end(needle));
	return i==begin(haystack);
    }
}

post::Response::Response(const std::string& buf)
    : status_line {begin(buf), crlf(buf)},
      body {crlfcrlf(buf), end(buf)}
{}

bool post::Response::success() const
{
    return starts_with(status_line, "HTTP/1.1 2");
}
