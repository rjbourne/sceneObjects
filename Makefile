
INCLUDE = -I C:/custom_C++_libs/includes/glm -I C:/custom_C++_libs/includes/glfw -I C:/custom_C++_libs/includes/glew -I C:/custom_C++_libs/includes/assimp

libsceneObjects.a: sceneObjects.o
	ar rcs libsceneObjects.a sceneObjects.o

sceneObjects.o: sceneObjects.cpp sceneObjects.hpp
	g++ sceneObjects.cpp -c -Wall -Wextra -Wshadow $(INCLUDE) -o sceneObjects.o