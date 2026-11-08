CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDLIBS = -lcryptopp -lUnitTest++

TARGET = vcalc_tests

SOURCES = src/vcalc.cpp \
          src/cml.cpp \
          src/db.cpp \
          src/authenticator.cpp \
          src/calculator.cpp \
          src/logging.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean