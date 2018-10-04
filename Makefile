# Makefile
#
# Copyright (c) 2018 Jörgen Grahn
# All rights reserved.

SHELL=/bin/bash
CXXFLAGS=-W -Wall -pedantic -std=c++11 -g -Os
CPPFLAGS=-I/usr/include/libxml2

weather: weather.o libweather.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -lweather -lxml2

libweather.a: sample.o
libweather.a: post.o
libweather.a: socket.o
	$(AR) -r $@ $^

.PHONY: foo
foo: post
	curl -LO --data-binary @post 'http://api.trafikinfo.trafikverket.se/v1.3/data.xml'

.PHONY: tags TAGS
tags: TAGS
TAGS:
	etags *.{h,cc}

.PHONY: clean
clean:
	$(RM) weather
	$(RM) *.o lib*.a
	$(RM) *.pyc
	$(RM) -r dep

love:
	@echo "not war?"

# DO NOT DELETE

$(shell mkdir -p dep)
DEPFLAGS=-MT $@ -MMD -MP -MF dep/$*.Td
COMPILE.cc=$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o: %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	mv dep/$*.{Td,d}

dep/%.d: ;
-include dep/*.d
