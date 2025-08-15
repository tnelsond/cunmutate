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

#define CATEYE 1

typedef struct mendel{
	SDL_Color color; // Eumelanin
	SDL_Color color2; // Phoemelanin
	SDL_Color ecolor;
	int x, y, w, h;
	int stripes;
	unsigned int state;
	int speed;
	int jump;
	float vx, vy;
	char sex;
	char eyetype;
	chrom *chr;
	struct mendel *next;
	int proc[GENE_NUM];
}mendel;

void mendel_print_proc(mendel *m){
	for(int i=0; i<GENE_NUM; ++i){
		SDL_Log("%d: %d", i, m->proc[i]);
	}
}

void mendel_express(mendel *m){
	hash_chrom(m->chr, m->proc);

	if(m->proc[gCateye]){
		m->eyetype = CATEYE;
	}
	else{
		m->eyetype = 0;
	}

	if(m->proc[gaw]){
		m->stripes = 2;
	}
	else if(m->proc[gAy]){
		m->stripes = 1;
	}

	if(m->proc[gI] == 0){
		m->color2.r = 0xFF;
		m->color2.g = 0xEE;
		m->color2.b = 0xDD;
	}
	else if(m->proc[gI] == 1){
		m->color2.r = 0xDD;
		m->color2.g = 0xAA;
		m->color2.b = 0x88;
	}
	else{
		m->color2.r = 0xBB;
		m->color2.g = 0x44;
		m->color2.b = 0;
	}

	/* Eumelanin production possible */
	if(m->proc[gB]){
		if(m->proc[gD]){
			m->color.r = m->color.g = m->color.b = 0x11;
			m->ecolor.r = 0x99;
			m->ecolor.g = 0x66;
			m->ecolor.b = 0x44;
		}
		else{
			m->color.r = m->color.g = m->color.b = 0x55;
			m->ecolor.r = 0x44;
			m->ecolor.g = 0x22;
			m->ecolor.b = 0x00;
		}
	}
	else{
		if(m->proc[gD]){
			m->color.r = 0x88;
			m->color.g = 0x77;
			m->color.b = 0x55;
			m->ecolor.r = m->ecolor.g = 0xAA;
			m->ecolor.b = 0xFF;
		}
		else{
			m->color.r = 0xCC;
			m->color.g = 0xBB;
			m->color.b = 0xAA;
			m->ecolor.r = m->ecolor.g = 0x77;
			m->ecolor.b = 0x44;
		}
	}

	if(m->proc[GENEPB]){
		m->color.r *= .8;
		m->color.g *= .8;
	}
	if(m->proc[GENEPR]){
		m->color.b *= .9;
		m->color.g *= .9;
	}
	if(m->proc[GENEPG]){
		m->color.r *= .7;
		m->color.b *= .7;
	}

	if(m->proc[gPigment] == 0){
		m->color.r = m->color.g = m->color.b =
		m->color2.r = m->color2.g = m->color2.b = 0xF0;

	}
	
	/*if(!m->proc[gK] && !m->proc[gkbr]){
		m->color = m->color2;
	}*/

	m->w = 100;
	m->h = 200;

	if(m->proc[gGrowth] == 1){
		m->h *= .8;
		m->w *= .9;
	}
	else if(m->proc[gGrowth] == 0){
		m->h *= .5;
		m->w *= .7;
	}
	if(m->proc[gGrowth2] == 1){
		m->h *= .9;
		m->w *= .8;
	}
	else if(m->proc[gGrowth2] == 0){
		m->h *= .75;
		m->w *= .8;
	}

	m->speed = 1;

	m->speed = 1;
	m->jump = 35;

	if(m->proc[gX] == 1){
		if(m->proc[gY] == 1){
			m->sex = MALE;
		}
	}
	else if(m->proc[gX] >= 2){
		m->sex = FEMALE;
	}

	if(m->h < m->w)
		m->h = m->w;
}

mendel *mendel_init(int x, int y, chrom *chr){
	/*SDL_Log("####### mendel_init\n");*/
	mendel *ret = malloc(sizeof(mendel));
	ret->color.r = 0xFF;
	ret->color.g = 0xFF;
	ret->color.b = 0xFF;
	ret->color.a = 0xFF;
	ret->color2.r = 0xFF;
	ret->color2.g = 0xFF;
	ret->color2.b = 0xFF;
	ret->color2.a = 0xFF;
	ret->ecolor.r = 0xFF;
	ret->ecolor.g = 0xDD;
	ret->ecolor.b = 0xDD;
	ret->ecolor.a = 0xFF;
	ret->x = x;
	ret->y = y;
	ret->w = 80;
	ret->h = 140;
	ret->stripes = 0;
	ret->state = ret->speed = ret->jump = 0;
	ret->vx = ret->vy = 0;
	ret->sex = 0;
	for(int i = 0; i < GENE_NUM-1; ++i){
		ret->proc[i] = 0;
	}
	ret->chr = chr;
	mendel_express(ret);
	print_source(ret->chr);
	mendel_print_proc(ret);

	return ret;
}

void mendel_free(mendel *m){
	chrom_free(m->chr);
	free(m);
}

mendel * mendel_breed(mendel *m, mendel *mo){
	return mendel_init((m->x + mo->x) / 2, (m->y + mo->y) / 2, chrom_breed(m->chr, mo->chr));
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
			//m->state &= ~UP;
		}
	}

	if(m->state & BREED){
		/*SDL_Log("Registering...\n");*/
		m->state &= ~BREED;
		mendel *mo = m;
		while((mo = mo->next)){
			if(mo == m){
				break;
			}
			/*SDL_Log("Looping\n");*/
			if(mendel_collide(m, mo)){
				/*SDL_Log("Collision\n");*/
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
						mnew->vy = 1;
						mendel_checky(mnew, w);
						mnew->vy = -20;
						mnew->next = m->next;	
						m->next = mnew;
					}
					break;
				}
			}
		}
	}

	/*SDL_Log("%d\n", m->state);*/

	m->vy += w->g;
	m->state &= ~ONGROUND;

	m->x += m->vx;
	mendel_checkx(m, w);

	m->y += m->vy;
	mendel_checky(m, w);
}

void mendel_draw(mendel *m, camera *view){
	SDL_FRect r;
	SDL_FRect rp = {m->x, m->y, m->w, m->h};
	camera_project(view, &rp);

	if(m->h > m->w){
		r.w = rp.w/3;
		r.h = rp.h - rp.w * 0.8;

		float angle = ((int)(m->x*90/(r.h - r.w /2))) % 90;
		if(angle > 45){
			angle = 90 - angle;
		}

		rp.y += ((r.h - r.w/2) - cos(angle * PI / 180) * (r.h - r.w /2));
		r.x = rp.x + rp.w / 4;
		r.y = rp.y + rp.w * 0.8;

		SDL_FPoint center = {r.w / 2, r.w / 2};
		SDL_SetTextureColorMod(atlas, m->color.r, m->color.g, m->color.b);		
		SDL_RenderTextureRotated(ren, atlas, &rleg, &r, angle, &center, 0);
		if(m->stripes){
			SDL_SetTextureColorMod(atlas, m->color2.r, m->color2.g, m->color2.b);		
			SDL_RenderTextureRotated(ren, atlas, (m->stripes == 1) ? &rlegstripes : &rlegspots, &r, angle, &center, 0);
		}
		r.x = rp.x + rp.w / 2;
		SDL_SetTextureColorMod(atlas, m->color.r, m->color.g, m->color.b);		
		SDL_RenderTextureRotated(ren, atlas, &rleg, &r, -angle, &center, 0);
		if(m->stripes){
			SDL_SetTextureColorMod(atlas, m->color2.r, m->color2.g, m->color2.b);		
			SDL_RenderTextureRotated(ren, atlas, (m->stripes == 1) ? &rlegstripes : &rlegspots, &r, -angle, &center, 0);
		}
	}

	r.x = rp.x;
	r.y = rp.y;
	r.w=rp.w;
	r.h=rp.w;
	SDL_SetTextureColorMod(atlas, m->color.r, m->color.g, m->color.b);		
	SDL_RenderTexture(ren, atlas, &rbodycircle, &r);

	if(m->stripes){
		SDL_SetTextureColorMod(atlas, m->color2.r, m->color2.g, m->color2.b);		
		SDL_RenderTextureRotated(ren, atlas, (m->stripes == 1) ? &rbodystripes : &rbodyspots, &r, 0, NULL, 0);
	}

	r.x = rp.x + rp.w/2 - rp.w/6;
	r.y = rp.y - rp.w/4;
	r.w=rp.w/3;
	r.h=rp.w/3;
	if(m->sex == MALE){
		SDL_SetTextureColorMod(atlas, 0x77, 0x77, 0x55);		
		SDL_RenderTextureRotated(ren, atlas, &rhorn, &r, 0, NULL, 0);
	}
	else if(m->sex == FEMALE){
		SDL_SetTextureColorMod(atlas, 0xFF, 0x77, 0x00);		
		SDL_RenderTextureRotated(ren, atlas, &rbow, &r, 0, NULL, 0);
	}

	r.x = rp.x + rp.w *3/10;
	r.x += ((m->state & RIGHT) ? 1 : (m->state & LEFT) ? -1 : 0) * rp.w/10;
	r.y = rp.y + rp.w *3/10;
	r.y += ((m->state & DOWN) ? 1 : (m->state & UP) ? -1 : 0) * rp.w/10;
	r.w = rp.w * .4;
	r.h = rp.w *.4;

	SDL_SetTextureColorMod(atlas, m->ecolor.r, m->ecolor.g, m->ecolor.b);		
	SDL_RenderTextureRotated(ren, atlas, ((m->eyetype == CATEYE) ? &rcateye : &reye), &r, 0, NULL, 0);

	r.x -= r.w / 4.2;
	r.y -= r.w / 4.2;
	r.w *= 1.49;
	r.h *= 1.49;
	SDL_SetTextureColorMod(atlas, 0xff, 0xff, 0xff);		
	SDL_RenderTextureRotated(ren, atlas, &reyewhitered, &r, 0, NULL, 0);

}
