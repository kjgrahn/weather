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
	$(AR) -r $@ $^

weather.o: sample.h
sample.o: sample.h


.PHONY: foo
foo: post
	curl -LO --data-binary @post 'http://api.trafikinfo.trafikverket.se/v1.3/data.xml'
