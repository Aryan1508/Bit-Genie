EXE := Bit-Genie
SRC := *.cpp generator/game.cpp generator/generator.cpp
CXX := g++

EVALFILE = defaultnet.nn

LFLAGS   := -lpthread
WFLAGS   := -Wall -Wextra
CXXFLAGS := -std=c++17 $(WFLAGS) -O3 -DNDEBUG -flto -march=native

default:
	$(CXX) -DEVALFILE=\"$(EVALFILE)\" $(CXXFLAGS) $(SRC) $(LFLAGS) -o $(EXE) 