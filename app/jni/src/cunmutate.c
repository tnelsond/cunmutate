#ifdef __ANDROID__
#include "SDL.h"
#define PATH ""
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#define PATH "assets/"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <stdlib.h>

#define FPS 60

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture *atlas = NULL;
SDL_Rect camera = {0, 0, 0, 0};

#include "atlas.h"
#include "dna.h"
#include "map.h"

typedef struct mendel{
	int r, g, b;
	int x, y, w, h;
	int stripes;
}mendel;

level *map;
int block_size = 64;

int init(){
	SDL_DisplayMode dispmode;
	SDL_Surface *surf;
	SDL_Init(SDL_INIT_VIDEO);
	if(SDL_GetCurrentDisplayMode(0, &dispmode) == 0){
		camera.w = dispmode.w;
		camera.h = dispmode.h;
	}
	win = SDL_CreateWindow("Unmutate 2.4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.w, camera.h, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
	surf = IMG_Load(PATH "atlas.png");
	if(!surf){
		SDL_Log("######## Image won't load!");
	}
	atlas = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);

	map = level_load(PATH "level1.txt");

	return 0;
}

void text_draw(int x, int y, int height, char *s){
	SDL_Rect r;
	SDL_Rect rt;
	int i = 0;
	int col = 0;
	int row = 0;
	rt.w = rtext.w;
	rt.h = rtext.h;
	r.y = y;
	r.h = height;
	r.w = (int)((height + 0.0)/rtext.h * rtext.w);
	while(s[i]){
		if(s[i] == '\n'){
			++row;
			col = 0;
			r.y = y + row * r.h;
		}
		else{
			if(isalpha(s[i])){
				r.x = x + col * r.w;
				rt.x = ((s[i] - 33) % 32) * rt.w + rtext.x;
				rt.y = ((s[i] - 33) / 32) * rt.h + rtext.y;
				SDL_RenderCopy(ren, atlas, &rt, &r);
			}
			++col;
		}
		++i;
	}
}

void level_draw(level *m){
	int i, j;
	SDL_Rect r;


	SDL_SetTextureColorMod(atlas, 0x33, 0x11, 0x00);		

	r.w = r.h = block_size;

	for(j=0; j<m->h; ++j){
		for(i=0; i<m->w; ++i){
			if(m->map[j][i] != AIR){
				r.x = i * block_size;
				r.y = j * block_size;
				SDL_RenderCopy(ren, atlas, &rblock, &r);
			}
		}
	}

	SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0x00);		
	text_draw(100, 20, 36, "Unmutate 2.4");
	SDL_SetTextureColorMod(atlas, 0x00, 0x00, 0x00);		
	text_draw(103, 23, 36, "Unmutate 2.4");
}

void draw(mendel *m){
	SDL_Rect r;
	r.x = m->x; r.y = m->y; r.w=m->w; r.h=m->w;
	SDL_SetTextureColorMod(atlas, m->r, m->g, m->b);		
	SDL_RenderCopyEx(ren, atlas, &rbodycircle, &r, 0, NULL, 0);

	if(m->stripes){
		SDL_SetTextureColorMod(atlas, 0x00, 0x88, 0x44);		
		SDL_RenderCopyEx(ren, atlas, &rbodystripes, &r, 0, NULL, 0);
	}

	r.x = m->x + m->w/2 - m->w/6;
	r.y = m->y - m->h/4;
	r.w=m->w/3;
	r.h=m->h/3;
	SDL_SetTextureColorMod(atlas, 0x77, 0x77, 0x55);		
	SDL_RenderCopyEx(ren, atlas, &rhorn, &r, 0, NULL, 0);

	r.x = m->x; r.y = m->y; r.w=m->w; r.h=m->h;
	r.x = m->x + m->w *3/10;
	r.y = m->y + m->h *3/10;
	r.w = m->w * .4;
	r.h = m->h *.4;
	SDL_SetTextureColorMod(atlas, 0x00, 0xFF, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &reye, &r, 0, NULL, 0);

	r.x -= r.w / 4.2;
	r.y -= r.h / 4.2;
	r.w *= 1.49;
	r.h *= 1.49;
	SDL_SetTextureColorMod(atlas, 0xff, 0xff, 0xff);		
	SDL_RenderCopyEx(ren, atlas, &reyewhitered, &r, 0, NULL, 0);

	r.x = m->x + m->w / 4;
	r.y = m->y + m->h * 0.9;
	r.w = m->h/3;
	r.h = m->w;
	SDL_SetTextureColorMod(atlas, m->r, m->g, m->b);		
	SDL_RenderCopyEx(ren, atlas, &rleg, &r, m->x, NULL, 0);
	SDL_SetTextureColorMod(atlas, 0x77, 0x22, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rlegstripes, &r, m->x, NULL, 0);
	r.x = m->x + m->w / 2;
	r.y = m->y + m->h * 0.9;
	r.w = m->h/3;
	r.h = m->w;
	SDL_SetTextureColorMod(atlas, 0xdd, 0x33, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rleg2, &r, m->x*2 + 45, NULL, 0);
}

int quit = 0;

mendel creature = {0x0, 0xFF, 0x55, 100, 100, 60, 60, 1};
mendel creature2 = {0x44, 0x33, 0x99, 200, 100, 155, 155, 0};

void eloop(){
	SDL_Event e;
	while( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT )
		{
			quit = 1;
		}
		else if(e.type == SDL_MOUSEMOTION){
			creature.x += e.motion.xrel;
			creature.y += e.motion.yrel;
		}
	}
	SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear(ren);

	draw(&creature);
	draw(&creature2);
	level_draw(map);

	SDL_RenderPresent(ren);
	#ifdef __EMSCRIPTEN__
	if(quit){
		emscripten_cancel_main_loop();
	}
	#endif
}

int main(){
	int i;
	chrom one;
	chrom two;
	chrom *three;
	chrom *four;
	chrom *five;
	chrom *six;
	init_genes();
	srand(16);
	init_chrom(&one, chroma);
	init_chrom(&two, chromb);
	three = crossover(&one, &two);
	four = crossover(&one, &two);
	five = crossover(three, four);
	six = crossover(four, &one);
	hash_chrom(&two);
	hash_chrom(four);

	if(genes[0].count >= 2){
		creature.r = 0xFF;
	}
	else if(genes[0].count == 1){
		creature.r = 0x88;
	}
	else{
		creature.r = 0;
	}
	if(genes[1].count == 1){
		creature.g/= 2;
		creature.r/= 2;
		creature.b/= 2;
	}
	else if(genes[1].count >= 2){
		creature.g/= 4;
		creature.r/= 4;
		creature.b/= 4;
	}

	for(i=0; i<GENE_NUM; ++i){
		SDL_Log("\n\n\n################### genes[%d]: %d %d\n", i, genes[i].count, genes[i].hash);
	}

	SDL_Log("\n\n\n################### Hello\n\n\n");
	init();

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(eloop, FPS, 1);
	#else
	while(!quit)
	{
		eloop();
		SDL_Delay(40);
	}
	#endif

	free_chrom(three);
	free_chrom(four);
	free_chrom(five);
	free_chrom(six);

	return 0;
}
