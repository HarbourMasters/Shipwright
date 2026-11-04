#pragma once

#ifndef Z64ACTOR_ENUM_H
#define Z64ACTOR_ENUM_H

#define DEFINE_ACTOR_INTERNAL(_0, enum, _2) enum,
#define DEFINE_ACTOR_UNSET(enum) enum,
#define DEFINE_ACTOR(_0, enum, _2) DEFINE_ACTOR_INTERNAL(_0, enum, _2)

enum ActorID {
#include "tables/actor_table.h"
    /* 0x0192 */ ACTOR_ID_MAX // originally "ACTOR_DLF_MAX"
};

// Actors added by Ship. They start past ACTOR_ID_MAX so it keeps working as the vanilla bound and as
// the "no actor" sentinel (see location_list.cpp). ActorDB hands out ids from ACTOR_ID_EXTRA_MAX up
// to anything registering dynamically at runtime.
enum ActorIDExtra {
    /* 0x0193 */ ACTOR_EN_PARTNER = ACTOR_ID_MAX + 1,
    ACTOR_ID_EXTRA_MAX
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

#endif