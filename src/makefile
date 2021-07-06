EXE := Bit-Genie
SRC := *.cpp
CXX := g++

LFLAGS   := -lpthread
TFLAGS   := -DTUNE -fopenmp
WFLAGS   := -Wall -Wextra
CXXFLAGS := -std=c++17 $(WFLAGS) -O3 -DNDEBUG -flto -march=native
RFLAGS   := -std=c++17 -O3 -DNDEBUG -static 

default:
	$(CXX) $(CXXFLAGS) $(SRC) $(LFLAGS) -o $(EXE)

tune: 
	$(CXX) $(CXXFLAGS) $(SRC) $(LFLAGS) $(TFLAGS) -o $(EXE)

release:
	$(CXX) $(RFLAGS) $(SRC) -mbmi2 -mavx2 -mpopcnt $(LFLAGS) -o $(EXE)-bmi2
	$(CXX) $(RFLAGS) $(SRC) -msse4.2 -msse4.1 -mssse3 -mpopcnt $(LFLAGS) -o $(EXE)-modern 
	$(CXX) $(RFLAGS) $(SRC) -mssse3 -mno-popcnt $(LFLAGS) -o $(EXE)-ssse3 