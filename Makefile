libsceneObjects.a: sceneObjects.o
	ar rcs libsceneObjects.a sceneObjects.o

sceneObjects.o: sceneObjects.cpp sceneObjects.hpp
	g++ sceneObjects.cpp -c -Wall -Wextra -Wshadow -o sceneObjects.o