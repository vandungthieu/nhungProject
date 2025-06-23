// buzzer.h
#ifndef BUZZER_H
#define BUZZER_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

void Buzzer_Init(void);
void playBeep(uint16_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif
