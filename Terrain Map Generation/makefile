CC=g++
CFLAGS=-c -Wall
LDFLAGS= 
SOURCES=gen.cpp ./libraries/lodepng/lodepng.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=gen

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@