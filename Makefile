CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -I./src

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -I./src

LDLIBS = -larchive -lcurl

TARGET = output/spk

SRC_C = $(wildcard src/*.c) \
	$(wildcard src/*/*.c) \
	$(wildcard src/*/*/*.c)

SRC_CPP = $(wildcard src/*.cpp) \
	$(wildcard src/*/*.cpp) \
	$(wildcard src/*/*/*.cpp)

OBJ_C = $(SRC_C:.c=.c.o)
OBJ_CPP = $(SRC_CPP:.cpp=.cpp.o)
OBJ = $(OBJ_C) $(OBJ_CPP)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p output
	$(CXX) $(OBJ) -o $@ $(LDLIBS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf output/*
	find src -name '*.o' -delete

.PHONY: all clean