#define AIR ' '
#define DIRT '#'
#define GRASS '~'
#define WATER 'w'

typedef struct world{
	char **map;
	int h, w, tilesize;
	float g;
}world;

world *w;

world *world_load(char *path){
	int r, c, f, i;
	char buf[512];
	SDL_RWops *rw;
	SDL_Log("######## LOADING LEVEL");
	world *w = malloc(sizeof(world));
	SDL_Log("######## SDL_RWFromFile");
	rw = SDL_RWFromFile(path, "r");
	if(rw == NULL){
		SDL_Log("######## FILE '%s' CANNOT BE OPENED!", path);
		return NULL;
	}
	SDL_Log("######## Opened");

	r = 0;
	c = 0;

	SDL_RWread(rw, buf, sizeof(buf), sizeof(buf));

	sscanf(buf, "r:%d,c:%d,s:%d,g:%f\n", &w->h, &w->w, &w->tilesize, &w->g);
	SDL_Log("######## r:%d, c:%d, s:%d, g:%f\n", w->h, w->w, w->tilesize, w->g);
	w->map = malloc(sizeof(char *) * w->h);
	for(r=0; r<w->h; ++r){
		w->map[r] = malloc(sizeof(char) * w->w);
	}
	
	r = 0;
	i = 0;
	while((f = buf[i++])){
		if(f == '\n')
			break;
	}
	while((f = buf[i])){
		if(f == '\n'){
			c = 0;
			++r;
		}
		else{
			if(r <= w->h && c <= w->w)
				w->map[r][c] = f;
		}
		++c;
		++i;
	}

	SDL_RWclose(rw);
	return w;
}

void world_close(world *w){
	int r;
	for(r = 0; r<w->h; ++r){
		free(w->map[r]);
	}
	free(w->map);
	free(w);
}

void world_draw(world *w){
	int i, j;
	SDL_Rect r;


	SDL_SetTextureColorMod(atlas, 0x33, 0x11, 0x00);		

	r.w = r.h = w->tilesize;

	for(j=0; j<w->h; ++j){
		for(i=0; i<=w->w; ++i){
			if(w->map[j][i] != AIR){
				r.x = i * w->tilesize;
				r.y = j * w->tilesize;
				SDL_RenderCopy(ren, atlas, &rblock, &r);
			}
		}
	}

	SDL_SetTextureColorMod(atlas, 0x00, 0x00, 0x00);		
	text_draw(103, 23, 36, "Unmutate 4.0a");
}
