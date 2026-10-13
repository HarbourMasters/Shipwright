#ifndef RANDO_TRAP_H
#define RANDO_TRAP_H

#include "Rando/Rando.h"
typedef enum {
    TRAP_FREEZE,
    TRAP_BLAST,
    TRAP_SHOCK,
    TRAP_JINX,
    TRAP_WALLET,
    TRAP_ENEMY,
    TRAP_TIME,
    TRAP_FIRE,
    TRAP_KNOCKBACK,
    TRAP_MAX
} TrapTypes;

extern int RollTrapType();
extern std::string GetTrapMessage();

#endif