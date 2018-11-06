#define GENE_NUM 10
typedef struct gene{
	int hash;
	int count;
}gene;

gene genes[GENE_NUM];

void init_gene(int i, char *s){
	int hash = 0;
	while(*s && aminos[*s & BIT6]){
		hash = hash * HASHFAC + aminos[*s & BIT6];
		++s;
	}
	genes[i].hash = hash;
}

void reset_gene_count(){
	int i = sizeof(genes);
	while(--i >= 0){
		genes[i].count = 0;
	}
}

void init_genes(){
	init_gene(0, "come Thou Fountof3very BlEssInG J");
	init_gene(1, "czzome my Fountof3very BlEssInGs J");
	init_gene(2, "call this black, it'sAdarkCoLoR, unlike J");
	init_gene(3, "cause they fear the stripes, they make them quakeJ");
	init_gene(4, "call this red, it'sAdarkCoLoR, unlike J");
}

char *chroma = "AAAAA come Thou Fountof3very BlEssInG Just to know that, you are near. call this black, it'sAdarkCoLoR, unlike Jaundice. cSpots are greatNevArwRong$fidoRov*rJust come on cause they fear the stripes, they make them quakeJ GGGGGGGGGG";
char *chromb = "AAAAA czzome my Fountof3very BlEssInGs Just to know that, you are near. call this red, it'sAdarkCoLoR, unlike Jaundice. cstripesAreWhatTigg ers have!Just come on GGGGGGGGGG";

