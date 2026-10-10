/*
 * File: z_bg_sinkai_kabe.c
 * Overlay: ovl_Bg_Sinkai_Kabe
 * Description: Manages the Deep Pythons in Pinnacle Rock
 *
 * While its name describes one of its responsibilities ("Deep Sea Wall", aka the invisible
 * wall blocking a Deep Python's den before you defeat it), it is also responsible for
 * spawning in the Deep Pythons when the area first loads, as well as signalling to the
 * Deep Python that it should extend its body. Furthermore, if the player defeats all
 * eight Deep Pythons and spawns the second Seahorse, this actor is responsible for spawning
 * it again if the player leaves and re-enters Pinnacle Rock.
 */

#include "z_bg_sinkai_kabe.h"
#include "overlays/actors/ovl_En_Ot/z_en_ot.h"
#include "objects/object_sinkai_kabe/object_sinkai_kabe.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgSinkaiKabe_Init(Actor* thisx, PlayState* play);
void BgSinkaiKabe_Destroy(Actor* thisx, PlayState* play);
void BgSinkaiKabe_Update(Actor* thisx, PlayState* play);
void BgSinkaiKabe_Reset(void);

void BgSinkaiKabe_WaitForPlayer(BgSinkaiKabe* this, PlayState* play);

ActorProfile Bg_Sinkai_Kabe_Profile = {
    /**/ ACTOR_BG_SINKAI_KABE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_SINKAI_KABE,
    /**/ sizeof(BgSinkaiKabe),
    /**/ BgSinkaiKabe_Init,
    /**/ BgSinkaiKabe_Destroy,
    /**/ BgSinkaiKabe_Update,
    /**/ NULL,
    /**/ BgSinkaiKabe_Reset,
};

static s32 sCurrentPythonIndex = 0;

void BgSinkaiKabe_Init(Actor* thisx, PlayState* play) {
    BgSinkaiKabe* this = (BgSinkaiKabe*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    Vec3f pos;
    s32 pad2;
    s32 shouldSpawnSeahorse;
    s32 csId;
    s32 i;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gPinnacleRockPythonDenCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Math_Vec3f_Copy(&pos, &this->dyna.actor.world.pos);
    pos.x += Math_SinS(this->dyna.actor.world.rot.y + 0x8000) * 3000.0f;
    pos.z += Math_CosS(this->dyna.actor.world.rot.y + 0x8000) * 3000.0f;
    csId = this->dyna.actor.csId;
    i = 0;

    // clang-format off
    while (csId != CS_ID_NONE) { this->csIdList[i] = csId; csId = CutsceneManager_GetAdditionalCsId(csId); i++; }
    // clang-format on

    this->dyna.actor.scale.x = 0.1f;
    this->dyna.actor.scale.y = 0.1f;
    this->dyna.actor.scale.z = 0.1f;
    this->pythonIndex = sCurrentPythonIndex;
    sCurrentPythonIndex++;

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_13_01)) {
        this->deepPython = Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_DRAGON, pos.x, pos.y,
                                              pos.z, 0, this->dyna.actor.world.rot.y, 1, this->dyna.actor.params);

        if (this->deepPython != NULL) {
            EnDragon* dragon = (EnDragon*)this->deepPython;

            dragon->grabCsId = this->csIdList[0];
            dragon->deathCsId = this->csIdList[1];
            dragon->actor.csId = this->dyna.actor.csId;
            Math_Vec3f_Copy(&dragon->burrowEntrancePos, &this->dyna.actor.world.pos);
            dragon->pythonIndex = this->pythonIndex;
        }
    } else {
        shouldSpawnSeahorse = false;
        if (((this->dyna.actor.params == 0) && CHECK_WEEKEVENTREG(WEEKEVENTREG_83_10)) ||
            ((this->dyna.actor.params == 1) && CHECK_WEEKEVENTREG(WEEKEVENTREG_83_20)) ||
            ((this->dyna.actor.params == 2) && CHECK_WEEKEVENTREG(WEEKEVENTREG_83_40)) ||
            ((this->dyna.actor.params == 3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_83_80)) ||
            ((this->dyna.actor.params == 4) && CHECK_WEEKEVENTREG(WEEKEVENTREG_84_01)) ||
            ((this->dyna.actor.params == 5) && CHECK_WEEKEVENTREG(WEEKEVENTREG_84_02)) ||
            ((this->dyna.actor.params == 6) && CHECK_WEEKEVENTREG(WEEKEVENTREG_84_04)) ||
            ((this->dyna.actor.params == 7) && CHECK_WEEKEVENTREG(WEEKEVENTREG_84_08))) {
            shouldSpawnSeahorse = true;
        }

        Math_Vec3f_Copy(&pos, &this->dyna.actor.home.pos);
        pos.x += (Math_SinS(this->dyna.actor.world.rot.y + 0x8000) * 500.0f);
        pos.y += -100.0f;
        pos.z += (Math_CosS(this->dyna.actor.world.rot.y + 0x8000) * 500.0f);
        if (shouldSpawnSeahorse) {
            Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_OT, pos.x, pos.y, pos.z, 0,
                                          this->dyna.actor.shape.rot.y, 0, SEAHORSE_PARAMS(SEAHORSE_TYPE_1, 0, 0),
                                          this->dyna.actor.csId, this->dyna.actor.halfDaysBits, NULL);
        }

        Actor_Kill(&this->dyna.actor);
    }

    this->actionFunc = BgSinkaiKabe_WaitForPlayer;
}

void BgSinkaiKabe_Destroy(Actor* thisx, PlayState* play) {
    BgSinkaiKabe* this = (BgSinkaiKabe*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

/**
 * Extends this actor's Deep Python if the player gets close enough.
 */
void BgSinkaiKabe_WaitForPlayer(BgSinkaiKabe* this, PlayState* play) {
    if (this->deepPython != NULL) {
        if (this->deepPython->update != NULL) {
            EnDragon* dragon = (EnDragon*)this->deepPython;

            if (dragon->action == DEEP_PYTHON_ACTION_IDLE) {
                Player* player = GET_PLAYER(play);

                if ((this->dyna.actor.xzDistToPlayer < 500.0f) ||
                    ((ABS_ALT(BINANG_SUB(this->dyna.actor.yawTowardsPlayer, this->dyna.actor.world.rot.y)) < 0x2000) &&
                     (this->dyna.actor.xzDistToPlayer < 700.0f))) {
                    if (fabsf(this->dyna.actor.world.pos.y - player->actor.world.pos.y) < 400.0f) {
                        dragon->action = DEEP_PYTHON_ACTION_EXTEND;
                    }
                }
            }
        } else {
            this->deepPython = NULL;
        }
    }

    if (this->deepPython == NULL) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgSinkaiKabe_Update(Actor* thisx, PlayState* play) {
    BgSinkaiKabe* this = (BgSinkaiKabe*)thisx;

    this->actionFunc(this, play);
}

void BgSinkaiKabe_Reset(void) {
    sCurrentPythonIndex = 0;
}
