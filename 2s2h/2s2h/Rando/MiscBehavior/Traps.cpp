#include "Traps.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "MiscBehavior.h"
#include "Rando/ActorBehavior/ActorBehavior.h"
#include "2s2h/DeveloperTools/SaveEditor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Time_Tag/z_en_time_tag.h"
void func_80833B18(PlayState* play, Player* thisx, s32 arg2, f32 speed, f32 velocityY, s16 arg5,
                   s32 invincibilityTimer);
void EnTimeTag_KickOut_Transition(EnTimeTag* enTimeTag, PlayState* play);
}

extern void UpdateGameTime(u16 gameTime);

int roll = TRAP_FREEZE;
const u16 timeSkipInterval = 4000;

std::map<TrapTypes, const char*> trapToCvarMap = {
    { TRAP_FREEZE, "gRando.Traps.Freeze" },      { TRAP_BLAST, "gRando.Traps.Blast" },
    { TRAP_SHOCK, "gRando.Traps.Shock" },        { TRAP_JINX, "gRando.Traps.Jinx" },
    { TRAP_WALLET, "gRando.Traps.Wallet" },      { TRAP_ENEMY, "gRando.Traps.Enemy" },
    { TRAP_TIME, "gRando.Traps.Time" },          { TRAP_FIRE, "gRando.Traps.Fire" },
    { TRAP_KNOCKBACK, "gRando.Traps.Knockback" }
};

std::vector<TrapTypes> getEnabledTrapTypes() {
    std::vector<TrapTypes> enabledTrapTypes;
    for (auto& trap : trapToCvarMap) {
        if (CVarGetInteger(trap.second, 0)) {
            enabledTrapTypes.push_back(trap.first);
        }
    }
    if (enabledTrapTypes.size() == 0) {
        enabledTrapTypes.push_back(TRAP_FREEZE);
    }
    return enabledTrapTypes;
};

int RollTrapType() {
    auto enabledTraps = getEnabledTrapTypes();
    roll = enabledTraps[rand() % enabledTraps.size()];
    return roll;
}

// clang-format off
std::vector<std::string> defaultTrapMessages = { 
    "This item is available in the %bRando DLC%w.",
    "This is what happens when %gCaladius%w is left unsupervised.",
    "Oh no!",
    "Uh oh!",
    "KEKW",
    "We've been trying to reach you about your Horse's %gextended warranty%w.",
    "Admit it, you wish this was %gGreg%w.",
    "Error 404: Item Not Found",
    "Get dunked on!",
    "This %rTrap%w is brought to you by today's sponsor...\n%gRaid Shadow Legends%w!",
};

std::vector<std::string> freezeTrapMessages = {
    "%rOcarina of Time%w called, they want their %bIce Trap%w back.",
    "Greetings from %bSnowhead%w! Wish you were here.",
    "This item was too %bcool%w for you anyway.",
    "Let me see your best %bKing Zora%w impersonation.",
    "There's no business like %bSnow%w business!",
    "How much does a polar bear weigh? Enough to break the %bice%w.",
    "You found the %yTrifo%w... Wait, nevermind...",
    "Quick Time Event! Don't die.",
};

std::vector<std::string> blastTrapMessages = {
    "Coming to you live from the %yThunderdome%w!",
    "There was supposed to be an Earth shattering %yKaboom%w!",
    "Pardon me while I %yburst%w.",
    "This is not the greatest %yblast%w in the world, this is just a tribute.",
    "Hey look, this %yitem%w is ticking!",
    "This is sure to make %gPamela%w leave the house.",
    "You found a %yDodongo Snack%w!",
};

std::vector<std::string> shockTrapMessages = {
    "We're losing him!\n%gCLEAR%w",
    "It's got what %gplants%w need.",
    "On todays episode of Grey's Anatomy...",
    "I'm giving it all I've got, captain!",
    "Now simulating a CD in a Microwave.",
    "Hang in there, for %gMeryl%w."
};

std::vector<std::string> jinxTrapMessages = {
    "Looks like someone's got a case of the %bMondays%w. :( ",
    "Your wrist hurts! The doctor says no more swords.",
};

std::vector<std::string> walletTrapMessages = {
    "Spare some %rchange%w?",
    "%rBreaking News%w: the Moon hasn't crashed, but Termina's economy sure has."
};

std::vector<std::string> enemyTrapMessages = {
    "You made a new friend!",
    // Like Like Specific, will need to adjust if new enemies are added.
    "Someone likes you! They %rLike Like%w you!",
    "You don't need a shield anyway.",
    "This item sucks.",
};

std::vector<std::string> timeTrapMessages = {
    "%yTime%w flashes before your eyes!",
    "You have played the %ySun's Song%w!",
    "The %rGoddess of Time%w smites you!",
    "Spent an hour and a half admiring this %bfake item%w.",
    "You found a great place to take a nap!",
    "Break time! Not like the world is ending right?",
    "I just need you to tell me how to get to the %ytime machine%w.",
    "Mweep",
};

std::map<TrapTypes, std::vector<std::string>> trapMessageList = {
    { TRAP_FREEZE, freezeTrapMessages },
    { TRAP_BLAST, blastTrapMessages },
    { TRAP_SHOCK, shockTrapMessages },
    { TRAP_JINX, jinxTrapMessages },
    { TRAP_WALLET, walletTrapMessages },
    { TRAP_ENEMY, enemyTrapMessages },
    { TRAP_TIME, timeTrapMessages },
    { TRAP_FIRE, defaultTrapMessages },
    { TRAP_KNOCKBACK, defaultTrapMessages },
};
// clang-format on

std::string GetTrapMessage() {
    RollTrapType();
    auto findIt = trapMessageList.find((TrapTypes)roll);
    if (findIt == trapMessageList.end()) {
        return defaultTrapMessages[rand() % defaultTrapMessages.size()];
    } else {
        return findIt->second[rand() % findIt->second.size()];
    }
}

void Rando::MiscBehavior::OfferTrapItem() {
    if (!gPlayState) {
        return;
    }

    switch (roll) {
        case TRAP_FREEZE:
            GameInteractor::Instance->events.emplace_back(
                GIEventTrap{ .action = []() { func_80833B18(gPlayState, GET_PLAYER(gPlayState), 3, 0, 0, 0, 0); } });
            break;
        case TRAP_BLAST:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                GET_PLAYER(gPlayState)->actor.colChkInfo.damage = 16;
                func_80833B18(gPlayState, GET_PLAYER(gPlayState), 2, 0, 0, 0, 0);
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_BOM, GET_PLAYER(gPlayState)->actor.world.pos.x,
                            GET_PLAYER(gPlayState)->actor.world.pos.y, GET_PLAYER(gPlayState)->actor.world.pos.z, 1, 0,
                            0, 0);
            } });
            break;
        case TRAP_SHOCK:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                GET_PLAYER(gPlayState)->actor.colChkInfo.damage = 16;
                func_80833B18(gPlayState, GET_PLAYER(gPlayState), 4, 0, 0, 0, 0);
            } });
            break;
        case TRAP_FIRE:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                Player* player = GET_PLAYER(gPlayState);
                for (int i = 0; i < 18; i++) {
                    player->bodyFlameTimers[i] = static_cast<uint8_t>(Rand_S16Offset(0, 200));
                }
                player->bodyIsBurning = true;
                func_80833B18(gPlayState, player, 0, 0, 0, 0, 0);
            } });
            break;
        case TRAP_KNOCKBACK:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                Player* player = GET_PLAYER(gPlayState);
                float strength = 4;
                func_800B8D98(gPlayState, &player->actor, strength * 5, player->actor.world.rot.y + 0x8000,
                              strength * 5);

                static HOOK_ID knockbackBounceHook = 0;
                GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(knockbackBounceHook);
                knockbackBounceHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
                    ACTOR_PLAYER, [](Actor* actor) {
                        Player* player = (Player*)actor;
                        if (player->actor.bgCheckFlags & 0x08 && abs(player->speedXZ) > 15.0f) {
                            player->yaw = ((player->actor.wallYaw - player->yaw) + player->actor.wallYaw) - 0x8000;
                            Player_PlaySfx(player, NA_SE_PL_BODY_HIT);
                        }

                        if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(
                                knockbackBounceHook);
                        }
                    });
            } });
            break;
        case TRAP_JINX:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                Actor_PlaySfx(&GET_PLAYER(gPlayState)->actor, NA_SE_EN_BUBLE_BITE);
                gSaveContext.jinxTimer = 1200;
            } });
            break;
        case TRAP_WALLET:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                int16_t currentRupees = gSaveContext.save.saveInfo.playerData.rupees;
                if (currentRupees != 0) {
                    Vec3f positional = GET_PLAYER(gPlayState)->actor.world.pos;
                    positional.y = GET_PLAYER(gPlayState)->actor.world.pos.y + 100.0f;
                    Item00Type rupee = ITEM00_RUPEE_GREEN;
                    int16_t spawnedRupees = 0;
                    int16_t remainingRupees = currentRupees;
                    for (int i = spawnedRupees; spawnedRupees < remainingRupees;) {
                        if (currentRupees >= 20) {
                            rupee = ITEM00_RUPEE_RED;
                            spawnedRupees += 20;
                            Rupees_ChangeBy(-20);
                            currentRupees -= 20;
                        } else if (currentRupees >= 5) {
                            rupee = ITEM00_RUPEE_BLUE;
                            spawnedRupees += 5;
                            Rupees_ChangeBy(-5);
                            currentRupees -= 5;
                        } else if (currentRupees >= 1) {
                            rupee = ITEM00_RUPEE_GREEN;
                            spawnedRupees += 1;
                            Rupees_ChangeBy(-1);
                            currentRupees -= 1;
                        }
                        EnItem00* rupeeActor = (EnItem00*)Item_DropCollectible(gPlayState, &positional, rupee);
                        rupeeActor->actor.speed = Rand_CenteredFloat(5.0f);
                        rupeeActor->unk152 = 600; // Extending Time before Despawning
                    }
                }
            } });
            break;
        case TRAP_ENEMY:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_RR, GET_PLAYER(gPlayState)->actor.world.pos.x,
                            GET_PLAYER(gPlayState)->actor.world.pos.y, GET_PLAYER(gPlayState)->actor.world.pos.z, 0, 0,
                            0, 1);
            } });
            break;
        case TRAP_TIME:
            GameInteractor::Instance->events.emplace_back(GIEventTrap{ .action = []() {
                u16 previous_time = gSaveContext.save.time;
                u16 new_time = gSaveContext.save.time + timeSkipInterval;
                if (previous_time < MORNING_TIME && new_time >= MORNING_TIME) {
                    // Handles case where Night -> Day
                    if (gSaveContext.save.day != 3) {
                        gSaveContext.save.day++;
                        gSaveContext.save.eventDayCount++;
                        UpdateGameTime(new_time);
                        Interface_NewDay(gPlayState, CURRENT_DAY);
                        // Load environment values for new day
                        Environment_NewDay(&gPlayState->envCtx);
                        // Clear weather from day 2
                        gWeatherMode = WEATHER_MODE_CLEAR;
                        gPlayState->envCtx.lightningState = LIGHTNING_OFF;
                    } else {
                        // Handles Moonfall case, prevents skipping past it by setting time right before Moonfall.
                        UpdateGameTime(MORNING_TIME - (timeSkipInterval / 10));
                    }
                } else {
                    // Every other case
                    UpdateGameTime(new_time);
                }
                TransitionFade_SetColor(&gPlayState->unk_18E48, 0x000000);
                R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
                Player_PlaySfx(GET_PLAYER(gPlayState), NA_SE_SY_TRANSFORM_MASK_FLASH);

                // Handle kickouts, if needed
                EnTimeTag* enTimeTag = (EnTimeTag*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor,
                                                                    ACTOR_EN_TIME_TAG, ACTORCAT_ITEMACTION, 99999.9f);
                if (enTimeTag != nullptr) {
                    TimeTagType timeTagType = (TimeTagType)TIMETAG_GET_TYPE(&enTimeTag->actor);
                    if (timeTagType == TIMETAG_KICKOUT_DOOR || timeTagType >= TIMETAG_KICKOUT_FINAL_HOURS) {
                        s16 kickoutHour = TIMETAG_KICKOUT_HOUR(&enTimeTag->actor);
                        s16 kickoutMinute = TIMETAG_KICKOUT_MINUTE(&enTimeTag->actor);
                        s32 kickoutTime = CLOCK_TIME(kickoutHour, kickoutMinute);
                        kickoutTime = ZERO_DAY_START(kickoutTime);
                        previous_time = ZERO_DAY_START(previous_time);
                        new_time = ZERO_DAY_START(new_time);
                        // If we were here before the kickout time, and now it's after, then get out of my house
                        if (previous_time <= kickoutTime && new_time >= kickoutTime) {
                            // Unless this is the Stock Pot Inn, and the room key is obtained
                            if (!(gPlayState->sceneId == SCENE_YADOYA &&
                                  Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY))) {
                                // This comes from EnTimeTag_KickOut_WaitForTime
                                Player_SetCsActionWithHaltedActors(gPlayState, &enTimeTag->actor, PLAYER_CSACTION_WAIT);
                                Message_StartTextbox(gPlayState, 0x1883 + TIMETAG_KICKOUT_GET_TEXT(&enTimeTag->actor),
                                                     NULL);
                                enTimeTag->actionFunc = EnTimeTag_KickOut_Transition;
                            }
                        }
                    }
                }
            } });
            break;
        default:
            break;
    }
}

void Rando::ActorBehavior::InitTrapsBehavior() {
    // Selectively disable object dependency for actors spawned by traps
    COND_VB_SHOULD(VB_ENABLE_OBJECT_DEPENDENCY, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_TRAPS], {
        ObjectId objectId = (ObjectId)va_arg(args, int);
        // Allow spawning of like-like & shield so it drops from like-like
        if (objectId == OBJECT_RR || objectId == OBJECT_GI_SHIELD_2) {
            *should = false;
        }
    });
}