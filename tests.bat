mingw32-make

cd "tests\\A Phongshader"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "B Camera"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "C Camera"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "D Phongshader"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "E Perlin"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "F Colormap"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "Ga Assimp"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "Gb Assimp"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "H Assimp"
del main.exe main.o
mingw32-make
.\main
cd ..

cd "I Ffmpeg"
del main.exe main.o
mingw32-make
.\main
cd ..