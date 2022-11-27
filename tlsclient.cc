/*
 * Copyright (c) 2022 Jörgen Grahn
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
#include "tlsclient.h"

#include <tls.h>

TLSClient::TLSClient(int fd, const std::string& servername)
    : ctx {tls_client()}
{
    tls_config* cfg = tls_config_new();
    tls_configure(ctx, cfg);
    tls_config_free(cfg);

    err = tls_connect_socket(ctx, fd, servername.c_str());
}

TLSClient::~TLSClient()
{
    tls_free(ctx);
}

TLSClient::operator bool () const
{
    return !err;
}

std::string TLSClient::error() const
{
    return tls_error(ctx);
}

bool TLSClient::write(const char* buf, size_t len)
{
    while (len) {
	ssize_t res = tls_write(ctx, buf, len);
	if (res == TLS_WANT_POLLIN || res == TLS_WANT_POLLOUT) continue;
	if (res == -1) {
	    err = true;
	    return false;
	}
	buf += res; len -= res;
    }
    return true;
}

std::string TLSClient::read()
{
    const size_t LIMIT = 100e3;
    std::string acc;

    while(1) {
	char buf[10000];
	const ssize_t res = tls_read(ctx, buf, sizeof buf);
	if (res == TLS_WANT_POLLIN || res == TLS_WANT_POLLOUT) continue;
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
