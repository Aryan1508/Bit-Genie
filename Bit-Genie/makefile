EXE := Bit-Genie
SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lpthread -static
CPPFLAGS := -std=c++17 -O3 -DNDEBUG -march=native -Wall -Wextra -static

$(EXE): $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
