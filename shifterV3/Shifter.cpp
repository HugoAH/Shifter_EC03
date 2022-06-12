#include "Arduino.h"
#include "Globales.h"
#include "Pins.h"
#include "Shifter.h"

void shift (byte *gear, volatile bool *p_up, volatile bool *p_down, int RPM){
    if (*p_down == true and RPM < RPM_downshift_max){   //si on veut passer la vitesse inférieur
      if (*gear <= 1){  // si on est en 1ere, il faut faire tourner le selecteur dans le sens inverse (on tourne dans le sens de la montée de rapport)
        NEUTRE(gear);
      }
      else{
        DOWNSHIFT();
      }
    }
    
    else if (*p_up == true and *gear < Gear_max){   //si on veut passer la vitesse supérieure      
      if (*gear == 0){  // si on est au neutre, il faut faire tourner le selecteur dans le sens inverse (on tourne dans le sens de la descente de rapport)
        DOWNSHIFT();
      }
      else{
        UPSHIFT();
      }
    }

    if (*p_down or *p_up){
      *p_down = false;
      *p_up = false;
      wait(T_rearmage);           // On attend T_rearmage ms
      
      EIFR = (0b11<<0);           // Reset les flags d'interruption pour éviter faux trigger
      // Pour reset EIFR il faut écrire 1, pas mettre la valeur du bit à 1 !
      
      EIMSK |= (0b11<<0);         // Réactivation des interruptions palettes  
      /* Equivalent to : 
      bitSet(EIMSK, INT0);
      bitSet(EIMSK, INT1);
      */
    }

}

////
void NEUTRE(byte *gear){
    int nb_try = 0;
  
    while (!digitalRead(p_g) and *gear!=0){   // On vérifie qu'on est pas déjà au neutre et que le pilote appuis sur la palette de gauche
        digitalWrite(shift_cut,LOW);
        digitalWrite(shift_cut,HIGH);
        
        if (*gear==1){
            digitalWrite(EV_up,HIGH);  //on envoie 5V sur la commande pneumatique pour monter le rapport (on ferme le transistor)
            wait(T_neutre_base + T_increment*nb_try);
            digitalWrite(EV_up,LOW);
        }
        else if (*gear==2){
            digitalWrite(EV_down,HIGH);  //on envoie 5V sur la commande pneumatique pour monter le rapport (on ferme le transistor)
            wait(T_neutre_base + T_increment*nb_try);
            digitalWrite(EV_down,LOW);
        }
        
        byte last_gear = *gear;         // On update que le rapport de boite engagé
        *gear = read_gear();

        if (*gear == last_gear){
            nb_try += 1;                // On incrémente le nb d'essaie pour augmenter la durée d'activation des électrovannes à l'essaie suivant si le rapport n'est pas passé
        }
        else{
            nb_try= 0;                  // Si on a sauté le point mort, on réinitialise le compteur
        }

        wait(100);                     
    }
}

////
void UPSHIFT(){
    digitalWrite(shift_cut,LOW);  //on envoie l'info à l'ECU qu'on shift pour qu'il coupe l'allumage pendant un temps réglé dans l'ECU
    digitalWrite(shift_cut,HIGH);

    digitalWrite(EV_up,HIGH);   //on envoie 5V sur la commande pneumatique pour monter le rapport (on ferme le transistor)
    wait(T_up);                 // On attend T_up milliseconde
    digitalWrite(EV_up,LOW);    //on stop le circuit pneumatique
}

////
void DOWNSHIFT(){
    digitalWrite(shift_cut,LOW);  //on envoie l'info à l'ECU qu'on shift pour qu'il coupe l'allumage pendant un temps régler dans l'ECU
    digitalWrite(shift_cut,HIGH);

    digitalWrite(EV_down,HIGH);   //on envoie 5V sur la commande pneumatique pour monter le rapport (on ferme le transistor)
    wait(T_down);                 //pendant T_down milliseconde
    digitalWrite(EV_down,LOW);    //on stop le circuit pneumatique
}

byte read_gear(){
    const float U_ref_gear[6] = {1, 1, 1, 1, 1, 1};   // Tension de seuil à paramétrer
    
    float U = analogRead(gear_sensor)*5/1023.0;

    if (U<U_ref_gear[0]){
        return(1);
    }
    else if (U<U_ref_gear[1]){
        return(0);
    }
    else if (U<U_ref_gear[2]){
        return(2);
    }
    else if (U<U_ref_gear[3]){
        return(3);
    }
    else if (U<U_ref_gear[4]){
        return(4);
    }
    else if (U<U_ref_gear[5]){
        return(5);
    }
    else{
        return(6);
    }
}

void wait(unsigned long waiting_time_ms){   // Fonction delais non bloquante
  unsigned long start_time = millis();                // millis() overflow au bout de 50 jours
  while (millis() - start_time < waiting_time_ms){
    __asm__ __volatile__ ("nop\n\t");                 // Instruction assembleur NOP : ne rien faire
  }
}
