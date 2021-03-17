
CFLAGS = -g -Wall -Wextra -Wshadow

CXX = g++

LIBS = -L C:/custom_C++_libs/libs/glfw -L C:/custom_C++_libs/libs/glew -L C:/custom_C++_libs/libs/assimp -lglew32s -lassimp -lzlibstatic -lIrrXML -lopengl32 -lglu32 -lglfw3 -lgdi32 

INCLUDE = -I C:/custom_C++_libs/includes/sceneObjects -I C:/custom_C++_libs/includes/glm -I C:/custom_C++_libs/includes/glew -I C:/custom_C++_libs/includes/assimp -I C:/custom_C++_libs/includes/glfw


all: main.exe cube.exe

main.exe: main.o sceneObjects.o
	$(CXX) main.o sceneObjects.o $(CFLAGS) $(LIBS) -o main.exe

main.o: main.cpp ..\\..\\sceneObjects.hpp
	g++ main.cpp $(CFLAGS) $(INCLUDE) -c -o main.o

cube.exe: cube.o sceneObjects.o
	$(CXX) cube.o sceneObjects.o $(CFLAGS) $(LIBS) -o cube.exe

cube.o: cube.cpp ..\\..\\sceneObjects.hpp
	g++ cube.cpp $(CFLAGS) $(INCLUDE) -c -o cube.o

sceneObjects.o: ..\\..\\sceneObjects.cpp ..\\..\\sceneObjects.hpp
	g++ ..\\..\\sceneObjects.cpp $(CFLAGS) $(INCLUDE) -c -o sceneObjects.o