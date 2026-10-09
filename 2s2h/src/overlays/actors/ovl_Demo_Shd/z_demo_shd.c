/*
 * File: z_demo_shd.c
 * Overlay: ovl_Demo_Shd
 * Description:
 */

#include "z_demo_shd.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoShd_Init(Actor* thisx, PlayState* play);
void DemoShd_Destroy(Actor* thisx, PlayState* play);
void DemoShd_Update(Actor* thisx, PlayState* play);
void DemoShd_Draw(Actor* thisx, PlayState* play);

ActorProfile Demo_Shd_Profile = {
    /**/ ACTOR_DEMO_SHD,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FWALL,
    /**/ sizeof(DemoShd),
    /**/ DemoShd_Init,
    /**/ DemoShd_Destroy,
    /**/ DemoShd_Update,
    /**/ DemoShd_Draw,
};

void DemoShd_Init(Actor* thisx, PlayState* play) {
}
void DemoShd_Destroy(Actor* thisx, PlayState* play) {
}
void DemoShd_Update(Actor* thisx, PlayState* play) {
}
void DemoShd_Draw(Actor* thisx, PlayState* play) {
}
