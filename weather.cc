/*
 * Copyright (c) 2018 Jörgen Grahn
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
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "sample.h"
#include "post.h"


namespace {

    /**
     * getaddrinfo() hints suitable for this, a HTTP client.
     */
    addrinfo tcp_client()
    {
	addrinfo a = {};
	a.ai_flags = AI_ADDRCONFIG;
	a.ai_family = AF_UNSPEC;
	a.ai_socktype = SOCK_STREAM;
	return a;
    }

    int weather(std::ostream& os,
		const std::string& key,
		const std::string& station)
    {
	const std::string host = "api.trafikinfo.trafikverket.se";

	const addrinfo hints = tcp_client();
	addrinfo* ais;
	const int err = getaddrinfo(host.c_str(), "http", &hints, &ais);
	if(err) {
	    std::cerr << "error: '" << host << "': " << gai_strerror(err) << '\n';
	    return 1;
	}

	os << "foo\n";
	return 0;
    }

    int weather(const std::string& key,
		const std::string& station,
		const std::string& file)
    {
	if(file.empty()) return weather(std::cout, key, station);
	std::ofstream os(file, std::ios::app);
	if(!os) {
	    std::cerr << "cannot open '" << file << "' for writing: "
		      << std::strerror(errno) << '\n';
	    return 1;
	}
	return weather(os, key, station);
    }
}


int main(int argc, char ** argv)
{
    const std::string prog = argv[0];
    const std::string usage = std::string("usage: ")
	+ prog + " -k key station\n"
	"       "
	+ prog + " -k key station file\n"
	"       "
	+ prog + " --help";
    const char optstring[] = "k:";
    const struct option long_options[] = {
	{"help", 0, 0, 'H'},
	{0, 0, 0, 0}
    };

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    std::string key;

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring,
			    &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'k':
	    key = optarg;
	    break;
	case 'H':
	    std::cout << usage << '\n';
	    return 0;
	    break;
	case ':':
	case '?':
	    std::cerr << usage << '\n';
	    return 1;
	    break;
	default:
	    break;
	}
    }

    const std::vector<const char*> args {argv+optind, argv+argc};
    std::string station;
    std::string file;
    switch(args.size()) {
    case 0:
    default:
	std::cerr << usage << '\n';
	return 1;
    case 2:
	file = args[1];
    case 1:
	station = args[0];
    }

    return weather(key, station, file);
}
