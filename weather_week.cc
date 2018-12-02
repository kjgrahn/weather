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
#include <ctime>

#include <getopt.h>

#include "plot.h"
#include "area.h"
#include "week.h"
#include "files...h"


namespace {
    std::time_t now()
    {
	return std::time(nullptr);
    }

    /**
     * Visualize/plot the week from 'files' to 'os'; return an exit
     * code.
     */
    int plot_week(const Week& when, Files& files, std::ostream& os)
    {
	const Area temperature{{-20, +30}, {700, 200}};
	const Area wind{temperature, {0, 20}, 50};
	WeekPlot plot{os, temperature, wind};
	plot.plot(when, files);
	return 0;
    }

    int plot_week(const Week& when, Files& files,
		  const std::string& image_name)
    {
	std::ofstream os(image_name);
	if(!os) {
	    std::cerr << "cannot open '" << image_name << "' for writing: "
		      << std::strerror(errno) << '\n';
	    return 1;
	}
	return plot_week(when, files, os);
    }
}


int main(int argc, char ** argv)
{
    const std::string prog = argv[0];
    const std::string usage = std::string("usage: ")
	+ prog + " [-o image-file] file ...\n"
	"       "
	+ prog + " --help\n"
	"       "
	+ prog + " --version";
    const char optstring[] = "o:";
    const struct option long_options[] = {
	{"help", 0, 0, 'H'},
	{"version", 0, 0, 'V'},
	{0, 0, 0, 0}
    };

    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    std::string image_name;
    const Week when {now()};

    int ch;
    while((ch = getopt_long(argc, argv,
			    optstring,
			    &long_options[0], 0)) != -1) {
	switch(ch) {
	case 'o':
	    image_name = optarg;
	    break;
	case 'H':
	    std::cout << usage << '\n';
	    return 0;
	    break;
	case 'V':
	    std::cout << "weather_week, part of Weather 2.1\n"
		      << "Copyright (c) 2018 Jörgen Grahn\n";
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

    Files files {argv+optind, argv+argc};

    if (image_name.size()) {
	return plot_week(when, files, image_name);
    }

    return plot_week(when, files, std::cout);
}
