CC=g++
CFLAGS=-c -Wall
LDFLAGS= 
SOURCES=graphs.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=graph

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@