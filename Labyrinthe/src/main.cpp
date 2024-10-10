#include <Arduino.h>
#include <libRobus.h>

//Définition des variables et des constantes
#define GaucheVertPin 2
#define GaucheRougePin 3
#define AvantVertPin 4
#define AvantRougePin 5
#define CapteurSonPin (A3)
#define CapteurAmbiantPin 8
#define X 45
#define Y 95

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
int PosX = 1;
int PosY = 0;
float CapteurAmbiant;
float CapteurSon;
char PosRetour[10];

//Fonction d'arrêt des moteurs
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};


//Fonction permettant une rotation de 90 degrés vers la droite
void tourneDroit(int time){
  Serial.print("Tourner 3 tours");
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse2);
  delay(time);
  MOTOR_SetSpeed(RIGHT, 0 );
  MOTOR_SetSpeed(LEFT, 0 );
  Serial.println("fin des 3 tours");
};

void tourneGauche(int time){
  Serial.print("Tourner 1/2 tours");
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse2);
  delay(time);//28560
  
  MOTOR_SetSpeed(RIGHT, 0 );
  MOTOR_SetSpeed(LEFT, 0 );
  Serial.println("fin des 1/2 tours");
};
void ClearEncoders(void){
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
};

void EncoderInit(void){
  ClearEncoders();
};

void Accelerate(float VitesseRecherche)
{
  int i = 1;
  for(; i<=10 ; i++)
  {
    
    static float pourcentageLM = VitesseRecherche/(10);
    static float pourcentageRM= (VitesseRecherche*STARTRATION)/(10);
    float LMSpeed = pourcentageLM*i;
    float RMSpeed = pourcentageRM*i;
        //A TESTER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if(LMSpeed <= 0.15)
    {
    MOTOR_SetSpeed(RIGHT,RMSpeed);
    MOTOR_SetSpeed(LEFT,LMSpeed);
    /*int32_t leftPID = ENCODER_Read(LEFT);
    int32_t rightPID = ENCODER_Read(RIGHT);
    PidLArray[i] = leftPID;
    PidRArray[i] = rightPID;*/
    delay(100);
    }
  }
}

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


void avance(int Distance, double MotorSpeed){
Serial.print("avance2");
    ClearEncoders();
  //Read encoder value for both motor (Value readout should be 0)
    float Rspeed = MotorSpeed*STARTRATION;
    float Lspeed = MotorSpeed;
    int32_t leftPID = ENCODER_Read(LEFT);
    int32_t rightPID = ENCODER_Read(RIGHT);

    float GoalDist = 95*(3200/WHEELCIRC);//140416 for 45cm

    Accelerate(MotorSpeed);
    
Serial.print("avance3");
    while(GoalDist > ((leftPID+rightPID)/2) )
    {
      //i++;  //Pour voir les valeurs lue des encodeurs à chaque changement.
      delay(50);
      leftPID = ENCODER_Read(LEFT);
      rightPID = ENCODER_Read(RIGHT);
      if(leftPID > (rightPID+5) || leftPID >(rightPID-5))//Do this comparison first since left motor is slower at the same power percentage
      {
        Serial.print("avance31");
        Rspeed+=((leftPID-rightPID)*0.000008);
        Lspeed-=((leftPID-rightPID)*0.000004);
        MOTOR_SetSpeed(RIGHT,Rspeed);
        MOTOR_SetSpeed(LEFT,Lspeed);
        
      }
      else if(leftPID < rightPID+5 || leftPID < (rightPID-5))
      {
        Serial.print("avance32");
        Lspeed+=((rightPID-leftPID)*0.000008);   
        Rspeed-=((rightPID-leftPID)*0.000004);
        MOTOR_SetSpeed(LEFT,Lspeed);
        MOTOR_SetSpeed(RIGHT,Rspeed);
        
      }
      else if((leftPID && rightPID) == 0)
      {
        Serial.print("avance33");
        MOTOR_SetSpeed(RIGHT,Rspeed);
        MOTOR_SetSpeed(LEFT,Lspeed);
        
      }

      /*PidLArray[i] = leftPID;//Pour voir les valeurs lue des encodeurs à chaque changement.
      PidRArray[i] = rightPID;
      SpeedLArray[i] = Rspeed;
      SpeedRArray[i] = Lspeed;*/
    }
Serial.print("avance4");
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
  if (GaucheVert && GaucheRouge)
  {
   return 0;
  }
  return 1;
}

//Fonction permettant de lire les capteurs droite
//Retourne 1 si un obstacle est détecté
//Retourne 0 si rien n'est détecté
int CheckAvant(){
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
if (CheckAvant() == 0)
  {
    avance(Y,vitesse);
      PosY++;
  }
else
  {
    tourneDroit(939);
    avance(X,vitesse);
    PosX++;
    if (CheckGauche == 0)
    {
      tourneGauche(939);
      avance(Y,vitesse);
      PosY++;
    }
    else
    {
      avance(X,vitesse);
      PosX++;
      tourneGauche(939);
      avance(Y,vitesse);
      PosY++;
    }
  }
}



//Fonction décrivant le cas que le robot commence son positionnement au milieu
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin
void Milieu()
{
  if (CheckAvant() == 0)
  {
    avance(Y,vitesse);
    PosY++;
  }
  else
  {
    if (CheckGauche() == 0)
    {
      tourneGauche(939);
      avance(X,vitesse);
      PosX--;
      tourneDroit(939);
      avance(Y,vitesse);
      PosY++;
    }
    tourneDroit(939);
    avance(X,vitesse);
    PosX++;
    tourneGauche(939);
    avance(Y,vitesse);
    PosY++;
  }
}



//Fonction décrivant le cas que le robot commence son positionnement à droite 
//Uniquement pour l'allée
//Incrémente automatiquement la position de Y à la fin
void Droite(){
  if (CheckAvant() == 0)
  {
    avance(Y,vitesse);
    PosY++;
  }
  else
  {
    tourneGauche(939);
    avance(X,vitesse);
    PosX--;
    if (CheckAvant() == 1)
    {
      tourneDroit(939);
      avance(Y,vitesse);
      PosY++;
    }
    else
    {
      avance(X,vitesse);
      PosX--;
      tourneDroit(939);
      avance(Y,vitesse);
      PosY++;
    }
  }
}


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
tourneDroit(30000)
tourneDroit(8000);
}
  //Boucle déclarant le départ à partir du sifflet
 /* if ((CapteurAmbiant + BufferSon) < CapteurSon)
  {
   depart = true;
   Serial.println("true");
  }
*/ 


  //Boucle déclarant chaque cas possible selon la position du robot pour l'allée
  /*while (PosY<5 && depart)
  {
    Serial.print("avance");
    avance(Y,vitesse);

  }

}*/
   /* switch (PosX)

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

  while (fin == true)
  {
    retour();
  }
}
*/