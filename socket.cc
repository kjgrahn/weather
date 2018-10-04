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
#include "socket.h"

#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


Socket::Socket(int fd)
    : fd(fd),
      err(fd==-1? errno: 0)
{}

Socket::~Socket()
{
    if(!invalid()) close(fd);
}

std::string Socket::error() const
{
    return strerror(err);
}

/**
 * Write all of 'buf', and don't return true unless it's all written
 * successfully.  Blocks, and assumes a decent socket timeout.  Sets
 * err.
 */
bool Socket::write(const void* buf, size_t count)
{
    const ssize_t res = ::write(fd, buf, count);
    if(res==-1 || count - res) {
	err = errno;
	return false;
    }
    return true;
}

/**
 * Read all data from the socket, blocking.  The following can happen:
 * - reads N octets successfully and returns them as a non-empty string
 * - encounters an I/O error, returns "" and error() contains error text
 * - reads unreasonably much data, aborts the reading and returns ""
 * - reads EOF immediately and returns ""
 *
 * The last two would look funny if they happened, wrt diagnostics.
 */
std::string Socket::read()
{
    const size_t LIMIT = 100e3;
    std::string acc;

    while(1) {
	char buf[10000];
	const ssize_t res = ::read(fd, buf, sizeof buf);
	if(res==-1) {
	    err = errno;
	    acc.clear();
	    break;
	}
	else if(res==0) {
	    break;
	}
	else {
	    acc.append(buf, buf + res);
	}
	if(acc.size() > LIMIT) {
	    acc.clear();
	    break;
	}
    }

    return acc;
}
