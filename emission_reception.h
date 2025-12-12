#include <Arduino.h>
void emission_receptionSetup (int canal);
void emettreMessage (char *pseudo_envoye, int *frequence, char *message, int *importance);
bool recevoirMessage ( char *pseudo_envoye, int *frequence, char *message, int *importance);
void changerCanal (int canal);
