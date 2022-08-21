#ifndef LED_H
#define LED_H

#define NB_LEDS 18
#define RPM_1ST_LED 3000
#define RPM_LIMITEUR 12000
#define RES (RPM_LIMITEUR - RPM_1ST_LED)/NB_LEDS
#define PERIOD_CALL_LED 100   // nb ms actualisation

void init_leds(CRGB leds, int nb_leds);
int Nb_leds_on(int rpm);
void RPM_leds(CRGB leds, int nb_leds, int nb_leds_on);
void Flag(CRGB leds, int nb_leds);


#endif
