EXE = Bit_Genie.exe

SOURCES = $(wildcard src/*.cpp)

OBJECTS = $(SOURCES:%.cpp=%.o)
DEPENDS = $(SOURCES:%.cpp=%.d)

CXXFLAGS += -Wall -Wno-enum-compare -Wcast-qual -Wshadow -std=c++17 -O2
CPPFLAGS += -MMD 

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

-include $(DEPENDS)

re: clean all

.PHONY: all clean re