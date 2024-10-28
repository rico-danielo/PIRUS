#include <LibRobus.h>
#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);
int couleur == (0);
int detecteur_de_ligne == (0);
int Initilisation (couleur){
  if (couleur == "rouge"){
    while (detecteur_de_ligne == (0)){
    MOTOR_SetSpeed(RIGHT,(0.3));
    MOTOR_SetSpeed(LEFT,(O.3));
    }
    while (couleur != "noir" && detecteur_de_ligne == (1)){
      MOTOR_SetSpeed(RIGHT,(0.5));
      MOTOR_SetSpeed(LEFT,(O.5));
    }
    tourneDroit(DEGD);
    while (couleur != "jaune" && detecteur_de_ligne == (1)){
      MOTOR_SetSpeed(RIGHT,(0.5));
      MOTOR_SetSpeed(LEFT,(O.5));
    }
    tourneDroit(2*DEGD);
  }
  if (couleur == "jaune"){
    while(detecteur_de_ligne ==(0)){
      MOTOR_SetSpeed(RIGHT,(0.3));
      MOTOR_SetSpeed(LEFT,(O.3));
    }    
  }
  if (couleur == "vert"){
 while (detecteur_de_ligne == (0)){
      MOTOR_SetSpeed(RIGHT,(0.3));
      MOTOR_SetSpeed(LEFT,(O.3));
    }
    while (couleur != "noir" && detecteur_de_ligne == (1)){
     MOTOR_SetSpeed(RIGHT,(0.5));
     MOTOR_SetSpeed(LEFT,(O.5));
    }
    while (detecteur_de_ligne == (0)){
      MOTOR_SetSpeed(RIGHT,(0.3));
      MOTOR_SetSpeed(LEFT,(O.3));
    }
     while (couleur != "jaune" && detecteur_de_ligne == (1)){
      MOTOR_SetSpeed(RIGHT,(0.5));
      MOTOR_SetSpeed(LEFT,(O.5));
    }
    tourneDroit(2*DEGD);    
  }
  }
  if (couleur == "bleu"){
   while (detecteur_de_ligne == (0)){
    MOTOR_SetSpeed(RIGHT,(0.3));
    MOTOR_SetSpeed(LEFT,(O.3));
    }
    while (couleur != "noir" && detecteur_de_ligne == (1)){
      MOTOR_SetSpeed(RIGHT,(0.5));
      MOTOR_SetSpeed(LEFT,(O.5));
    }
    tourneGauche(DEGL);
    while (couleur != "jaune" && detecteur_de_ligne == (1)){
      MOTOR_SetSpeed(RIGHT,(0.5));
      MOTOR_SetSpeed(LEFT,(O.5));
    }
    tourneDroit(2*DEGD);

}

#define LignePin 30 //pas oublié de mettre dans la fonction setup
void SuiveurLigne()
{
  float SortieLigne = analogRead(30) * (5.0 / 1023.0);
  MOTOR_SetSpeed(LEFT, 0.5);
  MOTOR_SetSpeed(RIGHT, 0.5);
  if(SortieLigne >= 3.80 && SortieLigne <= 4.10)
  {
    while(SortieLigne >= 3.80)
    {
      MOTOR_SetSpeed(LEFT, 0.49);
    }
  }
  else if(SortieLigne >= 1.80 && SortieLigne <= 2.20)
  {
    while(SortieLigne >= 1.80)
    {
      MOTOR_SetSpeed(RIGHT, 0.49);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}