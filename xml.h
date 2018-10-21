/* -*- c++ -*-
 *
 * Copyright (c) 2016 Jörgen Grahn
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

    struct elem {
	template <class T>
	explicit elem(const T& val) : val(val) {}
	std::string val;
    };

    struct attr {
	template <class Name, class Val>
	attr(const Name& name, const Val& val)
	    : name(name), val(val)
	{}
	std::string name;
	std::string val;
    };

    struct elem_end {};
    constexpr elem_end end;

    /**
     * Stream-like object for generating XML and feeding it into a
     * std::ostream.  Only handles the boring parts:
     *
     * - The XML declaration.
     * - Closing the elements with a </foo> or a <foo/>.
     * - Escaping & to &amp; and so on.
     * - Indenting for readability, if you ask for it
     *   (oops, not implemented yet!)
     *
     * Otherwise it provides a flattened view of the document.
     * To render an element, you feed it, in this order, with:
     * - an elem(name)
     * - zero or more attr(name, val)
     * - a mix of more elem()s and text
     * - an end()
     *
     * The text can be fed as any object which can be put on a
     * std::ostream.
     *
     * Everything is assumed to be utf-8 encoded. There's no
     * transcoding going on, and the resulting XML is marked as utf-8.
     * It's up to the user to keep the document well-formed in this
     * respect.
     */
    class ostream {
    public:
	ostream(std::ostream& os, const std::string& indent = "");
	ostream& operator<< (const elem& e);
	ostream& operator<< (const elem_end& e);
	ostream& operator<< (const attr& attr);
	ostream& operator<< (const char* val);
	ostream& operator<< (const std::string& val);
    private:
	ostream& content(const char* a, const char* b);
	struct elem {
	    explicit elem(const std::string& name)
		: name(name),
		  has_content(false)
	    {}
	    std::string name;
	    bool has_content;
	};
	std::stack<elem> stack;
	std::ostream& os;
    };

    template <class T>
    ostream& operator<< (ostream& xs, const T& val)
    {
	std::ostringstream oss;
	oss << val;
	return xs << oss.str();
    }
}

#endif
