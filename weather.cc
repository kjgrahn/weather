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
#include <cassert>

#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "sample.h"
#include "post.h"
#include "socket.h"


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

    timeval tv_of(double seconds)
    {
	time_t s = seconds;
	return {s, (seconds-s)*1e6 };
    }

    /**
     * Set the socket(7) SO_RCVTIMEO and SO_SNDTIMEO.
     */
    void timeout(int fd, double seconds)
    {
	const timeval tv = tv_of(seconds);
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
    }

    /**
     * From one or more struct addrinfo, try to socket() and
     * connect().  Returns the fd, or sets errno and returns -1.
     */
    int connect_one(addrinfo* ais)
    {
	for(addrinfo* p=ais; p; p=p->ai_next) {
	    int fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	    if(fd==-1) continue;

	    timeout(fd, 20.0);

	    int err = connect(fd, p->ai_addr, p->ai_addrlen);
	    if(!err) return fd;
	    close(fd);
	}
	return -1;
    }

    /**
     * Fetch the data and print it to 'os'.  Returns an exit code, and
     * may print error messages to stderr.
     */
    int weather(std::ostream& os,
		const std::string& key,
		const std::string& station)
    {
	auto& cerr = std::cerr;
	const std::string host = "api.trafikinfo.trafikverket.se";

	const addrinfo hints = tcp_client();
	addrinfo* ais;
	const int err = getaddrinfo(host.c_str(), "http", &hints, &ais);
	if(err) {
	    cerr << "error: '" << host << "': " << gai_strerror(err) << '\n';
	    return 1;
	}

	Socket fd {connect_one(ais)};
	freeaddrinfo(ais);
	if(fd.invalid()) {
	    cerr << "error: '" << host << "': cannot connect: "
		 << fd.error() << '\n';
	    return 1;
	}

	const auto req = post::req(host, key, station);

	if(!fd.write(req.data(), req.size())) {
	    cerr << "error: request failed: " << fd.error() << '\n';
	    return 1;
	}

	auto s = fd.read();
	if(s.empty()) {
	    cerr << "error: read failure: " << fd.error() << '\n';
	    return 1;
	}

	const post::Response resp {s};
	if(!resp.success()) {
	    cerr << "error: " << resp.status_line << '\n';
	    return 1;
	}

	os << resp.body;

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
