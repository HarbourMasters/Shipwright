#ifndef MOUSE_H
#define MOUSE_H

#pragma once

#include <libultraship/libultraship.h>

struct Player;

#ifdef __cplusplus
extern "C" {
#endif
void Mouse_UpdateAll();
void Mouse_RecenterCursor();
void Mouse_HandleThirdPerson(f32* newCamX, f32* newCamY);
void Mouse_HandleFirstPerson(struct Player* player);
void Mouse_HandleShield(f32* sp50, f32* sp54);
bool Mouse_HandleQuickspin(bool* should, s8* iter2, s8* sp3C);
void Mouse_UpdateQuickspinCount();
#ifdef __cplusplus
}; //extern "C"
#endif

//MOUSE_H
#endif
