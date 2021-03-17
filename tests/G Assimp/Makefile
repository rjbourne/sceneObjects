
CFLAGS = -g -Wall -Wextra -Wshadow

CXX = g++

LIBS = -L ..\\..\\BUILD\\ -L C:/custom_C++_libs/libs/glfw -L C:/custom_C++_libs/libs/glew -L C:/custom_C++_libs/libs/assimp -lsceneObjects -lglew32s -lassimp -lzlibstatic -lIrrXML -lopengl32 -lglu32 -lglfw3 -lgdi32 

INCLUDE = -I ..\\..\\HEADERS\\ -I C:/custom_C++_libs/includes/glm -I C:/custom_C++_libs/includes/glew -I C:/custom_C++_libs/includes/assimp -I C:/custom_C++_libs/includes/glfw

all: main.exe cube.exe

main.exe: main.o
	$(CXX) main.o $(CFLAGS) $(LIBS) -o main.exe

main.o: main.cpp
	g++ main.cpp $(CFLAGS) $(INCLUDE) -c -o main.o

cube.exe: cube.o
	$(CXX) cube.o $(CFLAGS) $(LIBS) -o cube.exe

main.o: cube.cpp
	g++ cube.cpp $(CFLAGS) $(INCLUDE) -c -o cube.o
