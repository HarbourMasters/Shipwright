extern "C" {
#include "z64actor.h"
}

#define CUSTOM_ITEM_FLAGS (actor->home.rot.x)
#define CUSTOM_ITEM_PARAM (actor->home.rot.z)

namespace CustomItem {

enum CustomItemFlags : int16_t {
    KILL_ON_TOUCH = 1 << 0,      // 0000 0000 0000 0001
    GIVE_OVERHEAD = 1 << 1,      // 0000 0000 0000 0010
    GIVE_ITEM_CUTSCENE = 1 << 2, // 0000 0000 0000 0100
    HIDE_TILL_OVERHEAD = 1 << 3, // 0000 0000 0000 1000
    KEEP_ON_PLAYER = 1 << 4,     // 0000 0000 0001 0000
    STOP_BOBBING = 1 << 5,       // 0000 0000 0010 0000
    STOP_SPINNING = 1 << 6,      // 0000 0000 0100 0000
    CALLED_ACTION = 1 << 7,      // 0000 0000 1000 0000
    TOSS_ON_SPAWN = 1 << 8,      // 0000 0001 0000 0000
    ABLE_TO_ZORA_RANG = 1 << 9,  // 0000 0010 0000 0000
};
void RegisterHooks();
EnItem00* Spawn(f32 posX, f32 posY, f32 posZ, s16 rot, s16 flags, s16 params, ActorFunc actionFunc = NULL,
                ActorFunc drawFunc = NULL);
}; // namespace CustomItem
