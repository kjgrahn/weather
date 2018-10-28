/* -*- c++ -*-
 *
 * Copyright (c) 2016 J�rgen Grahn
 * All rights reserved.
 *
 */
#ifndef XML_STREAM_H
#define XML_STREAM_H

#include <iosfwd>
#include <string>
#include <sstream>
#include <stack>

namespace xml {

    /**
     * An element tag, e.g. the foo in <foo>. Used to open an element.
     */
    struct elem {
	template <class T>
	explicit elem(const T& val) : val(val) {}
	std::string val;
    };

    /**
     * A key='value' attribute. You don't have to (should not) quote
     * the value.
     */
    struct attr {
	template <class Name, class Val>
	attr(const Name& name, const Val& val)
	    : name(name), val(val)
	{}
	std::string name;
	std::string val;
    };

    /**
     * The closing of an element.  All elements opened must eventually
     * be closed, if you want to create valid XML.
     */
    struct elem_end {};
    constexpr elem_end end;

    /**
     * Stream-like object for generating XML and feeding it into a
     * std::ostream.  Only handles the boring parts:
     *
     * - The XML declaration.
     * - Closing the elements with a </foo> or a <foo/>.
     * - Escaping & to &amp; and so on.
     * - Indenting for readability.
     * - Ending the document with a newline.
     *
     * Otherwise it provides a flattened view of the document.
     * To render an element, you feed it, in this order, with:
     * - an elem(name)
     * - zero or more attr(name, val)
     * - a mix of more elem()s and text (anything streamable)
     * - an end()
     *
     * The text can be fed as any object which can be put on a
     * std::ostream.
     *
     * Everything is assumed to be utf-8 encoded. There's no
     * transcoding going on, and the resulting XML is by default
     * marked as utf-8.  It's up to the user to keep the document
     * well-formed in this respect.
     */
    class ostream {
    public:
	ostream(std::ostream& os, unsigned indent = 2);
	ostream(std::ostream& os, const std::string& declaration,
		unsigned indent = 2);

	ostream& operator<< (const elem& e);
	ostream& operator<< (const elem_end& e);
	ostream& operator<< (const attr& attr);

	template <class T>
	ostream& operator<< (const T& val);

    private:
	ostream& text();

	void nl_indent() const;
	void flush_indent();

	std::stack<elem> stack;
	std::ostringstream ss;
	std::ostream& os;
	const unsigned indent;
	char prev;
    };


    template <class T>
    ostream& ostream::operator<< (const T& val)
    {
	ss << val;
	if(prev=='t') return *this;
	return text();
    }
}

#endif
