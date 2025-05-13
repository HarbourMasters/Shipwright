#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "global.h"

extern "C" {
#include "functions.h"
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayState* gPlayState;
}

#define CVAR_CRAWL_SPEED_NAME CVAR_ENHANCEMENT("CrawlSpeed")
#define CVAR_CRAWL_SPEED_DEFAULT 1
#define CVAR_CRAWL_SPEED_VALUE CVarGetInteger(CVAR_CRAWL_SPEED_NAME, CVAR_CRAWL_SPEED_DEFAULT)
#define CVAR_GLITCH_AIDING_NAME CVAR_ENHANCEMENT("GlitchAidingCrawlspaces")
#define CVAR_GLITCH_AIDING_DEFAULT 0
#define CVAR_GLITCH_AIDING_VALUE CVarGetInteger(CVAR_GLITCH_AIDING_NAME, CVAR_GLITCH_AIDING_DEFAULT)

extern "C" void ExitCrawlspace(Player* player, PlayState* play) {
    LinkAnimationHeader* animExit = (LinkAnimationHeader*)gPlayerAnim_link_child_tunnel_end;
    LinkAnimationHeader* animEnter = (LinkAnimationHeader*)gPlayerAnim_link_child_tunnel_start;

    if (player->linearVelocity > 0.0f) {
        // Leaving a crawlspace forwards
        player->actor.shape.rot.y = player->actor.wallYaw + 0x8000;
        LinkAnimation_Change(play, &player->skelAnime, animExit, ((CVAR_CRAWL_SPEED_VALUE + 1.0f) / 2.0f), 0.0f,
                             Animation_GetLastFrame(animExit), ANIMMODE_ONCE, 0.0f);
        Player_StartAnimMovement(play, player, 0x9D);
        OnePointCutscene_Init(play, 9601, 999, NULL, MAIN_CAM);
    } else {
        // Leaving a crawlspace backwards
        player->actor.shape.rot.y = player->actor.wallYaw;
        LinkAnimation_Change(play, &player->skelAnime, animEnter, -1.0f * ((CVAR_CRAWL_SPEED_VALUE + 1.0f) / 2.0f),
                             Animation_GetLastFrame(animEnter), 0.0f, ANIMMODE_ONCE, 0.0f);
        Player_StartAnimMovement(play, player, 0x9D);
        OnePointCutscene_Init(play, 9602, 999, NULL, MAIN_CAM);
    }
}

extern "C" void ExitCrawlspaceCS(PlayState* play, Camera* csCam, int16_t actionParameters, int16_t initTimer,
                                 CutsceneCameraPoint* atPoints, CutsceneCameraPoint* eyePoints) {
    s16 camCrawlTemp = CVAR_CRAWL_SPEED_VALUE;
    s16 camCrawlTimer = initTimer / camCrawlTemp;

    OnePointCutscene_SetCsCamPoints(csCam, actionParameters | 0x1000, camCrawlTimer, atPoints, eyePoints);
}

extern "C" void EnterCrawlspace(Player* player, PlayState* play) {
    LinkAnimationHeader* anim = (LinkAnimationHeader*)gPlayerAnim_link_child_tunnel_start;

    LinkAnimation_Change(play, &player->skelAnime, anim, ((CVAR_CRAWL_SPEED_VALUE + 1.0f) / 2.0f), 0.0f,
                         Animation_GetLastFrame(anim), ANIMMODE_ONCE, 0.0f);
}

extern "C" void IncreaseCrawlSpeed(Player* player, PlayState* play) {
    Input* sControlInput = &play->state.input[0];
    player->linearVelocity = sControlInput->rel.stick_y * 0.03f * CVAR_CRAWL_SPEED_VALUE;
}

void CrawlSpeed_Register() {
    bool shouldRegister = CVAR_CRAWL_SPEED_VALUE > 1;

    COND_VB_SHOULD(VB_CRAWL_SPEED_EXIT, shouldRegister, {
        Player* player = GET_PLAYER(gPlayState);
        bool excludeWellBackroom = (player->actor.world.pos.x > 950.0f) && (player->actor.world.pos.x < 1025.0f) &&
                                   (player->actor.world.pos.z > -1510.0f) && (player->actor.world.pos.z < -1490.0f) &&
                                   gPlayState->sceneNum == SCENE_BOTTOM_OF_THE_WELL;
        bool excludeGlitchAiding = CVAR_GLITCH_AIDING_VALUE;
        if (excludeGlitchAiding && excludeWellBackroom) {
            *should = true;
        } else {
            ExitCrawlspace(player, gPlayState);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_CRAWL_SPEED_EXIT_CS, shouldRegister, {
        Player* player = GET_PLAYER(gPlayState);
        Camera* csCam = va_arg(args, Camera*);
        s16 csId = va_arg(args, s16);
        s16 actionParameters = va_arg(args, s16);
        s16 initTimer = va_arg(args, s16);
        CutsceneCameraPoint* atPoints = va_arg(args, CutsceneCameraPoint*);
        CutsceneCameraPoint* eyePoints = va_arg(args, CutsceneCameraPoint*);
        bool excludeWellBackroom = (player->actor.world.pos.x > 950.0f) && (player->actor.world.pos.x < 1025.0f) &&
                                   (player->actor.world.pos.z > -1510.0f) && (player->actor.world.pos.z < -1490.0f) &&
                                   gPlayState->sceneNum == SCENE_BOTTOM_OF_THE_WELL;
        bool excludeGlitchAiding = CVAR_GLITCH_AIDING_VALUE;
        if (excludeGlitchAiding && excludeWellBackroom) {
            *should = true;
        } else {
            ExitCrawlspaceCS(gPlayState, csCam, actionParameters, initTimer, atPoints, eyePoints);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_CRAWL_SPEED_ENTER, shouldRegister, {
        Player* player = GET_PLAYER(gPlayState);
        EnterCrawlspace(player, gPlayState);
        *should = false;
    });

    COND_VB_SHOULD(VB_CRAWL_SPEED_INCREASE, shouldRegister, {
        Player* player = GET_PLAYER(gPlayState);
        IncreaseCrawlSpeed(player, gPlayState);
        *should = false;
    });
}

static RegisterShipInitFunc initSpeed(CrawlSpeed_Register, { CVAR_CRAWL_SPEED_NAME });