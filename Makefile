
CXX = g++

CPPFLAGS = -c -Wall -Wextra -Wshadow -O2

CPPDEBUGFLAGS = -c -Wall -Wextra -Wshadow -g

INCLUDE = -I HEADERS -I C:/custom_C++_libs/includes/glm -I C:/custom_C++_libs/includes/glfw -I C:/custom_C++_libs/includes/glew -I C:/custom_C++_libs/includes/assimp -I C:/custom_C++_libs/includes/stbi

SRC_DIR = SRC
OBJ_DIR = OBJ
BUILD_DIR = BUILD
OBJ_DEBUG_DIR = DEBUG/OBJ
BUILD_DEBUG_DIR = DEBUG/BUILD

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
OBJ_DEBUG_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DEBUG_DIR)/%.o, $(SRC_FILES))

all: $(BUILD_DIR)/libsceneObjects.a $(BUILD_DEBUG_DIR)/libsceneObjects.a

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp HEADERS/sceneObjects.hpp HEADERS/sceneModels.hpp
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(BUILD_DIR)/libsceneObjects.a: $(OBJ_FILES)
	ar rcs -o $@ $^

$(OBJ_DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp HEADERS/sceneObjects.hpp HEADERS/sceneModels.hpp
	$(CXX) $(CPPDEBUGFLAGS) $(INCLUDE) -c -o $@ $<

$(BUILD_DEBUG_DIR)/libsceneObjects.a: $(OBJ_DEBUG_FILES)
	ar rcs -o $@ $^