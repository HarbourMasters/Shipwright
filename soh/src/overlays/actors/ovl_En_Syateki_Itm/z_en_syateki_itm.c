#include "z_en_syateki_itm.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    SYATEKI_ROUND_GREEN_APPEAR,
    SYATEKI_ROUND_BLUE_SEQUENTIAL,
    SYATEKI_ROUND_GREEN_THROW,
    SYATEKI_ROUND_BLUE_SIMUL,
    SYATEKI_ROUND_RED_LEFT,
    SYATEKI_ROUND_RED_RIGHT,
    SYATEKI_ROUND_MAX
} EnSyatekItemRound;

void EnSyatekiItm_Init(Actor* thisx, PlayState* play);
void EnSyatekiItm_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiItm_Update(Actor* thisx, PlayState* play);

void EnSyatekiItm_Idle(EnSyatekiItm* this, PlayState* play);
void EnSyatekiItm_StartRound(EnSyatekiItm* this, PlayState* play);
void EnSyatekiItm_SpawnTargets(EnSyatekiItm* this, PlayState* play);
void EnSyatekiItm_CheckTargets(EnSyatekiItm* this, PlayState* play);
void EnSyatekiItm_CleanupGame(EnSyatekiItm* this, PlayState* play);
void EnSyatekiItm_EndGame(EnSyatekiItm* this, PlayState* play);

const ActorInit En_Syateki_Itm_InitVars = {
    ACTOR_EN_SYATEKI_ITM,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnSyatekiItm),
    (ActorFunc)EnSyatekiItm_Init,
    (ActorFunc)EnSyatekiItm_Destroy,
    (ActorFunc)EnSyatekiItm_Update,
    NULL,
    NULL,
};

static Vec3f sGreenAppearHome = { 0.0f, -10.0f, -270.0f };
static Vec3f sBlueSeqHome1 = { -220.0f, 66.0f, -320.0f };
static Vec3f sBlueSeqHome2 = { 260.0f, 66.0f, -320.0f };
static Vec3f sGreenThrowHome = { 0.0f, -10.0f, -270.0f };
static Vec3f sBlueSimulHome1 = { -220.0f, 66.0f, -320.0f };
static Vec3f sBlueSimulHome2 = { 260.0f, 66.0f, -320.0f };
static Vec3f sRedLeftHome1 = { 260.0f, 100.0f, -320.0f };
static Vec3f sRedLeftHome2 = { 360.0f, 100.0f, -320.0f };
static Vec3f sRedRightHome1 = { -230.0f, 94.0f, -360.0f };
static Vec3f sRedRightHome2 = { -400.0f, 94.0f, -360.0f };
static Vec3f sGreenAppearFinal = { 0.0f, 53.0f, -270.0f };
static Vec3f sBlueSeqFinal1 = { -60.0f, 63.0f, -320.0f };
static Vec3f sBlueSeqFinal2 = { 60.0f, 63.0f, -320.0f };
static Vec3f sGreenThrowFinal = { 0.0f, 0.0f, 0.0f };
static Vec3f sBlueSimulFinal1 = { -60.0f, 63.0f, -320.0f };
static Vec3f sBlueSimulFinal2 = { 60.0f, 63.0f, -320.0f };
static Vec3f sRedLeftFinal1 = { -230.0f, 0.0f, 0.0f };
static Vec3f sRedLeftFinal2 = { -230.0f, 0.0f, 0.0f };
static Vec3f sRedRightFinal1 = { 260.0f, 0.0f, 0.0f };
static Vec3f sRedRightFinal2 = { 260.0f, 0.0f, 0.0f };
static s16 sTargetColors[] = { 0, 1, 0, 1, 2, 2 };
static s16 sRupeeTypes[] = { 0, 1, 1, 0, 1, 1, 4, 4, 4, 4 };
static Vec3f sRupeePos[] = {
    { -40.0f, 0.0f, -90.0f }, { -20.0f, 0.0f, -90.0f }, { 0.0f, 0.0f, -90.0f },   { 20.0f, 0.0f, -90.0f },
    { 40.0f, 0.0f, -90.0f },  { -40.0f, 0.0f, -60.0f }, { -20.0f, 0.0f, -60.0f }, { 0.0f, 0.0f, -60.0f },
    { 20.0f, 0.0f, -60.0f },  { 40.0f, 0.0f, -60.0f },
};

void EnSyatekiItm_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnSyatekiItm* this = (EnSyatekiItm*)thisx;
    s32 i;

    this->man = (EnSyatekiMan*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_SYATEKI_MAN,
                                                  140.0f, 0.0f, 255.0f, 0, -0x4000, 0, 0);
    if (this->man == NULL) {
        // "Spawn error"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ エラー原 ☆☆☆☆ \n" VT_RST);
        Actor_Kill(&this->actor);
        return;
    }
    for (i = 0; i < 10; i++) {
        this->markers[i] =
            (EnExRuppy*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_RUPPY,
                                           sRupeePos[i].x, sRupeePos[i].y, sRupeePos[i].z, 0, 0, 0, 4);
        if (this->markers[i] == NULL) {
            // "Second spawn error"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ エラー原セカンド ☆☆☆☆ \n" VT_RST);
            Actor_Kill(&this->actor);
            return;
        }
        this->markers[i]->colorIdx = sRupeeTypes[i];
    }
    this->actionFunc = EnSyatekiItm_Idle;
}

void EnSyatekiItm_Destroy(Actor* thisx, PlayState* play) {
}

void EnSyatekiItm_Idle(EnSyatekiItm* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);

    if (this->signal == ENSYATEKI_START) {
        player->actor.world.pos.x = -12.0f;
        player->actor.world.pos.y = 20.0f;
        player->actor.world.pos.z = 182.0f;
        player->currentYaw = player->actor.world.rot.y = player->actor.shape.rot.y = 0x7F03;
        player->actor.world.rot.x = player->actor.shape.rot.x = player->actor.world.rot.z = player->actor.shape.rot.z =
            0;
        s32 ammunition = 15;
        if(CVar_GetS32("gCustomizeShootingGallery", 0)) {
            ammunition = CVar_GetS32(LINK_IS_ADULT ? "gAdultShootingGalleryAmmunition" : "gChildShootingGalleryAmmunition", 15);
        }
        func_8008EF44(play, ammunition);
        this->roundNum = this->hitCount = 0;
        for (i = 0; i < 6; i++) {
            this->roundFlags[i] = false;
        }
        for (i = 0; i < 10; i++) {
            this->markers[i]->galleryFlag = false;
        }
        this->actionFunc = EnSyatekiItm_StartRound;
    }
}

void EnSyatekiItm_StartRound(EnSyatekiItm* this, PlayState* play) {
    s32 i;
    s32 j;
    Player* player = GET_PLAYER(play);

    if (this->unkTimer == 0) {
        if (LINK_IS_ADULT && !(CVar_GetS32("gCustomizeShootingGallery", 0) && CVar_GetS32("gConstantAdultGallery", 0))) {
            for (i = 0, j = 0; i < SYATEKI_ROUND_MAX; i++) {
                if (this->roundFlags[i]) {
                    j++;
                }
            }
            if (j >= SYATEKI_ROUND_MAX) {
                player->actor.freezeTimer = 10;
                this->signal = ENSYATEKI_END;
                this->actionFunc = EnSyatekiItm_CleanupGame;
                return;
            }
            i = Rand_ZeroFloat(5.99f);
            while (this->roundFlags[i]) {
                i = Rand_ZeroFloat(5.99f);
            }
            this->roundNum = i + 1;
            this->roundFlags[i] = true;
        } else {
            this->roundNum++;
            if (this->roundNum > SYATEKI_ROUND_MAX) {
                player->actor.freezeTimer = 10;
                this->signal = ENSYATEKI_END;
                this->actionFunc = EnSyatekiItm_CleanupGame;
                return;
            }
        }

        this->timer = (this->roundNum == 1) ? 50 : 30;

        func_80078884(NA_SE_SY_FOUND);
        this->actionFunc = EnSyatekiItm_SpawnTargets;
    }
}

void EnSyatekiItm_SpawnTargets(EnSyatekiItm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 roundIdx;

    if (play->shootingGalleryStatus == -1) {
        player->actor.freezeTimer = 10;
        this->signal = ENSYATEKI_END;
        this->actionFunc = EnSyatekiItm_CleanupGame;
        return;
    }
    if (this->timer == 0) {
        for (i = 0; i < 2; i++) {
            Math_Vec3f_Copy(&this->targetHome[i], &zeroVec);
            Math_Vec3f_Copy(&this->targetFinal[i], &zeroVec);
            this->targets[i] = NULL;
        }
        this->numTargets = 2;
        this->curMarkers[0] = this->curMarkers[1] = NULL;
        roundIdx = this->roundNum - 1;

        switch (roundIdx) {
            case SYATEKI_ROUND_GREEN_APPEAR:
                Math_Vec3f_Copy(&this->targetHome[0], &sGreenAppearHome);
                Math_Vec3f_Copy(&this->targetFinal[0], &sGreenAppearFinal);
                this->curMarkers[0] = this->markers[0];
                this->numTargets = 1;
                break;
            case SYATEKI_ROUND_BLUE_SEQUENTIAL:
                Math_Vec3f_Copy(&this->targetHome[0], &sBlueSeqHome1);
                Math_Vec3f_Copy(&this->targetHome[1], &sBlueSeqHome2);
                Math_Vec3f_Copy(&this->targetFinal[0], &sBlueSeqFinal1);
                Math_Vec3f_Copy(&this->targetFinal[1], &sBlueSeqFinal2);
                this->curMarkers[0] = this->markers[1];
                this->curMarkers[1] = this->markers[2];
                break;
            case SYATEKI_ROUND_GREEN_THROW:
                Math_Vec3f_Copy(&this->targetHome[0], &sGreenThrowHome);
                Math_Vec3f_Copy(&this->targetFinal[0], &sGreenThrowFinal);
                this->curMarkers[0] = this->markers[3];
                this->numTargets = 1;
                break;
            case SYATEKI_ROUND_BLUE_SIMUL:
                Math_Vec3f_Copy(&this->targetHome[0], &sBlueSimulHome1);
                Math_Vec3f_Copy(&this->targetHome[1], &sBlueSimulHome2);
                Math_Vec3f_Copy(&this->targetFinal[0], &sBlueSimulFinal1);
                Math_Vec3f_Copy(&this->targetFinal[1], &sBlueSimulFinal2);
                this->curMarkers[0] = this->markers[4];
                this->curMarkers[1] = this->markers[5];
                break;
            case SYATEKI_ROUND_RED_LEFT:
                Math_Vec3f_Copy(&this->targetHome[0], &sRedLeftHome1);
                Math_Vec3f_Copy(&this->targetHome[1], &sRedLeftHome2);
                Math_Vec3f_Copy(&this->targetFinal[0], &sRedLeftFinal1);
                Math_Vec3f_Copy(&this->targetFinal[1], &sRedLeftFinal2);
                this->curMarkers[0] = this->markers[6];
                this->curMarkers[1] = this->markers[7];
                break;
            case SYATEKI_ROUND_RED_RIGHT:
                Math_Vec3f_Copy(&this->targetHome[0], &sRedRightHome1);
                Math_Vec3f_Copy(&this->targetHome[1], &sRedRightHome2);
                Math_Vec3f_Copy(&this->targetFinal[0], &sRedRightFinal1);
                Math_Vec3f_Copy(&this->targetFinal[1], &sRedRightFinal2);
                this->curMarkers[0] = this->markers[8];
                this->curMarkers[1] = this->markers[9];
                break;
        }

        for (i = 0; i < this->numTargets; i++) {
            this->targets[i] = (EnGSwitch*)Actor_SpawnAsChild(
                &play->actorCtx, &this->actor, play, ACTOR_EN_G_SWITCH, this->targetHome[i].x,
                this->targetHome[i].y, this->targetHome[i].z, 0, 0, 0, (ENGSWITCH_TARGET_RUPEE << 0xC) | 0x3F);
            if (this->targets[i] == NULL) {
                // "Rupee spawn error"
                osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ ルピーでエラー原 ☆☆☆☆ \n" VT_RST);
                Actor_Kill(&this->actor);
                return;
            }
            this->targets[i]->index = i;
            this->targets[i]->colorIdx = sTargetColors[roundIdx];
            Math_Vec3f_Copy(&this->targets[i]->targetPos, &this->targetFinal[i]);
            switch (roundIdx) {
                case SYATEKI_ROUND_BLUE_SEQUENTIAL:
                    if (i == 1) {
                        this->targets[i]->delayTimer = 60;
                    }
                    break;
                case SYATEKI_ROUND_GREEN_THROW:
                    this->targets[i]->actor.velocity.y = 15.0f;
                    this->targets[i]->actor.gravity = -1.0f;
                    this->targets[i]->moveMode = GSWITCH_THROW;
                    break;
                case SYATEKI_ROUND_RED_LEFT:
                    this->targets[i]->actor.velocity.x = -5.0f;
                    this->targets[i]->moveMode = GSWITCH_LEFT;
                    break;
                case SYATEKI_ROUND_RED_RIGHT:
                    this->targets[i]->actor.velocity.x = 7.0f;
                    this->targets[i]->moveMode = GSWITCH_RIGHT;
                    break;
            }
        }
        this->targetState[0] = this->targetState[1] = ENSYATEKIHIT_NONE;
        this->actionFunc = EnSyatekiItm_CheckTargets;
    }
}

void EnSyatekiItm_CheckTargets(EnSyatekiItm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 j;

    if (play->shootingGalleryStatus == -1) {
        player->actor.freezeTimer = 10;
        this->signal = ENSYATEKI_END;
        this->actionFunc = EnSyatekiItm_CleanupGame;
    } else {
        for (i = 0, j = 0; i < 2; i++) {
            if (this->targetState[i] != ENSYATEKIHIT_NONE) {
                if (this->targetState[i] == ENSYATEKIHIT_HIT) {
                    this->curMarkers[i]->galleryFlag = true;
                }
                j++;
            }
        }
        if (j == this->numTargets) {
            this->actionFunc = EnSyatekiItm_StartRound;
        }
    }
}

void EnSyatekiItm_CleanupGame(EnSyatekiItm* this, PlayState* play) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if ((this->targetState[i] == ENSYATEKIHIT_NONE) && (this->targets[i] != NULL)) {
            Actor_Kill(&this->targets[i]->actor);
        }
    }
    this->actionFunc = EnSyatekiItm_EndGame;
}

void EnSyatekiItm_EndGame(EnSyatekiItm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 10;
    if (this->signal == ENSYATEKI_RESULTS) {
        this->signal = ENSYATEKI_NONE;
        this->actionFunc = EnSyatekiItm_Idle;
    }
    if (this->signal == ENSYATEKI_START) {
        // "1 frame attack and defense!"
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        osSyncPrintf(VT_FGCOL(RED) "☆☆☆☆☆ １フレームの攻防！ ☆☆☆☆ \n" VT_RST);
        this->signal = ENSYATEKI_NONE;
        this->actionFunc = EnSyatekiItm_Idle;
    }
}

void EnSyatekiItm_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSyatekiItm* this = (EnSyatekiItm*)thisx;

    this->actionFunc(this, play);

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->unkTimer != 0) {
        this->unkTimer--;
    }
    if (BREG(0)) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}
