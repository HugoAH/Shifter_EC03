#define EV_up 9                       //pin sorties pour actionner le circuit pneumatique 
#define EV_down 8

#define compressor 10                //pin activant le compresseur
#define pressure_sensor A0
#define shift_cut 7                   //pin servant à dire à informer l'ECU qu'on shift pour couper l'allumage (régler temps de coupure dans le logiciel de gestion de l'ECU)

#define p_d 2                         //pin entrée pour donner l'ordre de changer de rapport (palettes droite/gauche)
#define p_g 3

#define gear_sensor A7
#define rpm_pin 5

#define LED_PIN 13       //METTRE LE PIN SUR LEQUEL ON BRANCHE LES LEDS
