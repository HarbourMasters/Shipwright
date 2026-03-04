#include <soh/OTRGlobals.h>
extern "C" {
extern PlayState* gPlayState;
#include "functions.h"
#include "overlays/actors/ovl_En_Ossan/z_en_ossan.h"
}

void RegisterShuffleSpeak() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_SPEAK).Get();

    COND_VB_SHOULD(VB_BUSINESS_SCRUB_SPEAK, shouldRegister, {
        if (!Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_DEKU)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_SPEAK, shouldRegister, {
        Actor* talkActor = GET_PLAYER(gPlayState)->talkActor;
        if (talkActor != NULL && talkActor->category == ACTORCAT_NPC &&
            !(talkActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)) {
            RandomizerInf inf = RAND_INF_MAX;
            switch (talkActor->id) {
                case ACTOR_EN_DNS:
                case ACTOR_EN_DNT_JIJI:
                case ACTOR_EN_HINTNUTS:
                case ACTOR_EN_KAKASI:
                case ACTOR_EN_KAKASI2:
                case ACTOR_EN_KAKASI3:
                case ACTOR_OBJ_DEKUJR:
                    inf = RAND_INF_CAN_SPEAK_DEKU;
                    break;
                case ACTOR_EN_GE1:
                case ACTOR_EN_GE3:
                case ACTOR_EN_NB:
                    // skip ACTOR_EN_GE2 to always be able to ask to be thrown in jail
                    inf = RAND_INF_CAN_SPEAK_GERUDO;
                    break;
                case ACTOR_EN_GO:
                case ACTOR_EN_GO2:
                case ACTOR_EN_GM:
                case ACTOR_EN_DU:
                    inf = RAND_INF_CAN_SPEAK_GORON;
                    break;
                case ACTOR_EN_ANI:
                case ACTOR_EN_BOM_BOWL_MAN:
                case ACTOR_EN_CS:
                case ACTOR_EN_DAIKU:
                case ACTOR_EN_DAIKU_KAKARIKO:
                case ACTOR_EN_DS:
                case ACTOR_EN_FU:
                case ACTOR_EN_GB:
                case ACTOR_EN_GIRLA:
                case ACTOR_EN_GUEST:
                case ACTOR_EN_HEISHI1:
                case ACTOR_EN_HEISHI2:
                case ACTOR_EN_HEISHI3:
                case ACTOR_EN_HEISHI4:
                case ACTOR_EN_HS:
                case ACTOR_EN_HS2:
                case ACTOR_EN_HY:
                case ACTOR_EN_IN:
                case ACTOR_EN_JS:
                case ACTOR_EN_MA1:
                case ACTOR_EN_MA2:
                case ACTOR_EN_MA3:
                case ACTOR_EN_MK:
                case ACTOR_EN_MM:
                case ACTOR_EN_MM2:
                case ACTOR_EN_MS:
                case ACTOR_EN_MU:
                case ACTOR_EN_NIW_GIRL:
                case ACTOR_EN_NIW_LADY:
                case ACTOR_EN_SSH:
                case ACTOR_EN_STH:
                case ACTOR_EN_SYATEKI_MAN:
                case ACTOR_EN_TA:
                case ACTOR_EN_TAKARA_MAN:
                case ACTOR_EN_TG:
                case ACTOR_EN_TK:
                case ACTOR_EN_PO_RELAY:
                case ACTOR_EN_TORYO:
                case ACTOR_EN_XC:
                case ACTOR_EN_ZL1:
                case ACTOR_EN_ZL2:
                case ACTOR_EN_ZL3:
                case ACTOR_EN_ZL4:
                case ACTOR_FISHING:
                    inf = RAND_INF_CAN_SPEAK_HYLIAN;
                    break;
                case ACTOR_EN_KO:
                case ACTOR_EN_SA:
                case ACTOR_EN_MD:
                case ACTOR_EN_SKJ:
                    inf = RAND_INF_CAN_SPEAK_KOKIRI;
                    break;
                case ACTOR_EN_DIVING_GAME:
                case ACTOR_EN_KZ:
                case ACTOR_EN_RU1:
                case ACTOR_EN_RU2:
                case ACTOR_EN_ZO:
                    inf = RAND_INF_CAN_SPEAK_ZORA;
                    break;
                case ACTOR_EN_OSSAN:
                    switch (talkActor->params) {
                        case OSSAN_TYPE_KOKIRI:
                            inf = RAND_INF_CAN_SPEAK_KOKIRI;
                            break;
                        case OSSAN_TYPE_KAKARIKO_POTION:
                        case OSSAN_TYPE_BOMBCHUS:
                        case OSSAN_TYPE_MARKET_POTION:
                        case OSSAN_TYPE_BAZAAR:
                        case OSSAN_TYPE_ADULT:
                        case OSSAN_TYPE_TALON:
                        case OSSAN_TYPE_INGO:
                        case OSSAN_TYPE_MASK:
                            inf = RAND_INF_CAN_SPEAK_HYLIAN;
                            break;
                        case OSSAN_TYPE_GORON:
                            inf = RAND_INF_CAN_SPEAK_GORON;
                            break;
                        case OSSAN_TYPE_ZORA:
                            inf = RAND_INF_CAN_SPEAK_ZORA;
                            break;
                    }
                    break;
                case ACTOR_EN_OWL:
                    if (!Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_DEKU) &&
                        !Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_GERUDO) &&
                        !Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_GORON) &&
                        !Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_HYLIAN) &&
                        !Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_KOKIRI) &&
                        !Flags_GetRandomizerInf(RAND_INF_CAN_SPEAK_ZORA)) {
                        *should = false;
                    }
                    return;
            }
            if (inf != RAND_INF_MAX && !Flags_GetRandomizerInf(inf)) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterShuffleSpeak, { "IS_RANDO" });