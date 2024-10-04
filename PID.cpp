/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/
#include <LibRobus.h>

/*
Variables globales et defines
#
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/
#define WHEELCIRC 23.938936 //Wheel circumference rounded up to 4 decimals
#define ENCODERFULLTURN 3200
#define STARTRATION 1.0325

bool bumperArr;
int vertpin = 48;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.40;

/*
Vos propres fonctions sont creees ici
*/

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
};

void recule(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -vitesse);
};

float vitesse2 = 0.4;
void tourneDroit(){
  Serial.print("Tourner 3 tours");
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse2);
  delay(11266);
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
    if(LMSpeed >= 0.15)
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
  /*for(int n = 0; n<i;n++)
  {
    Serial.print("PidL ");
    Serial.print(n);
    Serial.print(" :");
    Serial.println(PidLArray[n]);
    Serial.print("PidR ");
    Serial.print(n);
    Serial.print(" :");
    Serial.println(PidRArray[n]);
    Serial.print("Différence ");
    Serial.print(n);
    Serial.print(" :");
    Serial.println(PidLArray[n]-PidRArray[n]);
  }*/
};

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
};
/*void Ajustement(float Vitesse)
{
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
}*/
void Avancer(int Distance, double MotorSpeed){

    ClearEncoders();
  //Read encoder value for both motor (Value readout should be 0)
    float Rspeed = MotorSpeed*STARTRATION;
    float Lspeed = MotorSpeed;
    int32_t leftPID = ENCODER_Read(LEFT);
    int32_t rightPID = ENCODER_Read(RIGHT);

    float GoalDist = 45*(3200/WHEELCIRC);//140416 for 45cm

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

      /*PidLArray[i] = leftPID;//Pour voir les valeurs lue des encodeurs à chaque changement.
      PidRArray[i] = rightPID;
      SpeedLArray[i] = Rspeed;
      SpeedRArray[i] = Lspeed;*/
    }

    DecelerateToAStop(MotorSpeed);
    leftPID = ENCODER_Read(LEFT);
    rightPID = ENCODER_Read(RIGHT);
    delay(500);
};
/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/
void setup(){
  BoardInit(); 
  EncoderInit();

  delay(3000);

  /*tourneDroit();
  delay(500);*/

  for(int u = 0; u<9; u++)
  {
  Avancer(140416,0.40);
  delay(500);
  }

  tourneGauche(1878);
  delay(1000);
  for(int u = 0; u<9; u++)
  {
  Avancer(140416,0.40);
  delay(500);
  }


  /*delay(500);
  //initialisation
  float Speed = 0.40;


  

  //Clear both Encoder readouts
  ClearEncoders();
  //Read encoder value for both motor (Value readout should be 0)
  int32_t leftPID = ENCODER_Read(LEFT);
  int32_t rightPID = ENCODER_Read(RIGHT);
  //Turn on both motor
  int32_t PidLArray[200] = {0};
  int32_t PidRArray[200] = {0};
  uint32_t SpeedLArray[200] = {0};
  uint32_t SpeedRArray[200] = {0};
  int i = 0;
  int32_t GoalDist;
  uint8_t dist = 15;

for(int h = 0; h<2;h++)
{

  for(int u = 0; u < 9; u++)
  {
    GoalDist = (140416)/WHEELCIRC;
    Accelerate(Speed);
    

    while(GoalDist > ((leftPID+rightPID)/2) )
    {
      i++;

      delay(50);
      if((Rspeed < 60) || (Lspeed < 60))
      {
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
        leftPID = ENCODER_Read(LEFT);
        rightPID = ENCODER_Read(RIGHT);
        PidLArray[i] = leftPID;
        PidRArray[i] = rightPID;
        SpeedLArray[i] = Rspeed;
        SpeedRArray[i] = Lspeed;
      }
      else
      {
      DecelerateToAStop(Speed,10);
      Lspeed = 0;
      Rspeed=0;
      }
      tourneDroit();
    }

    DecelerateToAStop(Speed,10);
    leftPID = ENCODER_Read(LEFT);
    rightPID = ENCODER_Read(RIGHT);
    leftPID = ENCODER_Read(LEFT);
    rightPID = ENCODER_Read(RIGHT);
    delay(500);
  }
}
    Serial.println("-------------------------------------------------------------");
    Serial.print("Right motor speed: ");
    Serial.println(Rspeed);
    
    Serial.print("Left motor speed: ");
    Serial.println(Lspeed);
    
    for(int n = 0; n<i;n++)
    {
      Serial.print("PidL ");
      Serial.print(n);
      Serial.print(" :");
      Serial.println(PidLArray[n]);
      Serial.print("PidR ");
      Serial.print(n);
      Serial.print(" :");
      Serial.println(PidRArray[n]);
          Serial.print("Différence ");
      Serial.print(n);
      Serial.print(" :");
      Serial.println(PidLArray[n]-PidRArray[n]);
          Serial.print("SpeedL ");
      Serial.print(n);
      Serial.print(" :");
      Serial.println(SpeedLArray[n]);
      Serial.print("SpeedR ");
      Serial.print(n);
      Serial.print(" :");
      Serial.println(SpeedRArray[n]);
    }
Serial.print("Distance parcourue: ");
Serial.println((((leftPID+rightPID)/2)* WHEELCIRC )/ENCODERFULLTURN);*/


    while(1);
  //beep(3)]
}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {



 
  _delay_ms(0);
}


