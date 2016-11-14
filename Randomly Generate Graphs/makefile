CC=g++
CFLAGS=-c -Wall -std=c++11 -g
LDFLAGS= 
SOURCES=triangulation.cpp triangulation.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=graph

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@