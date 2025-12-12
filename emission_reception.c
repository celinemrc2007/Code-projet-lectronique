#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <RF24.h>

#define TAILLE_MAX 32

//Constantes nrf24
#define pinCE     7                               // On associe la broche "CE" du NRF24L01 à la sortie digitale D7 de l'arduino
#define pinCSN    8                               // On associe la broche "CSN" du NRF24L01 à la sortie digitale D8 de l'arduino
#define tunnel1 "PIPE1"                           // On définit un premier "nom de tunnel" (5 caractères), pour pouvoir envoyer des données à l'autre NRF24
#define tunnel2 "PIPE2"                           // On définit un second "nom de tunnel" (5 caractères), pour pouvoir recevoir des données de l'autre NRF24
RF24 radio(pinCE, pinCSN);                        // Instanciation du NRF24L01
const byte adresses[][6] = {tunnel1, tunnel2};    // Tableau des adresses de tunnel, au format "byte

void emission_receptionSetup (int canal) {
  //Affectation nrf24
  radio.begin();                           // Initialisation du module NRF24
  radio.setChannel(canal);                 //Permet que le canal choisi puisse envoyer et recevoir des donnes         
  radio.openWritingPipe(adresses[0]);      // Ouverture du "tunnel1" en ÉCRITURE
  radio.openReadingPipe(1, adresses[1]);   // Ouverture du "tunnel2" en LECTURE
  radio.setPALevel(RF24_PA_MIN);           // Sélection d'un niveau "MINIMAL" pour communiquer (pas besoin d'une forte puissance, pour nos essais)

  delay(2000);              // puis démarrage du programme
  radio.startListening(); 
}

void changerCanal (int canal) {
  radio.stopListening();
  radio.setChannel(canal);
  radio.startListening();
}


// ******** ENVOI ********
void emettreMessage (char *pseudo_envoye, int *frequence, char *message, int *importance) {

  bool reussite;
  radio.stopListening();
  delay(1000);

   reussite = radio.write(pseudo_envoye, TAILLE_MAX); 
  if (!reussite) {
    Serial.print("ECHEC envoi du pseudo ... ");
  }

  reussite = radio.write(frequence, sizeof(int));
  if (!reussite) {
    Serial.println("ECHEC envoi de la fréquence du buzzer... ");
  }
  reussite = radio.write(importance, sizeof(int));
  if (!reussite) {
    Serial.print("ECHEC envoi de l'importance du message ... ");
  }
 
  for(unsigned int i=0; i<strlen(message); i+=TAILLE_MAX) {
    reussite = radio.write(&message[i], TAILLE_MAX);
    if (!reussite) {
      Serial.print("ECHEC envoi d'un bloc du message.");
    }
  }                        
  delay(500);
  radio.startListening();
}

 // ******** RÉCEPTION ********
bool recevoirMessage (char *pseudo_envoye, int *frequence, char *message, int *importance  ) {
  if(radio.available()) {                               // On regarde si une donnée a été reçue
    delay(10);
    radio.read(frequence, sizeof(int));          // Lit le nombre envoyé par l'utilisateur
    delay(10);
    radio.read(importance, sizeof(int));        // Lit le nombre envoyé par l'utilisateur
    delay(10);
    radio.read(pseudo_envoye, TAILLE_MAX);
    int i=0;
    while(radio.available()) {
      delay(10);
      radio.read(&message[i], TAILLE_MAX);
      i+=TAILLE_MAX;
      Serial.println("Bloc recu.");   
    }
    return true; 
  }
  return false;
}
 
 

