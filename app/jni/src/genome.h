#define GENEMALE "cYou shall offer one male goat for a sin offering, and two male lambs a year old for a sacrifice of peace offerings.J"
#define GENEFEMALE "cHe answered, \"Haven't you read that he who made them from the beginning made them male and female, 5and said, 'For this cause a man shall leave his father and mother, and shall join to his wife; and the two shall become one flesh?'J"
#define GENEGROWTH "come Thou Fountof3very BlEssInG J"
#define GENEGROWTH2 "czzome my Fountof3very BlEssInGs J"
#define GENEPIGMENT "cNow therefore send me a man skillful to work in gold, and in silver, and in brass, and in iron, and in purple, and crimson, and blue, and who knows how to engrave engravings, to be with the skillful men who are with me in J."
#define GENEPR "cred is the color of the blood that flowedJ"
#define GENEPG "cand if you take cranberries and strew them like applesauce, they taste much more like prunes than rhubarb does.J"
#define GENEPB "cThe sky's awake, so I'm awake, so we have to PLAY!!J"

#define GENE_NUM 15

char *genesrc[] = {GENEFEMALE, GENEMALE, GENEGROWTH, GENEGROWTH2, GENEPIGMENT, GENEPR, GENEPG, GENEPB, GENEPB, GENEPB, GENEPB, GENEPB, GENEPB, GENEPB, GENEPB};
int genemap[GENE_NUM];

void init_gene(int i, char *s){
	int hash = 0;
	while(*s && aminos[*s & BIT6]){
		hash = hash * HASHFAC + aminos[*s & BIT6];
		++s;
	}
	genemap[i] = hash;
}

void init_genemap(){
	for(int i=0; i<GENE_NUM; ++i){
		init_gene(i, genesrc[i]);
	}
}
