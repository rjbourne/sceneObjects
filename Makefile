
CXX = g++

CPPFLAGS = -c -Wall -Wextra -Wshadow -O2

CPPDEBUGFLAGS = -c -Wall -Wextra -Wshadow -g

INCLUDE = -I HEADERS -I C:/custom_C++_libs/includes/glm -I C:/custom_C++_libs/includes/glfw -I C:/custom_C++_libs/includes/glew -I C:/custom_C++_libs/includes/assimp -I C:/custom_C++_libs/includes/stbi

SRC_DIR = SRC
HEADER_DIR = HEADERS
OBJ_DIR = RELEASE\\OBJ
BUILD_DIR = RELEASE\\BUILD
OBJ_DEBUG_DIR = DEBUG\\OBJ
BUILD_DEBUG_DIR = DEBUG\\BUILD

HEADER_FILES = $(wildcard $(HEADER_DIR)/*.hpp)
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
OBJ_DEBUG_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DEBUG_DIR)/%.o, $(SRC_FILES))

all: $(BUILD_DIR)/libsceneObjects.a $(BUILD_DEBUG_DIR)/libsceneObjects.a

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp HEADERS/sceneObjects.hpp HEADERS/sceneModels.hpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

$(BUILD_DIR)/libsceneObjects.a: $(OBJ_FILES) | $(BUILD_DIR)
	ar rcs -o $@ $^

$(OBJ_DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp HEADERS/sceneObjects.hpp HEADERS/sceneModels.hpp | $(OBJ_DEBUG_DIR)
	$(CXX) $(CPPDEBUGFLAGS) $(INCLUDE) -c -o $@ $<

$(BUILD_DEBUG_DIR)/libsceneObjects.a: $(OBJ_DEBUG_FILES) | $(BUILD_DEBUG_DIR)
	ar rcs -o $@ $^

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(OBJ_DEBUG_DIR):
	mkdir $(OBJ_DEBUG_DIR)

$(BUILD_DEBUG_DIR):
	mkdir $(BUILD_DEBUG_DIR)