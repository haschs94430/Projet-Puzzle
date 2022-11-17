/*		### BIBLIOTHEQUE ###		*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <MLV/MLV_all.h>
#include <assert.h>

/*		### MACRO ###		*/

#define TAILLEX 1025
#define TAILLEY 512

/*		### STRUCTURE ###		*/

typedef struct noeud{
		unsigned char val;
		struct noeud *f1, *f2, *f3, *f4;
}Noeud, *Arbre;

/*		### DEFINITION FONCTION ###		*/

/**
 * Cette fonction alloue un noeud et ecrit val à l'interieur,
 * la libèration de la memoire est à la charge de l'utilisateur 
 */ 

Arbre alloueNoeud(unsigned char val){
	Noeud *tmp;
	tmp = (Noeud *) malloc (sizeof(Noeud));
	if (tmp != NULL){
		tmp -> val = val;
		tmp->f1 = NULL;
		tmp->f2 = NULL; 
		tmp->f3 = NULL; 
		tmp->f4 = NULL; 
	}
	return tmp;
}

Noeud* chercheNoeud(Arbre a, int taille, int x_min, int y_min, int x_crt, int y_crt, int niveau){
	if( x_crt > x_min + taille || y_crt > y_min + taille)
		return NULL;
	
	if(NULL == a || niveau == 1)
		return a;
	taille /= 2;	
	/*Fils 1*/
	if( x_min + taille > x_crt && y_min + taille > y_crt)
		return chercheNoeud(a->f1, taille, x_min, y_min, x_crt , y_crt , niveau - 1);
	/*Fils 2*/	
	else if( x_min + taille < x_crt && y_min + taille > y_crt)
		return chercheNoeud(a->f2, taille, x_min + taille , y_min, x_crt , y_crt , niveau - 1);
	/*Fils 3*/	
	else if( x_min + taille > x_crt && y_min + taille < y_crt)
		return chercheNoeud(a->f3, taille, x_min, y_min + taille , x_crt , y_crt , niveau - 1);
	/*Fils 2*/	
	else
		return chercheNoeud(a->f4, taille, x_min + taille , y_min + taille , x_crt , y_crt , niveau - 1);
	
}

/**
 * Cette fonction prend en paramètre un image MLV, 3 int et l'adresse d'un int.
 * La fonction renvoie la couleur (en niveau de gris), contenu sur le pixel x1,y1
 * la fonction ecrit 1 si le carre représenté par les coodonnées 
 * (x1, y1), (x1 + taille, y1), (x1, (x1 + taille,, (x1 + taille,y1 + taille) 
 * ne contient qu'une seul couleur, ecrit 0 sinon
 */

unsigned char estUniforme(MLV_Image *im, int taille, int x1, int y1, int *uni){
	int i, j, rouge, vert, bleu, ref, temp;
	MLV_get_pixel_on_image(im, x1, y1, &rouge, &vert, &bleu, NULL);
	ref = (rouge + vert + bleu) / 3;
	
	for( i = x1; i < x1 + taille; ++i)
		for( j = y1; j < y1 + taille; ++j){
			MLV_get_pixel_on_image(im, i, j, &rouge, &vert, &bleu, NULL);
			temp = (rouge + vert + bleu) / 3;
			if(temp != ref){
				*uni = 0;
				return ref;
			}
		}
	*uni = 1;
	return ref;		
}

/**
 * Cette fonction prend en paramètre l'adresse d'un arbre, un image MLV carre,
 * et ecrit dans l'arbre les information de l'image.
 * Au premier appelle taille doit être egal à la taille d'un côté l'image,
 * et x1 et y1 doivent être egal à 0
 */ 

void ecrireArbre(Arbre *a,MLV_Image *im, int taille, int x1, int y1){
	unsigned char val;
	int uni;
	
	val = estUniforme(im, taille, x1, y1, &uni);
	*a = alloueNoeud(val);
	if( uni == 1) /*feuille*/
		return;
		
	ecrireArbre(&((*a)->f1), im, taille / 2, x1, y1);
	ecrireArbre(&((*a)->f2), im, taille / 2, x1 + taille / 2, y1);
	ecrireArbre(&((*a)->f3), im, taille / 2, x1, y1 + taille / 2);
	ecrireArbre(&((*a)->f4), im, taille / 2, x1 + taille / 2, y1 + taille / 2);
}

Arbre freeArbre(Arbre a){
    if(a == NULL){
        return NULL;
    }
    freeArbre(a->f1);
    freeArbre(a->f2);
    freeArbre(a->f3);
    freeArbre(a->f4);
    free(a);
    return NULL;
}

void TabNiveau(Arbre a, Noeud* lst[], int n){
	static int index = 0;
	if(a == NULL){
		return;
	}
	if(n != 1){
		TabNiveau(a->f1, lst, n - 1);
		TabNiveau(a->f2, lst, n - 1);
		TabNiveau(a->f3, lst, n - 1);
		TabNiveau(a->f4, lst, n - 1);
	}
	else{
		lst[index] = a;
		++index;
	}
	return;
}

void lireArbreAux(Arbre *a, FILE *in){
	
	int fils;
	int eti = 0;
	if(fscanf(in, "%d ",&fils) == EOF)
		return;
	
	if(fils == 1){
		fscanf(in, "%d ",&eti);
		*a = alloueNoeud(eti);
		return;
	}

	*a = alloueNoeud(eti);
	
	lireArbreAux(&((*a)->f1), in);
	lireArbreAux(&((*a)->f2), in);
	lireArbreAux(&((*a)->f3), in);
	lireArbreAux(&((*a)->f4), in);

}

Arbre lireArbreFichier(char *chemin){
	
	Arbre a = NULL;
	
	FILE *fichier = NULL;
	fichier = fopen(chemin, "r");
	if( fichier == NULL)
		return NULL;
	
	lireArbreAux(&a, fichier);
	
	fclose(fichier);
	return a;
}

void ecritArbreAux(Arbre a, FILE *out){
	
	int val;
	
	if( NULL == a)
		return;
	
	val = a->val;
	if(a->f1 != NULL){ /*noeud interne*/
		fprintf(out,"0 ");
	}
	else{
		fprintf(out,"1 %d ", val);
		return;
	}
	
	ecritArbreAux(a->f1, out);
	ecritArbreAux(a->f2, out);
	ecritArbreAux(a->f3, out);
	ecritArbreAux(a->f4, out);
}

void ecritArbreFichier(Arbre a, char *chemin){
	
	FILE *fichier = NULL;
	fichier = fopen(chemin, "w");
	if( fichier == NULL)
		return;
	
	ecritArbreAux(a, fichier);
	
	fclose(fichier);
}

/**
 * Prend en paramètre deux poiteur vers des objets de même type
 * ainsi que la taille des objets et enchange un et deux
 */

void memSwap(void* un, void* deux, int taille){
	char *u,*d;
	int i = 0;
	char temp = 0;

	u = (char*)un;
	d = (char*)deux;

	for( i = 0; i < taille; i++){
		temp = u[i];
		u[i] = d[i];
		d[i] = temp;
	}
}

/**
 * Fonction de mélange de tableau générique,
 * la fonction prend en paramètre l'adresse du premier element,
 * le nombre d'element ainsi que leurs tailles.
 * Mélange le tableau à l'aide d'une fonction memSwap
 */

void shuffle(Noeud** tab, int taille){
	int index,i;
	for(i = 0; i < taille; i++){
		index = i + ( rand() % (taille-i));
		memSwap(tab[i], tab[index], sizeof(Noeud));
	}
}

/**
 * Cette fonction prend en paramètre un entier `x` et un entier
 * positif `n` renvoie `x` puissance `n`
 */ 

int expo_rapide(int x, int n){
	int carre;
	
	assert(n >= 0);
	
	if(n == 0)
		return 1;
		
	carre = expo_rapide(x, n / 2);
	carre = carre * carre;
	
	if(n % 2)
		return carre * x;
	return carre;
}

/**
 * Cette fonction prend en paramètre un arbre, un image MLV carre,
 * et modifie l'image avec les informations contenu dans l'arbre
 * Au premier appelle taille doit être egal à la taille d'un côté l'image,
 * et x1 et y1 doivent être egal à 0
 */ 

void creeImage(Arbre a,MLV_Image *im, int taille, int x1, int y1){
	if(a->f1 == NULL){
		MLV_draw_filled_rectangle_on_image(x1, y1, taille, taille, 
			MLV_rgba( a->val, a->val, a->val, 255), im);
		return;	
	}
	creeImage(a->f1, im, taille / 2, x1, y1);
	creeImage(a->f2, im, taille / 2, x1 + taille / 2, y1);
	creeImage(a->f3, im, taille / 2, x1, y1 + taille / 2);
	creeImage(a->f4, im, taille / 2, x1 + taille / 2, y1 + taille / 2);
}

/**
 * Cette fonction prend en paramètre, un image MLV et adapte (si besoin est),
 * celle pour qu'elle soit de dimension `taille` x `taille`,
 * ajoute des bandes blanche si l'image n'es pas carré
 */ 


void adapteImage( MLV_Image *im, int taille){
	int x,y;
	MLV_Image *image;
	
	MLV_get_image_size(im, &x, &y);
	if( x == y){
		if( x == taille )
			return;
		else{
			MLV_resize_image(im, taille, taille);
			return;
		}
	}
	
	image = MLV_create_image(taille, taille);
	MLV_draw_filled_rectangle_on_image(0,0,
		taille, taille, MLV_COLOR_WHITE, image);
		
	MLV_resize_image_with_proportions(im, taille, taille);
	MLV_get_image_size(im, &x, &y);
	
	if(x < y)
		MLV_draw_image_on_image(im, image, (taille - x) / 2,0);
	else
		MLV_draw_image_on_image(im, image, 0, (taille - y) / 2);
		
	MLV_free_image(im);
	im = MLV_copy_image(image);
}

int compar_affichage(int x_im1, int y_im1, int x_im2, int y_im2, int taille){
	int i, j;
	int rp1, rp2;
	
	for( i = 0; i < taille; ++i)
		for(j = 0; j < taille; ++j){
			MLV_get_pixel(x_im1 + i, y_im1 + j, &rp1, NULL, NULL, NULL);
			MLV_get_pixel(x_im2 + i, y_im2 + j, &rp2, NULL, NULL, NULL);
			if( !(-3 < (rp1 - rp2)) && ((rp1 - rp2) < 3)){
				return 1;
			}
		}
	return 0;
}

int main(int argc, char **argv){

	Noeud* tab[64];
	char chemin[64] = "Image/";
	MLV_Image *image;
	MLV_Image *imMel;
	Arbre a;
	Arbre n1, n2;
	int x,y;
	int niveau;
	FILE *test;
	niveau = 3;
	srand(time(NULL));
	
	
	/* SAISIE CONTROLE*/
	
	do{
		printf("Entrer le nombre de piece (16 ou 64)\n");
		scanf("%d", &niveau); 
	}while(niveau != 64 && niveau != 16);
	
	if (niveau == 64)
		niveau = 4;
	else 
		niveau = 3;
		
	do{
		printf("Entrer le nom de l'image \n");
		scanf("%s", &chemin[6]);
		test = fopen(chemin, "r");
	}while(test == NULL);
	fclose(test);

	/* INITIALISATAION GRAPHIQUE / CREATION ARBRE*/
	MLV_create_window( "Arbre", "", TAILLEX, TAILLEY );
	image = MLV_load_image(chemin);
	adapteImage(image, 512);
	
	imMel = MLV_create_image(512,512);
	
	ecrireArbre(&a, image, 512, 0, 0);
	/*ecritArbreFichier(a, "Image/test.quad");
	a = lireArbreFichier("Image/test.quad");*/
	
	/* MELANGE*/
	TabNiveau(a, tab, niveau);
	creeImage(a, image, 512, 0, 0);
	shuffle(tab, expo_rapide(4, niveau - 1));
	creeImage(a, imMel, 512, 0, 0);
	
	/* AFFICHAGE*/
	MLV_draw_image(imMel,0,0);
	MLV_draw_image(image,513,0);
	MLV_actualise_window();
	
	while(compar_affichage(0, 0, 513, 0, 512)){
		n1 = n2 = NULL;
		
		
		while(n1 == NULL){
			MLV_wait_mouse(&x, &y);
			n1 = chercheNoeud(a, 512, 0, 0, x, y, niveau);
		}
		
		while(n2 == NULL){
			MLV_wait_mouse(&x, &y);
			n2 = chercheNoeud(a, 512, 0, 0, x, y, niveau);
		}
		/* Echange de piece*/
		memSwap(n1, n2, sizeof(Noeud));
		creeImage(a, imMel, 512, 0, 0);
		MLV_draw_image(imMel,0,0);
		MLV_draw_image(image,TAILLEX / 2 + 1,0);
		MLV_actualise_window();	
	}	
	MLV_actualise_window();	
	MLV_clear_window(MLV_COLOR_BLACK);
	MLV_actualise_window();
	MLV_draw_text_box(325, 225, 150, 50, "Bravo, \n vous avez fini \n le puzzle", 1, MLV_COLOR_GREY, MLV_COLOR_RED, MLV_COLOR_GREY, MLV_TEXT_CENTER, MLV_HORIZONTAL_CENTER, MLV_VERTICAL_CENTER);
	MLV_actualise_window();
	
	MLV_wait_seconds(2);
	MLV_free_window();
	
	MLV_free_image(image);
	MLV_free_image(imMel);
	freeArbre(a);
	return 0;
}
