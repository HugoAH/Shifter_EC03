#include "Pins.h"
#include "Globales.h"
#include "Shifter.h"

volatile bool p_up = false;
volatile bool p_down = false;

volatile int RPM = 0;

void Press_upshift(){
    bitClear(EIMSK, INT1);
    p_up = true;
}

void Press_downshift(){
    bitClear(EIMSK, INT0);
    p_down = true;
}

ISR(PCINT1_vect){             // Fonction appelé dès qu'une interruption PCINT1 est déclenché (CHANGE)
    if (digitalRead(pressure_sensor)){
        TCNT2 = 0;                // mettre le timer2 à 0
        bitSet(TIMSK2, TOIE2);    // Activation interrupt sur débordement du timer2
  }
  
    else{
        bitClear(TIMSK2, TOIE2);          // Désactivation interrupt sur débordement du timer2
        digitalWrite(compressor,HIGH);
  }
}

ISR(TIMER2_OVF_vect){                 // Fonction appelé dès débordement du timer2
    volatile static byte overflow_counter = 0;
    
    overflow_counter ++;
    if (overflow_counter == 30){     // ~500ms
        bitClear(TIMSK2, TOIE2);          // Désactivation interrupt sur débordement du timer2
        digitalWrite(compressor, LOW);
        overflow_counter = 0;
    }
}

ISR(PCINT2_vect){
    if (digitalRead(rpm_pin)){  // front montant
        int timer = TCNT1;
        //TCNT1 = 0;
        RPM = RPM_MAX*(1-timer*fpwm/ftim);
    }
    
    else{                       // front descendant
        TCNT1 = 0;                // mettre le timer1 à 0
    }
}

ISR(TIMER1_OVF_vect){
    RPM = 0;
}

void setup(){
    noInterrupts();            // On désactive toutes les interruptions
    
    ///// INITIALISATION DES IO
    pinMode(EV_up, OUTPUT);     // on configure les pins des électrovannes en sortie
    digitalWrite(EV_up, LOW);
    
    pinMode(EV_down, OUTPUT);
    digitalWrite(EV_down, LOW);
    
    pinMode(p_d, INPUT_PULLUP);    // quand la palette n'est pas pressé le pin est à 5V, quand on presse la palette le pin est à 0V
    pinMode(p_g, INPUT_PULLUP);    // on utilise la résistance de pull-up interne à l'arduino
        
    pinMode(pressure_sensor, INPUT_PULLUP);  // interupteur normalement ouvert
    pinMode(compressor, OUTPUT);
    digitalWrite(compressor, LOW);

    pinMode(shift_cut, OUTPUT);         // pin envoyant l'ordre à l'ECU de couper l'allumage lorsqu'on passe une vitesse (shift cut sur front descendant)
    digitalWrite(shift_cut, HIGH);

    pinMode(gear_sensor, INPUT);
    pinMode(rpm_pin, INPUT);

    ///// CONFIGURATION DES INTERRUPTIONS
    attachInterrupt(digitalPinToInterrupt(p_d), Press_upshift, FALLING);    // On attache la fonction "Press_upshift" à l'interruption sur front descendant du pin de la palette droite
    attachInterrupt(digitalPinToInterrupt(p_g), Press_downshift, FALLING);  // On attache la fonction "Press_downshift" à l'interruption sur front descendant du pin de la palette gauche

    bitSet(PCICR, PCIE1);     // Activation des interrupt sur PCINT1
    PCMSK1 = 0b00000001;      // Activation de la broche A0 sur l'interruption de PCINT1

    bitSet(PCICR, PCIE2);     // Activation des interrupt sur PCINT2
    PCMSK2 = 0b00100000;      // Activation de la broche 5 sur l'interruption de PCINT2

    bitSet(TIMSK1, TOIE1);    // Activation interrupt sur débordement du timer 1
    bitClear(TCCR1B, CS12);   // prescaler de 1
    bitClear(TCCR2B, CS11);
    bitSet(TCCR2B, CS10);
    
    bitClear(TIMSK2, TOIE2);  // Désactivation interrupt sur débordement du timer 2
    bitSet(TCCR2B, CS22);     // On divise par 1024 le timer2 (interruption toutes les 16.384ms)
    bitSet(TCCR2B, CS21);
    bitSet(TCCR2B, CS20);
    
    interrupts();             // On autorise les interruptions
}

void loop() {
    byte gear = read_gear();                                 // on rafraichi le mode du shifter (Normal ou Neutre)
    shift(&gear, &p_up, &p_down, RPM);                       // on passe le rapport
}
