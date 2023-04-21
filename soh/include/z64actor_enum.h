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

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

#endif