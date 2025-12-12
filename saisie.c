#include <Arduino.h>

//Demande à l'utilisateur s'il souhaite changer les parametres sauvegardees ou ecrire un message
void afficherMenu () {
    //Serial.println("Bipeur pret a recevoir. Que souhaitez-vous faire aujourd'hui ?");
    Serial.println("1: Changer les parametres sauvegardes");
    Serial.println("2: Ecrire un message");
    //Serial.println("Veuillez saisir le numéro correspondant à votre choix.");
}

//Demande à l'utilisateur de saisir son pseudo
void saisiePseudo (char *pseudo_recue) {
    Serial.println("Saisir votre pseudo.");
    while (!Serial.available()) delay(100);
    Serial.readBytesUntil('\n', pseudo_recue, 16);
}

//Demande à l'utilisateur de choisir la fréquence du buzzer entre les options proposées
int saisieFrequence () {
    Serial.println("A quelle frequence voulez-vous regler le buzzer ?");
    Serial.println("3: 200 Hz");
    Serial.println("4: 400 Hz");
    Serial.println("5: 600 Hz");
    Serial.println("6: 800 Hz");
    Serial.println("7: 1000 Hz");
    while (!Serial.available()) delay(100);
    int frequence = Serial.parseInt();
    while ((frequence<3)||(frequence>7)) {
        while (!Serial.available()) delay(100);
        frequence = Serial.parseInt();  
    }
    return frequence;
}
                                                                                
void saisieMessage (char *message_envoye) {                                                           //Fonction de saisie du message par l'utilisateur
    //Demande à l'utilisateur de saisir son message
    Serial.println("Veuillez saisir votre message.");
    while (!Serial.available()) delay(100);
    Serial.readBytesUntil('\n', message_envoye, 128);
}

//Demande à l'utilisateur si son message est important
int saisieImportance () {
    Serial.println("Votre message est-il important ?");
    Serial.println("8: urgent");
    Serial.println("9: priopritaire");
    Serial.println("10: non prioritaire");
    while (!Serial.available()) delay(100);
    int importance = Serial.parseInt();
    while ((importance<8)||(importance>10)) {
        while (!Serial.available()) delay(100);
        importance = Serial.parseInt();
    }
    return importance;
}


