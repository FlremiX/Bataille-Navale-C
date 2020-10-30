#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

//Définition des couleurs
#define normal "\x1b[0m" //Message normal (sans coloration)
#define bateaujoueur "\x1b[33m" //Case bateau joueur [JAUNE]
#define caseau "\x1b[34m" //Case eau [BLEU]
#define touche "\x1b[32m" //Case touché [VERT]
#define touchejoueur "\x1b[31m" //Un bateau du joueur est touché [ROUGE]
#define gras "\x1b[1m" //Balise gras
#define dgras "\x1b[22m" //Balise fin de gras
#define clear "\33[2J\n" //Clear la sortie du terminal

char *lgrille = "ABCDEFGHIJ"; //Lettre de la grille (utilisé pour la génération de grille)
char possiblecas[5]= "01234"; //Valeurs de case attribuable
char IAdisplaygrille[100] ="0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", IAhidegrille[100] ="0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", joueur[100]="0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; //Toutes les caches sont cachées [VALEUR PAR DÉFAUT : 0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]
char patern[100] = "1010101010010101010110101010100101010101101010101001010101011010101010010101010110101010100101010101"; //Pattern IA pour le damier

int grille(), IA(), placementlarg5(), placementlarg4(), placementlarg3(), placementlarg2(), joueurimput(), lettregrilleid(), IAtestcase(), nplacement(), debugmode();
int IAcoup[2]={404,404}; //Stock les dernières cases testées par l'IA (les 3 dernières)
int temp[7]={17,17,0, 00,1, 0, -3}; //Stock des informations diverses -> temp[0] -> Nombre de cases avec bateaux de l'IA touché par le joueur (valeur par défaut : 17) | temp[1] -> Nombre de cases avec bateaux du joueur touché par l'IA (valeur par défaut : 17) | temp[2] -> Nombre de tour écoulé | temp[3] -> option de debug | temp[4] -> difficultée | temp[5] -> afin de savoir comment quel patterne de damier il faut appliquer en fonction de la dernière rangé de damier

/*
Valeur de case = 0 : Case non-testée
Valeur de case = 1 : Eau
Valeur de case = 2 : Case de bateau IA/Ennemie 
Valeur de case = 3 : Case de de bateau joueur
Valeur de case = 4 : Case de bateau joueur touchée

Bateaux dispos (5 au total) : 
x1 Porte-Avion (5 cases)
x1 Croiseur (4 cases)
x2 sous-marins (3 cases 1 navire)
x1 torpilleur (2 cases)

Code de debug :
01 : log tout les modules logiques de l'IA
02 : IA en mode autonome (elle joue toute seul)
03 : IA en mode autonome et log activé 
*/

int main(int argc, char *arg[])
{
    if(argc>1){debugmode(arg[1]);}
    printf(bateaujoueur "Bienvenue dans le meilleur jeu de tout les temps qui est même pas encore totalement codé ! Ce jeu est génial vu que c'est moi qui l'ai dev (oui c'est de l'auto-fellation)\n" normal);
    joueurimput(2);
    grille(joueur);
    joueurimput(0);
    printf(gras "[L'IA est en train de placer ses bateaux toute seule comme une grande :D]\n" normal);
    IA(0);
    printf(gras "[SYMBOLES :]\n" dgras "X : Case non testée\n" caseau "X : Case d'eau\n" touche "O : Bateau ennemi touché\n" touchejoueur "O : Un de vos bateau a été touché\n" bateaujoueur "■ : Un de vos bateaux\n\n" normal "Voici une grille vide pour vous aider à placer vos bateaux :\n");
    printf(clear"\nGrille du joueur :\n");
    grille(joueur);
    if(temp[3] == 02 || temp[3] == 03){printf(clear "\nTest de dev : Test de l'IA (en mode autonome)\n");}
    int play = rand()%2; //Pour déterminer si c'est l'IA ou je le joueur qui joue en premier -> 0 : joueur, 1 -> IA
    if(play==0){printf("C'est vous qui jouez en premier !\n");}else{printf("C'est l'IA qui joue en premier !\n");}
    while (true)
    {
        if(play==0 && temp[3] != 02 && play==0 && temp[3] != 03){
            joueurimput(1);
            IA(1);
        }else if(play==1 && temp[3] != 02 && play==1 && temp[3] != 03){
            IA(1);
            joueurimput(1);
        }else{
            IA(1);
        }
        temp[2]++;
        printf("Tour : %d - Vos vie restante : [%d/17]\nLes vie restante de l'IA : [%d/17]\n" gras "[SYMBOLES :]\n" dgras "X : Case non testée\n" caseau "X : Case d'eau\n" touche "O : Bateau ennemi touché\n" touchejoueur "O : Un de vos bateau a été touché\n" bateaujoueur "■ : Un de vos bateaux\n" normal "Votre grille :\n\n", temp[2], temp[1], temp[0]);
        grille(joueur);
        printf("\n\nGrille de l'IA :\n\n");
        grille(IAdisplaygrille);
        if(temp[1]==0 || temp[1]<0){
            printf(touchejoueur gras ">>> Vous avez perdu(e) <<<" normal "\nVoici la grille de l'IA :\n");
            grille(IAhidegrille);
            break;
        }else if(temp[0]==0 || temp[0]<0){
            printf(touche gras ">>> Vous avez gagné <<<\nIl vous restait encore " gras "%d" normal touche gras " vie(s) !\n" normal, temp[1]);
            break;
        }
    }
    exit(0);
}

int grille(char *memvalue)
{
    float place=0;
    int test=0, loop=0;
    char *temp = memvalue;
    for (int i = 0; i < 10; i++)
    {
        if(loop==0){
            printf("    1   2   3   4   5   6   7   8   9   10\nA   ");
            if(temp[0] ==possiblecas[0]){printf(normal "X   ");}else if(temp[0] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[0]==possiblecas[2]){printf(touche "O   ");}else if(temp[0]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[0]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[1] ==possiblecas[0]){printf(normal "X   ");}else if(temp[1] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[1]==possiblecas[2]){printf(touche "O   ");}else if(temp[1]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[1]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[2] ==possiblecas[0]){printf(normal "X   ");}else if(temp[2] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[2]==possiblecas[2]){printf(touche "O   ");}else if(temp[2]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[2]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[3] ==possiblecas[0]){printf(normal "X   ");}else if(temp[3] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[3]==possiblecas[2]){printf(touche "O   ");}else if(temp[3]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[3]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[4] ==possiblecas[0]){printf(normal "X   ");}else if(temp[4] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[4]==possiblecas[2]){printf(touche "O   ");}else if(temp[4]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[4]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[5] ==possiblecas[0]){printf(normal "X   ");}else if(temp[5] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[5]==possiblecas[2]){printf(touche "O   ");}else if(temp[5]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[5]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[6] ==possiblecas[0]){printf(normal "X   ");}else if(temp[6] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[6]==possiblecas[2]){printf(touche "O   ");}else if(temp[6]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[6]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[7] ==possiblecas[0]){printf(normal "X   ");}else if(temp[7] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[7]==possiblecas[2]){printf(touche "O   ");}else if(temp[7]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[7]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[8] ==possiblecas[0]){printf(normal "X   ");}else if(temp[8] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[8]==possiblecas[2]){printf(touche "O   ");}else if(temp[8]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[8]==possiblecas[4]){printf(touchejoueur "O   ");}
            if(temp[9] ==possiblecas[0]){printf(normal "X\n");}else if(temp[9] ==possiblecas[1]){printf(caseau "X\n");}else if(temp[9]==possiblecas[2]){printf(touche "O\n");}else if(temp[9]==possiblecas[3]){printf(bateaujoueur "■\n");}else if(temp[9]==possiblecas[4]){printf(touchejoueur "O\n");}
            printf("");
            loop++;
            }else{
                printf(normal "%c   ", lgrille[loop]);
                if(temp[10*i] ==possiblecas[0]){printf(normal "X   ");}else if(temp[10*i] ==possiblecas[1]){printf(caseau "X   ");}else if(IAdisplaygrille[10*i]==possiblecas[2]){printf(touche "O   ");}else if(temp[10*i]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[10*i]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[1+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[1+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[1+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[1+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[1+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[2+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[2+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[2+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[2+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[2+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[3+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[3+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[3+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[3+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[3+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[4+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[4+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[4+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[4+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[4+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[5+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[5+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[5+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[5+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[5+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[6+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[6+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[6+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[6+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[6+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[7+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[7+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[7+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[7+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[7+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[8+(10*i)] ==possiblecas[0]){printf(normal "X   ");}else if(temp[8+(10*i)] ==possiblecas[1]){printf(caseau "X   ");}else if(temp[8+(10*i)]==possiblecas[2]){printf(touche "O   ");}else if(temp[8+(10*i)]==possiblecas[3]){printf(bateaujoueur "■   ");}else if(temp[8+(10*i)]==possiblecas[4]){printf(touchejoueur "O   ");}
                if(temp[9+(10*i)] ==possiblecas[0]){printf(normal "X\n");}else if(temp[9+(10*i)] ==possiblecas[1]){printf(caseau "X\n");}else if(temp[9+(10*i)]==possiblecas[2]){printf(touche "O\n");}else if(temp[9+(10*i)]==possiblecas[3]){printf(bateaujoueur "■\n");}else if(temp[9+(10*i)]==possiblecas[4]){printf(touchejoueur "O\n");}
                loop++;
            }
    }
    printf(normal);
}

int joueurimput(int type){
    /*
    Gère les données saisie par l'utilisateur
    type 0 : Placement des navires du joueur
    type 1 : Tour du joueur
    */
   char userimput[5],*toconvert=malloc(256);
   int textoint;
   if(type==0){
       int state=0; //state permet de savoir où l'utilisateur en est dans le placement de ses navires (pour ne pas devoir tout reprendre depuis le début en cas de valeurs incorrecte)
       while (true)
       {
       int place1=0, place2=0;//Case du tableaux correspondant au début indiqué par le joueur (return) et a la fin (return)
       if(state==0){
       printf("Où le porte-avion doit-être placé ? (donner le début) [5 CASES - " gras "METTRE ''Q'' POUR QUITTER LE PROGRAMME" normal "]\n>");
       scanf("%s", userimput);
       int check =lettregrilleid(userimput, 0, 0); 
       if(check==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(check==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint=atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               place1=check+textoint;
               printf("Quel est la case de fin ? [Q pour quitter le programme]\n>");
                      scanf("%s", userimput);
       if(lettregrilleid(userimput, 0, 0)==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint = atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               check=lettregrilleid(userimput, 0, 0);
               place2=check+textoint;
                place1--, place2--;
               int totest=place1-place2;
               if(totest<0 && totest !=5 && totest !=-5){totest--;}else if(totest>0 && totest !=5 && totest !=-5){totest++;} //Permet de placer les bateaux aux cases indiquées sans avoir de décallage et permet de savoir si un bateau est placé sur la largeur ou non
               if(place2>39 && totest!=5 && totest<0 && totest!=-5 && totest<0){
                   //Placement par le bas
                   if(joueur[place1]!=possiblecas[3] && joueur[place1+10]!=possiblecas[3] && joueur[place1+20]!=possiblecas[3] && joueur[place1+30]!=possiblecas[3] && joueur[place1+40]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1+10]='3', joueur[place1+20]='3', joueur[place1+30]='3', joueur[place1+40]='3'; 
                    state=1;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
               }else if(place1>39 && totest>0 && totest!=5 && totest!=-5){
                   //Placement par le haut
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-10]!=possiblecas[3] && joueur[place1-20]!=possiblecas[3] && joueur[place1-30]!=possiblecas[3] && joueur[place1-40]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-10]='3', joueur[place1-20]='3', joueur[place1-30]='3', joueur[place1-40]='3'; 
                    state=1;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
                }else if(place1>-1 && place1<10 && totest==5 || place1>10 && place2<20 && totest==5 || place1>20 && place2<30 && totest==5 || place1>30 && place2<40 && totest==5 || place1>40 && place2<50 && totest==5 || place1>50 && place2<60 && totest==5 || place1>60 && place2<70 && totest==5 || place1>70 && place2<80 && totest==5 || place1>80 && place2<90 && totest==5 || place1>90 && place2<100 && totest==5){
                    //Placement par la largeur
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-1]!=possiblecas[3] && joueur[place1-2]!=possiblecas[3] && joueur[place1-3]!=possiblecas[3] && joueur[place1-4]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-1]='3', joueur[place1-2]='3', joueur[place1-3]='3', joueur[place1-4]='3'; 
                    state=1;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else if(place1>-1 && place1<10 && totest==-5 || place2>10 && place1<20 && totest==-5 || place2>20 && place1<30 && totest==-5 || place2>30 && place1<40 && totest==-5 || place2>40 && place1<50 && totest==-5 || place2>50 && place1<60 && totest==-5 || place2>60 && place1<70 && totest==-5 || place2>70 && place1<80 && totest==-5 || place2>80 && place1<90 && totest==-5 || place2>90 && place1<100 && totest==-5){
                    //Placement par la largeur
                    if(joueur[place2]!=possiblecas[3] && joueur[place2-1]!=possiblecas[3] && joueur[place2-2]!=possiblecas[3] && joueur[place2-3]!=possiblecas[3] && joueur[place2-4]!=possiblecas[3]){
                    joueur[place2]='3', joueur[place2-1]='3', joueur[place2-2]='3', joueur[place2-3]='3', joueur[place2-4]='3'; 
                    state=1;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else{printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}
           }
            }
           }
       }
       }else if(state==1){
       printf("Où le croiseur doit-être placé ? (donner le début) [4 CASES - " gras "METTRE ''Q'' POUR QUITTER LE PROGRAMME" normal "]\n>");
       scanf("%s", userimput);
       int check =lettregrilleid(userimput, 0, 0);
       if(check==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint=atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               place1=check+textoint;
               printf("Quel est la case de fin ? [Q pour quitter le programme]\n>");
                      scanf("%s", userimput);
       if(lettregrilleid(userimput, 0, 0)==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint = atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               check=lettregrilleid(userimput, 0, 0);
               place2=check+textoint;
                place1--, place2--;
               int totest=place1-place2;
               if(totest<0 && totest !=4 && totest !=-4){totest--;}else if(totest>0 && totest !=4 && totest !=-4){totest++;} //Permet de placer les bateaux aux cases indiquées sans avoir de décallage et permet de savoir si un bateau est placé sur la largeur ou non
               if(place2>29 && totest!=4 && totest<0 && totest!=-4 && totest<0){
                   //Placement par le bas
                   if(joueur[place1]!=possiblecas[3] && joueur[place1+10]!=possiblecas[3] && joueur[place1+20]!=possiblecas[3] && joueur[place1+30]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1+10]='3', joueur[place1+20]='3', joueur[place1+30]='3'; 
                    state=2;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
               }else if(place1>29 && totest>0 && totest!=4 && totest!=-4){
                   //Placement par le haut
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-10]!=possiblecas[3] && joueur[place1-20]!=possiblecas[3] && joueur[place1-30]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-10]='3', joueur[place1-20]='3', joueur[place1-30]='3'; 
                    state=2;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
                }else if(place1>-1 && place1<10 && totest==4 || place1>10 && place2<20 && totest==4 || place1>20 && place2<30 && totest==4 || place1>30 && place2<40 && totest==4 || place1>40 && place2<50 && totest==4 || place1>50 && place2<60 && totest==4 || place1>60 && place2<70 && totest==4 || place1>70 && place2<80 && totest==4 || place1>80 && place2<90 && totest==4 || place1>90 && place2<100 && totest==4){
                    //Placement par la largeur
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-1]!=possiblecas[3] && joueur[place1-2]!=possiblecas[3] && joueur[place1-3]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-1]='3', joueur[place1-2]='3', joueur[place1-3]='3'; 
                    state=2;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else if(place1>-1 && place1<10 && totest==-4 || place2>10 && place1<20 && totest==-4 || place2>20 && place1<30 && totest==-4 || place2>30 && place1<40 && totest==-4 || place2>40 && place1<50 && totest==-4 || place2>50 && place1<60 && totest==-4 || place2>60 && place1<70 && totest==-4 || place2>70 && place1<80 && totest==-4 || place2>80 && place1<90 && totest==-4 || place2>90 && place1<100 && totest==-4){
                    //Placement par la largeur
                    if(joueur[place2]!=possiblecas[3] && joueur[place2-1]!=possiblecas[3] && joueur[place2-2]!=possiblecas[3] && joueur[place2-3]!=possiblecas[3]){
                    joueur[place2]='3', joueur[place2-1]='3', joueur[place2-2]='3', joueur[place2-3]='3'; 
                    state=2;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else{printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}
           }
            }
           }
       }
       }else if(state ==2 || state==3){
       printf("Où le sous-marin (%d/2) doit-être placé ? (donner le début) [3 CASES - " gras "METTRE ''Q'' POUR QUITTER LE PROGRAMME" normal "]\n>", state-1);
       scanf("%s", userimput);
       int check =lettregrilleid(userimput, 0, 0);
       if(check==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint=atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               place1=check+textoint;
               printf("Quel est la case de fin ? [Q pour quitter le programme]\n>");
                      scanf("%s", userimput);
       if(lettregrilleid(userimput, 0, 0)==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint = atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               check=lettregrilleid(userimput, 0, 0);
               place2=check+textoint;
                place1--, place2--;
               int totest=place1-place2;
               if(totest<0 && totest !=3 && totest !=-3){totest--;}else if(totest>0 && totest !=3 && totest !=-3){totest++;} //Permet de placer les bateaux aux cases indiquées sans avoir de décallage et permet de savoir si un bateau est placé sur la largeur ou non
               if(place2>19 && totest!=3 && totest<0 && totest!=-3 && totest<0){
                   //Placement par le bas
                   if(joueur[place1]!=possiblecas[3] && joueur[place1+10]!=possiblecas[3] && joueur[place1+20]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1+10]='3', joueur[place1+20]='3'; 
                    if(state==2){state=3;}else{state=4;}
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
               }else if(place1>19 && totest>0 && totest!=3 && totest!=-3){
                   //Placement par le haut
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-10]!=possiblecas[3] && joueur[place1-20]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-10]='3', joueur[place1-20]='3'; 
                    if(state==2){state=3;}else{state=4;}
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
                }else if(place1>-1 && place1<10 && totest==3 || place1>10 && place2<20 && totest==3 || place1>20 && place2<30 && totest==3 || place1>30 && place2<40 && totest==3 || place1>40 && place2<50 && totest==3 || place1>50 && place2<60 && totest==3 || place1>60 && place2<70 && totest==3 || place1>70 && place2<80 && totest==3 || place1>80 && place2<90 && totest==3 || place1>90 && place2<100 && totest==3){
                    //Placement par la largeur
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-1]!=possiblecas[3] && joueur[place1-2]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-1]='3', joueur[place1-2]='3'; 
                    if(state==2){state=3;}else{state=4;}
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else if(place1>-1 && place1<10 && totest==-3 || place2>10 && place1<20 && totest==-3 || place2>20 && place1<30 && totest==-3 || place2>30 && place1<40 && totest==-3 || place2>40 && place1<50 && totest==-3 || place2>50 && place1<60 && totest==-3 || place2>60 && place1<70 && totest==-3 || place2>70 && place1<80 && totest==-3 || place2>80 && place1<90 && totest==-3 || place2>90 && place1<100 && totest==-3){
                    //Placement par la largeur
                    if(joueur[place2]!=possiblecas[3] && joueur[place2-1]!=possiblecas[3] && joueur[place2-2]!=possiblecas[3]){
                    joueur[place2]='3', joueur[place2-1]='3', joueur[place2-2]='3'; 
                    if(state==2){state=3;}else{state=4;}
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else{printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}
           }
            }
           }
       }
       }else if(state==4){
       printf("Où le torpilleur doit-être placé ? (donner le début) [2 CASES - " gras "METTRE ''Q'' POUR QUITTER LE PROGRAMME" normal "]\n>");
       scanf("%s", userimput);
       int check =lettregrilleid(userimput, 0, 0);
       if(check==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint=atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               place1=check+textoint;
               printf("Quel est la case de fin ? [Q pour quitter le programme]\n>");
                      scanf("%s", userimput);
       if(lettregrilleid(userimput, 0, 0)==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){state=6; break;}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint = atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               check=lettregrilleid(userimput, 0, 0);
               place2=check+textoint;
                place1--, place2--;
               int totest=place1-place2;
               if(totest<0 && totest !=2 && totest !=-2){totest--;}else if(totest>0 && totest !=2 && totest !=-2){totest++;} //Permet de placer les bateaux aux cases indiquées sans avoir de décallage et permet de savoir si un bateau est placé sur la largeur ou non
               if(place2>9 && totest!=2 && totest<0 && totest!=-2 && totest<0){
                   //Placement par le bas
                   if(joueur[place1]!=possiblecas[3] && joueur[place1+10]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1+10]='3'; 
                    state=5;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
               }else if(place1>29 && totest>0 && totest!=2 && totest!=-2){
                   //Placement par le haut
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-10]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-10]='3'; 
                    state=5;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);}
                }else if(place1>-1 && place1<10 && totest==2 || place1>10 && place2<20 && totest==2 || place1>20 && place2<30 && totest==2 || place1>30 && place2<40 && totest==2 || place1>40 && place2<50 && totest==2 || place1>50 && place2<60 && totest==2 || place1>60 && place2<70 && totest==2 || place1>70 && place2<80 && totest==2 || place1>80 && place2<90 && totest==2 || place1>90 && place2<100 && totest==2){
                    //Placement par la largeur
                    if(joueur[place1]!=possiblecas[3] && joueur[place1-1]!=possiblecas[3]){
                    joueur[place1]='3', joueur[place1-1]='3'; 
                    state=5;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else if(place1>-1 && place1<10 && totest==-2 || place2>10 && place1<20 && totest==-2 || place2>20 && place1<30 && totest==-2 || place2>30 && place1<40 && totest==-2 || place2>40 && place1<50 && totest==-2 || place2>50 && place1<60 && totest==-2 || place2>60 && place1<70 && totest==-2 || place2>70 && place1<80 && totest==-2 || place2>80 && place1<90 && totest==-2 || place2>90 && place1<100 && totest==-2){
                    //Placement par la largeur
                    if(joueur[place2]!=possiblecas[3] && joueur[place2-1]!=possiblecas[3]){
                    joueur[place2]='3', joueur[place2-1]='3'; 
                    state=5;
                   }else{printf(touchejoueur "Impossible de placer un bateau dans l'emplacement indiqué au risque de créer un Titanic bis\n" normal);} 
               }else{printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}
           }
            }
           }
       }
       }else{break;}
       }
       if(state==6){
           //L'utilisateur a demandé cesser l'éxecution du programme
           printf(gras touchejoueur "\n\n> L'utilisateur a demandé la fin de l'éxecution du programme <\n\n" normal);
           exit(0);
       }
   }else if(type ==1){
       //Tour du joueur
       while (true)
       {
        printf("Quelle case voulez-vous tester ? [Q pour quitter le programme]\n>");
        scanf("%s",userimput);
        if(lettregrilleid(userimput, 0, 0)==999){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else if(lettregrilleid(userimput, 0, 0)==727){ printf(gras touchejoueur "\n\n> L'utilisateur a demandé la fin de l'éxecution du programme <\n\n" normal); exit(0);}else{
           strcpy(toconvert,&userimput[1]);//Nombre théorique de la case
           textoint = atoi(toconvert); //Conversion string -> int
           if(textoint==0 || textoint>11){printf(touchejoueur "La valeur fournie est incorrecte\n" normal);}else{
               int testplace = textoint+lettregrilleid(userimput, 0, 0);
               testplace--;
               if(IAhidegrille[testplace] == possiblecas[3]){
                   //Le joueur a touché un bateau
                   printf(touche "> Vous avez touché un des navires de l'IA ! <\n\n" normal);
                   IAhidegrille[testplace] ='2', IAdisplaygrille[testplace] ='2';
                   temp[0]--;
                   break;
               }else{
                    printf(caseau "> Vous n'avez pas touché un des navires de l'IA ! <\n\n" normal);
                   if(IAhidegrille[testplace] == possiblecas[0]){IAhidegrille[testplace] ='1', IAdisplaygrille[testplace] ='1';}
                   break;
               }
           }
        }
       }
   }else if(type ==2){
       //Sélection du niveau de difficulté
       printf("Sélectionner votre niveau :\n" gras "[0] - Niveau facile\n[1] - Niveau plus compliqué\n" normal "[Mettre le numéro correspond au niveau de difficulté choisi]\n>");
       scanf("%s", userimput);
       switch (userimput[0])
       {
       case '0':
           temp[4] =0;
           break;
       
      case '1':
           temp[4] =1, temp[5] = rand()%2;
           break;
        
       default:
           break;
       }
   }
}

int IA(int action)
{
    if(action ==0){ //Placement des bateaux de l'IA
        for (int i = 1; i < 6; i++)
        {
            srand(time(NULL));
            int randomplace = rand()%100, placememnttype=rand()%2; //randomplace = case random, placementtype = type de placement (Sur la longueur [0]/Sur la largeur [1])
            while(true){
                randomplace=rand()%100, placememnttype=rand()%2;
                if(IAhidegrille[randomplace] != possiblecas[3]){break;}
            }
           if(i==1 && placememnttype==0){
               //Placement du porte-avion sur la longueur (5 cases)
               if(randomplace<60 && randomplace>50){
                   if((rand()%2)==0){
                       if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3] && IAhidegrille[randomplace+30]!=possiblecas[3] && IAhidegrille[randomplace+40]!=possiblecas[3]){
                       //Placement en partant du bas
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3', IAhidegrille[randomplace+30]='3', IAhidegrille[randomplace+40]='3';
                       }else if(IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3] && IAhidegrille[randomplace-30]!=possiblecas[3] && IAhidegrille[randomplace-40]!=possiblecas[3]){
                        //Placement en partant du haut si le placement vers le bas est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3', IAhidegrille[randomplace-30]='3', IAhidegrille[randomplace-40]='3';
                       }else{
                        //Placement du porte-avion sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg5(randomplace)==0){i=0;}
                       }
                   }else{
                       if(IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3] && IAhidegrille[randomplace-30]!=possiblecas[3] && IAhidegrille[randomplace-40]!=possiblecas[3]){
                       //Placement en partant du haut
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3', IAhidegrille[randomplace-30]='3', IAhidegrille[randomplace-40]='3';
                       }else if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3] && IAhidegrille[randomplace+30]!=possiblecas[3] && IAhidegrille[randomplace+40]!=possiblecas[3]){
                        //Placement en partant du bas si le placement vers le haut est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3', IAhidegrille[randomplace+30]='3', IAhidegrille[randomplace+40]='3';
                       }else{
                        //Placement du porte-avion sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg5(randomplace)==0){i=0;}
                       }
                   }
               }else if(randomplace>50 && IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3] && IAhidegrille[randomplace-30]!=possiblecas[3] && IAhidegrille[randomplace-40]!=possiblecas[3]){
                   //Placement automatique par le haut
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3', IAhidegrille[randomplace-30]='3', IAhidegrille[randomplace-40]='3';
               }else if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3] && IAhidegrille[randomplace+30]!=possiblecas[3] && IAhidegrille[randomplace+40]!=possiblecas[3]){
                   //Placement automatique par le bas
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3', IAhidegrille[randomplace+30]='3', IAhidegrille[randomplace+40]='3';
               }else{i=0;}              
           }else if(i==1 && placememnttype==1){
                //Placement du porte-avion sur la longueur (5 cases) si les autres placements ne sont pas possibles
                if(placementlarg5(randomplace)==0){i=0;}
        }else if(i==2 && placememnttype==0){
            if(randomplace<50 && randomplace>60){
                   if((rand()%2)==0){
                       if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3] && IAhidegrille[randomplace+30]!=possiblecas[3]){
                       //Placement en partant du bas
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3', IAhidegrille[randomplace+30]='3';
                       }else if(IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3] && IAhidegrille[randomplace-30]!=possiblecas[3]){
                        //Placement en partant du haut si le placement vers le bas est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3', IAhidegrille[randomplace-30]='3';
                       }else{
                        //Placement du croiseur sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg4(randomplace)==0){i=1;}
                       }
                   }else{
                       if(IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3] && IAhidegrille[randomplace-30]!=possiblecas[3]){
                       //Placement en partant du haut
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3', IAhidegrille[randomplace-30]='3';
                       }else if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3] && IAhidegrille[randomplace+30]!=possiblecas[3]){
                        //Placement en partant du bas si le placement vers le haut est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3', IAhidegrille[randomplace+30]='3';
                       }else{
                        //Placement du croiseur sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg4(randomplace)==0){i=1;}
                       }
                   }
               }else if(randomplace>40 && IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3] && IAhidegrille[randomplace-30]!=possiblecas[3]){
                   //Placement automatique par le haut
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3', IAhidegrille[randomplace-30]='3';
               }else if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3] && IAhidegrille[randomplace+30]!=possiblecas[3]){
                   //Placement automatique par le bas
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3', IAhidegrille[randomplace+30]='3';
               }else{i=1;}             
           }else if(i==2 && placememnttype==1){
            if(placementlarg4(randomplace)==0){i=1;}
        }else if(i==3 && placememnttype==0||i==4 && placememnttype==0){
            if(randomplace<40 && randomplace>50){
                   if((rand()%2)==0){
                       if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3]){
                       //Placement en partant du bas
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3';
                       }else if(IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3]){
                        //Placement en partant du haut si le placement vers le bas est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3';
                       }else{
                        //Placement du croiseur sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg3(randomplace)==0){if(i==2){i=1;}else{i=2;}}
                       }
                   }else{
                       if(IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3]){
                       //Placement en partant du haut
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3';
                       }else if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3]){
                        //Placement en partant du bas si le placement vers le haut est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3';
                       }else{
                        //Placement du croiseur sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg3(randomplace)==0){if(i==2){i=1;}else{i=2;}}
                       }
                   }
               }else if(randomplace>40 && IAhidegrille[randomplace-10]!=possiblecas[3] && IAhidegrille[randomplace-20]!=possiblecas[3]){
                   //Placement automatique par le haut
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3', IAhidegrille[randomplace-20]='3';
               }else if(IAhidegrille[randomplace+10]!=possiblecas[3] && IAhidegrille[randomplace+20]!=possiblecas[3]){
                   //Placement automatique par le bas
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3', IAhidegrille[randomplace+20]='3';
               }else if(i==3){i=2;}else{i=4;}
        }else if(i==3 && placememnttype==1||i==4 && placememnttype==1){
            if(placementlarg3(randomplace)==0){if(i==2){i=1;}else{i=2;}}
        }else if(i==5 && placememnttype==0){
            if(randomplace<30 && randomplace>70){
                   if((rand()%2)==0){
                       if(IAhidegrille[randomplace+10]!=possiblecas[3]){
                       //Placement en partant du bas
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3';
                       }else if(IAhidegrille[randomplace-10]!=possiblecas[3]){
                        //Placement en partant du haut si le placement vers le bas est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3';
                       }else{
                        //Placement du croiseur sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg2(randomplace)==0){i=3;}
                       }
                   }else{
                       if(IAhidegrille[randomplace-10]!=possiblecas[3]){
                       //Placement en partant du haut
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3';
                       }else if(IAhidegrille[randomplace+10]!=possiblecas[3]){
                        //Placement en partant du bas si le placement vers le haut est impossible mais que le placement vers le haut si
                       IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3';
                       }else{
                        //Placement du croiseur sur la longueur (5 cases) si les autres placements ne sont pas possibles
                        if(placementlarg2(randomplace)==0){i=3;}
                       }
                   }
               }else if(randomplace>20 && IAhidegrille[randomplace-10]!=possiblecas[3]){
                   //Placement automatique par le haut
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace-10]='3';
               }else if(IAhidegrille[randomplace+10]!=possiblecas[3]){
                   //Placement automatique par le bas
                   IAhidegrille[randomplace]='3', IAhidegrille[randomplace+10]='3';
               }else{i=4;}             
           }else if(i==5 && placememnttype==1){
            if(placementlarg2(randomplace)==0){i=3;}
        }
    }
}else if(action==1){
    /*
    Gestion du tour pour l'IA 
    Si la dernière tentative a touché un navire, l'IA enregistre automatiquement la case touchée a l'emplacement 0 du tableau "IAcoup" et mettra à 2 (navire joueur touché) l'emplacement 1 de "IAcoup"
    IAcoup[2] -> Case de droite (IAcoup[3] pour la valeur de la case de droite)
    IAcoup[4] -> Case de gauche (IAcoup[5] pour la valeur de la case de gauche)
    IAcoup[6] -> Case du haut (IAcoup[7] pour la valeur de la case du haut)
    IAcoup[8] -> Case du bas (IAcoup[9] pour la valeur de la case du bas)
            Haut
              |
    Gauche -  x - Droite
              |
             Bas
    Si une case a la valeur 4, cela signifie qu'elle ne doit pas être testée par l'IA (en fonction de son emplacement)
    */
    if(IAcoup[0]==404){
        if(temp[3]==01 || temp[3]==03){printf("B0\n");}
        IAtestcase(4); //Type 4 -> Type par défaut
    }else{
        if(temp[3]==01 || temp[3]==03){printf("IACOUP0 : %d\nIACOUP0VAL : %c\nIACOUP+10VAL : %c\nIACOUP-10VAL : %c\nIACOUP-1VAL : %c\nIACOUP+1VAL : %c\n", IAcoup[0],joueur[IAcoup[0]], joueur[IAcoup[0]+10], joueur[IAcoup[0]-10], joueur[IAcoup[0]-1], joueur[IAcoup[0]+1]);}
        int choixtest =0; //Pour choisir l'emplacement que l'IA testeras
        if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           //Module logique B1
           if(temp[3]==01 || temp[3]==03){printf("B1\n");}
           choixtest=rand()%4;
           if(choixtest==0){IAtestcase(0);}
           else if(choixtest==1){IAtestcase(1);}
           else if(choixtest==2){IAtestcase(2);}
           else{IAtestcase(3);}
       }else if(IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           //Module logique B2
           if(temp[3]==01 || temp[3]==03){printf("B2\n");}
           choixtest=rand()%3;
           if(choixtest==0){IAtestcase(1);}
           else if(choixtest==1){IAtestcase(2);}
           else{IAtestcase(3);}
       }else if(IAcoup[0]+10 <100 && lettregrilleid("",5, IAcoup[0]) !=0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           //Module logique B3
           if(temp[3]==01 || temp[3]==03){printf("B3\n");}
           choixtest=rand()%3;
           if(choixtest==0){IAtestcase(0);}
           else if(choixtest==1){IAtestcase(2);}
           else{IAtestcase(3);}
       }else if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-10] !=possiblecas[4]){
           //Module logique B4
           if(temp[3]==01 || temp[3]==03){printf("B4\n");}
           choixtest=rand()%3;
           if(choixtest==0){IAtestcase(0);}
           else if(choixtest==1){IAtestcase(1);}
           else{IAtestcase(3);}
       }else if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           //Module logique B5
           if(temp[3]==01 || temp[3]==03){printf("B5\n");}
           choixtest=rand()%3;
           if(choixtest==0){IAtestcase(0);}
           else if(choixtest==1){IAtestcase(2);}
           else{IAtestcase(1);}
       }else if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+10] !=possiblecas[4]){
           //Module logique B6
           if(temp[3]==01 || temp[3]==03){printf("B6\n");}
           choixtest=rand()%2;
           if(choixtest==0){IAtestcase(0);}
           else{IAtestcase(1);}
       }else if(IAcoup[0]-10 >0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4]){
           //Module logique B7
           if(temp[3]==01 || temp[3]==03){printf("B7\n");}
           choixtest=rand()%2;
           if(choixtest==0){IAtestcase(1);}
           else{IAtestcase(3);}
       }else if(IAcoup[0]+10 <100 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4]){
           //Module logique B8
           if(temp[3]==01 || temp[3]==03){printf("B8\n");}
           choixtest=rand()%2;
           if(choixtest==0){IAtestcase(3);}
           else{IAtestcase(0);}
       }else if(IAcoup[0]+10 <100 && lettregrilleid("",5, IAcoup[0]) !=0 && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[4] && joueur[IAcoup[0]+10] !=possiblecas[4]){
           //Module logique B9
           if(temp[3]==01 || temp[3]==03){printf("B9\n");}
           choixtest=rand()%2;
           if(choixtest==0){IAtestcase(0);}
           else{IAtestcase(2);}
       }else if(IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[4] && joueur[IAcoup[0]-10] !=possiblecas[4]){
           //Module logique B10
           if(temp[3]==01 || temp[3]==03){printf("B10\n");}
           choixtest=rand()%2;
           if(choixtest==0){IAtestcase(2);}
           else{IAtestcase(1);}
       }else if(IAcoup[0]+10 <100 && joueur[IAcoup[0]+10] != possiblecas[1] && joueur[IAcoup[0]+10] != possiblecas[4]){
           //Module logique B11
           if(temp[3]==01 || temp[3]==03){printf("B11\n");}
           IAtestcase(0);
       }else if(IAcoup[0]-10 >0 && joueur[IAcoup[0]-10] != possiblecas[1] &&  joueur[IAcoup[0]-10] != possiblecas[4]){
           //Module logique B12
           if(temp[3]==01 || temp[3]==03){printf("B12\n");}
           IAtestcase(1);
       }else if(lettregrilleid("",5, IAcoup[0]) !=0  && joueur[IAcoup[0]-1] != possiblecas[1] && joueur[IAcoup[0]-1] != possiblecas[4]){
           //Module logique B13
           if(temp[3]==01 || temp[3]==03){printf("B13\n");}
           IAtestcase(2);
       }else if(lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+1] != possiblecas[1] && joueur[IAcoup[0]+1] != possiblecas[4]){
           //Module logique B14
           if(temp[3]==01 || temp[3]==03){printf("B14\n");}
           IAtestcase(3);
       }else{
           //Module logique B15
           if(temp[3]==01 || temp[3]==03){printf("B15\n");}
           IAcoup[0]=404, IAcoup[1]=404;
           IAtestcase(4);
       }
    }
}
}

int placementlarg5(int randomplace){
float test =randomplace/10;
int test1 = test; 
    if(randomplace>0 && randomplace<10){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<10){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>0){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;}
    }else if(randomplace>10 && randomplace<20){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<20){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>10){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;} 
    }else if(randomplace>20 && randomplace<30){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<30){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>20){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;}
    }else if(randomplace>30 && randomplace<40){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>30){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;} 
    }else if(randomplace>40 && randomplace<50){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;}
    }else if(randomplace>50 && randomplace<60){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;} 
    }else if(randomplace>60 && randomplace<70){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;}
    }else if(randomplace>70 && randomplace<80){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;} 
    }else if(randomplace>80 && randomplace<90){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;}
    }else{
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && IAhidegrille[randomplace+4] != possiblecas[3] && randomplace+4<100){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3', IAhidegrille[randomplace+4]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && IAhidegrille[randomplace-4] != possiblecas[3] && randomplace-4>90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3', IAhidegrille[randomplace-4]='3';
        }else{return 0;} 
    }
}

int placementlarg4(int randomplace){
//Placement du croiseur sur la longueur (4 cases) si les autres placements ne sont pas possibles
float test =randomplace/10;
int test1 = test; 
    if(randomplace>0 && randomplace<10){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<10){
            IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>0){
            IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
        }else{return 0;}
        }else if(randomplace>10 && randomplace<20){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<20){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>10){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;} 
        }else if(randomplace>20 && randomplace<30){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<30){
                    IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>20){
                    IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;}
        }else if(randomplace>30 && randomplace<40){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>30){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;} 
        }else if(randomplace>40 && randomplace<50){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;}
        }else if(randomplace>50 && randomplace<60){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;} 
        }else if(randomplace>60 && randomplace<70){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;}
        }else if(randomplace>70 && randomplace<80){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;} 
        }else if(randomplace>80 && randomplace<90){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;}
        }else{
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && IAhidegrille[randomplace+3] != possiblecas[3] && randomplace+3<100){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3', IAhidegrille[randomplace+3]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && IAhidegrille[randomplace-3] != possiblecas[3] && randomplace-3>90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3', IAhidegrille[randomplace-3]='3';
            }else{return 0;} 
    }
}

int placementlarg3(int randomplace){
//Placement des sous-marins sur la longueur (4 cases) si les autres placements ne sont pas possibles
float test =randomplace/10;
int test1 = test; 
    if(randomplace>0 && randomplace<10){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<10){
            IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>0){
            IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
        }else{return 0;}
        }else if(randomplace>10 && randomplace<20){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<20){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>10){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;} 
        }else if(randomplace>20 && randomplace<30){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<30){
                    IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>20){
                    IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;}
        }else if(randomplace>30 && randomplace<40){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>30){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;} 
        }else if(randomplace>40 && randomplace<50){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;}
        }else if(randomplace>50 && randomplace<60){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;} 
        }else if(randomplace>60 && randomplace<70){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;}
        }else if(randomplace>70 && randomplace<80){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;} 
        }else if(randomplace>80 && randomplace<90){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;}
        }else{
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && IAhidegrille[randomplace+2] != possiblecas[3] && randomplace+2<100){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3', IAhidegrille[randomplace+2]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && IAhidegrille[randomplace-2] != possiblecas[3] && randomplace-2>90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3', IAhidegrille[randomplace-2]='3';
            }else{return 0;} 
    }
}

int placementlarg2(int randomplace){
//Placement des sous-marins sur la longueur (4 cases) si les autres placements ne sont pas possibles
float test =randomplace/10;
int test1 = test; 
    if(randomplace>0 && randomplace<10){
        if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<10){
            IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
        }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>0){
            IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
        }else{return 0;}
        }else if(randomplace>10 && randomplace<20){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<20){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>10){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;} 
        }else if(randomplace>20 && randomplace<30){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<30){
                    IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>20){
                    IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;}
        }else if(randomplace>30 && randomplace<40){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>30){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;} 
        }else if(randomplace>40 && randomplace<50){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>40){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;}
        }else if(randomplace>50 && randomplace<60){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>50){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;} 
        }else if(randomplace>60 && randomplace<70){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>60){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;}
        }else if(randomplace>70 && randomplace<80){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>70){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;} 
        }else if(randomplace>80 && randomplace<90){
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>80){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;}
        }else{
            if(test == test1 && IAhidegrille[randomplace+1] != possiblecas[3] && randomplace+1<100){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace+1]='3';
            }else if(IAhidegrille[randomplace-1] != possiblecas[3] && randomplace-1>90){
                IAhidegrille[randomplace]='3', IAhidegrille[randomplace-1]='3';
            }else{return 0;} 
    }
}

int IAtestcase(int type){
    int randomcase = rand()%100;
    if(type ==0){
        if(temp[3]==01 || temp[3]==03){printf("a");}
        //Test la case du bas
        if(joueur[IAcoup[0]+10] == possiblecas[3]){
            if(temp[3]==01 || temp[3]==03){printf("1\n");}
            printf(touchejoueur "\n\n> L'IA a choisi la case %c%d a touchée un de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]+10), lettregrilleid("", 2, IAcoup[0]+10));
            joueur[IAcoup[0]+10] = '4';
            nplacement(0,IAcoup[0]+10);
            temp[1]--;
        }else{
            if(temp[3]==01 || temp[3]==03){printf("2\n");}
            printf(caseau "\n\n> L'IA a choisi la case %c%d et n'a touché aucun de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]+10), lettregrilleid("", 2, IAcoup[0]+10));
            joueur[IAcoup[0]+10] = '1';
            nplacement(1,IAcoup[0]);
        }
    }else if(type ==1){
        if(temp[3]==01 || temp[3]==03){printf("b");}
        //Test la case du haut
        if(joueur[IAcoup[0]-10] == possiblecas[3]){
            if(temp[3]==01 || temp[3]==03){printf("1\n");}
            printf(touchejoueur "\n\n> L'IA a choisi la case %c%d a touchée un de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]-10), lettregrilleid("", 2, IAcoup[0]-10));
            joueur[IAcoup[0]-10] = '4';
            nplacement(0,IAcoup[0]-10);
            temp[1]--;
        }else{
            if(temp[3]==01 || temp[3]==03){printf("2\n");}
            printf(caseau "\n\n> L'IA a choisi la case %c%d et n'a touché aucun de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]-10), lettregrilleid("", 2, IAcoup[0]-10));
            joueur[IAcoup[0]-10] = '1';
            nplacement(1,IAcoup[0]);
        }
    }else if(type ==2){
        if(temp[3]==01 || temp[3]==03){printf("c");}
        //Test la case de gauche
        if(joueur[IAcoup[0]-1] == possiblecas[3]){
            if(temp[3]==01 || temp[3]==03){printf("1\n");}
            printf(touchejoueur "\n\n> L'IA a choisi la case %c%d a touchée un de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]-1), lettregrilleid("", 2, IAcoup[0]-1));
            joueur[IAcoup[0]-1] = '4';
            nplacement(0,IAcoup[0]-1);
            temp[1]--;
        }else{
            if(temp[3]==01 || temp[3]==03){printf("2\n");}
            printf(caseau "\n\n> L'IA a choisi la case %c%d et n'a touché aucun de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]-1), lettregrilleid("", 2, IAcoup[0]-1));
            joueur[IAcoup[0]-1] = '1';
            nplacement(1,IAcoup[0]);
        }
    }else if(type ==3){
        if(temp[3]==01 || temp[3]==03){printf("d");}
        if(joueur[IAcoup[0]+1] == possiblecas[3]){
            if(temp[3]==01 || temp[3]==03){printf("1\n");}
            printf(touchejoueur "\n\n> L'IA a choisi la case %c%d a touchée un de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]+1), lettregrilleid("", 2, IAcoup[0]+1));
            joueur[IAcoup[0]+1] = '4';
            nplacement(0,IAcoup[0]+1);
            temp[1]--;
        }else{
            if(temp[3]==01 || temp[3]==03){printf("2\n");}
            printf(caseau "\n\n> L'IA a choisi la case %c%d et n'a touché aucun de vos navires <\n\n" normal, lettregrilleid("", 1, IAcoup[0]+1), lettregrilleid("", 2, IAcoup[0]+1));
            joueur[IAcoup[0]+1] = '1';
            nplacement(1,IAcoup[0]);
        }
    }else if(type ==4){
        if(temp[3]==01 || temp[3]==03){printf("e");}
            while (true) //Pour éviter que l'IA teste une case qu'elle a déjà testé
            {
                if(temp[4]==0){ //Recherche en full aléatoire pour le mode "facile"
                    if(joueur[randomcase]!=possiblecas[1] && joueur[randomcase]!=possiblecas[4]){break;}
                    else{randomcase=rand()%100;}
                }else if(temp[4]==1){//Recherche par damier pour le mode "difficile"
                temp[6]++;
                if(temp[3]==01 || temp[3]==03){printf("VAL-I : %d\nLettregrille : %c\n", temp[6], lettregrilleid("", 1, temp[6]+1));}
                    if(temp[6]+1<100 && joueur[temp[6]] != possiblecas[1] && joueur[temp[6]] != possiblecas[4] && joueur[temp[6]+1] != possiblecas[1] && joueur[temp[6]+1] != possiblecas[4] && temp[5] ==0 && patern[temp[6]+1] == '1'){
                        randomcase=temp[6]+1;
                        break;
                    }else if(temp[6]+2<100 && joueur[temp[6]+1] != possiblecas[1] && joueur[temp[6]+1] != possiblecas[4] && joueur[temp[6]+2] != possiblecas[1] && joueur[temp[6]+2] != possiblecas[4] && temp[5] ==1 && patern[temp[6]+1] == '0'){
                        randomcase=temp[6]+2;
                        break;
                    }else if(temp[6]+1 >100 || temp[6]+2 >100){
                        temp[4]=0;
                        break;
                    }
                }
            }
        //La dernière tentative de l'IA n'a touché aucun bateau -> l'IA choisi une case au hasard
        if(temp[3]==01 || temp[3]==03){printf("RANDOM : %d\nRANDOMVAL : %c\n", randomcase, joueur[randomcase]);}
        if(joueur[randomcase] == possiblecas[3]){
            if(temp[3]==01 || temp[3]==03){printf("1\n");}
            joueur[randomcase]='4';
            IAcoup[0]=randomcase, IAcoup[1]=randomcase;
            temp[1]--;
            nplacement(0,IAcoup[0]);
            printf(touchejoueur "\n\n> L'IA a choisi la case %c%d a touchée un de vos navires <\n\n" normal, lettregrilleid("", 1, randomcase), lettregrilleid("", 2, randomcase));
        }else{
            if(temp[3]==01 || temp[3]==03){printf("2\n");}
            //L'IA a testé une case d'eau
            joueur[randomcase]='1';
            printf(caseau "\n\n> L'IA a choisi la case %c%d et n'a touché aucun de vos navires <\n\n" normal, lettregrilleid("", 1, randomcase), lettregrilleid("", 2, randomcase));
        }
    }
}

int lettregrilleid(char imput[10], int type, int id){
    if(type==0){
         //Pour renvoyer les ID des lettres de la grille
       switch (tolower(imput[0]))
       {
       case 'a':
           return 0;
           break;
       
       case 'b':
            return 10;
            break;

       case 'c':
            return 20;
            break;

       case 'd':
            return 30;
            break;

       case 'e':
            return 40;
            break;

       case 'f':
            return 50;
            break;

       case 'g':
            return 60;
            break;

       case 'h':
            return 70;
            break;

       case 'i':
            return 80;
            break;

       case 'j':
            return 90;
            break;
        
        case 'q':
            return 727; //Code renvoyé si l'utilisateur demande a quitter la boucle (q pour quitter)
            break;

       default:
       return 999; //Code renvoyé si le nombre fournie n'existe pas sur la grille
           break;
       }   
    }else if(type ==1){
        //Pour renvoyer la lettre correspond a un ID donné
        if(id>-1 && id<10){return 'A';}
        else if(id>9 && id<20){return 'B';}
        else if(id>19 && id<30){return 'C';}
        else if(id>29 && id<40){return 'D';}
        else if(id>39 && id<50){return 'E';}
        else if(id>49 && id<60){return 'F';}
        else if(id>59 && id<70){return 'G';}
        else if(id>69 && id<80){return 'H';}
        else if(id>79 && id<90){return 'I';}
        else{return 'J';}
    }else if(type ==2){
        //Pour renvoyer le numéro de case touché
        id++;
        if(id==9 || id==19 ||id==29 ||id==39 ||id==49 ||id==59 ||id==69 ||id==79 ||id==89 ||id==99){return 10;}
        else if(id>-1 && id<10){return id++;}
        else if(id>9 && id<20){return (id++)-10;}
        else if(id>19 && id<30){return (id++)-20;}
        else if(id>29 && id<40){return (id++)-30;}
        else if(id>39 && id<50){return (id++)-40;}
        else if(id>49 && id<60){return (id++)-50;}
        else if(id>59 && id<70){return (id++)-60;}
        else if(id>69 && id<80){return (id++)-70;}
        else if(id>79 && id<90){return (id++)-80;}
        else{return (id++)-90;}
    }else if(type ==3){
        //Pour renvoyer l'ID suppérieur par rapport à l'ID de case touché
        if(id>-1 && id<10){return 20;}
        else if(id>9 && id<20){return 30;}
        else if(id>19 && id<30){return 40;}
        else if(id>29 && id<40){return 50;}
        else if(id>39 && id<50){return 60;}
        else if(id>49 && id<60){return 70;}
        else if(id>59 && id<70){return 80;}
        else if(id>69 && id<80){return 90;}
        else if(id>89 && id<90){return 100;}
        else if(id>89 && id <100){return 100;}
        else{return 0;}
    }else if(type ==4){
        //Pour renvoyer l'ID inférieur par rapport à l'ID de case touché
        if(id<0){return 0;}
        else if(id>0 && id<10){return 10;}
        else if(id>9 && id<20){return 10;}
        else if(id>19 && id<30){return 20;}
        else if(id>29 && id<40){return 30;}
        else if(id>39 && id<50){return 40;}
        else if(id>49 && id<60){return 50;}
        else if(id>59 && id<70){return 60;}
        else if(id>69 && id<80){return 70;}
        else if(id>79 && id<90){return 80;}
        else{return 90;}
    }else if(type ==5){
        //Déplacement a gauche
        int testval =0;
        if(id==0 || id==10 || id==20 || id==30 || id==40 || id==50 || id==60 || id==70 || id==80 || id==90){return 0;}
        else if(id<0){return 1;}
        else if(id>0 && id<10){
            testval=9-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>9 && id<20){
            testval=19-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>19 && id<30){
            testval=29-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>29 && id<40){
            testval=39-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>39 && id<50){
            testval=49-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>49 && id<60){
            testval=59-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>59 && id<70){
            testval=69-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>69 && id<80){
            testval=79-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>79 && id<90){
            testval=89-id;
            if(testval>=0){return 1;}else{return 0;}
        }
        else if(id>89 && id <100){
            testval=99-id;
            if(testval>=0){return 1;}else{return 0;}
        }else{return 0;}
    }else if(type ==6){
        //Déplacement a droite
        if(id==9 || id==19 || id==29 || id==39 || id==49 || id==59 || id==69 || id==79 || id==89 || id==99){return 1;}
        int testval =0;
        if(id<0){return 1;}
        else if(id>=0 && id<10){
            testval=9-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>9 && id<20){
            testval=19-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>19 && id<30){
            testval=29-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>29 && id<40){
            testval=39-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>39 && id<50){
            testval=49-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>49 && id<60){
            testval=59-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>59 && id<70){
            testval=69-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>69 && id<80){
            testval=79-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>79 && id<90){
            testval=89-id;
            if(testval>=0){return 0;}else{return 1;}
        }
        else if(id>89 && id <100){
            testval=99-id;
            if(testval>=0){return 0;}else{return 1;}
        }else{return 0;}
    }
}

int nplacement(int type, int checkcase){
    if(type ==0){
        //Pour un nouveau placement en cas de touche de l'IA
        if(temp[3]==01 || temp[3]==03){printf("Nouvelle valeur prise : %d (ancienne : %d)\n", checkcase, IAcoup[0]);}
        IAcoup[0] = checkcase;
    }else if(type==1){
        //Pour adapter la capacité de "rebon" de l'IA
        if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && lettregrilleid("",5, IAcoup[0]) !=0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
            IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4] && joueur[IAcoup[0]-10] !=possiblecas[4]){
            IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]-1] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && IAcoup[0]-10 >0 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+10] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]-10 >0 && lettregrilleid("",6, IAcoup[0]) !=-1 && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]+1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[4] && joueur[IAcoup[0]+1] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && lettregrilleid("",5, IAcoup[0]) !=0 && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]+10] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[4] && joueur[IAcoup[0]+10] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]-10 >0 && lettregrilleid("",5, IAcoup[0]) !=0 && joueur[IAcoup[0]-1] !=possiblecas[1] && joueur[IAcoup[0]-10] !=possiblecas[1] && joueur[IAcoup[0]-1] !=possiblecas[4] && joueur[IAcoup[0]-10] !=possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]+10 <100 && joueur[IAcoup[0]+10] != possiblecas[1] && joueur[IAcoup[0]+10] != possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(IAcoup[0]-10 >0 && joueur[IAcoup[0]-10] != possiblecas[1] &&  joueur[IAcoup[0]-10] != possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(lettregrilleid("",5, IAcoup[0]) !=0  && joueur[IAcoup[0]-1] != possiblecas[1] && joueur[IAcoup[0]-1] != possiblecas[4]){
           IAcoup[0] = checkcase;
       }else if(lettregrilleid("",6, IAcoup[0]) !=1 && joueur[IAcoup[0]+1] != possiblecas[1] && joueur[IAcoup[0]+1] != possiblecas[4]){
           IAcoup[0] = checkcase;
        //Capacité de rebon a partir de la première touche effectué
        if(IAcoup[1]+10 <100 && IAcoup[1]-10 >0 && lettregrilleid("",5, IAcoup[1]-1) !=0 && lettregrilleid("",6, IAcoup[1]+1) !=1 && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]+1] !=possiblecas[1] && joueur[IAcoup[1]-1] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[4] && joueur[IAcoup[1]-10] !=possiblecas[4] && joueur[IAcoup[1]+1] !=possiblecas[4] && joueur[IAcoup[1]-1] !=possiblecas[4]){
            IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]-10 >0 && lettregrilleid("",5, IAcoup[1]-1) !=0 && lettregrilleid("",6, IAcoup[1]+1) !=1 && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]+1] !=possiblecas[1] && joueur[IAcoup[1]-1] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[4] && joueur[IAcoup[1]+1] !=possiblecas[4] && joueur[IAcoup[1]-1] !=possiblecas[4]){
            IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && lettregrilleid("",5, IAcoup[1]-1) !=0 && lettregrilleid("",6, IAcoup[1]+1) !=1 && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]+1] !=possiblecas[1] && joueur[IAcoup[1]-1] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[4] && joueur[IAcoup[1]+1] !=possiblecas[4] && joueur[IAcoup[1]-1] !=possiblecas[4]){
            IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && IAcoup[1]-10 >0 && lettregrilleid("",6, IAcoup[1]+1) !=1 && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]+1] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[4] && joueur[IAcoup[1]+1] !=possiblecas[4] && joueur[IAcoup[1]-10] !=possiblecas[4]){
            IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && IAcoup[1]-10 >0 && lettregrilleid("",5, IAcoup[1]-1) !=0 && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]-1] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[4] && joueur[IAcoup[1]+10] !=possiblecas[4] && joueur[IAcoup[1]-1] !=possiblecas[4]){
            IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && IAcoup[1]-10 >0 && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[4] && joueur[IAcoup[1]+10] !=possiblecas[4]){
           IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]-10 >0 && lettregrilleid("",6, IAcoup[1]+1) !=-1 && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]+1] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[4] && joueur[IAcoup[1]+1] !=possiblecas[4]){
           IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && lettregrilleid("",6, IAcoup[1]+1) !=1 && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]+1] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[4] && joueur[IAcoup[1]+1] !=possiblecas[4]){
           IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && lettregrilleid("",5, IAcoup[1]-1) !=0 && joueur[IAcoup[1]-1] !=possiblecas[1] && joueur[IAcoup[1]+10] !=possiblecas[1] && joueur[IAcoup[1]-1] !=possiblecas[4] && joueur[IAcoup[1]+10] !=possiblecas[4]){
          IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]-10 >0 && lettregrilleid("",5, IAcoup[1]-1) !=0 && joueur[IAcoup[1]-1] !=possiblecas[1] && joueur[IAcoup[1]-10] !=possiblecas[1] && joueur[IAcoup[1]-1] !=possiblecas[4] && joueur[IAcoup[1]-10] !=possiblecas[4]){
        IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]+10 <100 && joueur[IAcoup[1]+10] != possiblecas[1] && joueur[IAcoup[1]+10] != possiblecas[4]){
        IAcoup[0]=IAcoup[1];
       }else if(IAcoup[1]-10 >0 && joueur[IAcoup[1]-10] != possiblecas[1] &&  joueur[IAcoup[1]-10] != possiblecas[4]){
        IAcoup[0]=IAcoup[1];
       }else if(lettregrilleid("",5, IAcoup[1]-1) !=0  && joueur[IAcoup[1]-1] != possiblecas[1] && joueur[IAcoup[1]-1] != possiblecas[4]){
        IAcoup[0]=IAcoup[1];
       }else if(lettregrilleid("",6, IAcoup[1]+1) !=1 && joueur[IAcoup[1]+1] != possiblecas[1] && joueur[IAcoup[1]+1] != possiblecas[4]){
        IAcoup[0]=IAcoup[1];
       }else{
           IAcoup[0]=404, IAcoup[1]=404;
       }
       }
    }
}

int debugmode(char *debugoption){
    /*
    Code de debug :
    01 : log tout les modules logiques de l'IA
    02 : IA en mode autonome (elle joue toute seul)
    03 : IA en mode autonome et log activé 
    */
   if(debugoption[0]=='d'){temp[3]=1;}
   else if(debugoption[0]=='i'){temp[3]=2;}
   else if(debugoption[0]=='a'){temp[3]=3;}
   else if(debugoption[0]=='p'){temp[3]=4;}
   else{
        printf("Option non-supporté !\nd [debug] : active les logs des modules logiques\ni [iaonly] : IA en mode autonome\np [(auto)-placement] : L'IA place les bateaux tout seul\na [all] : log + IA en mode autonome\n");
        exit(0);
   }
   printf("Debug code : %d\n", temp[3]);
}
