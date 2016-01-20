# Makefile
#
# Copyright (c) 2013 Jörgen Grahn
# All rights reserved.

SHELL=/bin/bash

all: libxml.a
all: test/test

libxml.a: xml.o
	$(AR) -r $@ $^

CFLAGS=-W -Wall -pedantic -ansi -g -Os
CXXFLAGS=-W -Wall -pedantic -std=c++11 -g -Os

.PHONY: check checkv
check: test/test
	./test/test
checkv: test/test
	valgrind -q ./test/test -v

test/libtest.a: test/xml.o
	$(AR) -r $@ $^

test/%.o: CPPFLAGS+=-I.

test/test.cc: test/libtest.a
	testicle -o $@ $^

test/test: test/test.o test/libtest.a libxml.a
	$(CXX) $(CXXFLAGS) -o $@ test/test.o -Ltest/ -ltest -L. -lxml

.PHONY: tags TAGS
tags: TAGS
TAGS:
	etags {,test/}*.{h,cc}

.PHONY: depend
depend:
	makedepend -- $(CPPFLAGS) -- -Y -I. *.cc test/*.cc

.PHONY: clean
clean:
	$(RM) *.o lib*.a
	$(RM) test/test test/test.cc test/*.o test/lib*.a
	$(RM) Makefile.bak

love:
	@echo "not war?"

# DO NOT DELETE

xml.o: xml.h
test/xml.o: xml.h
