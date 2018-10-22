/*
 * Copyright (C) 2016 J�rgen Grahn.
 * All rights reserved.
 */
#include <xml.h>

#include <orchis.h>
#include <fstream>

using orchis::TC;

namespace {
    void assert_xml(std::ostringstream& ss,
		    const char* s)
    {
	std::string ref = "<?xml version='1.0' encoding='UTF-8'?>\n";
	ref += s;
	orchis::assert_eq(ss.str(), ref);
    }

    class Hello {};

    std::ostream& operator<< (std::ostream& os, const Hello&)
    {
	return os << "Hello & world!";
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

    void string(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	const std::string bar = "bar";
	xs << elem("foo") << bar << end;

	assert_xml(ss, "<foo>bar</foo>");
    }

    void object(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	const Hello hello;
	xs << elem("foo") << hello << end;

	assert_xml(ss, "<foo>Hello &amp; world!</foo>");
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

    void attribute_quoting(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo") << attr("bar", "don't") << end;

	assert_xml(ss,
		   "<foo bar='don&apos;t'/>");
    }

    namespace indent {

	void simple(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss, "..");
	    xs << elem("foo")
	       <<   "bar"
	       <<   elem("bat") << "fred" << end
	       <<   elem("barney") << end
	       << end;

	    assert_xml(ss,
		       "<foo>\n"
		       "..bar\n"
		       "..<bat>\n"
		       "....fred\n"
		       "..</bat>\n"
		       "..<barney/>\n"
		       "</foo>");
	}
    }

    namespace huge {

	void flat(TC)
	{
	    std::ofstream nil;
	    xml::ostream xs(nil);
	    xs << elem("foo");
	    for(unsigned i=0; i<1000; i++) {
		xs << elem("bar") << end;
	    }
	    xs << end;
	}

	void nested(TC)
	{
	    std::ofstream nil;
	    xml::ostream xs(nil);
	    xs << elem("foo");
	    for(unsigned i=0; i<1000; i++) {
		xs << elem("bar");
	    }
	    for(unsigned i=0; i<1000; i++) {
		xs << end;
	    }
	    xs << end;
	}

	void gigantic(TC)
	{
	    std::ofstream nil;
	    xml::ostream xs(nil);
	    xs << elem("base");
	    for(unsigned i=0; i<1e6; i++) {
		xs << "intro" << elem("elem");
	    }
	    xs << "innermost";
	    for(unsigned i=0; i<1e6; i++) {
		xs << end << "postscript";
	    }
	    xs << end;
	}
    }

    namespace quoting {

	void ampersand(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss);
	    xs << elem("foo") << " a&b " << end;
	    assert_xml(ss, "<foo> a&amp;b </foo>");
	}

	void lt_gt(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss);
	    xs << elem("foo") << " <> " << end;
	    assert_xml(ss, "<foo> &lt;&gt; </foo>");
	}
    }
}
