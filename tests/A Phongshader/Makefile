main.exe: main.o sceneObjects.o
	g++ main.o sceneObjects.o -g -Wall -Wextra -Wshadow -lopengl32 -lglu32 -lglew32 -lglfw3 -lgdi32 -o main.exe

main.o: main.cpp ..\\..\\sceneObjects.hpp
	g++ main.cpp -g -Wall -Wextra -Wshadow -c -o main.o

sceneObjects.o: ..\\..\\sceneObjects.cpp ..\\..\\sceneObjects.hpp
	g++ ..\\..\\sceneObjects.cpp -g -Wall -Wextra -Wshadow -c -o sceneObjects.o