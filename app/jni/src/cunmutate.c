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
#include <math.h>

#define FPS 60

#define PI 3.1415

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture *atlas = NULL;
SDL_Rect camera = {0, 0, 0, 0};

char bufs[32];

#include "atlas.h"

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
			if(isgraph(s[i])){
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

#include "dna.h"
#include "map.h"
#include "mendel.h"

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

	w = world_load(PATH "level1.txt");

	return 0;
}

mendel *mc = NULL;

int quit = 0;

void eloop(){
	SDL_Event e;
	while( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT )
		{
			quit = 1;
		}
		if(e.type == SDL_KEYDOWN){
			if(e.key.keysym.sym == SDLK_w){
				mc->state |= UP;
			}
			else if(e.key.keysym.sym == SDLK_a){
				mc->state |= LEFT;
			}
			else if(e.key.keysym.sym == SDLK_s){
				mc->state |= DOWN;
			}
			else if(e.key.keysym.sym == SDLK_d){
				mc->state |= RIGHT;
			}
			else if(e.key.keysym.sym == SDLK_j){
				mc->state |= BREED;
			}
			else if(e.key.keysym.sym == SDLK_k){
				mendel *m = mc;
				while((m = m->next)){ /* Need to free killed mendel yet */
					if(m->next == mc){
						mc = m;
						mc->next = mc->next->next;
						break;
					}
				}
			}
			else if(e.key.keysym.sym == SDLK_TAB){
				mc->state &= ~(UP | LEFT | DOWN | RIGHT | BREED);
				mc = mc->next;
			}
		}
		else if(e.type == SDL_KEYUP){
			if(e.key.keysym.sym == SDLK_w){
				mc->state &= ~UP;
			}
			else if(e.key.keysym.sym == SDLK_a){
				mc->state &= ~LEFT;
			}
			else if(e.key.keysym.sym == SDLK_s){
				mc->state &= ~DOWN;
			}
			else if(e.key.keysym.sym == SDLK_d){
				mc->state &= ~RIGHT;
			}
		}
		else if(e.type == SDL_MOUSEMOTION){
			mc->vx += e.motion.xrel;
			mc->vy += e.motion.yrel;
		}
	}

	SDL_SetRenderDrawColor(ren, 0x99, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear(ren);


	int i = 0;
	mendel *m = mc->next;
	do{
		++i;
		mendel_draw(m);
		m = m->next;
	}while(m != mc->next);
	world_draw(w);

	SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0x00);
	SDL_Rect r = {mc->x + mc->w / 2 - 25, mc->y - 50, 50, 50};
	SDL_RenderCopy(ren, atlas, &rtriangle, &r);

	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF );
	SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0x00);
	sprintf(bufs, "Mendels: %d\n", i);
	text_draw(10, 50, 34, bufs);

	/* Update Mendels */
	m = mc;
	do{
		if(m->state != ONGROUND || m->vx != 0)
			mendel_update(m, w);
		m = m->next;
	}while(m != mc);
	

	SDL_RenderPresent(ren);
	#ifdef __EMSCRIPTEN__
	if(quit){
		emscripten_cancel_main_loop();
	}
	#endif
}

int main(){
	init_genemap();
	srand(6);

	mc = mendel_init(100, 100, chrom_chain_init(6, chroma, chromb, chromc, chromd, chrome, chromf));
	mc->next = mendel_init(130, 100, chrom_chain_init(6, chroma, chromb, chromd, chromd, chrome, chromf));
	mc->next->next = mc;
	
	init();

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(eloop, FPS, 1);
	#else
	while(!quit)
	{
		eloop();
		SDL_Delay(16);
	}
	#endif

	return 0;
}
