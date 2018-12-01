#define GENE_NUM 21
#define gXs "cHe answered, \"Haven't you read that he who made them from the beginning made them male and female, 5and said, 'For this cause a man shall leave his father and mother, and shall join to his wife; and the two shall become one flesh?'J"
#define gX 0

#define gYs "cYou shall offer one male goat for a sin offering, and two male lambs a year old for a sacrifice of peace offerings.J"
#define gY 1

/* Agouti Series */
#define gAys "caguitiaslkdjfljJ"
#define gAy 2

#define gaws "c9827845jkashkjfhakjhkhaj HJjhfaksjhdfiuy8972J"
#define gaw 3

#define gats "calksdjf98 8h3uhuyu89aysdufhkjah987sdyf8 J"
#define gat 4

// ga is the absence I'd assume 

/* Liver Series */
#define gBs "cvnasdflasdfioju weoirulaksjdfk J"
#define gB 5
// gb is the absence I'd assume 

/* Dilution Series */
#define gDs "cDilution askldjfkljasoidf lxcvyhhJ"
#define gD 6

#define gds "cDilutionlkajsdkfl alksjdflkj alsjJ"
#define gd 7
// gdl is the absence I'd assume 

/* Extension Series */
#define gEms "cliqwjeij kljsldkj aiisdjoifjqlkw ejsidfjoi2108934J"
#define gEm 8

#define gEgs "c023498ujalkjf980u123nr8jhsdJ"
#define gEg 9

#define gEhs "c023498ujalkjalkju123nr8jhsdJ"
#define gEh 10

#define gEs "clakdsflkja sd;lkfj 2389045yur8J"
#define gE 11
// ge is the absence I'd assume

/* Greying Series */
#define gGs "clkajsdf 89yu289u 189yu98uafdijlkj8J"
#define gG 12
// gg is the absence I'd assume


/* Harlequin Series */
#define gHs "c 102387489u98 sdfajlkdfih89qay dfhwsydf789ydsax789 749 928J"
#define gH 13
// gh is the absence I'd assume

/* Phaeomelanin Intensity Series */
#define gIs "clkasjd fiu98u892u8u4r89a8f987y978y98yh5tuihuhhJ"
#define gI 14

/* Eumelanin Series */
#define gKs "JI will pass through all your flock today, removing from there every speckled and spotted one, and every black one among the sheep, and the spotted and speckled among the goats. This will be my hireJ"
#define gK 15
#define gkbrs "JI will pass through all your flock today, removing from there every speckled and spotted one, and every black one among the sheep, and the spotted and speckled among the goats. This will be my hireJ"
#define gkbr 16
// gk is the absence I'd assume

/* Merle Series */
#define gMs "ckl 120983748u89dsauf aiodjlkfjlakjlkJ"
#define gM 17
// gm is the absence I'd assume

/* Spotting Series */
#define gSs "c190827438 8972159 98df98shv978fhn9832htr9hnsidJ"
#define gS 18
// gs is the absence I'd assume

/* Ticking Series */
#define gTs "c198897ty98t yd9syh89fyaiusdhfjkhwue x hkjhkj hsahf;J"
#define gT 19

#define gTrs "cka df889xcuvoiujhiouh289 8ifj8iy82J"
#define gTr 20
// gt is the absence I'd assume

#define GENEGROWTH "come Thou Fountof3very BlEssInG J"
#define GENEGROWTH2 "czzome my Fountof3very BlEssInGs J"
#define GENEPIGMENT "cNow therefore send me a man skillful to work in gold, and in silver, and in brass, and in iron, and in purple, and crimson, and blue, and who knows how to engrave engravings, to be with the skillful men who are with me in J."
#define GENEPR "cred is the color of the blood that flowedJ"
#define GENEPG "cand if you take cranberries and strew them like applesauce, they taste much more like prunes than rhubarb does.J"
#define GENEPB "cThe sky's awake, so I'm awake, so we have to PLAY!!J"

char *genesrc[GENE_NUM] = {gXs, gYs, gAys, gaws, gats, gBs, gDs, gds, gEms, gEgs, gEhs, gEs, gGs, gHs, gIs, gKs, gkbrs, gMs, gSs, gTs, gTrs};

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
