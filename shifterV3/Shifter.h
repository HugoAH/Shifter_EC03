#ifndef SHIFTER_H
#define SHIFTER_H

void shift(byte *gear, volatile bool *p_up, volatile bool *p_down, int RPM);
void NEUTRE(byte *gear);
void UPSHIFT(void);
void DOWNSHIFT(void);
byte read_gear(void);
void wait(int waiting_time_ms);
#endif