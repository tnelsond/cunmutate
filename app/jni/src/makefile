CFLAGS = -pedantic -Wall -O2
SDL = `sdl2-config --cflags --libs` -lm -lSDL2_image

cunmutate.elf : cunmutate.c atlas.h dna.h map.h genome.h mendel.h
	gcc $(CFLAGS) $(SDL) cunmutate.c -o cunmutate.elf

test.html : cunmutate.c atlas.h dna.h map.h genome.h mendel.h
	emcc $(CFLAGS) cunmutate.c -O2 -s TOTAL_MEMORY=256MB -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file assets/atlas.png  --preload-file assets/level1.txt -v -o test.html
