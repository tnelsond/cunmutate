CFLAGS = -pedantic -Wall -O2
SDL = -lSDL3 -lm -lSDL3_image

cunmutate.elf : assets cunmutate.c atlas.h dna.h map.h genome.h mendel.h
	gcc $(CFLAGS) $(SDL) cunmutate.c -o cunmutate.elf
assets : ../../src/main/assets
	rm -rf assets
	cp -R ../../src/main/assets ./

test.html : assets cunmutate.c atlas.h dna.h map.h genome.h mendel.h
	emcc $(CFLAGS) cunmutate.c -O2 -s TOTAL_MEMORY=256MB -s --use-port=sdl3 --preload-file assets/ -v -o test.html
	emrun test.html
