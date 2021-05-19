#include <stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>
#include <windows.h>
#include<math.h>
//Fonctions concernants le type cellule

typedef struct Cell
{
    int Nombre;
    char Type;
    char Couleur; //r: pour le rouge ; n: pour le noir
    struct Cell *suivant;
}Cell;

int estvide(Cell *p)
{
    if(p==NULL) return 1 ;
    return 0 ;
}

int taille(Cell *p)
{
    int i=0;
    while(p!=NULL)
    {
        i++;
        p=p->suivant;
    }
    return i;
}

int taille_tableau_cellule(Cell **p)
{
    int i,s=0;
    for(i=0;i<4;i++)
    {
        s+=taille(*(p+i));
    }
    return s;
}
void *Empiler(Cell **p, int n,char t,char c)
{
Cell *nouv;
nouv=(Cell*)malloc(sizeof(Cell));
nouv->Nombre=n;
nouv->Type=t;
nouv->Couleur=c;
nouv->suivant=(*p);
*p=nouv;
}

void Depiler(Cell **p)
{
 Cell *temp;
 if(!(*p)) return;
 temp=*p;
 (*p)=(*p)->suivant;
 free(temp);
}

Cell *renverser_pile(Cell *P) //fonction qui renvse totalement la pile
{
    Cell *P1;
    P1=NULL;
    while (P!=NULL)
    {
        Empiler(&P1,P->Nombre,P->Type,P->Couleur);
        P=P->suivant;
    }
    return P1;
}

int triee(Cell *source,int n) //fonction qui permet de verifier si les premiers n élements de la pile sont triés
{                             //on a mis comme argument source car ce sont les piles de source de deplacement qui sont conernées
    int i;
        for(i=1;i<n;i++)
        {
            if(source->Nombre!=(source->suivant->Nombre)-1 || source->Couleur==source->suivant->Couleur) return 0;
            source=source->suivant;
        }
        return 1;
}

typedef struct Operations //Elle sera utilisée pour empiler toutes les opération pour pouvoir les annuler par la suite
{
    char source;
    char destination;
    int nbr_cartes;
    struct Operations *suivant;
}Operations;

void *Empiler_Operations(Operations **p, char Source,char Destination,int Nbr_cartes)
{
Operations *nouv;
nouv=(Operations*)malloc(sizeof(Operations));
nouv->source=Source;
nouv->destination=Destination;
nouv->nbr_cartes=Nbr_cartes;
nouv->suivant=(*p);
*p=nouv;
}

void Depiler_Operation(Operations **p)
{
    Operations *temp;
    if(!(*p)) return;
    temp=*p;
    (*p)=(*p)->suivant;
    free(temp);
}



//Preparation du jeu

void distribution(Cell **c) //distribution des carte alétoirement dans grande colonne de 52 cases
{
    int T[52];
    srand(time(NULL));
    int tmp=0,i,j,verif;
    char t;
    char co='x';
    for(i=0;i<52;i++)
    {
        do
        {
            verif=0;
            tmp=rand ()%52+1;
            for(j=0;j<i;j++)
            {
                if(T[j]==tmp)
                {
                    verif=1;
                }
            }
        }while (verif==1);
    T[i]=tmp;

    }
    for(i=0;i<52;i++)
    {
        if(((T[i]-1)/13)==0)
        {
            t= 6;
            co='n';
        }
        if(((T[i]-1)/13)==1)
        {
            t=5 ;
            co='n';
        }
        if(((T[i]-1)/13)==2)
        {
            t= 3 ;
            co='r';
        }
        if(((T[i]-1)/13)==3)
        {
            t= 4 ;
            co='r';
        }
        if(T[i]%13!=0) Empiler(c,T[i]%13,t,co);
        else Empiler(c,13,t,co);
    }

}

void creation_colonne(Cell *c, Cell **colonnes) //creation de 8 colonnes (de 8 ou 7 cases) en les rempliant à partir du grande colonne
{
    int i;
    int j;
    for(i=0;i<8;i++)
    {
        if(i<4)
        {
            for(j=0;j<7;j++)
            {
                Empiler(colonnes+i,c->Nombre,c->Type,c->Couleur);
                Depiler(&c);
            }
        }

        else
        {
            for(j=0;j<6;j++)
            {
                Empiler(colonnes+i,c->Nombre,c->Type,c->Couleur);
                Depiler(&c);
            }

        }
    }
}

void creation_jeu(Cell ***colonnes,Cell ***cellule_libre,Cell ***cellule_destination) // creation et initialision de toutes les cellules
{
    int i;
    Cell *c;
    c=NULL;
    *colonnes=(Cell**)malloc(8*sizeof(Cell*));
    *cellule_libre=(Cell**)malloc(4*sizeof(Cell*));
    *cellule_destination=(Cell**)malloc(4*sizeof(Cell*));
    for(i=0;i<8;i++)
    {
        *((*colonnes)+i) =NULL;
    }
    for(i=0;i<4;i++)
    {
        *((*cellule_libre)+i) =NULL;
    }
    for(i=0;i<4;i++)
    {
         *((*cellule_destination)+i)=NULL;
    }
    distribution(&c);
    creation_colonne(c,*colonnes);
}


//Fonction qui calcule le nombre de deplacements autorisés

int nbr_deplacement_valable(Cell **Colonnes,Cell **libre)// cas usuel
{
    int i,s=0,dep=0;
    for(i=0;i<8;i++)
    {
        if(estvide(*(Colonnes+i)))
            s++;
    }
    dep+=pow(2,s);
    s=1;
    for(i=0;i<4;i++)
    {
        if(estvide(*(libre+i)))
            s++;
    }
    dep*=s;
    return dep;
}

int nbr_deplacement_valable_cas_special(Cell **Colonnes,Cell **libre) //le cas où une colonne vide est la destination
{
    int i,s=-1,dep=0;
    for(i=0;i<8;i++)
    {
        if(estvide(*(Colonnes+i)))
            s++;
    }
    if(s==-1) s=0;
    dep+=pow(2,s);
    s=1;
    for(i=0;i<4;i++)
    {
        if(estvide(*(libre+i)))
            s++;
    }
    dep*=s;
    return dep;
}




//Fonctions d'affichage
void Color(int t,int f) //fonction pour faciliter le choix du couleur
{
    HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H,f*16+t);
}

void affichageelement(Cell *p) //afficher le sommet de la pile
{

    char c;
    if(p==NULL)
    {   Color(9,0);
        printf("[  - ]  ");
    }
    else
    {
        if(p->Nombre==1) c='A';
    if(p->Nombre==11) c='J';
    if(p->Nombre==12) c='Q';
    if(p->Nombre==13) c='K';

    if(p->Couleur=='n')
    {
        Color(0,7);
        if (p->Nombre>10||p->Nombre<2)
        {
            printf("[ %c-%c]",c,p->Type);
            Color(0,0);
            printf("  ");
        }

        else
        {
            printf("[%2d-%c]",p->Nombre,p->Type);
            Color(0,0);
            printf("  ");
        }
    }
    else
    {
        Color(12,7);
        if (p->Nombre>10||p->Nombre<2)
        {
            printf("[ %c-%c]",c,p->Type);
            Color(0,0);
            printf("  ");
        }
        else
        {
            printf("[%2d-%c]",p->Nombre,p->Type);
            Color(0,0);
            printf("  ");
        }
    }
    }
}

void affichage_temps(clock_t temps)
{
    int h=0,m=0,s=0;
    clock_t t;
    t=clock()-temps;
    h=((int) t/CLOCKS_PER_SEC)/3600;
    m=((int) t/CLOCKS_PER_SEC -h*3600)/60;
    s=(int) t/CLOCKS_PER_SEC -h*3600 -m*60;
    printf("temps ecoule:   %2d:%2d:%2d",h,m,s);
}
void affichage_jeu(Cell **colonnes,Cell **cellule_libre,Cell **cellule_destination,int couts_joues,clock_t temps) //afficher toutes les cellules du jeu
{
    int i;
    Cell **temp;
    temp=(Cell**)malloc(8*sizeof(Cell*));
    Color(15,0);
    printf("\nNombre de deplacements disponible:%d\n\n ",nbr_deplacement_valable(colonnes,cellule_libre));
    for(i=0;i<4;i++)
    {
        affichageelement(*(cellule_libre+i));
        Color(0,0);
    }
    printf("\t  ");
    for(i=0;i<4;i++)
    {
        affichageelement(*(cellule_destination+i));
        Color(0,0);
    }
    Color(14,0);
    printf("\n    A       B       C       D         \t     E       F       G       H\n\n");
    for(i=0;i<8;i++)                                // L'Affichage va passer par 8 cellules temporaires qui sont l'inverse des cellules originales
    {
        *(temp+i)=*(colonnes+i);
    }

    for(i=0;i<8;i++)
    {
        *(temp+i)=renverser_pile(*(temp+i));
    }
    printf("\n");
    while(*(temp)!=NULL||*(temp+1)!=NULL||*(temp+2)!=NULL||*(temp+3)!=NULL||*(temp+4)!=NULL||*(temp+5)!=NULL||*(temp+6)!=NULL||*(temp+7)!=NULL)
    {   Color(0,0);
        printf("      ");
        for(i=0;i<8;i++)

        {   if((*(temp+i)))
            {   affichageelement(*(temp+i));
                *(temp+i)=(*(temp+i))->suivant;
            }
            else {Color(0,0);;printf("        ");}
        }
        printf("\n");
    }
    Color(14,0);
    printf("\n \t I       J       K       L       M       N       O       P\n");
    Color(15,0);
    printf("\n\nCartes restantes:%d \t\t\t\t\t Coups joues:%d\n",52-taille_tableau_cellule(cellule_destination),couts_joues);
    printf("\t\t\t");affichage_temps(temps);
    printf("\n");
}
void congratulation(int coups_joues,clock_t temps)//Message
{
    Color(14,0);
    printf("\n*******************************************************************************\n");
    printf("\n*******************************************************************************\n");
    printf("\n*******************************************************************************\n");
    printf("\n***************************CONGRATULATION!!!!**********************************\n");
    printf("\n****************************VOUS AVEZ GANGE!***********************************\n");
    printf("\n*******************************************************************************\n");
    printf("\n*******************************************************************************\n");
    printf("\n*******************************************************************************\n");
    Color(15,0);
    printf("\n\n  temps ecoule ");affichage_temps(temps);
    printf("\t\t\t\tCoups joues:%d\n\n",coups_joues);

    getch();
}


void Regles()//Message
{
    Color(14,0);
    printf("\n\nObjectif\n\n");
    Color(15,0);
    printf("Creez quatre piles de 13 cartes, une par couleur dans chacune des quatre \ncellules de destination. Chaque pile doit etre composee en allant de la carte la plus faible (as) a la carte la plus forte (roi).\n\n\n");
    Color(14,0);
    printf("Table\n\n");
    Color(15,0);
    printf("FreeCell se joue avec un seul jeu de 52 cartes, toutes revelees, reparties sur \nhuit colonnes. Vous devez tirer des cartes de ces colonnes pour former \nvos quatre piles.\n\n");
    printf("Dans le coin superieur gauche se trouvent quatre cellules libres, ou vous \nstockez temporairement des cartes en cours de jeu.\n\n");
    printf("Dans le coin superieur droit se trouvent quatre cellules de destination, ou vous formez les piles necessaires pour gagner.\n\n\n");
    Color(14,0);
    printf("Regles:\n\n");
    Color(15,0);
    printf("Tirez des cartes au bas de chaque colonne et deplacez-les comme suit :\n\n"),
    printf("D une colonne a une cellule libre. Une seule carte peut occuper chaque cellule \nlibre a la fois.\n\n");
    printf("D une colonne a une autre colonne (ou d une cellule libre a une colonne). Les \ncartes doivent etre placees dans une colonne dans l ordre decroissant en \nalternant les couleurs (rouge et noir).\n\n");
    printf("D une colonne a une cellule de destination. Chaque pile doit comporter une seule couleur et commencer par un as.\n");
}

void Comment_jouer()//Message
{
    Color(14,0);
    printf("Pour afficher l AIDE\n");
    Color(15,0);
    printf("\ntaper ?\n");
    Color(14,0);
    printf("\nPour un deplacement automatiquemnt vers la cellule de destination\n");
    Color(15,0);
    printf("\ntaper T\n");Color(14,0);
    printf("\nPour annuler\n");
    Color(15,0);
    printf("\ntaper Z\n");
    Color(14,0);
    printf("\nPour deplacer une carte :\n");
    Color(15,0);
    printf("\nChoisissez la lettre correspondante a votre carte source \nSuivie de / \nSuivie la lettre correspondante a votre carte destination\nsuivie de / \nSuivie de nombre de carte a deplacer ");
    printf("\nExemple d'utilisation: A/G/3");
    printf("\nExemple d'utilisation: E/D/1");
    printf("\nExemple d'utilisation: H/B/6");
    Color(12,0);
    printf("\n\n\n\t\t\tAttention au MAJISCULE");
}


//Fonctions de deplacement

void deplacer(Cell **source,Cell **destination,int n) //fonction concernant tous ls cas(pas de condition)
{
    int i;
    Cell **temp;
    temp=(Cell**)malloc(sizeof(Cell*));
    if((*source)!=NULL)
    {
        for(i=0;i<n;i++)
        {
            Empiler(temp,(*source)->Nombre,(*source)->Type,(*source)->Couleur);
            Depiler(source);
        }
        for(i=0;i<n;i++)
        {
            Empiler(destination,(*temp)->Nombre,(*temp)->Type,(*temp)->Couleur);
            Depiler(temp);
        }
    }
}
//Ces fonction retourne 1 si le deplacement est effectué et 0 sinon
int deplacer_vers_destination(Cell **source,Cell **destination)//deplacer vers cellules de destination
{
     if((*source)!=NULL && ((((*destination)==NULL) && (*source)->Nombre==1) || ((*destination)!=NULL &&(*source)->Type==(*destination)->Type && (*source)->Nombre==(*destination)->Nombre +1)) )
    {
        deplacer(source,destination,1);
        return 1;
    }
    else
    { //  printf("\ndeplacement vers destination impossible\n");
        return 0;
    }
}

int deplacer_vers_libre(Cell **source,Cell **destination) //deplacer vers cellules libres
{
    if((*source)!=NULL && (*destination)==NULL &&(*destination)!=(*source))
    {
        deplacer(source,destination,1);
        return 1;
    }
    else
    {
       // printf("\ndeplacement vers libre impossible\n");
        return 0;
    }
}

int deplacer_libre_vers_colonne(Cell **source,Cell **destination) //deplacer à partir d'une cellule libre vers une colonne de jeu
{
    if((*source)!=NULL && ((*destination)==NULL||((*destination)!=NULL && (*source)->Nombre==(*destination)->Nombre-1 && (*source)->Couleur!=(*destination)->Couleur)))
    {
        deplacer(source,destination,1);
        return 1;
    }
    else
    {
      //  printf("\ndeplacement de libre vers colonnes impossible\n");
        return 0;
    }
}

int deplacer_colonne_vers_colonne(Cell **source,Cell **destination,Cell **colonnes,Cell **libre,int n)//deplacer à partir d'une colonne de jeu vers une colonne de jeu
{
    Cell *temp;
    int i=1;
    temp=*source;
    if((*source)!=NULL && taille(*source)>=n && (*destination)!=(*source) && triee(*source,n))
    {
        while(i<n)
        {
            temp=temp->suivant;
            i++;
        }
    }
    else
    {
       // printf("\ndeplacement de colonne vers colonne impossible1\n");
        return 0;
    }
    if(((*destination)!=NULL && temp->Nombre==(*destination)->Nombre -1 && (temp)->Couleur!=(*destination)->Couleur && nbr_deplacement_valable(colonnes,libre)>=n)||((*destination)==NULL && n<=nbr_deplacement_valable_cas_special(colonnes,libre)) )
    {
        deplacer(source,destination,n);
        return 1;
    }
    else
    {
       // printf("\ndeplacement de colonnes vers colonne impossible2\n");
        return 0;
    }
}

int deplacement_Finale(char source,char dest,int i,Cell **destination,Cell **libre,Cell **colonnes)// fonction qui englobe tous le deplacement, elle prends en argument le choix source comme source et dest comme destionation
{
    if(dest>68 && dest<73) //deplacer vers Cellule de destination (caractére entre E et H)
    {
        if(source>64 && source<69)//à partir de Cellule de libre (caractére entre A et D)
        {
            if(deplacer_vers_destination(libre+(source-65),destination +(dest-69) )) return 1;
        }
        else
        {
            if(source>72 && source<81)//à partir de Colonne de jeu (caractére entre I et P)
            {
                if(deplacer_vers_destination(colonnes+(source-73),destination +(dest-69) )) return 1;
            }
            else
            {
   //             printf("\ndeplacement impossible1\n");
                return 0;
            }
        }
    }
    if(dest>64 && dest<69)//deplacer vers Cellule libre (caractére entre A et D)
    {
        if(source>64 && source<69)//à partir de Cellule de libre (caractére entre A et D)
        {
            if(deplacer_vers_libre(libre+(source-65),libre +(dest-65) )) return 1;
        }
        else
        {
            if(source>72 && source<81)//à partir de Colonne de jeu (caractére entre I et P)
            {
                if(deplacer_vers_libre(colonnes+(source-73),libre +(dest-65) )) return 1;
            }
            else
                {
     //               printf("\ndeplacement impossible2\n");
                    return 0 ;
                }
        }
    }
    if(dest>72 && dest<81)//deplacer vers Colonne de jeu (caractére entre I et P)
    {
        if(source>64 && source<69)//à partir de Cellule de libre(caractére entre A et D)
        {
           if(deplacer_libre_vers_colonne(libre+(source-65),colonnes +(dest-73) )) return 1;

        }
        else
        {
            if(source>72 && source<81)//à partir de Colonne de jeu (caractére entre I et P)
            {
                if(deplacer_colonne_vers_colonne(colonnes+(source-73),colonnes +(dest-73),colonnes,libre,i )) return 1;
            }
            else
                {
       //             printf("\ndeplacement impossible3\n");
                    return 0;
                }
        }

    }

    //printf("\ndeplacement impossible4\n");
    return 0;


}

void annuller(char source,char dest,int n,Cell **destination,Cell **libre,Cell **colonnes)//Inverser l'operation
{
    if(dest>68 && dest<73) //deplacer vers Cellule de destination
    {
        if(source>64 && source<69)//à partir de Cellule de libre
        {
            deplacer(libre+(source-65),destination +(dest-69),1);
        }

        if(source>72 && source<81)//à partir de Colonne de jeu
        {
            deplacer(colonnes+(source-73),destination +(dest-69),1);
        }


    }
    if(dest>64 && dest<69)//deplacer vers Cellule libre
    {
        if(source>64 && source<69)//à partir de Cellule de libre
        {
            deplacer(libre+(source-65),libre +(dest-65),1);
        }

        if(source>72 && source<81)//à partir de Colonne de jeu
        {
            deplacer(colonnes+(source-73),libre +(dest-65),1);
        }
        if(source>68 && source<73)//à partir de Cellule de destination
        {
            deplacer(destination+(source-69),libre +(dest-65),1);
        }

    }
    if(dest>72 && dest<81)//deplacer vers Colonne de jeu
    {
        if(source>64 && source<69)//à partir de Cellule de libre
        {
           deplacer(libre+(source-65),colonnes +(dest-73),1);
        }

        if(source>72 && source<81)//à partir de Colonne de jeu
        {
            deplacer(colonnes+(source-73),colonnes +(dest-73),n);
        }
        if(source>68 && source<73)//à partir de Cellule de destination
        {
            deplacer(destination+(source-69),colonnes +(dest-73),1);
        }
    }
}

void deplacement_automatique_vers_cellule_de_destination(Cell **colonnes,Cell **cellule_libre,Cell **cellule_destination,int *coups_joues,Operations **Op)
{
    int i,j;
    for (i=0;i<4;i++)           //deplacement vers cellule de destination
    {
        for(j=0;j<8;j++)        //deplacement depuis colonnes de jeu
        {
            if (deplacer_vers_destination(colonnes+j,cellule_destination+i))
            {
                (*(coups_joues))++;
                Empiler_Operations(Op,73+j,69+i,1);
            }
        }
        for(j=0;j<4;j++)        //deplacement depuis cellules libres
        {
            if (deplacer_vers_destination(cellule_libre+j,cellule_destination+i))
            {
                (*(coups_joues))++;
                Empiler_Operations(Op,65+j,69+i,1);
            }

        }
    }
}

//Fonctions de jeu
void aide(Cell **colonnes,Cell **cellule_libre,Cell **cellule_destination)
{
    int i,j,n;
    printf("\nAIDE:\n");
    for (i=0;i<4;i++)           //deplacement vers collones de destination
    {
        for(j=0;j<8;j++)        //deplacement depuis collones de jeu
        {
            if(deplacer_vers_destination(colonnes+j,cellule_destination+i))
            {
                printf("%c/%c/1\n",j+73,i+69);
                deplacer(cellule_destination+i,colonnes+j,1);
            }
        }
        for(j=0;j<4;j++)        //deplacement depuis colonnes libres
        {
            if(deplacer_vers_destination(cellule_libre+j,cellule_destination+i))
            {
                printf("%c/%c/1\n",j+65,i+69);
                deplacer(cellule_destination+i,cellule_libre+j,1);
            }
        }
    }

    for (i=0;i<8;i++)           //deplacement vers collones de jeu
    {
        for(j=0;j<8;j++)        //deplacement vers collones de jeu
        {
            for(n=1;n<=13;n++)
            {
                if(deplacer_colonne_vers_colonne(colonnes+j,colonnes+i,colonnes,cellule_libre,n))
                {   printf("%c/%c/%d\n",j+73,i+73,n);
                    deplacer(colonnes+i,colonnes+j,n);
                }
            }
        }
        for(j=0;j<4;j++)        //deplacement depuis colonnes libres
        {
            if (deplacer_libre_vers_colonne(cellule_libre+j,colonnes+i))
            {
                printf("%c/%c/1\n",j+65,i+73);
                deplacer(colonnes+i,cellule_libre+j,1);
            }
        }
    }

    for (i=0;i<4;i++)           //deplacement vres colonnes libres
    {
        for(j=0;j<8;j++)        //deplacement depuis collones de jeu
        {
            if (deplacer_vers_libre(colonnes+j,cellule_libre+i))
            {
                printf("%c/%c/1\n",j+73,i+65);
                deplacer(cellule_libre+i,colonnes+j,1);
            }

        }
        for(j=0;j<4;j++)        //deplacement depuis colonnes libres
        {
            if (deplacer_vers_libre(cellule_libre+j,cellule_libre+i))
            {
                printf("%c/%c/1\n",j+65,i+65);
                deplacer(cellule_libre+i,cellule_libre+j,1);
            }
        }
    }
}

void jouer(Cell **colonnes,Cell **cellule_libre,Cell **cellule_destination)// la fonction qui englobe tout le jeu
{
    clock_t temps;
    temps=clock();
    int choix;
    int coups_joues=0;
    int n;
    char source,destination,help;
    Operations *Op=NULL;
    printf("\t\t\t *******FREECELL*******\n\n\n\n");
    do
    {   Color(7,0);
        printf("Menu:\n");
        printf("-------------------------------------------\n");
        Color(15,0);
        printf("Preciser votre choix et cliquer sur Entrer:\n 1-Jouer \n 2-Regles et principes de base\n 3-Comment jouer\n 4-Quitter\n");
        Color(12,0);
        printf("\n\n\n\nRemarque :\nSi vous choisissez de lire Regles et principe ou Comment jouer\nCliquer sur n'impOrte quelle touche pour revenir au menu\n\n");
        Color(7,0);
        printf("Choix:  ");
        scanf("%d",&choix);
        if(choix==1)

        {   coups_joues=0;
            system ("cls");
            temps=clock();
            creation_jeu(&colonnes,&cellule_libre,&cellule_destination);
            while(taille_tableau_cellule(cellule_destination)<52)
            {
                affichage_jeu(colonnes,cellule_libre,cellule_destination,coups_joues,temps);
                Color(7,0);
                printf("operation :   ");
                scanf("%c/%c/%d%c",&source,&destination,&n,&help);
                if(source=='?' || source=='Z'|| source=='T')
                {
                    if(source=='?')
                    {
                        aide(colonnes,cellule_libre,cellule_destination);
                        getch();
                        system ("cls");
                    }
                    if(source=='T')
                    {
                        deplacement_automatique_vers_cellule_de_destination(colonnes,cellule_libre,cellule_destination,&coups_joues,&Op);
                        system ("cls");
                    }
                    if(source=='Z')
                    {
                        if(Op!=NULL)
                        {
                            annuller(Op->destination,Op->source,Op->nbr_cartes,cellule_destination,cellule_libre,colonnes);
                            Depiler_Operation(&Op);
                            coups_joues++;
                        }
                        system ("cls");
                    }

                }

                else
                {
                    system ("cls");
                    if(deplacement_Finale(source,destination,n,cellule_destination,cellule_libre,colonnes))
                    {
                        coups_joues++;
                        Empiler_Operations(&Op,source,destination,n);
                    }
                    else
                        printf("\nDeplacement de %d carte de %c vers %c impossible\n",n,source,destination);
                }
            }
            congratulation(coups_joues,temps);
            while(Op!=NULL)
                Depiler_Operation(&Op);
        }
        if(choix==2)
        {system ("cls");Regles() ;getch();}
        if(choix==3)
        {system ("cls");Comment_jouer();getch();}
        system ("cls");

    }while(choix!=4);


}


int main()
{
    Cell **colonnes;
    Cell **cellule_libre;
    Cell **cellule_destination;
    jouer(colonnes,cellule_libre,cellule_destination);
    return 0;
}
