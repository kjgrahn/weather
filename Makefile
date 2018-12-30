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
all: weather_week
all: test/test

weather: weather.o libweather.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -lweather -lxml2

weather_week: weather_week.o libweek.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -lweek

libweather.a: sample.o
libweather.a: render.o
libweather.a: post.o
libweather.a: socket.o
	$(AR) -r $@ $^

libweek.a: week.o
libweek.a: plot.o
libweek.a: area.o
libweek.a: reckon.o
libweek.a: curves.o
libweek.a: value.o
libweek.a: xml.o
libweek.a: files...o
	$(AR) -r $@ $^

# tests

.PHONY:  checkv
check: test/test
	./test/test
checkv: test/test
	valgrind -q ./test/test -v

test/test: test/test.o test/libtest.a libweather.a libweek.a
	$(CXX) $(CXXFLAGS) -o $@ test/test.o -Ltest/ -ltest -L. -lweather -lweek -lxml2

test/test.cc: test/libtest.a
	orchis -o $@ $^

test/libtest.a: test/test_xml.o
test/libtest.a: test/test_week.o
test/libtest.a: test/test_curves.o
test/libtest.a: test/test_value.o
test/libtest.a: test/test_groups.o
test/libtest.a: test/test_xmlwrite.o
test/libtest.a: test/test_files.o
	$(AR) -r $@ $^

test/test_%.o: CPPFLAGS+=-I.

# other

.PHONY: install
install: weather weather.1 weather.5
	install -m555 weather{,_week} $(INSTALLBASE)/bin/
	install -m644 weather{,_week}.1 $(INSTALLBASE)/man/man1/
	install -m644 weather.5 $(INSTALLBASE)/man/man5/

.PHONY: tags TAGS
tags: TAGS
TAGS:
	etags *.{h,cc}

.PHONY: clean
clean:
	$(RM) weather{,_week}
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
