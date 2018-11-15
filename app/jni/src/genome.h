#define GENE_NUM 15
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
	init_gene(0, "cow a boy. This is my super lonJ");
	init_gene(1, "c sex autosome and whatever blah blah garJ");
	init_gene(2, "come Thou Fountof3very BlEssInG J");
	init_gene(3, "czzome my Fountof3very BlEssInGs J");
	init_gene(4, "call this black, it'sAdarkCoLoR, unlike J");
	init_gene(5, "cause they fear the stripes, they make them quakeJ");
	init_gene(6, "call this red, it'sAdarkCoLoR, unlike J");
	init_gene(7, "cthe stripes their Colors fade, but black is the greatest bestest one we might ever possibly see, alright? just yaw J");
	init_gene(8, "caba daba dee %@#()!*)(*$+_=oh dear oh dear, my eyes are bluedyJ");
	init_gene(9, "cSpots are greatNevArwRong$fidoRov*rJ");
	init_gene(10, "cPigments are very very diffihard to make properly, THIS is a protein that will make them right. Don'T you DARE lose THIS!!!J");
	init_gene(11, "cLotta legs are for laksjdvoizjlkAD@#$%*( Sorry, I sometimes type seemingly random thingies, oh well, sorry, so very sorry, very sorry, eh, meh, huh?J");
	init_gene(12, "c short legs are great, really really amazing, stupendous J");
	init_gene(13, "can have green in them too, BUT it's a very very complicated color that no one knows, but J");
	init_gene(14, "can have red in them too, BUT it's a very complicated color that no one knows, but J");
}

char *chroma = "AAAAAcPigments are very very diffihard to make properly, THIS is a protein that will make them right. Don'T you DARE lose THIS!!!Just relax and come Thou Fountof3very BlEssInG Just to know that, you are near. call this black, it'sAdarkCoLoR, unlike JaundiCe. cSpots are greatNevArwRong$fidoRov*rJust come Jon cause they fear the stripes, they make them quakeJ oh no what to do blah blah blooedy Cabby caba daba dee %@#()!*)(*$+_=oh dear oh dear, my eyes are bluedyJesus loves meGGGGGGGGGGGGGGGGGGG";
char *chromb = "AAAAA czzome my Fountof3very BlEssInGs Just to know that, you are near. call this red, it'sAdarkCoLoR, unlike JaundicPigments are very very diffihard to make properly, THIS is a protein that will make them right. Don'T you DARE lose THIS!!!Je. cstripesAreWhatTigg ers have!Just come on J cthe stripes their Colors fade, but black is the greatest bestest one we might ever possibly see, alright? just yaw JGGGGGGGGGG";
char *chromc = "AAAAA I'm definitely cow a boy. This is my super lonJ and necessary chromosome.JGGGGGGGGGGG";
char *chromd = "AAAAA Generic sex autosome and whatever blah blah garJ and necessary chromosome.JGGGGGGGGGGG";
char *chrome = "AAAAAAA oh deario cLotta legs are for laksjdvoizjlkAD@#$%*( Sorry, I sometimes type seemingly random thingies, oh well, sorry, so very sorry, very sorry, eh, meh, huh?J Eyes can have green in them too, BUT it's a very very complicated color that no one knows, but Jesus does, he always does. Yep GGGGGGGGG";

char *chromf = "AAAAAAA oh deario c short legs are great, really really amazing, stupendous J rightio Eyes can have red in them too, BUT it's a very complicated color that no one knows, but Jesus does, he always does. Yep GGGGGGGGGG";
