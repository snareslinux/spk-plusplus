CC := gcc
CXX := g++

CFLAGS = -Wall -Wextra -std=c23 -I./src $(shell pkg-config --cflags libarchive) $(shell pkg-config --cflags openssl)
CXXFLAGS = -Wall -Wextra -std=c++23 -I./src $(shell pkg-config --cflags libarchive) $(shell pkg-config --cflags openssl)

LDLIBS = $(shell pkg-config --libs libarchive) -lcurl $(shell pkg-config --libs openssl)

TARGET = output/spk

SRC_C = $(wildcard src/*.c) \
	$(wildcard src/*/*.c) \
	$(wildcard src/*/*/*.c)

SRC_CPP = $(wildcard src/*.cpp) \
	$(wildcard src/*/*.cpp) \
	$(wildcard src/*/*/*.cpp)

-include .config
CONFIG_DEVELOPMENT_TOOLS ?= n

ifeq ($(CONFIG_DEVELOPMENT_TOOLS),y)
	CFLAGS += -DCONFIG_DEVELOPMENT_TOOLS
	CXXFLAGS += -DCONFIG_DEVELOPMENT_TOOLS
else
	SRC_C := $(filter-out src/development/%,$(SRC_C))
	SRC_CPP := $(filter-out src/development/%,$(SRC_CPP))
endif

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