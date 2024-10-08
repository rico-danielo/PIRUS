#include <Arduino.h>
#include <libRobus.h>

//Définition des variables et des constantes
#define GaucheVertPin 24
#define GaucheRougePin 26
#define AvantVertPin 28
#define AvantRougePin 30
#define CapteurSonPin (A3)
#define CapteurAmbiantPin (A2)
#define X 36
#define Y 80

#define WHEELCIRC 23.938936 //Wheel circumference rounded up to 4 decimals
#define ENCODERFULLTURN 3200
#define STARTRATION 1.0325
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.40;
float vitesse2 = 0.4;

bool GaucheVert;
bool GaucheRouge;
bool AvantVert;
bool AvantRouge;
bool depart;
bool fin = false;
int BufferSon = 50;
int PosX = 0;
int PosY = 0;
float CapteurAmbiant;
float CapteurSon;
char PosRetour[10];

//Fonction d'arrêt des moteurs
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};


//Fonction permettant une rotation vers la droite
void tourneDroit(int time){

  MOTOR_SetSpeed(RIGHT, -0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse2);
  delay(time);
  MOTOR_SetSpeed(RIGHT, 0 );
  MOTOR_SetSpeed(LEFT, 0 );
  delay(50);
};

//Fonction permettant de tourner à gauche
void tourneGauche(int time){

  MOTOR_SetSpeed(RIGHT, 0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse2);
  delay(time);//28560

  MOTOR_SetSpeed(RIGHT, 0 );
  MOTOR_SetSpeed(LEFT, 0 );
  delay(50);
};

//Clear les encoders
void ClearEncoders(void){
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
};


//Initialiser les encoders
void EncoderInit(void){
  ClearEncoders();
};


//Fonction permettant l'accélération du robot jusqu'à une certaine vitesse
void Accelerate(float VitesseRecherche)
{
  int i = 1;
  for(; i<=10 ; i++)
  {
    
    static float pourcentageLM = VitesseRecherche/(10);
    static float pourcentageRM= (VitesseRecherche*STARTRATION)/(10);
    float LMSpeed = pourcentageLM*i;
    float RMSpeed = pourcentageRM*i;
    if(LMSpeed >= 0.15)
    {
    MOTOR_SetSpeed(RIGHT,RMSpeed);
    MOTOR_SetSpeed(LEFT,LMSpeed);
    delay(100);
    }
  }
}


//permet l'arret du robot
void DecelerateToAStop(float VitesseRecherche)
{
  for(int i=10;i>=1;i--)
  {
    static float pourcentageLM = VitesseRecherche/(10);
    static float pourcentageRM= (VitesseRecherche*STARTRATION)/(10);
    float LMSpeed = pourcentageLM*i;
    float RMSpeed = pourcentageRM*i;
    if(LMSpeed >= 0.15)
    {
    MOTOR_SetSpeed(RIGHT,RMSpeed);
    MOTOR_SetSpeed(LEFT,LMSpeed);
    delay(100);
    }
  }

    MOTOR_SetSpeed(RIGHT,0);
    MOTOR_SetSpeed(LEFT,0);
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}


//Fonction permettant de faire avancer le robot
void avance(int Distance, double MotorSpeed){
    ClearEncoders();
  //Read encoder value for both motor (Value readout should be 0)
    float Rspeed = MotorSpeed*STARTRATION;
    float Lspeed = MotorSpeed;
    int32_t leftPID = ENCODER_Read(LEFT);
    int32_t rightPID = ENCODER_Read(RIGHT);

    float GoalDist = Distance*(3200/WHEELCIRC);//140416 for 45cm
    Accelerate(MotorSpeed);

    while(GoalDist > ((leftPID+rightPID)/2) )
    {
      //i++;  //Pour voir les valeurs lue des encodeurs à chaque changement.
      delay(50);
      leftPID = ENCODER_Read(LEFT);
      rightPID = ENCODER_Read(RIGHT);
      if(leftPID > (rightPID+5) || leftPID >(rightPID-5))//Do this comparison first since left motor is slower at the same power percentage
      {
        Rspeed+=((leftPID-rightPID)*0.000008);
        Lspeed-=((leftPID-rightPID)*0.000004);
        MOTOR_SetSpeed(RIGHT,Rspeed);
        MOTOR_SetSpeed(LEFT,Lspeed);
        
      }
      else if(leftPID < rightPID+5 || leftPID < (rightPID-5))
      {
        Lspeed+=((rightPID-leftPID)*0.000008);   
        Rspeed-=((rightPID-leftPID)*0.000004);
        MOTOR_SetSpeed(LEFT,Lspeed);
        MOTOR_SetSpeed(RIGHT,Rspeed);
        
      }
      else if((leftPID && rightPID) == 0)
      {
        MOTOR_SetSpeed(RIGHT,Rspeed);
        MOTOR_SetSpeed(LEFT,Lspeed);
        
      }
    }
    DecelerateToAStop(MotorSpeed);
    leftPID = ENCODER_Read(LEFT);
    rightPID = ENCODER_Read(RIGHT);
    delay(500);
}


//Fonction permettant de lire les capteurs gauche
//Retourne 1 si un obstacle est détecté
//Retourne 0 si rien n'est détecté
int CheckGauche(){
  GaucheRouge = digitalRead(GaucheRougePin);
  GaucheVert = digitalRead(GaucheVertPin); 
  if (!GaucheVert || !GaucheRouge)
  {
    return 1;
  }
  return 0;
}

//Fonction permettant de lire les capteurs droite
//Retourne 1 si un obstacle est détecté
//Retourne 0 si rien n'est détecté
int CheckAvant(){
  AvantRouge = digitalRead(AvantRougePin);
  AvantVert = digitalRead(AvantVertPin);
  if (!AvantRouge || !AvantVert)
  {
    return 1;
  }
  return 0;
}


//Fonction décrivant le cas que le robot commence son positionnement à gauche 
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin 
void Gauche()
{
Serial.print("GAUCHE");
if (CheckAvant() == 0)
  {
      Serial.print("gauche");
  }
else
  {
    tourneDroit(939);
    avance(X,vitesse);
    PosX++;
    if (CheckGauche() == 0)
    {
      Serial.print("Milieu");
    }
    else
    {
      Serial.print("Droite");
      avance(X,vitesse);
      PosX++;
    }
    tourneGauche(939);
  }
  avance(Y,vitesse);
  PosY++;
}



//Fonction décrivant le cas que le robot commence son positionnement au milieu
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin
void Milieu()
{
  Serial.print("Milieu");
  if (CheckAvant() == 0)
  {
    Serial.print("avance");
  }
  else if (CheckGauche() == 0)
    {
      Serial.print("gauche");
      tourneGauche(939);
      avance(X,vitesse);
      PosX--;
      tourneDroit(939);

    }
    else
    {
      Serial.print("droite");
      tourneDroit(939);
      avance(X,vitesse);
      PosX++;
      tourneGauche(939);

    }
    avance(Y,vitesse);
    PosY++;
}



//Fonction décrivant le cas que le robot commence son positionnement à droite 
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin
void Droite(){
Serial.print("Droite");
  if (CheckAvant() == 0)
  {
    Serial.print("Droite");
  }
  else
  {
    tourneGauche(939);
    avance(X,vitesse);
    PosX--;
    if (CheckAvant() == 1)
    {
        Serial.print("Milieu");
    }
    else
    {
        Serial.print("Gauche");
      avance(X,vitesse);
      PosX--;
    }
    tourneDroit(939);
  }
  avance(Y,vitesse);
  PosY++;
}

//Fonction permettant le retour
//Cette fonction n'a pas été vérifier à date
void retour()
{
  tourneDroit(939);
  tourneDroit(939);
  avance(Y,vitesse);
  PosY--;
  while(PosY > 0)
  {
    if (PosRetour[PosY-1]<PosX)
   {
      tourneDroit(939);
      avance(X,vitesse);
      if (PosRetour[PosY-1]<PosX)
      {
       avance(X,vitesse);
     }
     tourneGauche(939);
   }
   if(PosRetour[PosY-1]>PosX)
   {
     tourneGauche(939);
     avance(X,vitesse);
     if (PosRetour[PosY-1]>PosX)
     {
       avance(X,vitesse);
     }
     tourneDroit(939);
   }
   avance(Y,vitesse);
   PosY--;
  }
}














//initialisation des Pins
void setup() {
  BoardInit();
  EncoderInit();
  AX_BuzzerON();
  delay(100);
  AX_BuzzerOFF();
  pinMode(AvantVertPin, INPUT);
  pinMode(AvantRougePin, INPUT);
  pinMode(GaucheVertPin, INPUT);
  pinMode(GaucheRougePin, INPUT);
  pinMode(CapteurAmbiantPin, INPUT);
  pinMode(CapteurSonPin, INPUT);

}


void loop()
{
  //Loop permettant l'observation des capteurs de son jusqu'au départ du sifflet
  CapteurAmbiant = analogRead(CapteurAmbiantPin);
  Serial.print("Ambiant");
  Serial.println(CapteurAmbiant);

  CapteurSon = analogRead(CapteurSonPin);
  Serial.print("CapteurSon");
  Serial.println(CapteurSon);
  
  //Boucle déclarant le départ à partir du sifflet
  if ((CapteurAmbiant + BufferSon) < CapteurSon)
  {
   depart = true;
   Serial.println("true");
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

    if (PosY == 5)
    {
      fin = true;
    }
  }

//initialisation du retour
  while (fin == true)
  {
    retour();
  }
}
