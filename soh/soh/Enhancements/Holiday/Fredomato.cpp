#include "Holiday.hpp"
#include <libultraship/libultraship.h>
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "objects/object_dog/object_dog.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Enhancements/randomizer/3drando/location_access.hpp"
#include "soh/Enhancements/randomizer/entrance.h"

#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_md/object_md.h"
#include "src/overlays/actors/ovl_Door_Ana/z_door_ana.h"
extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"

extern PlayState* gPlayState;
void DoorAna_SetupAction(DoorAna* doorAna, DoorAnaActionFunc actionFunc);
void DoorAna_GrabPlayer(DoorAna* doorAna, PlayState* play);
}

#define AUTHOR "Fredomato"
#define CVAR(v) "gHoliday." AUTHOR "." v

static CollisionPoly snowballPoly;
static f32 raycastResult;

const s16 entrances[] = {
    0x0000, 0x0209, 0x0004, 0x0242, 0x0028, 0x0221, 0x0169, 0x0215, 0x0165, 0x024A, 0x0010, 0x021D, 0x0082, 0x01E1, 0x0037, 0x0205,
    0x0098, 0x02A6, 0x0088, 0x03D4, 0x0008, 0x03A8, 0x0467, 0x023D, 0x0433, 0x0443, 0x0437, 0x0447, 0x009C, 0x033C, 0x00C9, 0x026A,
    0x00C1, 0x0266, 0x0043, 0x03CC, 0x045F, 0x0309, 0x03A0, 0x03D0, 0x007E, 0x026E, 0x0530, 0x01D1, 0x0507, 0x03BC, 0x0388, 0x02A2,
    0x0063, 0x01D5, 0x0528, 0x03C0, 0x043B, 0x0067, 0x02FD, 0x0349, 0x0550, 0x04EE, 0x039C, 0x0345, 0x05C8, 0x05DC, 0x0072, 0x034D,
    0x030D, 0x0355, 0x037C, 0x03FC, 0x0380, 0x03C4, 0x004F, 0x0378, 0x02F9, 0x042F, 0x05D0, 0x05D4, 0x052C, 0x03B8, 0x016D, 0x01CD,
    0x00B7, 0x0201, 0x003B, 0x0463, 0x0588, 0x057C, 0x0578, 0x0340, 0x04C2, 0x03E8, 0x04BE, 0x0482, 0x0315, 0x045B, 0x0371, 0x0394,
    0x0272, 0x0211, 0x0053, 0x0472, 0x0453, 0x0351, 0x0384, 0x044B, 0x03EC, 0x04FF, 0x0700, 0x0800, 0x0701, 0x0801, 0x0702, 0x0802,
    0x0703, 0x0803, 0x0704, 0x0804, 0x0705, 0x0805, 0x0706, 0x0806, 0x0707, 0x0807, 0x0708, 0x0808, 0x0709, 0x0809, 0x070A, 0x080A,
    0x070B, 0x080B, 0x070C, 0x080C, 0x070D, 0x080D, 0x070E, 0x080E, 0x070F, 0x080F, 0x0710,         0x0711, 0x0811, 0x0712, 0x0812,
    0x0713, 0x0813, 0x0714, 0x0814, 0x0715, 0x0815, 0x0716, 0x0816, 0x0717, 0x0817, 0x0718, 0x0818, 0x0719, 0x0819,         0x081A,
    0x071B, 0x081B, 0x071C, 0x081C, 0x071D, 0x081D, 0x071E, 0x081E, 0x071F, 0x081F, 0x0720, 0x0820, 0x004B, 0x035D, 0x031C, 0x0361,
    0x002D, 0x050B, 0x044F, 0x0359, 0x05E0, 0x020D, 0x011E, 0x0286, 0x04E2, 0x04D6, 0x01DD, 0x04DA, 0x00FC, 0x01A9, 0x0185, 0x04DE,
    0x0102, 0x0189, 0x0117, 0x018D, 0x0276, 0x01FD, 0x00DB, 0x017D, 0x00EA, 0x0181, 0x0157, 0x01F9, 0x0328, 0x0560, 0x0129, 0x022D,
    0x0130, 0x03AC, 0x0123, 0x0365, 0x00B1, 0x0033, 0x0138, 0x025A, 0x0171, 0x025E, 0x00E4, 0x0195, 0x013D, 0x0191, 0x014D, 0x01B9,
    0x0246, 0x01C1, 0x0147, 0x01BD, 0x0108, 0x019D, 0x0225, 0x01A1, 0x0219, 0x027E, 0x0554, 0x00BB, 0x0282, 0x0600, 0x04F6, 0x0604,
    0x01F1, 0x0568, 0x05F4, 0x040F, 0x0252, 0x040B, 0x00C5, 0x0301, 0x0407, 0x000C, 0x024E, 0x0305, 0x0175, 0x0417, 0x0423, 0x008D,
    0x02F5, 0x0413, 0x02B2, 0x0457, 0x047A, 0x010E, 0x0608, 0x0564, 0x060C, 0x0610, 0x0580 
};

static bool midoGrottoInit = false;
static SkelAnime midoSkelAnime;
static Vec3s midoJointTable[17];
static Vec3s midoMorphTable[17];

static void RandomGrotto_WaitOpen(DoorAna* doorAna, PlayState* play) {
    if (!midoGrottoInit) {
        midoGrottoInit = true;
        SkelAnime_InitFlex(play, &midoSkelAnime, (FlexSkeletonHeader*)&gMidoSkel, (AnimationHeader*)&gMidoWalkingAnim, midoJointTable, midoMorphTable, 17);
    }
    SkelAnime_Update(&midoSkelAnime);

    Actor* actor = &doorAna->actor;
    Player* player = GET_PLAYER(play);
    Math_SmoothStepToF(&actor->world.pos.x, player->actor.world.pos.x, 0.1f, 10.0f, 0.0f);
    Math_SmoothStepToF(&actor->world.pos.z, player->actor.world.pos.z, 0.1f, 10.0f, 0.0f);
    Math_SmoothStepToF(&actor->world.pos.y, player->actor.world.pos.y, 0.1f, 10.0f, 0.0f);

    Math_ApproachS(&doorAna->actor.shape.rot.y, doorAna->actor.yawTowardsPlayer, 5, 0xBB8);

    if (Math_StepToF(&actor->scale.x, 0.01f, 0.001f)) {
        if ((actor->targetMode != 0) && (play->transitionTrigger == TRANS_TRIGGER_OFF) && (player->stateFlags1 & PLAYER_STATE1_FLOOR_DISABLED) && (player->av1.actionVar1 == 0)) {
            play->nextEntranceIndex = RandomElement(entrances);
            DoorAna_SetupAction((DoorAna*)actor, DoorAna_GrabPlayer);
        } else {
            if (!Player_InCsMode(play) && !(player->stateFlags1 & (PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_IN_WATER)) &&
                actor->xzDistToPlayer <= 15.0f && -50.0f <= actor->yDistToPlayer &&
                actor->yDistToPlayer <= 15.0f) {
                player->stateFlags1 |= PLAYER_STATE1_FLOOR_DISABLED;
                actor->targetMode = 1;
            } else {
                actor->targetMode = 0;
            }
        }
    }
    Actor_SetScale(actor, actor->scale.x);
}

static void RandomGrotto_Draw(Actor* actor, PlayState* play) {
    if (!midoGrottoInit) {
        return;
    }
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGrottoDL);

    Matrix_Translate(0.0f, -2700.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gMidoEyeOpenTex);
    func_80034BA0(play, &midoSkelAnime, NULL, NULL, actor, 255);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void SpawnRandomGrotto() {
    Vec3f pos;
    pos.y = 9999.0f;
    int spawnAttempts = 0;
    while (spawnAttempts < 50) {
        if (GET_PLAYER(gPlayState) != nullptr) {
            pos.x = GET_PLAYER(gPlayState)->actor.world.pos.x;
            pos.z = GET_PLAYER(gPlayState)->actor.world.pos.z;
        } else {
            pos.x = 0;
            pos.z = 0;
        }
        // X/Z anywhere from -1000.0 to +1000.0 from player 
        pos.x += (float)(Random(0, 2000)) - 1000.0f;
        pos.z += (float)(Random(0, 2000)) - 1000.0f;

        raycastResult = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &snowballPoly, &pos);

        if (raycastResult > BGCHECK_Y_MIN) {
            Actor* grotto = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, pos.x, raycastResult, pos.z, 0, 0, 0, 0, false);
            midoGrottoInit = false;
            DoorAna_SetupAction((DoorAna*)grotto, RandomGrotto_WaitOpen);
            grotto->draw = RandomGrotto_Draw;
            break;
        }

        spawnAttempts++;
    }
}

static void ConfigurationChanged() {
    COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("KrampusHole"), 0), SpawnRandomGrotto);
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);

    if (UIWidgets::EnhancementCheckbox("The Krampus Hole", CVAR("KrampusHole"))) {
        ConfigurationChanged();
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    ConfigurationChanged();
    // #endregion
}

static Holiday holiday(DrawMenu, RegisterMod);
