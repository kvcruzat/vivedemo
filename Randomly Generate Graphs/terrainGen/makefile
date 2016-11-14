CC=g++
CFLAGS=-c -Wall -std=c++11 -g
LDFLAGS= 
SOURCES=terrainGen.cpp utils.cpp DiamondSquare.cpp terrainGen.h utils.h DiamondSquare.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=terrainGen

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@