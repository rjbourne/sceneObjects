
CXX = g++

CPPFLAGS = -c -Wall -Wextra -Wshadow

INCLUDE = -I HEADERS -I C:/custom_C++_libs/includes/glm -I C:/custom_C++_libs/includes/glfw -I C:/custom_C++_libs/includes/glew -I C:/custom_C++_libs/includes/assimp -I C:/custom_C++_libs/includes/stbi

SRC_DIR = SRC
OBJ_DIR = OBJ
BUILD_DIR = BUILD

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(INCLUDE) -c -o $@ $<

libsceneObjects.a: $(OBJ_FILES)
	ar rcs -o $(BUILD_DIR)/$@ $^