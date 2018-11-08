#define AIR ' '
#define DIRT '#'
#define GRASS '~'
#define WATER 'w'

typedef struct level{
	char **map;
	int h, w;	
}level;

level *level_load(char *path){
	int r, c, f, i;
	char buf[512];
	SDL_RWops *rw;
	SDL_Log("######## LOADING LEVEL");
	level *v = malloc(sizeof(level));
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

	sscanf(buf, "r:%d,c:%d\n", &v->h, &v->w);
	SDL_Log("######## r:%d, c:%d\n", v->h, v->w);
	v->map = malloc(sizeof(char *) * v->h);
	for(r=0; r<v->h; ++r){
		v->map[r] = malloc(sizeof(char) * v->w);
	}
	
	r = 0;
	i = 0;
	while((f = buf[i])){
		++i;
		if(f == '\n')
			break;
	}
	while((f = buf[i])){
		if(f == '\n'){
			c = 0;
			++r;
		}
		else{
			++c;
			if(r < v->h && c < v->w)
				v->map[r][c] = f;
		}
		++i;
	}

	SDL_RWclose(rw);
	return v;
}

void level_close(level *m){
	int r;
	for(r = 0; r<m->h; ++r){
		free(m->map[r]);
	}
	free(m->map);
	free(m);
}
