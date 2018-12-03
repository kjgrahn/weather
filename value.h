/* -*- c++ -*-
 *
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
#ifndef WEATHER_VALUE_H
#define WEATHER_VALUE_H

#include <iosfwd>

/**
 * A sample value. Since all sampled values (wind, temperature) are
 * numeric, with limited precision, we can store them as fixed-point,
 * with one decimal.
 *
 * No protection against overflow; your values * 10 need to fit in an
 * int.
 *
 * Includes conversion to and from double and strings like "-14.4".
 */
class Value {
public:
    Value() : repr(0) {}
    Value(double n);
    Value(const char* a, const char* b);

    double value() const { return repr/10.0; }

    bool operator== (Value other) const { return repr==other.repr; }
    bool operator!= (Value other) const { return repr!=other.repr; }
    bool operator< (Value other) const  { return repr < other.repr; }
    bool operator! () const { return !repr; }

    std::ostream& put(std::ostream& os) const;

private:
    int repr;
};

inline
std::ostream& operator<< (std::ostream& os, const Value& val)
{
    return val.put(os);
}

#endif
