#ifdef __ANDROID__
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define FPS 60

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture *atlas = NULL;
SDL_Rect camera = {0, 0, 0, 0};

#include "atlas.h"
#include "dna.h"

typedef struct mendel{
	int r, g, b;
	int x, y, w, h;
	int stripes;
}mendel;

int init(){
	SDL_DisplayMode dispmode;
	SDL_Surface *surf;
	SDL_Init(SDL_INIT_VIDEO);
	if(SDL_GetCurrentDisplayMode(0, &dispmode) == 0){
		camera.w = dispmode.w/4;
		camera.h = dispmode.h/4;
	}
	win = SDL_CreateWindow("Shapes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.w, camera.h, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
	surf = IMG_Load("atlas.png");
	if(!surf){
		SDL_Log("######## Image won't load!");
	}
	atlas = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);
	return 0;
}

void draw(mendel *m){
	SDL_Rect r;
	r.x = m->x; r.y = m->y; r.w=m->w; r.h=m->w;
	SDL_SetTextureColorMod(atlas, m->r, m->g, m->b);		
	SDL_RenderCopyEx(ren, atlas, &rbodycircle, &r, 0, NULL, 0);

	if(m->stripes){
		SDL_SetTextureColorMod(atlas, 0x77, 0x22, 0x00);		
		SDL_RenderCopyEx(ren, atlas, &rbodystripes, &r, 0, NULL, 0);
	}

	r.x = m->x + m->w*2/5;
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
	SDL_RenderCopyEx(ren, atlas, &reyewhite, &r, 0, NULL, 0);

	r.x = m->x + m->w / 4;
	r.y = m->y + m->h * 0.9;
	r.w = m->h/3;
	r.h = m->w;
	SDL_SetTextureColorMod(atlas, 0xdd, 0x33, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rleg, &r, m->x, NULL, 0);
	SDL_SetTextureColorMod(atlas, 0x77, 0x22, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rlegspots, &r, m->x, NULL, 0);
	r.x = m->x + m->w / 2;
	r.y = m->y + m->h * 0.9;
	r.w = m->h/3;
	r.h = m->w;
	SDL_SetTextureColorMod(atlas, 0xdd, 0x33, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rleg2, &r, m->x*2 + 45, NULL, 0);
}

int quit = 0;

mendel creature = {0x0, 0xFF, 0x55, 100, 100, 300, 300, 1};

void eloop(){
	SDL_Rect tr = {50, 40, 200, 200};
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


	return 0;
}
