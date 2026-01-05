CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O2
INCLUDES := -Iinclude

SRC := $(shell find src -name '*.cpp')
TARGET := a.out

.PHONY: all clean run

all: $(TARGET)

$(TARGET): main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

