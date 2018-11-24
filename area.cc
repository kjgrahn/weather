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
#include "area.h"


/**
 * A plot area at the top of the Plot.
 */
Area::Area(const Scale& scale, const Dimensions& dim)
    : offset(0),
      dim(dim),
      scale(scale)
{}

/**
 * A plot area right below another area.
 */
Area::Area(const Area& above, const Scale& scale, unsigned height)
    : offset{above.offset + above.dim.height},
      dim{above.dim.width, height},
      scale{scale}
{}

/**
 * The Plot x coordinate for val, a time in 0--1
 * (monday--sunday).
 */
double Area::xscale(double val) const
{
    return dim.width * val;
}

/**
 * The Plot y coordinate for val, a value in the scale.
 */
double Area::yscale(double val) const
{
    return offset + (scale.max - val) * dim.height / (scale.max - scale.min);
}
