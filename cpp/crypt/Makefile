CC = g++
CFLAGS = -c -Wall -Wextra -std=c++11 -O3 -march=native
LDFLAGS =
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = crypt.exe

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -s -lAdvapi32

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

