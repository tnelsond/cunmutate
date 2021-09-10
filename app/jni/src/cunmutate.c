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

typedef struct camera{
	SDL_Rect r;
	float scale;
}camera;

struct {
	SDL_FingerID jid;
	float x, y;
}touch;

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture *atlas = NULL;
camera view = {{0, 0, 0, 0}, 1.0f};

void camera_project(camera *view, SDL_Rect *r){
	r->x = (r->x - view->r.x) * view->scale;
	r->y = (r->y - view->r.y) * view->scale;
	r->w *= view->scale;
	r->h *= view->scale;
}


float camera_scale(camera *view, int x){
	return view->scale * x;
}

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
	SDL_Surface *surf;
	SDL_DisplayMode dispmode;
	SDL_Init(SDL_INIT_VIDEO);
	if(SDL_GetCurrentDisplayMode(0, &dispmode) == 0){
		view.r.w = dispmode.w;
		view.r.h = dispmode.h;
	}
	win = SDL_CreateWindow("Unmutate 4.0a", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, view.r.w, view.r.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

void touch_event(SDL_Event *e){
	if(e->type == SDL_MULTIGESTURE){
			view.r.w *= 1 + e->mgesture.dDist;
			SDL_Log("########## %f", e->mgesture.dDist);
		}
	else if(e->type == SDL_FINGERDOWN){
		if(e->tfinger.x > 0.5){
			if(e->tfinger.y > 0.5){
				mc->state |= UP;
			}
			else{
				mc->state |= BREED;
			}
		}
		else{
			touch.jid = e->tfinger.touchId;
			touch.x = e->tfinger.x;
			touch.y = e->tfinger.y;
		}
	}
	else if(e->type == SDL_FINGERUP){
		if(touch.jid == e->tfinger.touchId){
			mc->state &= ~(LEFT|RIGHT);
		}
		if(e->tfinger.x > 0.5){
			mc->state &= ~UP;
		}
	}
	if(e->type == SDL_FINGERMOTION){
		if(touch.jid == e->tfinger.touchId){
			if(e->tfinger.x <= 0.5){
				if(e->tfinger.x > touch.x + 0.02){
					mc->state &= ~LEFT;
					mc->state |= RIGHT;
				}
				else if(e->tfinger.x < touch.x - 0.02){
					mc->state &= ~RIGHT;
					mc->state |= LEFT;
				}
				else{
					mc->state &= ~(LEFT|RIGHT);
				}
			}
		}
	}
}

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
				if(mc->next != mc){
					mendel *m = mc;
					while((m = m->next)){
						if(m->next == mc){
							mendel_free(mc);
							mc = m;
							mc->next = mc->next->next;
							break;
						}
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

		else if(e.type == SDL_MULTIGESTURE || e.type == SDL_FINGERDOWN || e.type == SDL_FINGERUP || e.type == SDL_FINGERMOTION){
			touch_event(&e);
		}
		
		else if(e.type == SDL_WINDOWEVENT){
			if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				view.r.w = e.window.data1;
				view.r.h = e.window.data2;
			}
		}
	}

	view.r.x = mc->x - (view.r.w / 2) / view.scale;
	view.r.y = mc->y - (view.r.h / 2) / view.scale;

	/* Update Mendels */
	int i = 0;
	mendel *m = mc;
	do{
		++i;
		if(m->state != ONGROUND || m->vx != 0)
			mendel_update(m, w);
		m = m->next;
	}while(m != mc);

	/* Draw everything */

	SDL_SetRenderDrawColor(ren, 0x99, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear(ren);

	m = mc->next;
	do{
		mendel_draw(m, &view);
		m = m->next;
	}while(m != mc->next);

	world_draw(w, &view);

	SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0x00);
	SDL_Rect r = {mc->x + mc->w / 2 - 25, mc->y - 50, 50, 50};
	camera_project(&view, &r);
	SDL_RenderCopy(ren, atlas, &rtriangle, &r);

	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF );
	SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0x00);
	sprintf(bufs, "Mendels: %d\n", i);
	text_draw(10, 50, 34, bufs);

	SDL_RenderPresent(ren);
	#ifdef __EMSCRIPTEN__
	if(quit){
		emscripten_cancel_main_loop();
	}
	#endif
}

int main(){
	SDL_Log("Hi");
	init_genemap();
	srand(6);

	SDL_Log("Hi");

	/* Mendels are in a looped singly linked list */
	mc = mendel_init(160, 100, chrom_chain_init(6,
		"AAAAAAAA" gXs "GGGGGGGGGG",
		"AAAAAAA" gXs "GGGGGGGG",
		"AAAAAA" gBs "GGGGGGG",
		"AAAAAAAAAA" gBs "GGGGGG",
		"AAAAAA" gKs gIs "GGGGGGGGGGGGGG",
		"AAAAAA" gaws gIs "GGGGGGG"));
	mc->next = mendel_init(170, 100, chrom_chain_init(6,
		"AAAAAAAAAA" gXs "GGGGGGGGGGGGG",
		"AAAAAA" gYs "GGGGGGGG",
		"AAAAAA" GENEGROWTH  "GGGGGG",
		"AAAAAAA" GENEGROWTH2 "Pizza" GENEGROWTH "GGGGGGGGG",
		"AAAAAAAAAA" gCateyes "lkjaslkdjf" gDs gIs "GGGGGG",
		"AAAAAAA" gkbrs gIs gaws "GGGGGGGGGGG"));
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
