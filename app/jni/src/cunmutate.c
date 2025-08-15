#ifdef __ANDROID__
#include "SDL.h"
#define PATH ""
#else
#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#define PATH "assets/"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include <stdlib.h>
#include <math.h>
#include <ctype.h>

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

void camera_project(camera *view, SDL_FRect *r){
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
	SDL_FRect r;
	SDL_FRect rt;
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
				SDL_RenderTexture(ren, atlas, &rt, &r);
			}
			++col;
		}
		++i;
	}
}

#include "dna.h"
#include "map.h"
#include "mendel.h"

mendel *mc = NULL;
int quit = 0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv){
	SDL_Surface *surf;
	SDL_Init(SDL_INIT_VIDEO);
	view.r.w = 400;
	view.r.h = 400;
	if (!SDL_CreateWindowAndRenderer("cunmutate", view.r.w, view.r.h, SDL_WINDOW_RESIZABLE, &win, &ren)) {
		SDL_Log("Couldn't create screen: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	//Enable VSync
	if(!SDL_SetRenderVSync(ren, 1 ))
	{
		SDL_Log( "Could not enable VSync! SDL error: %s\n", SDL_GetError() );
	}

	//SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
	surf = IMG_Load(PATH "atlas.png");
	if(!surf){
		SDL_Log("######## Image won't load!");
		return SDL_APP_FAILURE;
	}
	atlas = SDL_CreateTextureFromSurface(ren, surf);
	SDL_SetTextureScaleMode(atlas, SDL_SCALEMODE_NEAREST);
	SDL_DestroySurface(surf);

	SDL_Log("GENEMAP: ");
	init_genemap();
	srand(6);

	SDL_Log("Generating Genetic Data");

	/* Mendels are in a looped singly linked list */
	mc = mendel_init(160, 100, chrom_chain_init(6,
		"AAAAAAAA" gXs "randomgarbage" gPigments "GGGGGGGGGG",
		"AAAAAAA" gXs "GGGGGGGG",
		"AAAAAA" gBs "Biggerisbetter" gGrowths "GGGGGGG",
		"AAAAAAAAAA" gBs "GGGGGG",
		"AAAAAA" gKs gIs "GGGGGGGGGGGGGG",
		"AAAAAA" gaws gIs "GGGGGGG"));
	mc->next = mendel_init(170, 100, chrom_chain_init(6,
		"AAAAAAAAAA" gXs "randomgarbage" gPigments "GGGGGGGGGGGGG",
		"AAAAAA" gYs "GGGGGGGG",
		"AAAAAA" gGrowths "suesdaybongphon" GENEPBs "GGGGGG",
		"AAAAAAA" gGrowth2s "Pizza" gGrowths "GGGGGGGGG",
		"AAAAAAAAAA" gCateyes "lkjaslkdjf" gDs gIs "GGGGGG",
		"AAAAAAA" gkbrs gIs gaws "GGGGGGGGGGG"));
	mc->next->next = mc;
	

	w = world_load(PATH "level1.txt");

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
	SDL_DestroyWindow(win);
	win = NULL;
}

void touch_event(SDL_Event *e){
	/*
	if(e->type == SDL_MULTIGESTURE){
			view.r.w *= 1 + e->mgesture.dDist;
			SDL_Log("########## %f", e->mgesture.dDist);
		}
	*/
	if(e->type == SDL_EVENT_FINGER_DOWN){
		if(e->tfinger.x > 0.5){
			if(e->tfinger.y > 0.5){
				mc->state |= UP;
			}
			else{
				mc->state |= BREED;
			}
		}
		else{
			touch.jid = e->tfinger.touchID;
			touch.x = e->tfinger.x;
			touch.y = e->tfinger.y;
		}
	}
	else if(e->type == SDL_EVENT_FINGER_UP){
		if(touch.jid == e->tfinger.touchID){
			mc->state &= ~(LEFT|RIGHT);
		}
		if(e->tfinger.x > 0.5){
			mc->state &= ~UP;
		}
	}
	if(e->type == SDL_EVENT_FINGER_MOTION){
		if(touch.jid == e->tfinger.touchID){
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

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *e){
	if( e->type == SDL_EVENT_QUIT )
	{
		quit = 1;
		return SDL_APP_FAILURE;
	}
	if(e->type == SDL_EVENT_KEY_DOWN){
		if(e->key.key == SDLK_W){
			mc->state |= UP;
		}
		else if(e->key.key == SDLK_A){
			mc->state |= LEFT;
		}
		else if(e->key.key == SDLK_S){
			mc->state |= DOWN;
		}
		else if(e->key.key == SDLK_D){
			mc->state |= RIGHT;
		}
		else if(e->key.key == SDLK_J){
			mc->state |= BREED;
		}
		else if(e->key.key == SDLK_K){
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
		else if(e->key.key == SDLK_TAB){
			mc->state &= ~(UP | LEFT | DOWN | RIGHT | BREED);
			mc = mc->next;
		}
	}
	else if(e->type == SDL_EVENT_KEY_UP){
		if(e->key.key == SDLK_W){
			mc->state &= ~UP;
		}
		else if(e->key.key == SDLK_A){
			mc->state &= ~LEFT;
		}
		else if(e->key.key == SDLK_S){
			mc->state &= ~DOWN;
		}
		else if(e->key.key == SDLK_D){
			mc->state &= ~RIGHT;
		}
	}

	/*else if(e.type == SDL_MULTIGESTURE || e.type == SDL_EVENT_FINGER_DOWN || e.type == SDL_EVENT_FINGER_UP || e.type == SDL_EVENT_FINGER_MOTION){
		touch_event(&e);
	}*/
	
	/*
	else if(e.type == SDL_WindowEvent){
		if(e.window.event == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED){
			view.r.w = e.window.data1;
			view.r.h = e.window.data2;
		}
	}
	*/
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate){

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
	SDL_FRect r = {mc->x + mc->w / 2 - 25, mc->y - 50, 50, 50};
	camera_project(&view, &r);
	SDL_RenderTexture(ren, atlas, &rtriangle, &r);

	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF );
	SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0x00);
	sprintf(bufs, "Mendels: %d\n", i);
	text_draw(10, 50, 34, bufs);

	SDL_RenderPresent(ren);

	/*SDL_Delay(16);*/
	return SDL_APP_CONTINUE;
}
