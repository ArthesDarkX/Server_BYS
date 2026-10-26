
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude -o
LDLIBS = -lcryptopp

TARGET = vcalc

SOURCES = src/vcalc.cpp \
          src/cml.cpp \
          src/db.cpp \
          src/authenticator.cpp \
          src/server.cpp \
          src/calculator.cpp \
          src/logging.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) $(CXXFLAGS) $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean