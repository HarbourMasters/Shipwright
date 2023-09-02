/*
 * File: z_en_river_sound.c
 * Overlay: ovl_En_River_Sound
 * Description: Ambient Sound Effects
 */

#include "z_en_river_sound.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void EnRiverSound_Init(Actor* thisx, PlayState* play);
void EnRiverSound_Destroy(Actor* thisx, PlayState* play);
void EnRiverSound_Update(Actor* thisx, PlayState* play);
void EnRiverSound_Draw(Actor* thisx, PlayState* play);

const ActorInit En_River_Sound_InitVars = {
    ACTOR_EN_RIVER_SOUND,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnRiverSound),
    (ActorFunc)EnRiverSound_Init,
    (ActorFunc)EnRiverSound_Destroy,
    (ActorFunc)EnRiverSound_Update,
    (ActorFunc)EnRiverSound_Draw,
    NULL,
};

void EnRiverSound_Init(Actor* thisx, PlayState* play) {
    EnRiverSound* this = (EnRiverSound*)thisx;

    this->playSound = 0;
    this->pathIndex = (this->actor.params >> 8) & 0xFF;
    this->actor.params = this->actor.params & 0xFF;

    if (this->actor.params >= RS_MAX) { // used for ganon and ganon_boss scenes
        func_800F4870(this->actor.params - RS_MAX);
        Actor_Kill(&this->actor);
    } else if (this->actor.params == RS_UNK_F7) {
        Audio_PlayNatureAmbienceSequence(NATURE_ID_KOKIRI_REGION);
        Actor_Kill(&this->actor);
    } else if (this->actor.params == RS_SARIAS_SONG) {
        // Always have leading music in rando
        if ((!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) || CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) && !gSaveContext.n64ddFlag) {
            Actor_Kill(&this->actor);
        }
    }
}

void EnRiverSound_Destroy(Actor* thisx, PlayState* play) {
    EnRiverSound* this = (EnRiverSound*)thisx;

    if (this->actor.params == RS_SARIAS_SONG) {
        Audio_ClearSariaBgmAtPos(&this->actor.projectedPos);
    } else if (this->actor.params == RS_UNK_13) {
        Audio_ClearSariaBgm2();
    }
}

s32 func_80AE6A54(Vec3f* arg0, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3) {
    Vec3f vec[3];
    f32 temp;

    vec[0].x = arg0->x - arg2->x;
    vec[0].y = arg0->y - arg2->y;
    vec[0].z = arg0->z - arg2->z;

    vec[1].x = arg1->x - arg2->x;
    vec[1].y = arg1->y - arg2->y;
    vec[1].z = arg1->z - arg2->z;

    vec[2].x = vec[1].x - vec[0].x;
    vec[2].y = vec[1].y - vec[0].y;
    vec[2].z = vec[1].z - vec[0].z;

    temp = DOTXYZ(vec[2], vec[0]);

    if ((DOTXYZ(vec[2], vec[1]) * temp) < 0.0f) {
        temp = -temp / (SQ(vec[2].x) + SQ(vec[2].y) + SQ(vec[2].z));

        arg3->x = (vec[2].x * temp) + arg0->x;
        arg3->y = (vec[2].y * temp) + arg0->y;
        arg3->z = (vec[2].z * temp) + arg0->z;

        return true;
    }

    return false;
}

/**
 * Writes the position along the river path to `soundPos` based on the `hearPos`, which is usually the position of the
 * player.
 * Returns true if the distance between the `hearPos` and `soundPos` is less than 10000, false if not.
 */
s32 EnRiverSound_GetSoundPos(Vec3s* points, s32 numPoints, Vec3f* hearPos, Vec3f* soundPos) {
    s32 i;
    s32 pointIdx;
    s32 sp78[2] = { 0, 0 };
    Vec3f pointLoc;
    Vec3f sp60;
    Vec3f sp54;
    Vec3f vec;
    f32 pointDist = 10000.0f;
    Vec3s* point;

    for (i = 0; i < numPoints; i++) {
        f32 dist;

        vec.x = points[i].x;
        vec.y = points[i].y;
        vec.z = points[i].z;
        dist = Math_Vec3f_DistXYZ(hearPos, &vec);

        if (dist < pointDist) {
            pointDist = dist;
            pointIdx = i;
        }
    }

    if (pointDist >= 10000.0f) {
        return false;
    }

    point = &points[pointIdx];
    pointLoc.x = point->x;
    pointLoc.y = point->y;
    pointLoc.z = point->z;

    if (pointIdx != 0) {
        vec.x = point[-1].x;
        vec.y = point[-1].y;
        vec.z = point[-1].z;
        sp78[0] = func_80AE6A54(&vec, &pointLoc, hearPos, &sp54);
    }

    if (pointIdx + 1 != numPoints) {
        vec.x = point[1].x;
        vec.y = point[1].y;
        vec.z = point[1].z;
        sp78[1] = func_80AE6A54(&pointLoc, &vec, hearPos, &sp60);
    }

    if (sp78[0] && sp78[1]) {
        if (!func_80AE6A54(&sp54, &sp60, hearPos, soundPos)) {
            soundPos->x = (sp54.x + sp60.x) * 0.5f;
            soundPos->y = (sp54.y + sp60.y) * 0.5f;
            soundPos->z = (sp54.z + sp60.z) * 0.5f;
        }
    } else if (sp78[0]) {
        soundPos->x = sp54.x;
        soundPos->y = sp54.y;
        soundPos->z = sp54.z;
    } else if (sp78[1]) {
        soundPos->x = sp60.x;
        soundPos->y = sp60.y;
        soundPos->z = sp60.z;
    } else {
        soundPos->x = pointLoc.x;
        soundPos->y = pointLoc.y;
        soundPos->z = pointLoc.z;
    }

    return true;
}

void EnRiverSound_Update(Actor* thisx, PlayState* play) {
    Path* path;
    Vec3f* pos;
    Player* player = GET_PLAYER(play);
    EnRiverSound* this = (EnRiverSound*)thisx;
    s32 sp34;

    if ((thisx->params == RS_UNK_0) || (thisx->params == RS_UNK_4) || (thisx->params == RS_UNK_5)) {
        path = &play->setupPathList[this->pathIndex];
        pos = &thisx->world.pos;

        if (EnRiverSound_GetSoundPos(SEGMENTED_TO_VIRTUAL(path->points), path->count, &player->actor.world.pos, pos)) {
            if (BgCheck_EntityRaycastFloor4(&play->colCtx, &thisx->floorPoly, &sp34, thisx, pos) !=
                BGCHECK_Y_MIN) {
                // Get the sound volume pitch based on the speed of the river current under the actor
                this->soundPitchIndex = SurfaceType_GetConveyorSpeed(&play->colCtx, thisx->floorPoly, sp34);
            } else {
                this->soundPitchIndex = 0;
            }

            if (this->soundPitchIndex == 0) {
                if (thisx->params == RS_UNK_4) {
                    this->soundPitchIndex = 0;
                } else if (thisx->params == RS_UNK_0) {
                    this->soundPitchIndex = 1;
                } else {
                    this->soundPitchIndex = 2;
                }
            } else {
                this->soundPitchIndex--;
                this->soundPitchIndex = CLAMP_MAX(this->soundPitchIndex, 2);
            }
        }
    } else if ((thisx->params == RS_UNK_13) || (thisx->params == RS_UNK_19)) {
        func_8002DBD0(&player->actor, &thisx->home.pos, &thisx->world.pos);
    } else if (play->sceneNum == SCENE_DODONGOS_CAVERN_BOSS && Flags_GetClear(play, thisx->room)) {
        Actor_Kill(thisx);
    }
}

void EnRiverSound_Draw(Actor* thisx, PlayState* play) {
    static s16 soundEffects[] = {
        0,
        NA_SE_EV_WATER_WALL - SFX_FLAG,
        NA_SE_EV_MAGMA_LEVEL - SFX_FLAG,
        NA_SE_EV_WATER_WALL_BIG - SFX_FLAG,
        0,
        0,
        NA_SE_EV_MAGMA_LEVEL_M - SFX_FLAG,
        NA_SE_EV_MAGMA_LEVEL_L - SFX_FLAG,
        NA_SE_EV_WATERDROP - SFX_FLAG,
        NA_SE_EV_FOUNTAIN - SFX_FLAG,
        NA_SE_EV_CROWD - SFX_FLAG,
        0,
        NA_SE_EV_SARIA_MELODY - SFX_FLAG,
        0,
        NA_SE_EV_SAND_STORM - SFX_FLAG,
        NA_SE_EV_WATER_BUBBLE - SFX_FLAG,
        NA_SE_EV_KENJA_ENVIROMENT_0 - SFX_FLAG,
        NA_SE_EV_KENJA_ENVIROMENT_1 - SFX_FLAG,
        NA_SE_EV_EARTHQUAKE - SFX_FLAG,
        0,
        NA_SE_EV_TORCH - SFX_FLAG,
        NA_SE_EV_COW_CRY_LV - SFX_FLAG,
    };
    static f32 soundPitch[] = { 0.7f, 1.0f, 1.4f };
    EnRiverSound* this = (EnRiverSound*)thisx;

    if (!(this->playSound)) {
        this->playSound = true;
    } else if ((this->actor.params == RS_UNK_0) || (this->actor.params == RS_UNK_4) ||
               (this->actor.params == RS_UNK_5)) {
        Audio_PlaySoundRiver(&this->actor.projectedPos, soundPitch[this->soundPitchIndex]);
    } else if (this->actor.params == RS_UNK_11) {
        func_800F4A54(90);
    } else if (this->actor.params == RS_SARIAS_SONG) {
        func_800F4E30(&this->actor.projectedPos, this->actor.xzDistToPlayer);
    } else if (this->actor.params == RS_UNK_13) {
        Audio_PlaySariaBgm(&this->actor.home.pos, NA_BGM_SARIA_THEME, 1000);
    } else if (this->actor.params == RS_UNK_19) {
        Audio_PlaySariaBgm(&this->actor.home.pos, NA_BGM_GREAT_FAIRY, 800);
    } else if ((this->actor.params == RS_SANDSTORM) || (this->actor.params == RS_CHAMBER_OF_SAGES_1) ||
               (this->actor.params == RS_CHAMBER_OF_SAGES_2) || (this->actor.params == RS_RUMBLING)) {
        func_800788CC(soundEffects[this->actor.params]);
    } else {
        Audio_PlayActorSound2(&this->actor, soundEffects[this->actor.params]);
    }
}
