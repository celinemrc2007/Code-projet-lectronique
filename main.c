
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include "emission_reception.h"
#include "saisie.h"

//Constantes encodeur
#define pinArduinoRaccordementSignalSW  2       // La pin D2 de l'Arduino recevra la ligne SW du module KY-040
#define pinArduinoRaccordementSignalCLK 3       // La pin D3 de l'Arduino recevra la ligne CLK du module KY-040
#define pinArduinoRaccordementSignalDT  4

//Constantes ecran OLED
#define LARGEUR_ECRAN 128 // Largeur de l'écran OLED, en pixels
#define HAUTEUR_ECRAN 32 // 64 // Hauteur de l'écran OLED, en pixels
// Déclaration de l'écran connecté en I2C (pins SDA et SCL)
Adafruit_SSD1306 display(LARGEUR_ECRAN, HAUTEUR_ECRAN, &Wire, -1);       // La pin D4 de l'Arduino recevra la ligne DT du module KY-040
//Si l'utilisateur appuie sur le bouton le buzzer s'arrête et la led rgb s'éteint.
int xPosition = 0;
int yPosition = 0;

// Variables encodeur
                                    // Cette variable nous permettra de savoir combien de crans nous avons parcourus sur l'encodeur
                                    // (sachant qu'on comptera dans le sens horaire, et décomptera dans le sens anti-horaire)
int etatPrecedentLigneSW;           // Cette variable nous permettra de stocker le dernier état de la ligne SW lu, afin de le comparer à l'actuel
int etatPrecedentLigneCLK;          // Cette variable nous permettra de stocker le dernier état de la ligne CLK lu, afin de le comparer à l'actuel

//Variables led rgb
int redPin = 5;
int greenPin = 6;
int bluePin = 9;

//Variables buzzer
int buzzerPin = 10;
float Tone;

//Variables bouton
int boutonPin = A6;
int etat_bouton=0;

//Variable pseudo
char pseudo_envoye[16] = "Les ingenieurs";
char pseudo_recue[16];

//Variable message
char message[128];

//Variable canal utilisé
int canal = 50;

//Variable son du buzzer
int frequence = 5;

//Variable importance du message
int importance;

//Sauvegarde des données de l'utilisateur
/**
void sauvegardePseudo () {
  EEPROM.put (0, pseudo);
}

void sauvegardeFrequence () {
  EEPROM.put (30, frequence);
}

//Recupere les données de l'utilisateur
void recuperationPseudo () {
  EEPROM.get (0, pseudo); 
}

void recuperationFrequence () {
  EEPROM.get (30, frequence); 
}
*/

void choisirCanal () {
// Lecture des signaux du KY-040 arrivant sur l'arduino
    int etatActuelDeLaLigneCLK = digitalRead(pinArduinoRaccordementSignalCLK);
    int etatActuelDeLaLigneDT = digitalRead(pinArduinoRaccordementSignalDT);
    Serial.println(etatActuelDeLaLigneCLK);
    Serial.println(etatActuelDeLaLigneDT);
    
   

    // ******************************************
    // On regarde si la ligne CLK a changé d'état
    // ******************************************
    if(etatActuelDeLaLigneCLK != etatPrecedentLigneCLK) {

      // On mémorise cet état, pour éviter les doublons
      etatPrecedentLigneCLK = etatActuelDeLaLigneCLK;

      if(etatActuelDeLaLigneCLK == LOW) {
        
        // On compare le niveau de la ligne CLK avec celui de la ligne DT
        // --------------------------------------------------------------
        // Nota : - si CLK est différent de DT, alors cela veut dire que nous avons tourné l'encodeur dans le sens horaire
        //        - si CLK est égal à DT, alors cela veut dire que nous avons tourné l'encodeur dans le sens anti-horaire

        if(etatActuelDeLaLigneCLK != etatActuelDeLaLigneDT) {
            // CLK différent de DT => cela veut dire que nous comptons dans le sens horaire
            // Alors on incrémente le compteur
            if (canal<126) canal++;
        }
        else {
            // CLK est identique à DT => cela veut dire que nous comptons dans le sens antihoraire
            // Alors on décrémente le compteur
            if (canal>0) canal--;
        }
        changerCanal(canal);

        // Petit délai de 1 ms, pour filtrer les éventuels rebonds sur CLK
        delay(1);
      }
    }   
}

void afficherMessage() {
    display.setCursor(xPosition,yPosition);
    display.print("Canal: ");
    display.print(canal);
    display.print("  De ");
    display.print(pseudo_recue);
    display.println(":");
    display.println(message);
    display.display();

}

// Joue la fréquence donné par l'utilisateur sur le buzzer
void gererFrequence (int frequence) {
  switch (frequence) {
    case 3: tone(buzzerPin, 200);
      break;
    case 4: tone(buzzerPin, 400);
      break;
    case 5: tone(buzzerPin, 600);
      break;
    case 6: tone(buzzerPin, 800);
      break;
    case 7: tone(buzzerPin, 1000);
      break;  
  }
}

//Allume la led rgb d'une certaine couleur pour indiquer l'importance du message reçue
void gererImportance (int importance_recue) {
switch (importance) {
    //La led s'allume en rouge si le message est urgent.
    case 8: analogWrite(redPin, HIGH);
            analogWrite(greenPin, LOW);
            analogWrite(bluePin, LOW);
      //break;
    //La led s'allume en verte si le message est prioritaire.
    case 9: analogWrite(redPin, LOW);
            analogWrite(greenPin, HIGH);
            analogWrite(bluePin, LOW);
      break;
    //La led s'allume en bleue si le message est non prioritaire.
    case 10: analogWrite(redPin, LOW);
            analogWrite(greenPin, LOW);
            analogWrite(bluePin, HIGH);
      break;
  }
}

//Sur commande de l'utilisateur, arrête tous les signaux 
//qui avertissent l'utilisateur de la récéption d'un message
void arreterSignaux () {
      noTone(buzzerPin); // arrêt du buzzer
      analogWrite(redPin, LOW);
      analogWrite(greenPin, LOW);
      analogWrite(bluePin, LOW);
}

// ========================
// Initialisation programme
// ========================
void setup () {
  Serial.begin(115200);

  //Affectation buzzer
  pinMode(buzzerPin, OUTPUT);
  noTone(buzzerPin);

  emission_receptionSetup (canal);

  //Affectation led rgb et led verte
  // Configuration des broches de la led rgb en sortie
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  //Affectation bouton
  pinMode(boutonPin,INPUT);
 
//Affectation encodeur
  // Configuration des pins de notre Arduino Nano en "entrées", car elles recevront les signaux du KY-040
  pinMode(pinArduinoRaccordementSignalSW, INPUT);         
  pinMode(pinArduinoRaccordementSignalDT, INPUT);
  pinMode(pinArduinoRaccordementSignalCLK, INPUT);

  // Mémorisation des valeurs initiales, au démarrage du programme
  etatPrecedentLigneSW = digitalRead(pinArduinoRaccordementSignalSW);
  etatPrecedentLigneCLK = digitalRead(pinArduinoRaccordementSignalCLK);

  // Petite pause pour laisser se stabiliser les signaux, avant d'attaquer la boucle loop
  delay(200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();
  delay(500); //500ms

  //recuperationPseudo ();
  //recuperationFrequence ();
  
  afficherMenu ();
}

void loop() {
  if (Serial.available() > 0) {                                              // Vérifie si des données sont disponibles dans le moniteur série
    int choix = Serial.parseInt();
    while ((choix>2)||(choix<1)) {
      while (!Serial.available()) delay(100);
      choix = Serial.parseInt();
    }
    switch (choix) {
      case 1: 
        saisiePseudo (pseudo_envoye);
        frequence = saisieFrequence ();
        //sauvegardePseudo ();
        //sauvegardeFrequence ();
        break;
        
      case 2:
        saisieMessage (message);
        importance = saisieImportance ();
        emettreMessage (pseudo_envoye, &frequence, message, &importance);
        afficherMenu();
        break;
    } 

  }
  
  if (recevoirMessage (pseudo_recue, &frequence, message, &importance)) {
    Serial.println("recu");
    gererFrequence (frequence);
    gererImportance (importance);
  } else {
    display.setCursor(xPosition,yPosition);
    display.print("Canal: ");
    display.println(canal);
    display.display();
  }
    // Quand le texte est entièrement sorti à droite, repartir à gauche
    /*
    if (xPosition <= LARGEUR_ECRAN) {
    xPosition ++; // Défilement vers la droite
    }
    else {
    xPosition = 0; // retour à gauche
    yPosition = 8; //retour à la ligne d'en dessous
    }
    delay(30); // vitesse du défilement
    */             
  choisirCanal();
  etat_bouton = analogRead(boutonPin);
  if (etat_bouton < 300) {
    Serial.println("Bonton appuye");
    arreterSignaux ();
  }
  delay(1000);
  //Serial.println("loop5");
}




