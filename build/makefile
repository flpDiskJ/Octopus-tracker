do:
	g++ -c ~/code/c/octapus/src/main.cpp `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -c ~/code/c/octapus/src/headers/tracker.cpp `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -c ~/code/c/octapus/src/headers/util.cpp `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -o octapus main.o tracker.o util.o `sdl2-config --cflags --libs` -lSDL2_ttf -lm -I~/code/c/octapus/src/headers/
	rm main.o tracker.o util.o
stephen:
	g++ -c ~/tracker/src/main.cpp `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -c ~/tracker/src/headers/tracker.cpp `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -o octapus main.o tracker.o `sdl2-config --cflags --libs` -lSDL2_ttf -lm -I~/tracker/src/headers/
	rm main.o tracker.o
debug:
	g++ -c ~/code/c/octapus/src/main.cpp -g `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -c ~/code/c/octapus/src/headers/tracker.cpp -g `sdl2-config --cflags --libs` -lSDL2_ttf
	g++ -o octapus main.o tracker.o -g `sdl2-config --cflags --libs` -lSDL2_ttf -lm -I~/code/c/octapus/src/headers/
	rm main.o tracker.o
