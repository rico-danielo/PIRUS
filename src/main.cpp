#include <Arduino.h>
#include <LibRobus.h>

#define STARTRATION 1.0965
#define WHEELCIRC 23.938936 //Wheel circumference rounded up to 4 decimals
#define GaucheVertPin 24
#define GaucheRougePin 26
#define AvantVertPin 28
#define AvantRougePin 30
#define X 31
#define DEGD 0
#define DEGL 0

float vitesse2 = 0.4;
bool GaucheVert;
bool GaucheRouge;
float vitesse = 0.40;
static int couleurRechercher = 1;
int couleur;

//Signature de la Fonction permettant de retourner l'objet dans le but correspondant:
void retourObjet();




void ClearEncoders(void){
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
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
    Serial.print("Goal Dist: ");
    Serial.println(GoalDist);
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

//Fonction permettant de tourner à gauche
void tourneGauche(int time){

  MOTOR_SetSpeed(RIGHT, 0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse2);
  delay(time);//28560

  MOTOR_SetSpeed(RIGHT, 0 );
  MOTOR_SetSpeed(LEFT, 0 );
  delay(50);
};

//Fonction permettant une rotation de 90 degrés vers la droite
void tourneDroit(int time){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse2);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse2);
  delay(time);
  MOTOR_SetSpeed(RIGHT, 0 );
  MOTOR_SetSpeed(LEFT, 0 );
  delay(50);
}

//Fonction d'arrêt des moteurs
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

bool capteurLigne(){

}

// rouge =1, jaune = 2, vert = 3, bleu = 4
int capteurCouleur(){

}

int deposerButCoter(){
  avance(X,vitesse);
  tourneDroit(DEGD);
  avance(X,vitesse);
  arret();
  // fonction deposer l'objet
  couleurRechercher++;
  tourneDroit(2*DEGD);
  //fonction initialisation
  return couleurRechercher;
}

int deposerButFace(){
  avance(X,vitesse);
  //MOTOR_SetSpeed(LEFT, 0.3);
  //MOTOR_SetSpeed(RIGHT,0.3);
  arret();
  // fonction deposer l'objet
  couleurRechercher++;
  tourneDroit(2*DEGD);
  return couleurRechercher;
}

void trouverLigne(){
    do{
        int i = 1;
        if(i <= 10){
            MOTOR_SetSpeed(LEFT, 0.1*i);
            MOTOR_SetSpeed(RIGHT,0.1*i);
            delay(50);
        }
    }while(capteurLigne);
    arret();
    tourneDroit(DEGD);
}



// Fonction permettant de retourner l'objet dans le but correspondant:
void retourObjet() {
    switch (couleurRechercher)
    {
    case 1:
      //fonction initialisation
      //fonction rechercher objet
      if(capteurLigne()){
        //fonction suivre ligne
        if(capteurCouleur() == couleurRechercher){
            deposerButFace();
        }else{
          tourneDroit(DEGD);
          //fonction suivre ligne
          deposerButCoter();
        } 
      }else{
          trouverLigne();
           // fonction pour suivre la ligne
           deposerButCoter();
      }
      
      do {
          //fonction suivre la ligne
      }while(capteurCouleur()); //capteurCouleur()  == bleu
      tourneDroit(2*DEGD);
      break;
    
    case 2:
      //fonction rechercher objet
       if(capteurLigne()){
        //fonction suivre ligne
        if(capteurCouleur() == couleurRechercher){
            deposerButFace();
        }else{
          tourneDroit(DEGD);
          //fonction suivre ligne
          deposerButCoter();
        } 
      }else{
          trouverLigne();
           // fonction pour suivre la ligne
           deposerButCoter();
      }

      do {
          //fonction suivre la ligne
      }while(capteurCouleur()); //capteurCouleur()  == noir
      arret();
      break;

    case 3:
      //fonction rechercher objet
       if(capteurLigne()){
        //fonction suivre ligne
        if(capteurCouleur() == couleurRechercher){
            deposerButFace();
        }else{
          tourneDroit(DEGD);
          //fonction suivre ligne
          deposerButCoter();
        } 
      }else{
          trouverLigne();
           // fonction pour suivre la ligne
           deposerButCoter();
      }

      break;

    case 4:
      //fonction rechercher objet
       if(capteurLigne()){
        //fonction suivre ligne
        if(capteurCouleur() == couleurRechercher){
            deposerButFace();
        }else{
          tourneDroit(DEGD);
          //fonction suivre ligne
          deposerButCoter();
        } 
      }else{
          trouverLigne();
           // fonction pour suivre la ligne
           deposerButCoter();
      }
      arret();
      break;
    
    default:
      break;
    }
    // if(capteurCouleur() == couleur){
    //     // couleur == couleurRechercher
    //     deposerButCoter();
    // }else if(capteurCouleur() == couleur){
    //     // couleur == autre couleur 
    //     tourneGauche(DEGL);
    //     // fonction pour suivre la ligne
    //     if(capteurCouleur() == couleur){
    //       // couleur == couleurRechercher
    //       deposerButCoter();
    //     }  
    // }else{
    //   tourneGauche(2*DEGL);
    //   // fonction pour suivre la ligne
    //   if(capteurCouleur() == couleur){
    //       // couleur == couleurRechercher
    //       deposerButFace();
    //     } 
    // }
  }

void setup() {
  retourObjet();
}

void loop() {

  while (couleurRechercher < 5)
  {
    retourObjet();
  }
  
}

// void objetCouleurCorrespondant(){
//     switch (couleurRechercher)
//     {
//     case 1:
//       /* code */
//       break;
    
//     default:
//       break;
//     }
// }