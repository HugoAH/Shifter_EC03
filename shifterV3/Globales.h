// DUREE ACTIVATION ELECTRO-VANNES :
#define T_up 300                   //(ms) temps d'activation du circuit pneumatique pour monter un rapport
#define T_down 300                 //(ms) temps d'activation du circuit pneumatique pour descendre un rapport
#define T_increment 10             //(ms) pas d'incrementation pour la recherce du neutre
#define T_neutre_base 50

// SECURITE 
#define Gear_max 5
#define RPM_downshift_max 9000
#define RPM_min_allume 1000
#define T_rearmage 500               //(ms) durée pause après passage d'un rapport

//RPM
#define RPM_MAX 15000
#define ftim 16000000
#define fpwm 250.0
