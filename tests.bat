mingw32-make

cd "tests\\A Phongshader"
mingw32-make
.\main
cd ..

cd "B Camera"
mingw32-make
.\main
cd ..

cd "C Camera"
mingw32-make
.\main
cd ..

cd "D Phongshader"
mingw32-make
.\main
cd ..

cd "E Perlin"
mingw32-make
.\main
cd ..

cd "F Colormap"
mingw32-make
.\main
cd ..

cd "G Assimp"
mingw32-make
.\main
.\cube
cd ..

cd "H Assimp"
mingw32-make
.\main
cd ..

cd "I Ffmpeg"
mingw32-make
.\main
cd ..