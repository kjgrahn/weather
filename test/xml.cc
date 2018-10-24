/*
 * Copyright (C) 2016 Jörgen Grahn.
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

	assert_xml(ss,
		   "<foo>\n"
		   "  bar\n"
		   "</foo>");
    }

    void string(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	const std::string bar = "bar";
	xs << elem("foo") << bar << end;

	assert_xml(ss,
		   "<foo>\n"
		   "  bar\n"
		   "</foo>");
    }

    void newline_suppression(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	const std::string bar = "bar\nbaz\n";
	xs << elem("foo") << bar << end;

	assert_xml(ss,
		   "<foo>\n"
		   "  bar\n"
		   "baz\n"
		   "</foo>");
    }

    void strings(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo") << "foo" << "bar"
	   << elem("bar") << "baz" << end
	   << end;

	assert_xml(ss,
		   "<foo>\n"
		   "  foobar\n"
		   "  <bar>\n"
		   "    baz\n"
		   "  </bar>\n"
		   "</foo>");
    }

    void object(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	const Hello hello;
	xs << elem("foo") << hello << end;

	assert_xml(ss,
		   "<foo>\n"
		   "  Hello &amp; world!\n"
		   "</foo>");
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
		   "<foo\n"
		   "  bar='baz'>\n"
		   "  <bat\n"
		   "    u='v'/>\n"
		   "</foo>");
    }

    void attribute_quoting(TC)
    {
	std::ostringstream ss;
	xml::ostream xs(ss);
	xs << elem("foo") << attr("bar", "don't") << end;

	assert_xml(ss,
		   "<foo\n"
		   "  bar='don&apos;t'/>");
    }

    namespace indent {

	void simple(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss);
	    xs << elem("foo")
	       <<   "bar"
	       <<   elem("bat") << "fred" << end
	       <<   elem("barney") << end
	       << end;

	    assert_xml(ss,
		       "<foo>\n"
		       "  bar\n"
		       "  <bat>\n"
		       "    fred\n"
		       "  </bat>\n"
		       "  <barney/>\n"
		       "</foo>");
	}

	void zero(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss, 0);
	    xs << elem("foo")
	       <<   "bar"
	       <<   elem("bat") << "fred" << end
	       <<   elem("barney") << end
	       << end;

	    assert_xml(ss,
		       "<foo>\n"
		       "bar\n"
		       "<bat>\n"
		       "fred\n"
		       "</bat>\n"
		       "<barney/>\n"
		       "</foo>");
	}

	void attribute(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss);
	    xs << elem("foo") << attr("bar", "baz")
	       << end;

	    assert_xml(ss,
		       "<foo\n"
		       "  bar='baz'/>");
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
	    for(unsigned i=0; i<1e4; i++) {
		xs << "intro" << elem("elem");
	    }
	    xs << "innermost";
	    for(unsigned i=0; i<1e4; i++) {
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
	    xs << elem("foo") << "a&b" << end;
	    assert_xml(ss,
		       "<foo>\n"
		       "  a&amp;b\n"
		       "</foo>");
	}

	void lt_gt(TC)
	{
	    std::ostringstream ss;
	    xml::ostream xs(ss);
	    xs << elem("foo") << "<>" << end;
	    assert_xml(ss,
		       "<foo>\n"
		       "  &lt;&gt;\n"
		       "</foo>");
	}
    }
}
