/*
 * Copyright (C) 2016 Jörgen Grahn.
 * All rights reserved.
 */
#include <xml.h>

#include <orchis.h>

using orchis::TC;

namespace {
    void assert_xml(std::ostringstream& ss,
		    const char* s)
    {
	std::string ref = "<?xml version='1.0' encoding='UTF-8'?>\n";
	ref += s;
	orchis::assert_eq(ss.str(), ref);
    }
}

namespace stream {

    using xml::elem;
    using xml::attr;
    using xml::end;

    void basic(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo") << "bar" << end;

	assert_xml(ss, "<foo>bar</foo>");
    }

    void empty(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo") << end;

	assert_xml(ss, "<foo/>");
    }

    void attributes(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo") << attr("bar", "baz")
	   <<   elem("bat") << attr("u", "v") << end
	   << end;

	assert_xml(ss,
		   "<foo bar='baz'>"
		   "<bat u='v'/>"
		   "</foo>");
    }

    void flat(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo");
	for(unsigned i=0; i<1000; i++) {
	    xs << elem("bar") << end;
	}
	xs << end;
    }
}
