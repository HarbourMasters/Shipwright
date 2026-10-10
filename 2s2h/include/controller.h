#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "PR/os_cont.h"

#define BTN_A           A_BUTTON
#define BTN_B           B_BUTTON
#define BTN_Z           Z_TRIG
#define BTN_START       START_BUTTON
#define BTN_DUP         U_JPAD
#define BTN_DDOWN       D_JPAD
#define BTN_DLEFT       L_JPAD
#define BTN_DRIGHT      R_JPAD
#define BTN_RESET       0x0080 /* "neutral reset": Corresponds to holding L+R and pressing S */
#define BTN_L           L_TRIG
#define BTN_R           R_TRIG
#define BTN_CUP         U_CBUTTONS
#define BTN_CDOWN       D_CBUTTONS
#define BTN_CLEFT       L_CBUTTONS
#define BTN_CRIGHT      R_CBUTTONS

#define CHECK_BTN_ALL(state, combo) (~((state) | ~(combo)) == 0)
#define CHECK_BTN_ANY(state, combo) (((state) & (combo)) != 0)

#endif
