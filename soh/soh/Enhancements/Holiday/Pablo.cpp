#include "Holiday.hpp"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "src/overlays/effects/ovl_Effect_Ss_En_Ice/z_eff_ss_en_ice.h"

extern "C" {
    extern PlayState* gPlayState;
}

#define AUTHOR "Pablo"
#define CVAR(v) "gHoliday." AUTHOR "." v

#pragma region Shiny

static Vec3f shineSpots[12] = {
    { 20.0f, 20.0f,   0.0f }, {  10.0f, 40.0f,  10.0f }, { -10.0f, 40.0f,  10.0f }, { -20.0f, 20.0f,   0.0f },
    { 10.0f, 40.0f, -10.0f }, { -10.0f, 40.0f, -10.0f }, {   0.0f, 20.0f, -20.0f }, {  10.0f,  0.0f,  10.0f },
    { 10.0f,  0.0f, -10.0f }, {   0.0f, 20.0f,  20.0f }, { -10.0f,  0.0f,  10.0f }, { -10.0f,  0.0f, -10.0f },
};

static u8 shinyableActorIds[51] = {
    ACTOR_EN_WALLMAS,
    ACTOR_EN_ZF,
    ACTOR_EN_YUKABYUN,
    ACTOR_EN_WF,
    ACTOR_EN_WEIYER,
    ACTOR_EN_VM,
    ACTOR_EN_TUBO_TRAP,
    ACTOR_EN_VALI,
    ACTOR_EN_TP,
    ACTOR_EN_TORCH2,
    ACTOR_EN_TITE,
    ACTOR_EN_TEST,
    ACTOR_EN_SW,
    ACTOR_EN_ST,
    ACTOR_EN_SKB,
    ACTOR_EN_SKJ,
    ACTOR_EN_PEEHAT,
    ACTOR_EN_SB,
    ACTOR_EN_RR,
    ACTOR_EN_REEBA,
    ACTOR_EN_RD,
    ACTOR_EN_PO_SISTERS,
    ACTOR_EN_PO_FIELD,
    ACTOR_EN_POH,
    ACTOR_EN_KAREBABA,
    ACTOR_EN_OKUTA,
    ACTOR_EN_NY,
    ACTOR_EN_MB,
    ACTOR_EN_IK,
    ACTOR_EN_GOMA,
    ACTOR_EN_GELDB,
    ACTOR_EN_FZ,
    ACTOR_EN_FLOORMAS,
    ACTOR_EN_FIREFLY,
    ACTOR_EN_FD,
    ACTOR_EN_EIYER,
    ACTOR_EN_DODONGO,
    ACTOR_EN_DODOJR,
    ACTOR_EN_DH,
    ACTOR_EN_DEKUBABA,
    ACTOR_EN_CROW,
    ACTOR_EN_CLEAR_TAG,
    ACTOR_EN_BW,
    ACTOR_EN_BUBBLE,
    ACTOR_EN_AM,
    ACTOR_EN_BILI,
    ACTOR_EN_BIGOKUTA,
    ACTOR_EN_BB,
    ACTOR_EN_BA,
    ACTOR_EN_ANUBICE,
    ACTOR_DOOR_KILLER
};

u8 CanBeShiny(Actor* actor) {
    for (u8 i = 0; i < ARRAY_COUNT(shinyableActorIds); i += 1) {
        if (shinyableActorIds[i] == actor->id) {
            return true;
        }
    }
    return false;
}

void ApplyShinyness(Actor* actor) {
    if (!CanBeShiny(actor)) {
        assert(false);
        return;
    }

    actor->isShiny = true;
    actor->colChkInfo.health *= 4;
    actor->maximumHealth *= 4;
    actor->scale.x *= 1.25f;
    actor->scale.y *= 1.25f;
    actor->scale.z *= 1.25f;
}

void RenderShines(Actor* actor) {
    if (!CanBeShiny(actor) || !actor->isShiny) {
        assert(false);
        return;
    }

    if (gSaveContext.gameMode != GAMEMODE_NORMAL || gPlayState->pauseCtx.state != 0) {
        return;
    }

    if (Rand_ZeroOne() < 0.1f) {
        Vec3f shinePos;
        s32 i = (s32)(Rand_ZeroOne() * ARRAY_COUNT(shineSpots));

        shinePos.x = actor->world.pos.x + shineSpots[i].x;
        shinePos.y = actor->world.pos.y + shineSpots[i].y;
        shinePos.z = actor->world.pos.z + shineSpots[i].z;

        EffectSsEnIceInitParams initParams;

        initParams.actor = actor;
        initParams.pos = shinePos;
        initParams.type = 0;

        // 50/50 chance of red or green
        if (Rand_Next() % 2 == 0) {
            initParams.primColor.r = 255;
            initParams.primColor.g = 0;
            initParams.envColor.r = 255;
            initParams.envColor.g = 0;
        } else {
            initParams.primColor.r = 0;
            initParams.primColor.g = 255;
            initParams.envColor.r = 0;
            initParams.envColor.g = 255;
        }

        initParams.primColor.b = 0;
        initParams.primColor.a = 250;
        initParams.envColor.b = 0;
        initParams.scale = 0.5f;

        EffectSs_Spawn(gPlayState, EFFECT_SS_EN_ICE, 80, &initParams);
    }
}

void SpawnShinyReward(Actor* actor) {
    if (!CanBeShiny(actor)) {
        assert(false);
        return;
    }

    for (u8 i = 0; i < 10; i += 1) {
        Item_DropCollectible(gPlayState, &actor->world.pos, ITEM00_RUPEE_RED);
        Item_DropCollectible(gPlayState, &actor->world.pos, ITEM00_RUPEE_GREEN);
    }
}

void RegisterShiny() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        if (CVarGetInteger(CVAR("Shiny.Enabled"), 0) && CanBeShiny(actor) && Rand_ZeroOne() < (1.0f / (s32)CVarGetInteger(CVAR("Shiny.Chance"), 8192))) {
            ApplyShinyness(actor);
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnEnemyDefeat>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        if (CVarGetInteger(CVAR("Shiny.Enabled"), 0) && actor->isShiny) {
            SpawnShinyReward(actor);
        }
    });

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorDraw>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        if (CVarGetInteger(CVAR("Shiny.Enabled"), 0) && actor->isShiny) {
            RenderShines(actor);
        }
    });
}

void ShinyDrawImGui() {
    UIWidgets::PaddedEnhancementCheckbox("Enable Shiny Enemies", CVAR("Shiny.Enabled"), true, false);
    UIWidgets::Tooltip("Allows enemies to be shiny.\nShiny enemies are 25% bigger and have 4 times the health but drop the equivalent of a gold rupee upon death");

    UIWidgets::PaddedEnhancementSliderInt("Shiny Chance: %d", "##ShinyChance", CVAR("Shiny.Chance"), 1, 8192, "", 8192, true, true, false, false, "");
    UIWidgets::Tooltip("The chance for an enemy to be shiny is 1 / Shiny Chance");
}

#pragma endregion

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    ShinyDrawImGui();
}

static void RegisterMod() {
    RegisterShiny();
}

static Holiday holiday(DrawMenu, RegisterMod);