#include <Arduino.h>
#include <libRobux.h>

//Définition des variables et des constantes
#define GaucheVertPin = 1;
#define GaucheRougePin = 2;
#define AvantVertPin = 3;
#define AvantRougePin = 4;


bool GaucheVert;
bool GaucheRouge;
bool AvantVert;
bool AvantRouge;
bool depart;
int PosX = 1;
int PosY = 0;
float vitesse;
char PosRetour[];

//Fonction d'arrêt des moteurs
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};


//Fonction d'avancer (plus longue distance que avanceX)
//Ajoute aussi 1 à la position en Y à chaque utilisation
//À utiliser seulement lors de déplacement dans l'axe des Y(5m)
void avanceY(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
};

//Fonction d'avancer (plus petite distance que avanceY)
//Ne change pas la position en X dans cette fonction
void avanceX(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
}


//Fonction permettant une rotation de 90 degrés vers la droite
void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
};


//Fonction permettant une rotation de 90 degrés vers la gauche
void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);
};


//Fonction permettant de lire les capteurs gauche
//Retourne 1 si un obstacle est détecté
//Retourne 0 si rien n'est détecté
void CheckGauche(){
  GaucheRouge = digitalRead(GaucheRougePin);
  GaucheVert = digitalRead(GaucheVertPin); 
if (GaucheVert && GaucheRouge)
{
  return 0;
}
return 1;
}

//Fonction permettant de lire les capteurs droite
//Retourne 1 si un obstacle est détecté
//Retourne 0 si rien n'est détecté
void CheckAvant(){
  AvantRouge = digitalRead(AvantRougePin);
  AvantVert = digitalRead(AvantVertPin);
  if (AvantRouge && AvantVert)
  {
    return 0;
  }
  return 1;
}


//Fonction décrivant le cas que le robot commence son positionnement à gauche 
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin 
void Gauche()
{
if (CheckAvant() = 0)
  {
    avanceY();
      PosY++;
  }
else
  {
    tourneDroit();
    avanceX();
    PosX++;
    if (CheckGauche = 0)
    {
      tourneGauche();
      avanceY();
      PosY++;
    }
    else
    {
      avanceX();
      PosX++;
      tourneGauche();
      avanceY();
      PosY++;
    }
  }
}



//Fonction décrivant le cas que le robot commence son positionnement au milieu
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin
void Milieu()
{
  if (CheckAvant() = 0)
  {
    avanceY();
    PosY++;
  }
  else
  {
    if (CheckGauche() = 0)
    {
      tourneGauche();
      avanceX();
      PosX--;
      tourneDroit();
      avanceY();
      PosY++;
    }
    tourneDroit();
    avanceX();
    PosX++;
    tourneGauche();
    avanceY();
    PosY++;
  }
}



//Fonction décrivant le cas que le robot commence son positionnement à droite 
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin
void Droite(){
  if (CheckAvant() = 0)
  {
    avanceY();
    PosY++;
  }
  else
  {
    tourneGauche();
    avanceX();
    PosX--;
    if (CheckAvant() = 1)
    {
      tourneDroit();
      avanceY();
      PosY++;
    }
    else
    {
      avanceX();
      PosX--;
      tourneDroit();
      avanceY();
      PosY++;
    }
  }
}


void retour()
{
  tourneDroit();
  tourneDroit();
  avanceY();
  PosY--;
  while(PosY > 0)
  {
    if (PosRetour[PosY-1]<PosX)
   {
      tourneDroit();
      avanceX();
      if (PosRetour[PosY-1]<PosX)
      {
       avanceX()
     }
     tourneGauche();
   }
   if(PosRetour[PosY-1]>PosX)
   {
     tourneGauche();
     avanceX();
     if (PosRetour[PosY-1]>PosX)
     {
       avanceX();
     }
     tourneDroit();
   }
   avanceY();
   PosY--;
  }
}














//initialisation des Pins
void setup() {
  pinMode(AvantVertPin, INPUT);
  pinMode(AvantRougePin, INPUT);
  pinMode(GaucheVertPin, INPUT);
  pinMode(GaucheRougePin, INPUT);
}


void loop(){

//Boucle déclarant le départ à partir du sifflet
if (/* condition */)
{
  depart = true;
}


//Boucle déclarant chaque cas possible selon la position du robot pour l'allée
while (PosY<5 && depart)
{
 switch (PosX)

{
case 0:
    PosRetour[PosY]=0;
    Gauche();
  break;

case 1:
     PosRetour[PosY]=1;
     Milieu();
  break;

  case 2:
    PosRetour[PosY]=2;
    Droite();
  break;

default:
  break;
}

//Le robot arrête après avoir traversé la ligne de fin
}
arret();

retour();
}