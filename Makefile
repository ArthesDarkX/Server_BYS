CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDLIBS = -lcryptopp
LDLIBS_TEST = -lcryptopp -lUnitTest++

TARGET = vcalc
TARGET_TEST = vcalc_tests

SOURCES = src/vcalc.cpp \
          src/cml.cpp \
          src/server.cpp \
          src/db.cpp \
          src/authenticator.cpp \
          src/calculator.cpp \
          src/logging.cpp

SOURCES_TEST = src/vcalc_test.cpp \
          src/cml.cpp \
          src/db.cpp \
          src/authenticator.cpp \
          src/calculator.cpp \
          src/logging.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

test: $(TARGET_TEST)

$(TARGET_TEST) : $(SOURCES_TEST)
	$(CXX) $(CXXFLAGS) $(SOURCES_TEST) -o $(TARGET_TEST) $(LDLIBS_TEST) 

clean:
	rm -f $(TARGET) $(TARGET_TEST)

.PHONY: all clean test
