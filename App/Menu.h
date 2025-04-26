#include <stdint.h>
#ifndef __MENU_H
#define __MENU_H


#include <stdint.h>
#include <stdbool.h>

extern volatile uint8_t MenuAutoBackSeconds;
extern volatile uint8_t is_Lv_1_Menu_Open;

void AboutMessage(void);
void Lv_1_Menu(void);

#endif
