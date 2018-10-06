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

weather: weather.o libweather.a
	$(CXX) $(CXXFLAGS) -o $@ $< -L. -lweather -lxml2

libweather.a: sample.o
libweather.a: post.o
libweather.a: socket.o
	$(AR) -r $@ $^

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
	@mv dep/$*.{Td,d}

dep/%.d: ;
-include dep/*.d
