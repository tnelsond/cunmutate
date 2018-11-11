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

#include "atlas.h"
#include "dna.h"
#include "map.h"

typedef struct mendel{
	int r, g, b;
	int x, y, w, h;
	int stripes;
	unsigned int state;
	int speed;
	int jump;
	float vx, vy;
}mendel;

/* INPUT FLAGS */
#define UP 1
#define LEFT 2
#define DOWN 4
#define RIGHT 8
#define ONGROUND 16

world *w;

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

void world_draw(world *w){
	int i, j;
	SDL_Rect r;


	SDL_SetTextureColorMod(atlas, 0x33, 0x11, 0x00);		

	r.w = r.h = w->tilesize;

	for(j=0; j<w->h; ++j){
		for(i=0; i<w->w; ++i){
			if(w->map[j][i] != AIR){
				r.x = i * w->tilesize;
				r.y = j * w->tilesize;
				SDL_RenderCopy(ren, atlas, &rblock, &r);
			}
		}
	}

	SDL_SetTextureColorMod(atlas, 0x00, 0x00, 0x00);		
	text_draw(103, 23, 36, "Unmutate 2.4");
}

void mendel_draw(mendel *m){
	SDL_Rect r;

	int dy;

	r.x = m->x + m->w / 4;
	r.y = m->y + m->w * 0.9;
	r.w = m->w/3;
	r.h = m->h - m->w * 0.9;
	float angle = (m->x*45/(m->h - m->w)) % 90;
	if(angle > 45){
		angle = 90 - angle;
	}

	dy = r.h - cos(angle * PI / 180) * r.h;
	r.y += dy;

	SDL_Point center = {r.w / 2, r.h / 10};
	SDL_SetTextureColorMod(atlas, m->r, m->g, m->b);		
	SDL_RenderCopyEx(ren, atlas, &rleg, &r, angle, &center, 0);
	SDL_SetTextureColorMod(atlas, 0x77, 0x22, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rlegstripes, &r, angle, &center, 0);
	r.x = m->x + m->w / 2;
	SDL_SetTextureColorMod(atlas, m->r, m->g, m->b);		
	SDL_RenderCopyEx(ren, atlas, &rleg, &r, -angle, &center, 0);
	SDL_SetTextureColorMod(atlas, 0x77, 0x22, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &rlegstripes, &r, -angle, &center, 0);

	r.x = m->x;
	r.y = m->y + dy;
	r.w=m->w;
	r.h=m->w;
	SDL_SetTextureColorMod(atlas, m->r, m->g, m->b);		
	SDL_RenderCopy(ren, atlas, &rbodycircle, &r);

	if(m->stripes){
		SDL_SetTextureColorMod(atlas, 0x00, 0x88, 0x44);		
		SDL_RenderCopyEx(ren, atlas, &rbodystripes, &r, 0, NULL, 0);
	}

	r.x = m->x + m->w/2 - m->w/6;
	r.y = m->y - m->w/4 + dy;
	r.w=m->w/3;
	r.h=m->w/3;
	SDL_SetTextureColorMod(atlas, 0x77, 0x77, 0x55);		
	SDL_RenderCopyEx(ren, atlas, &rhorn, &r, 0, NULL, 0);

	r.x = m->x; r.y = m->y; r.w=m->w; r.h=m->h;
	r.x = m->x + m->w *3/10;
	r.y = m->y + m->w *3/10 + dy;
	r.w = m->w * .4;
	r.h = m->w *.4;
	SDL_SetTextureColorMod(atlas, 0x00, 0xFF, 0x00);		
	SDL_RenderCopyEx(ren, atlas, &reye, &r, 0, NULL, 0);

	r.x -= r.w / 4.2;
	r.y -= r.w / 4.2;
	r.w *= 1.49;
	r.h *= 1.49;
	SDL_SetTextureColorMod(atlas, 0xff, 0xff, 0xff);		
	SDL_RenderCopyEx(ren, atlas, &reyewhitered, &r, 0, NULL, 0);

}

int quit = 0;

mendel creature = {
	0x0, 0xFF, 0x55,
	200, 100, 100, 160,
	0,
	0,
	2, 60};
mendel creature2 = {
	0x44, 0x33, 0x99,
	200, 100, 155, 200,
	1,
	0,
	1, 50};

void mendel_checkx(mendel *m, world *w){
	if(m->vx == 0){
		return;
	}
	int c1;
	int c2;
	if(m->vx < 0){
		c1 = (m->x - 1) / w->tilesize;
		c2 = (m->x + m->w) / w->tilesize;
	}
	else{
		c1 = (m->x) / w->tilesize;
		c2 = (m->x + m->w + 1) / w->tilesize;
	}
	if(c1 < 0)
		c1 = 0;
	if(c2 < 0)
		return;
	if(c1 >= w->w)
		return;
	if(c2 >= w->w)
		c2 = w->w - 1; 

	int r2 = (m->y + m->h - 1) / w->tilesize;
	int r1 = (m->y + 1) / w->tilesize;
	
	if(r1 < 0)
		r1 = 0;
	if(r1 >= w->h)
		return;
	if(r2 >= w->h)
		r2 = w->h - 1;
	if(r2 < 0)
		return;

	int done = 0;
	int cor = -m->w;
	int c = c1;
	int ce = c2;
	int cstep = 1;
	if(m->vx < 0){
		c = c2;
		ce = c1;
		cor = w->tilesize;
		cstep = -1;
	}

	/*SDL_Rect rect = {0, 0, w->tilesize, w->tilesize};	
	SDL_SetTextureColorMod(atlas, 0xFF, 0x00, 0xFF);*/
	while(!done){	
		/*rect.x = c * w->tilesize;*/
		for(int r = r1; r <= r2; ++r){
			/*rect.y = r * w->tilesize;
			SDL_RenderCopy(ren, atlas, &rclearblock, &rect);*/
			int b = w->map[r][c];
			if(b != AIR){
				m->x = c * w->tilesize + cor;
				m->vx = 0;
				done = 1;	
			}
		}
		c += cstep;
		if((cstep > 0) && (c > ce))
			break;
		if((cstep < 0) && (c < ce))
			break;
	}
}

void mendel_checky(mendel *m, world *w){
	if(m->vy == 0){
		return;
	}
	int r1;
	int r2;
	if(m->vy < 0){
		r1 = (m->y - 1) / w->tilesize;
		r2 = (m->y + m->h) / w->tilesize;
	}
	else{
		r1 = (m->y) / w->tilesize;
		r2 = (m->y + m->h + 1) / w->tilesize;
	}
	
	if(r1 < 0)
		r1 = 0;
	if(r1 >= w->h)
		return;
	if(r2 >= w->h)
		r2 = w->h - 1;
	if(r2 < 0)
		return;

	int c1 = (m->x + 1) / w->tilesize;
	int c2 = (m->x + m->w - 1) / w->tilesize + 1;
	if(c1 < 0)
		c1 = 0;
	if(c2 < 0)
		return;
	if(c1 >= w->w)
		return;
	if(c2 > w->w)
		c2 = w->w; 

	int cor = -m->h;
	int done = 0;
	int r = r1;
	int re = r2;
	int rstep = 1;
	if(m->vy < 0){
		r = r2;
		re = r1;
		cor = w->tilesize;
		rstep = -1;
	}

	/*SDL_Rect rect = {0, 0, w->tilesize, w->tilesize};	
	SDL_SetTextureColorMod(atlas, 0x00, 0xFF, 0xFF);*/
	while(!done){	
		/*rect.y = r * w->tilesize;*/
		for(int c = c1; c < c2; ++c){
			/*rect.x = c * w->tilesize;
			SDL_RenderCopy(ren, atlas, &rclearblock, &rect);*/
			int b = w->map[r][c];
			if(b != AIR){
				m->y = r * w->tilesize + cor;
				if(m->vy > 0){
					m->state |= ONGROUND;
				}
				m->vy = 0;
				done = 1;	
			}
		}
		r += rstep;
		if((rstep > 0) && (r > re))
			break;
		if((rstep < 0) && (r < re))
			break;
	}
}

void mendel_update(mendel *m){
	if(m->state & ONGROUND){
		m->vx += (((m->state & RIGHT) ? 1 : 0) + ((m->state & LEFT) ? -1 : 0)) * m->speed;
		m->vx *= 0.98;
	}
	else{
		m->vx += (((m->state & RIGHT) ? 0.75 : 0) + ((m->state & LEFT) ? -0.75 : 0)) * m->speed;
		m->vx *= 0.99;
	}
	m->vy += ((m->state & DOWN) ? 1 : 0) * m->speed;
	if(m->state & ONGROUND){
		if(m->state & UP){
			m->vy = -m->jump;
		}
	}

	SDL_Log("%d\n", m->state);

	m->vy += w->g;
	m->state &= ~ONGROUND;

	m->x += m->vx;
	mendel_checkx(m, w);

	m->y += m->vy;
	mendel_checky(m, w);
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
				creature.state |= UP;
			}
			else if(e.key.keysym.sym == SDLK_a){
				creature.state |= LEFT;
			}
			else if(e.key.keysym.sym == SDLK_s){
				creature.state |= DOWN;
			}
			else if(e.key.keysym.sym == SDLK_d){
				creature.state |= RIGHT;
			}
		}
		else if(e.type == SDL_KEYUP){
			if(e.key.keysym.sym == SDLK_w){
				creature.state &= ~UP;
			}
			else if(e.key.keysym.sym == SDLK_a){
				creature.state &= ~LEFT;
			}
			else if(e.key.keysym.sym == SDLK_s){
				creature.state &= ~DOWN;
			}
			else if(e.key.keysym.sym == SDLK_d){
				creature.state &= ~RIGHT;
			}
		}
		else if(e.type == SDL_MOUSEMOTION){
			creature.vx += e.motion.xrel;
			creature.vy += e.motion.yrel;
		}
	}

	SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear(ren);


	mendel_draw(&creature);
	mendel_draw(&creature2);
	world_draw(w);

	/* Update Creatures */
	mendel_update(&creature);	
	mendel_update(&creature2);	

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
