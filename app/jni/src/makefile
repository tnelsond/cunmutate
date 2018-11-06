CFLAGS = -ansi -pedantic -Wall -O2
SDL = `sdl2-config --cflags --libs` -lm -lSDL2_image

game.elf : dna.c genome.h
	gcc $(CFLAGS) dna.c -o game.elf

cunmutate.elf : cunmutate.c atlas.h dna.h genome.h
	gcc $(CFLAGS) $(SDL) cunmutate.c -o cunmutate.elf

test.html : cunmutate.c
	emcc $(CFLAGS) cunmutate.c -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file gamegdx.png -o test.html
