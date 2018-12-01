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

char* file_read(const char* filename) {
        SDL_RWops *rw = SDL_RWFromFile(filename, "r");
        if (rw == NULL) return NULL;

        Sint64 res_size = SDL_RWsize(rw);
        char* res = (char*)malloc(res_size + 1);

        Sint64 nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
                nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
                nb_read_total += nb_read;
                buf += nb_read;
        }
        SDL_RWclose(rw);
        if (nb_read_total != res_size) {
                free(res);
                return NULL;
        }

        res[nb_read_total] = '\0';
        return res;
}

world *world_load(char *path){
	int r, c, f, i;
	char *buf;
	SDL_Log("######## LOADING LEVEL");
	world *w = malloc(sizeof(world));
	SDL_Log("######## SDL_RWFromFile");
	buf = file_read(path);
	printf(buf);
	if(buf == NULL){
		SDL_Log("######## FILE '%s' CANNOT BE OPENED!", path);
		return NULL;
	}
	SDL_Log("######## Opened");

	r = 0;
	c = 0;

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
			c = -1;
			++r;
		}
		else{
			if(r <= w->h && c <= w->w)
				w->map[r][c] = f;
		}
		++c;
		++i;
	}

	free(buf);
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

void world_draw(world *w, camera *view){
	int i, j;
	SDL_Rect r;

	SDL_SetTextureColorMod(atlas, 0x33, 0x11, 0x00);		

	r.w = r.h = w->tilesize;

	for(j=0; j<w->h; ++j){
		for(i=0; i<w->w; ++i){
			if(w->map[j][i] != AIR){
				if(w->map[j][i] == GRASS){
					SDL_SetTextureColorMod(atlas, 0x33, 0x55, 0x00);		
				}
				else{
					SDL_SetTextureColorMod(atlas, 0x33, 0x11, 0x00);		
				}
				r.x = (i * w->tilesize);
				r.y = (j * w->tilesize);
				r.w = r.h = w->tilesize;
				camera_project(view, &r);
				SDL_RenderCopy(ren, atlas, &rblock, &r);
			}
		}
	}

	SDL_SetTextureColorMod(atlas, 0x00, 0x00, 0x00);		
	text_draw(103, 23, 36, "Unmutate 4.0a");
}
