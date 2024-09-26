#include <Arduino.h>
#include <libRobux.h>

// put function declarations here:
bool GaucheVert;
bool GaucheRouge;
bool AvantVert;
bool AvantRouge;
bool DroiteVert;
bool DroiteRouge;
int GaucheVertPin = 1;
int GaucheRougePin = 2;
int AvantVertPin = 3;
int AvantRougePin = 4;
int DroiteVertPin = 5;
int DroiteRougePin = 6;
int PosX = 1;
int PosY = 0;

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avanceY(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
  PosY++;
};

void avanceX(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
  
}


void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);
};

void CheckGauche(){
  GaucheRouge = digitalRead(GaucheRougePin);
  GaucheVert = digitalRead(GaucheVertPin); 
if (GaucheVert && GaucheRouge)
{
  return 0;
}
return 1;
}

void CheckAvant(){
  AvantRouge = digitalRead(AvantRougePin);
  AvantVert = digitalRead(AvantVertPin);
  if (AvantRouge && AvantVert)
  {
    return 0;
  }
  return 1;
}

void Gauche()
{
if (CheckAvant() = 0)
  {
    avanceY();
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
    }
    else
    {
      avanceX();
      PosX++;
      tourneGauche();
      avanceY();
    }
  }
}

void Milieu()
{
  if (CheckAvant() = 0)
  {
    avanceY();
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
    }
    tourneDroit();
    avanceX();
    PosX++;
    tourneGauche();
    avanceY();
  }
}


void Droite(){
  if (CheckAvant() = 0)
  {
    avanceY();
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
    }
    else
    {
      avanceX();
      PosX--;
      tourneDroit();
      avanceY();
    }
  }
}




void setup() {
  // put your setup code here, to run once:
  pinMode(AvantVertPin, INPUT);
  pinMode(AvantRougePin, INPUT);
  pinMode(GaucheVertPin, INPUT);
  pinMode(GaucheRougePin, INPUT);
  pinMode(DroiteVertPin, INPUT);
  pinMode(DroiteRougePin, INPUT);
 PosX = 1;
 PosY = 0;
}


void loop(){
while (PosY<5)
{
 switch (PosX)

{
case 0:
  Gauche();
  break;
case 1:
  Milieu();
  break;
  case 2:
  Droite();
  break;

default:
  break;
}

}
arret();
}