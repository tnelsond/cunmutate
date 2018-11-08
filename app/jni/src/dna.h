#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define T 0
#define U 0
#define C 1
#define A 2
#define G 3

#define AMINO 63 

#define BIT2 0x3
#define BIT4 0x0F
#define BIT6 0x3F
#define BIT8 0xFF
/* 128 & 64, 32 & 16, 8 & 4, 2 & 1 */

#define BASENAME(b) ((b) == A ? 'A' : (b) == U ? 'U' : (b) == G ? 'G' : 'C')
#define MIN(a, b)(a) > (b) ? (b) : (a)
#define MAX(a, b)(a) > (b) ? (a) : (b)

#define STP 0
#define MET 1 /* Start */
#define START 1
#define PHE 2
#define LEU 3
#define SER 4
#define TYR 5
#define CYS 6
#define TRP 7
#define PRO 8
#define HIS 9
#define GLN 10
#define ARG 11
#define ILE 12
#define THR 13
#define ASN 14
#define LYS 15
#define VAL 16
#define ALA 17
#define ASP 18
#define GLU 19
#define GLY 20

#define HASHFAC 19

typedef unsigned char bases;
typedef unsigned char amino;

amino aminos[] = {
/*   UU   UC   UA   UG   CU   CC   CA   CG   AU   AC   AA   AG   GU   GC   GA   GG */
/*U*/PHE, PHE, LEU, LEU, SER, SER, SER, SER, TYR, TYR, STP, STP, CYS, CYS, STP, TRP,
/*C*/LEU, LEU, LEU, LEU, PRO, PRO, PRO, PRO, HIS, HIS, GLN, GLN, ARG, ARG, ARG, ARG,
/*A*/ILE, ILE, ILE, MET, THR, THR, THR, THR, ASN, ASN, LYS, LYS, SER, SER, ARG, ARG,
/*G*/VAL, VAL, VAL, VAL, ALA, ALA, ALA, ALA, ASP, ASP, GLU, GLU, GLY, GLY, GLY, GLY 
};

#include"genome.h"

typedef struct chrom chrom;

struct chrom{
	bases *b;
	int len;
	chrom *next;
	/* Debug */
	int split;
};

int hash_protein(amino *pro){
	int hash = 0;
	while(*pro){
		hash = hash * HASHFAC + *pro;
		++pro;
	}
	return hash;
}

bases get_base(chrom *chr, int i){
	if(i < chr->len){
		return (chr->b[i/4] >> (6 - (i % 4) * 2)) & BIT2;
	}
	else
		return -1;
}

bases get_triplet(chrom *chr, int i){
	if((i + 2) > chr->len){
		return -1;
	}
	if((i % 4) == 0){
		return (chr->b[i/4] >> 2) & BIT6;
	}
	if((i % 4) == 1){
		return chr->b[i/4] & BIT6;
	}
	if((i % 4) == 2){
		return ((chr->b[i/4] << 2) + (chr->b[i/4+1] >> 6)) & BIT6;
	}
	return ((chr->b[i/4] << 4) + (chr->b[i/4+1] >> 4)) & BIT6;
}

void set_triplet(chrom *chr, int i, bases b){
	b &= BIT6;
	if(i % 4 == 0){
		chr->b[i/4] = b << 2;
	}
	else if(i % 4 == 1){
		chr->b[i/4] |= b;
	}
	else if(i % 4 == 2){
		chr->b[i/4] |= b >> 2;
		chr->b[i/4+1] = b << 6;
	}
	else{
		chr->b[i/4] |= b >> 4;
		chr->b[i/4+1] = b << 4;
	}
}

void set_base(chrom *chr, int i, bases b){
	b &= BIT2;
	/* 00 00 00 00 */
	chr->b[i/4] &= ~(BIT2 << ((3  - (i % 4)) * 2));
	chr->b[i/4] |= b << ((3  - (i % 4))*2);
}

amino get_amino(chrom *chr, int i){
	return aminos[get_triplet(chr, i)];
}

int find_homologue(chrom *x, chrom *y, int * xi){
	int yi, i;
	int match = 0;
	int grainsize = 21;
	int oldxi = *xi;
	while(!match){
		for(yi=0; yi < y->len - grainsize; ++yi){
			for(i=0; i < grainsize; ++i){
				if(get_base(x, *xi + i) != get_base(y, yi + i)){
					break;
				}
			}
			if(i == grainsize){
				match = yi + 1;
			}
			if(abs(match - *xi) < abs(yi - *xi)){
				break;
			}
		}
		*xi = *xi + 1;
		if(*xi >= x->len - grainsize){
			*xi = 0;
		}
		if(*xi == oldxi){
			break;
		} 
	}
	if(!match){
		*xi = match;
	}
	return match;
}


void print_bases_debug(chrom *chr){
	int i;
	bases b;
	for(i=0; i<chr->len; ++i){
		if(i % 3 == 0){
			printf(". %d:", get_amino(chr, i));
		}
		b = get_base(chr, i);
		putchar(BASENAME(b));
	}
	putchar('\n');
}

void print_bases(chrom *chr){
	int i;
	bases b;
	for(i=0; i<chr->len; ++i){
		b = get_base(chr, i);
		if(i == chr->split)
			putchar('.');
		putchar(BASENAME(b));
	}
}

void print_source(chrom *chr){
	int i;
	bases b;
	for(i=0; i < chr->len; i+=3){
		b = get_triplet(chr, i);
		putchar((b < 32) ? (b + 64) : b);
	}
	putchar('\n');
}

int hash_gene(chrom *chr, int i){
	int hash = 0;
	int inside = 0;
	for(; i < chr->len; ++i){
		amino a = get_amino(chr, i);
		if(a == START){
			inside = 1;
		}
		if(inside){
			if(a == STP){
				break;
			}
			i += 2;
		}
		hash = hash * HASHFAC + a;
	}
	return hash;
}

void increment_hash(int hash){
	int i;
	for(i=0; i<GENE_NUM; ++i){
		if(genes[i].hash == hash){
			++genes[i].count;
			break;
		}
	}
}

void hash_chrom(chrom *chr){
	int i;
	int hash = 0;
	int inside = 0;
	for(i = 0; i < chr->len; ++i){
		amino a = get_amino(chr, i);
		if(a == START){
			inside = 1;
		}
		if(inside){
			if(a == STP){
				inside = 0;
				printf("\nhash: %d\n", hash);
				increment_hash(hash);
				hash = 0;
			}
			else{
				hash = hash * HASHFAC + a;
			}
			i += 2;
		}
	}
}

void print_binary_triplet(bases b){
	int bi;
	for(bi = 5; bi >= 0; --bi){
		putchar((b & (1 << bi)) ? '1' : '0');
	}
}

/* Add extra zeros to match source */
void print_binary(chrom *chr){
	int i;
	bases b;
	for(i = 0; i < chr->len; i += 3){
		b = get_triplet(chr, i);
		print_binary_triplet(b);
	}

}

void print_binary_text(char *s){
	int i;
	int slen = strlen(s);
	for(i = 0; i < slen; ++i){
		print_binary_triplet(s[i]);
	}
}

/* Takes 3 2bit DNA bases char input and packs them into a 4 2bit DNA base. */
void init_chrom(chrom *chr, char *s){
	int i = 0;
	int slen = strlen(s);
	chr->split = -1;
	chr->len = slen * 3; 
	chr->b = malloc(sizeof(bases) * (chr->len/4 + ((chr->len % 4) ? 1 : 0)));
	for(i=0; i < chr->len; i += 3){
		set_triplet(chr, i, s[i/3]);
	}
	putchar('\n');
}

void free_chrom(chrom *chr){
	free(chr->b);
}

chrom *crossover(chrom *x, chrom *y){
	int i, xe, ys;
	int grainsize = 21;
	chrom *ret = malloc(sizeof(chrom));
	xe = rand() % (x->len - grainsize);
	ys = find_homologue(x, y, &xe);
	ret->len = xe + y->len - ys;
	/*
	printf("%d : %d\n%d %d %d\n", xe, ys, x->len, y->len, ret->len);
	for(i=0; i<xe; ++i){
		putchar('-');
	}
	putchar('@');
	putchar('\n');
	*/
	ret->b = malloc(sizeof(bases)*ret->len/4);

	/* DEBUG */
	x->split = xe;
	y->split = ys;
	ret->split = xe;
	/* DEBUG */

	for(i=0; i<xe; ++i){
		set_base(ret, i, get_base(x, i));
	}
	while(ys < y->len){
		set_base(ret, i, get_base(y, ys));
		++i; ++ys;
	}
	return ret;
}

void express(chrom *a, chrom *b){
}
