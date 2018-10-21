# Makefile
#
# Copyright (c) 2018 Jörgen Grahn
# All rights reserved.

SHELL=/bin/bash
INSTALLBASE=/usr/local

CXXFLAGS=-W -Wall -pedantic -std=c++11 -g -Os
CPPFLAGS=-I/usr/include/libxml2

.PHONY: all
all: weather
all: test/test

weather: weather.o libweather.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -lweather -lxml2

libweather.a: sample.o
libweather.a: post.o
libweather.a: socket.o
libweather.a: week.o
libweather.a: xml.o
libweather.a: files...o
	$(AR) -r $@ $^

# tests

.PHONY:  checkv
check: test/test
	./test/test
checkv: test/test
	valgrind -q ./test/test -v

test/test: test/test.o test/libtest.a libweather.a
	$(CXX) $(CXXFLAGS) -o $@ test/test.o -Ltest/ -ltest -L. -lweather -lxml2

test/test.cc: test/libtest.a
	orchis -o $@ $^

test/libtest.a: test/test_xml.o
test/libtest.a: test/test_week.o
test/libtest.a: test/test_xmlwrite.o
	$(AR) -r $@ $^

test/test_%.o: CPPFLAGS+=-I.

# other

.PHONY: install
install: weather weather.1 weather.5
	install -m555 weather $(INSTALLBASE)/bin/
	install -m644 weather.1 $(INSTALLBASE)/man/man1/
	install -m644 weather.5 $(INSTALLBASE)/man/man5/

.PHONY: tags TAGS
tags: TAGS
TAGS:
	etags *.{h,cc}

.PHONY: clean
clean:
	$(RM) weather
	$(RM) *.o lib*.a
	$(RM) test/*.o test/lib*.a
	$(RM) test/test test/test.cc
	$(RM) -r dep
	$(RM) -r TAGS

love:
	@echo "not war?"

# DO NOT DELETE

$(shell mkdir -p dep/test)
DEPFLAGS=-MT $@ -MMD -MP -MF dep/$*.Td
COMPILE.cc=$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o: %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	@mv dep/$*.{Td,d}

dep/%.d: ;
dep/test/%.d: ;
-include dep/*.d
-include dep/test/*.d
