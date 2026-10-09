/*
 * File: z_obj_tokei_tobira.c
 * Overlay: ovl_Obj_Tokei_Tobira
 * Description: Clock Tower - Swinging Doors
 */

#include "z_obj_tokei_tobira.h"
#include "objects/object_tokei_tobira/object_tokei_tobira.h"

#define FLAGS 0x00000000

void ObjTokeiTobira_Init(Actor* thisx, PlayState* play);
void ObjTokeiTobira_Destroy(Actor* thisx, PlayState* play);
void ObjTokeiTobira_Update(Actor* thisx, PlayState* play);
void ObjTokeiTobira_Draw(Actor* thisx, PlayState* play);
void ObjTokeiTobira_StartCutscene(ObjTokeiTobira* this);

ActorProfile Obj_Tokei_Tobira_Profile = {
    /**/ ACTOR_OBJ_TOKEI_TOBIRA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TOKEI_TOBIRA,
    /**/ sizeof(ObjTokeiTobira),
    /**/ ObjTokeiTobira_Init,
    /**/ ObjTokeiTobira_Destroy,
    /**/ ObjTokeiTobira_Update,
    /**/ ObjTokeiTobira_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

Vec3f D_80ABD760 = { 0.0f, 0.0f, 80.0f };
s16 D_80ABD76C[] = {
    -0x4000, // OBJTOKEITOBIRA_TYPE_0
    0x4000,  // OBJTOKEITOBIRA_TYPE_1
};
CollisionHeader* D_80ABD770[] = {
    &object_tokei_tobira_Colheader_0012B0, // OBJTOKEITOBIRA_TYPE_0
    &object_tokei_tobira_Colheader_001590, // OBJTOKEITOBIRA_TYPE_1
};
f32 D_80ABD778[] = {
    1.0f,  // OBJTOKEITOBIRA_TYPE_0
    -1.0f, // OBJTOKEITOBIRA_TYPE_1
};
Gfx* D_80ABD780[] = {
    object_tokei_tobira_DL_001108, // OBJTOKEITOBIRA_TYPE_0
    object_tokei_tobira_DL_0013E8, // OBJTOKEITOBIRA_TYPE_1
};

void ObjTokeiTobira_Init(Actor* thisx, PlayState* play) {
    ObjTokeiTobira* this = (ObjTokeiTobira*)thisx;
    s32 pad;
    s32 type = OBJTOKEITOBIRA_GET_TYPE(&this->dyna.actor);
    Vec3f posOffset;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    DynaPolyActor_LoadMesh(play, &this->dyna, D_80ABD770[type]);

    if (type == OBJTOKEITOBIRA_TYPE_0) {
        Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_TOKEI_TOBIRA,
                           this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                           this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z,
                           OBJTOKEITOBIRA_PARAMS(OBJTOKEITOBIRA_TYPE_1));
    }

    Matrix_RotateYS(D_80ABD76C[type] + this->dyna.actor.shape.rot.y, MTXMODE_NEW);
    Matrix_MultVec3f(&D_80ABD760, &posOffset);
    this->dyna.actor.world.pos.x += posOffset.x;
    this->dyna.actor.world.pos.y += posOffset.y;
    this->dyna.actor.world.pos.z += posOffset.z;

    if ((type == OBJTOKEITOBIRA_TYPE_0) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_04) &&
        (play->sceneId == SCENE_CLOCKTOWER) && (gSaveContext.sceneLayer == 0) && (this->dyna.actor.csId > CS_ID_NONE)) {
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actionFunc = ObjTokeiTobira_StartCutscene;
    }
}

void ObjTokeiTobira_Destroy(Actor* thisx, PlayState* play) {
    ObjTokeiTobira* this = (ObjTokeiTobira*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjTokeiTobira_StartCutscene(ObjTokeiTobira* this) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        SET_WEEKEVENTREG(WEEKEVENTREG_59_04);
        this->actionFunc = NULL;
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void ObjTokeiTobira_Update(Actor* thisx, PlayState* play) {
    s32 pad1;
    ObjTokeiTobira* this = (ObjTokeiTobira*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad2;
    s32 type = OBJTOKEITOBIRA_GET_TYPE(&this->dyna.actor);
    f32 sp48 = D_80ABD778[type];
    s32 pad3;

    if (player->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) {
        if (DynaPoly_GetActor(&play->colCtx, player->actor.wallBgId) == &this->dyna) {
            f32 sp40;
            Vec3f sp34;

            Actor_WorldToActorCoords(&this->dyna.actor, &sp34, &player->actor.world.pos);
            sp40 = sp34.x * sp48;

            if (sp40 > 20.0f) {
                if (sp34.z > 0.0f) {
                    this->unk160 += (sp40 - 20.0f) * sp48 * (16.0f / 3.0f);
                    this->unk168 = 1;
                    if (this->unk16C <= 0) {
                        Actor_PlaySfx(&this->dyna.actor, NA_SE_OC_WOOD_GATE_OPEN);
                        this->unk16C = 80;
                    }
                }

                if (sp40 > 48.0f) {
                    ObjTokeiTobira* tobira;

                    if (type == OBJTOKEITOBIRA_TYPE_0) {
                        tobira = (ObjTokeiTobira*)this->dyna.actor.child;
                    } else {
                        tobira = (ObjTokeiTobira*)this->dyna.actor.parent;
                    }

                    if (tobira != NULL) {
                        tobira->unk168 = 1;
                    }
                }
            }
        }
    } else if (this->unk168 == 1) {
        Vec3f sp24;

        Actor_WorldToActorCoords(&this->dyna.actor, &sp24, &player->actor.world.pos);

        if ((sp24.z > 0.0f) && (sp24.z < 30.0f)) {
            this->unk168 = 1;
            this->unk160 += sp48 * 290.0f;
        }
    }

    if (this->unk16C > 0) {
        this->unk16C--;
    }

    this->unk168 = 0;
    this->unk160 *= 0.87f;
    this->unk164 += this->unk160;
    if ((this->unk164 * sp48) > 14336.0f) {
        this->unk160 *= 0.1f;
        this->unk164 = 14336.0f * sp48;
    } else if ((this->unk164 * sp48) < -1000.0f) {
        this->unk160 *= 0.1f;
        this->unk164 = -1000.0f * sp48;
    }

    this->dyna.actor.shape.rot.y = (s32)this->unk164 + this->dyna.actor.home.rot.y;

    if (this->actionFunc != NULL) {
        this->actionFunc(this);
    }
}

void ObjTokeiTobira_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, D_80ABD780[OBJTOKEITOBIRA_GET_TYPE(thisx)]);
}
