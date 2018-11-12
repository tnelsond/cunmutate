/* INPUT FLAGS */
#define UP 1
#define LEFT 2
#define DOWN 4
#define RIGHT 8
#define ONGROUND 16
#define BREED 32

/* SEX */
#define MALE 1
#define FEMALE 2

typedef struct mendel{
	int r, g, b;
	int x, y, w, h;
	int stripes;
	unsigned int state;
	int speed;
	int jump;
	float vx, vy;
	char sex;
	chrom *chr;
	struct mendel *next;
}mendel;

mendel creature = {
	0x0, 0xFF, 0x55,
	200, 100, 100, 160,
	0,
	0,
	1, 30, 0, 0,
	0,
	NULL,
	NULL};
mendel creature2 = {
	0x44, 0x33, 0x99,
	200, 100, 155, 200,
	1,
	0,
	1, 30, 0, 0,
	0,
	NULL,
	&creature};

mendel * mendel_breed(mendel *m, mendel *mo){
	mendel *ret = malloc(sizeof(mendel));
	ret->chr = crossover(m->chr, mo->chr);
	SDL_Log("####### Breed\n");
	ret->r = 0x00;
	ret->g = 0x00;
	ret->b = 0xFF;
	ret->x = (m->x + mo->x) / 2;
	ret->y = (m->y + mo->y) / 2;
	ret->w = 80;
	ret->h = 140;
	ret->stripes = 2;
	ret->state = ret->speed = ret->jump = 0;
	ret->vx = ret->vy = 0;
	ret->sex = 0;
	return ret;
}

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
	if(c1 > w->w)
		return;
	if(c2 > w->w)
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
	if(c1 > w->w)
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

int mendel_collide(mendel *m, mendel *mo){
	return (m->x < mo->x + mo->w) && (m->y < mo->y + mo->h) && (m->x + m->w > mo->x) && (m->y + m->h > mo->y);
}

void mendel_update(mendel *m, world *w){
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

	if(m->state & BREED){
		SDL_Log("Registering...\n");
		m->state &= ~BREED;
		mendel *mo = m;
		while((mo = mo->next)){
			SDL_Log("%d\n", mo);
			if(mo == m){
				break;
			}
			SDL_Log("Looping\n");
			if(mendel_collide(m, mo)){
				SDL_Log("Collision\n");
				if(m->sex + mo->sex == MALE + FEMALE){
					if(m->x > mo->x){
						m->vx = 10;
						mo->vx = -10;
					}
					else{
						m->vx = -10;
						mo->vx = 10;
					}
					mendel *mnew = mendel_breed(m, mo);
					if(mnew){
						mnew->vy -= 20;
						mnew->next = m->next;	
						m->next = mnew;
					}
					break;
				}
			}
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
	if(m->sex == MALE){
		SDL_SetTextureColorMod(atlas, 0x77, 0x77, 0x55);		
		SDL_RenderCopyEx(ren, atlas, &rhorn, &r, 0, NULL, 0);
	}
	else if(m->sex == FEMALE){
		SDL_SetTextureColorMod(atlas, 0xFF, 0x77, 0x00);		
		SDL_RenderCopyEx(ren, atlas, &rbow, &r, 0, NULL, 0);
	}

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
