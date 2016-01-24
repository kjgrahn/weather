/* Copyright (c) 2016 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "xml.h"

#include <iostream>
#include <algorithm>
#include <cstring>

using xml::ostream;

namespace {

    template <class It>
    void escape(std::ostream& os, It a, It b)
    {
	const char special[] = "&<>";

	while (a!=b) {
	    It c = std::find_first_of(a, b,
				      std::begin(special),
				      std::end(special));
	    os.write(a, c-a);
	    a = c;
	    if (a!=b) {
		switch(*a++) {
		case '&': os << "&amp;"; break;
		case '<': os << "&lt;"; break;
		case '>': os << "&gt;"; break;
		}
	    }
	}
    }
}


ostream::ostream(std::ostream& os)
    : os(os)
{
    os << "<?xml version='1.0' encoding='UTF-8'?>\n";
}

ostream& ostream::operator<< (const xml::elem& e)
{
    if(!stack.empty()) {
	auto& parent = stack.top();
	if (!parent.has_content++) os << '>';
    }
    os << '<' << e.val;
    stack.push(elem{e.val});
    return *this;
}

ostream& ostream::operator<< (const xml::elem_end&)
{
    const elem elem = stack.top();
    stack.pop();
    if (elem.has_content) {
	os << "</" << elem.name << '>';
    }
    else {
	os << "/>";
    }
    return *this;
}

ostream& ostream::operator<< (const xml::attr& attr)
{
    os << ' ' << attr.name << "='" << attr.val << "'";
    return *this;
}

ostream& ostream::content(const char* a, const char* b)
{
    bool begun = stack.top().has_content++;
    if (!begun) os << '>';
    escape(os, a, b);
    return *this;
}

ostream& ostream::operator<< (const char* val)
{
    return content(val, val + std::strlen(val));
}

ostream& ostream::operator<< (const std::string& val)
{
    const char* s = val.data();
    return content(s, s + val.size());
}
