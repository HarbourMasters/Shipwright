/*
 * File: z_shot_sun.c
 * Overlay: ovl_Shot_Sun
 * Description: Leftover sun hitbox from OOT and fairy spawner
 */

#include "z_shot_sun.h"
#include "overlays/actors/ovl_Item_Etcetera/z_item_etcetera.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void ShotSun_Init(Actor* thisx, PlayState* play);
void ShotSun_Destroy(Actor* thisx, PlayState* play);
void ShotSun_Update(Actor* thisx, PlayState* play);

void ShotSun_UpdateForOcarina(ShotSun* this, PlayState* play);
void ShotSun_UpdateHyliaSun(ShotSun* this, PlayState* play);

ActorProfile Shot_Sun_Profile = {
    /**/ ACTOR_SHOT_SUN,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ShotSun),
    /**/ ShotSun_Init,
    /**/ ShotSun_Destroy,
    /**/ ShotSun_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000020, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 60, 0, { 0, 0, 0 } },
};

void ShotSun_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ShotSun* this = (ShotSun*)thisx;

    if ((SHOTSUN_GET_TYPE(thisx) == SHOTSUN_FAIRY_SPAWNER_SUNS) ||
        (SHOTSUN_GET_TYPE(thisx) == SHOTSUN_FAIRY_SPAWNER_STORMS)) {
        this->fairySpawnerState = SPAWNER_OUT_OF_RANGE; // never read after here
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        this->actionFunc = ShotSun_UpdateForOcarina;
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    } else {
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        this->actionFunc = ShotSun_UpdateHyliaSun;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void ShotSun_Destroy(Actor* thisx, PlayState* play) {
    ShotSun* this = (ShotSun*)thisx;

    if ((SHOTSUN_GET_TYPE(thisx) != SHOTSUN_FAIRY_SPAWNER_SUNS) &&
        (SHOTSUN_GET_TYPE(thisx) != SHOTSUN_FAIRY_SPAWNER_STORMS)) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void ShotSun_SpawnFairy(ShotSun* this, PlayState* play2) {
    s32 type = SHOTSUN_GET_TYPE(&this->actor);
    s32 fairyType = 0;
    PlayState* play = play2;

    if (this->timer > 0) {
        this->timer--;
    } else {
        CutsceneManager_Stop(this->actor.csId);
        switch (type) {
            case SHOTSUN_FAIRY_SPAWNER_SUNS:
                fairyType = FAIRY_TYPE_7;
                break;

            case SHOTSUN_FAIRY_SPAWNER_STORMS:
                fairyType = FAIRY_TYPE_7;
                break;

            default:
                break;
        }

        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.home.pos.x, this->actor.home.pos.y,
                    this->actor.home.pos.z, 0, 0, 0, FAIRY_PARAMS(fairyType, false, 0));
        Actor_Kill(&this->actor);
    }
}

void ShotSun_TriggerFairy(ShotSun* this, PlayState* play) {
    if ((this->actor.csId == CS_ID_NONE) || CutsceneManager_IsNext(this->actor.csId)) {
        if (this->actor.csId != CS_ID_NONE) {
            CutsceneManager_Start(this->actor.csId, &this->actor);
        }

        this->actionFunc = ShotSun_SpawnFairy;
        this->timer = 50;

        //! @bug: This Demo_Kankyo spawn will crash because parameter 0x11 is an invalid value (type goes up to 4)
        Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_KANKYO, this->actor.home.pos.x, this->actor.home.pos.y,
                    this->actor.home.pos.z, 0, 0, 0, 0x11);

        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_TRE_BOX_APPEAR);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void ShotSun_UpdateForOcarina(ShotSun* this, PlayState* play) {
    s32 type = SHOTSUN_GET_TYPE(&this->actor);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        switch (play->msgCtx.lastPlayedSong) {
            case OCARINA_SONG_STORMS:
                if (type == SHOTSUN_FAIRY_SPAWNER_STORMS) {
                    this->actionFunc = ShotSun_TriggerFairy;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                }
                break;
            case OCARINA_SONG_SUNS:
                if (type == SHOTSUN_FAIRY_SPAWNER_SUNS) {
                    this->actionFunc = ShotSun_TriggerFairy;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                }
                break;
        }
    }
}

void ShotSun_UpdateHyliaSun(ShotSun* this, PlayState* play) {
    Vec3s cylinderPos;
    Player* player = GET_PLAYER(play);
    EnItem00* collectible;
    s32 pad;
    Vec3f spawnPos;

    //! FAKE:
    if (1) {}

    if (this->collider.base.acFlags & AC_HIT) {
        Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
        if (INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_ETCETERA, 700.0f, -800.0f, 7261.0f, 0, 0, 0,
                        ITEM_ETC_ARROW_FIRE);
            gSaveContext.cutsceneTrigger = 1;
        } else {
            spawnPos.x = 700.0f;
            spawnPos.y = -800.0f;
            spawnPos.z = 7261.0f;
            collectible = (EnItem00*)Item_DropCollectible(play, &spawnPos, ITEM00_MAGIC_JAR_BIG);
            if (collectible != NULL) {
                collectible->unk152 = 0x1770;
                collectible->actor.speed = 0.0f;
            }
        }

        Actor_Kill(&this->actor);
        return;
    }

    if ((this->actor.xzDistToPlayer > 120.0f) || (CURRENT_TIME < CLOCK_TIME(6, 30)) ||
        (CURRENT_TIME >= CLOCK_TIME(7, 30))) {
        return;
    }

    cylinderPos.x = player->bodyPartsPos[PLAYER_BODYPART_HEAD].x + (play->envCtx.sunPos.x * (1.0f / 6.0f));
    cylinderPos.y = (player->bodyPartsPos[PLAYER_BODYPART_HEAD].y - 30.0f) + (play->envCtx.sunPos.y * (1.0f / 6.0f));
    cylinderPos.z = player->bodyPartsPos[PLAYER_BODYPART_HEAD].z + (play->envCtx.sunPos.z * (1.0f / 6.0f));

    this->hitboxPos = cylinderPos;

    Collider_SetCylinderPosition(&this->collider, &cylinderPos);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ShotSun_Update(Actor* thisx, PlayState* play) {
    ShotSun* this = (ShotSun*)thisx;

    this->actionFunc(this, play);
}
