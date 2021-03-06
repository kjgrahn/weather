/* Copyright (c) 2016 J�rgen Grahn
 * All rights reserved.
 *
 */
#include "xml.h"

#include <iostream>
#include <array>
#include <algorithm>

using xml::ostream;

namespace {

    template <class A>
    void escape(std::ostream& os, const A& special,
		const char* a, const char* b)
    {
	while (a!=b) {
	    auto c = std::find_first_of(a, b,
					begin(special),
					end(special));
	    os.write(a, c-a);
	    a = c;
	    if (a!=b) {
		switch(*a++) {
		case '&': os << "&amp;"; break;
		case '<': os << "&lt;"; break;
		case '>': os << "&gt;"; break;
		case '"': os << "&quot;"; break;
		case '\'': os << "&apos;"; break;
		}
	    }
	}
    }

    void escape(std::ostream& os, const char* a, const char* b)
    {
	constexpr std::array<char, 3> special {'&', '<', '>'};
	escape(os, special, a, b);
    }

    std::ostream& escape(std::ostream& os, const xml::attr& attr)
    {
	constexpr std::array<char, 5> special {'&', '<', '>', '"', '\''};

	auto a = attr.val.data();
	auto b = a + attr.val.size();
	escape(os, special, a, b);
	return os;
    }
}


ostream::ostream(std::ostream& os, unsigned indent)
    : ostream(os,
	      "<?xml version='1.0' encoding='UTF-8'?>",
	      indent)
{}

ostream::ostream(std::ostream& os,
		 const std::string& declaration,
		 unsigned indent)
    : os(os),
      indent(indent),
      prev('?')
{
    os << declaration;
}

/**
 * Open an element.
 */
ostream& ostream::operator<< (const xml::elem& e)
{
    switch(prev) {
    case 'e':
    case 'a':
	os << '>';
	nl_indent();
	break;
    case 't':
	flush_indent();
	break;
    case '.':
    default:
	nl_indent();
	break;
    }
    os << '<' << e.val;
    stack.push(e);
    prev = 'e';
    return *this;
}

/**
 * End an element. There must be an open element.
 */
ostream& ostream::operator<< (const xml::elem_end&)
{
    const elem e = stack.top();
    stack.pop();

    switch(prev) {
    case 'e':
    case 'a':
	os << "/>";
	break;
    case 't':
	flush_indent();
	os << "</" << e.val << '>';
	break;
    case '.':
	nl_indent();
	os << "</" << e.val << '>';
	break;
    }
    if(stack.empty()) os << '\n';
    prev = '.';
    return *this;
}

/**
 * Add a key=value attribute. All attributed must be added immediately
 * after opening the element.
 */
ostream& ostream::operator<< (const xml::attr& attr)
{
    switch(prev) {
    case 'e':
    case 'a':
	nl_indent();
	os << attr.name << "='";
	escape(os, attr) << "'";
	break;
    }
    prev = 'a';
    return *this;
}

/**
 * Add a piece of text.  (The actual text, meanwhile, goes to a
 * std::ostringstream, from whence it's flushed later.)
 */
ostream& ostream::text()
{
    switch(prev) {
    case 'e':
    case 'a':
	os << '>';
	nl_indent();
	break;
    case 't':
	break;
    case '.':
	nl_indent();
	break;
    }
    prev = 't';
    return *this;
}

void ostream::nl_indent() const
{
    static const std::string s = "\n                    ";
    const unsigned nsp = s.size() - 1;
    unsigned n = stack.size() * indent;
    os.write(s.data(), 1 + std::min(nsp, n));
    n -= std::min(nsp, n);

    while(n) {
	os.write(s.data() + 1, std::min(nsp, n));
	n -= std::min(nsp, n);
    }
}

void ostream::flush_indent()
{
    std::string s = ss.str();
    ss.str("");

    if(s.empty() || s.back() != '\n') s += '\n';

    auto a = s.data();
    auto b = a + s.size();
    if(a!=b && b[-1]=='\n') b--;
    escape(os, a, b);
    nl_indent();
}
