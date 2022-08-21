#include "Arduino.h"
#include <FastLED.h>
#include "led.hpp"



void init_leds(CRGB leds, int nb_leds){
  for(int i=0; i<(nb_leds/3); i++){
    leds[i] = CRGB(0, 255, 0);      //On adresse aux premier tiers des LEDs la couleur souhaité
   }

  for(int i=nb_leds/3; i<(2*nb_leds/3); i++){
    leds[i] = CRGB(255, 0, 0);      //On adresse au 2eme tiers des LEDs la couleur souhaité
  }

  for(int i=2*nb_leds/3; i<=nb_leds; i++){
    leds[i] = CRGB(0, 0, 255);      //On adresse aux dernier tiers des LEDs la couleur souhaité
  }

}

int Nb_leds_on(int rpm)   //SP retournant combien de leds il faut allumer
{
    if(rpm<RPM_1ST_LED)
    {
      return 0;   // Si les RPM sont inferieurs aux plus petits RPM d'allumage aucune lEDs de s'allume 
    }

    if(rpm>RPM_LIMITEUR)
    {
      return(-1);      // Si les RPM sont superieurs aux RPM du rupteur on retourne -1 pour signifier que les RPM sont au dela du rupteur
    }

    for(int i=1;i<20;i++)
    {
      if((RES*(i-1)+RPM_1ST_LED) <= rpm && (RES*(i)+RPM_1ST_LED) > rpm)
      {
        return i;   //Encadre les RPM entre 2 multiples de la résolution puis renvoie le multiplicateur pour trouver le nb leds a allumer
      }
    }
}

void RPM_leds(CRGB leds, int nb_leds, int nb_leds_on)   //permet de choisir la couleur d'allumage des LEDs et fait l'allumage
{
  static bool rupteur_switch = 0;
  
  init_leds(leds, nb_leds);

  if(nb_leds_on == 0){
    for(int i=0; i<nb_leds; i++){
      leds[i] = CRGB(0, 0, 0);
    }
  }

  else if(nb_leds_on == -1){
    if(rupteur_switch){ 
      for(int i=0; i<nb_leds; i++){
        leds[i] = CRGB(0, 0, 0);
      }
    }
    else{
      for(int i=0; i<nb_leds; i++){
        leds[i] = CRGB(255, 0, 0);
      }
    }
    rupteur_switch = !rupteur_switch;
  }

  else if(nb_leds_on < nb_leds){
    for(int i=nb_leds_on; i<nb_leds; i++){
      leds[i] = CRGB(0, 0, 0);       //Pour allumer le bon nombre de LEDs on donne l'ordre aux LEDs entre Nb_leds_on et NUM_LEDS de ne pas s'allumer en leur associant la couleur noir
    }
  }
  
  FastLED.show();     // Maintenant que toutes les LEDs ont étés associées à leurs couleurs on peut les allumer    
}


void Flag(CRGB leds, int nb_leds)                     //Le but du mode est de créer un allumage BLEU-BLANC-ROUGE
{
  for(int i=0; i<nb_leds/3; i++)
  {
    leds[i] = CRGB(0, 0, 255);     //Choix de la couleur du premier tiers des LEDs
  }

  for(int i=nb_leds/3; i<2*nb_leds/3; i++)
  {
    leds[i] = CRGB(200, 200, 200); //Choix de la couleur du second tiers des LEDs
  }

  for(int i=2*nb_leds/3; i<nb_leds; i++)
  {
    leds[i] = CRGB(255, 0, 0);     //Choix de la couleur du dernier tiers des LEDs
  }

  FastLED.show();                  //Allumage du drapeau
}
