#include <soh/OTRGlobals.h>

extern "C" {
#include "overlays/actors/ovl_En_Hy/z_en_hy.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

void BuildEnHyMessage_BlueFire(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("%cBlue Fire%w! I'll trade you %rsomething special%w for it. No returns! I get new "
                                      "special inventory %bevery 7 years or so%w...",
                                      "%cBlaues Feuer%w! Ich tausche es gegen %retwas Besonderes%w. Und nicht feilschen, okay! Ich bekomme neue "
                                      "besondere Ware %balle 7 Jahre oder so%w...",
                                      "%cFeu bleu%w ! Je l'échange contre %rquelque chose de spécial%w. Pas de retour ! Je reçois de nouveaux "
                                      "articles spéciaux %btous les 7 ans environ%w..."
    );
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildEnHyMessage_Fish(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("A %pfish%w! I'll trade you %rsomething special%w for it. No returns! I get new "
                                      "special inventory %bevery 7 years or so%w...",
                                      "Ein %pFisch%w! Ich tausche ihn gegen %retwas Besonderes%w. Und nicht feilschen, okay! Ich bekomme neue "
                                      "besondere Ware %balle 7 Jahre oder so%w...",
                                      "Un %ppoisson%w ! Je l'échange contre %rquelque chose de spécial%w. Pas de retour ! Je reçois de nouveaux "
                                      "articles spéciaux %btous les 7 ans environ%w..."
    );
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildEnHyMessage_Bug(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("A tiny %gbug%w! I'll trade you %rsomething special%w for it. No returns! I get new "
                                      "special inventory %bevery 7 years or so%w...",
                                      "Ein kleiner %gKäfer%w! Ich tausche ihn gegen %retwas Besonderes%w. Und nicht feilschen, okay! Ich bekomme neue "
                                      "besondere Ware %balle 7 Jahre oder so%w...",
                                      "Un petit %ginsecte%w ! Je l'échange contre %rquelque chose de spécial%w. Pas de retour ! Je reçois de nouveaux "
                                      "articles spéciaux %btous les 7 ans environ%w..."
    );
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

uint8_t EnHy_RandomizerHoldsItem(CheckIdentity identity) {
    if (identity.randomizerCheck == RC_UNKNOWN_CHECK || !IS_RANDO || Flags_GetRandomizerInf(identity.randomizerInf)) {
        return false;
    }
    return true;
}

void RegisterShuffleBeggar() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_BEGGAR).Get();

    COND_VB_SHOULD(VB_BEGGAR_GIVE_ITEM, shouldRegister, {
        EnHy* enHy = va_arg(args, EnHy*);
        Player* player = GET_PLAYER(gPlayState);

        if (enHy->actor.textId != 0x70F3) {
            CheckIdentity identity =
                OTRGlobals::Instance->gRandomizer->IdentifyBeggar(gPlayState->sceneNum, player->exchangeItemId);

            if (EnHy_RandomizerHoldsItem(identity)) {
                Flags_SetRandomizerInf(identity.randomizerInf);
                *should = false;
            } 
        }
    });

    COND_ID_HOOK(OnOpenText, TEXT_BEGGAR_BUY_BLUE_FIRE, shouldRegister, BuildEnHyMessage_BlueFire);
    COND_ID_HOOK(OnOpenText, TEXT_BEGGAR_BUY_FISH, shouldRegister, BuildEnHyMessage_Fish);
    COND_ID_HOOK(OnOpenText, TEXT_BEGGAR_BUY_BUGS, shouldRegister, BuildEnHyMessage_Bug);

}

void Rando::StaticData::RegisterBeggarLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    //            Randomizer Check          Randomizer Check                            Quest            RCType             Area                        Actor ID        Scene ID                    Params  Short Name              Hint Text Key                   Vanilla Item        Spoiler Collection Check
    locationTable[RC_MK_BEGGAR_BUG]         = Location::Base(RC_MK_BEGGAR_BUG,          RCQUEST_BOTH,    RCTYPE_BEGGAR,     RCAREA_MARKET,              ACTOR_EN_HY,    SCENE_MARKET_DAY,           0x00,   "Beggar Bug",           RHT_BEGGAR_MARKET,              RG_PURPLE_RUPEE,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_MK_BEGGAR_BUG));
    locationTable[RC_MK_BEGGAR_FISH]        = Location::Base(RC_MK_BEGGAR_FISH,         RCQUEST_BOTH,    RCTYPE_BEGGAR,     RCAREA_MARKET,              ACTOR_EN_HY,    SCENE_MARKET_DAY,           0x00,   "Beggar Fish",          RHT_BEGGAR_MARKET,              RG_PURPLE_RUPEE,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_MK_BEGGAR_FISH));
    locationTable[RC_MK_BEGGAR_BLUE_FIRE]   = Location::Base(RC_MK_BEGGAR_BLUE_FIRE,    RCQUEST_BOTH,    RCTYPE_BEGGAR,     RCAREA_MARKET,              ACTOR_EN_HY,    SCENE_MARKET_DAY,           0x00,   "Beggar Blue Fire",     RHT_BEGGAR_MARKET,              RG_HUGE_RUPEE,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_MK_BEGGAR_BLUE_FIRE));
    locationTable[RC_KAK_BEGGAR_BUG]        = Location::Base(RC_KAK_BEGGAR_BUG,         RCQUEST_BOTH,    RCTYPE_BEGGAR,     RCAREA_KAKARIKO_VILLAGE,    ACTOR_EN_HY,    SCENE_KAKARIKO_VILLAGE,     0x00,   "Beggar Bug",           RHT_BEGGAR_KAKARIKO_VILLAGE,    RG_PURPLE_RUPEE,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_BEGGAR_BUG));
    locationTable[RC_KAK_BEGGAR_FISH]       = Location::Base(RC_KAK_BEGGAR_FISH,        RCQUEST_BOTH,    RCTYPE_BEGGAR,     RCAREA_KAKARIKO_VILLAGE,    ACTOR_EN_HY,    SCENE_KAKARIKO_VILLAGE,     0x00,   "Beggar Fish",          RHT_BEGGAR_KAKARIKO_VILLAGE,    RG_PURPLE_RUPEE,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_BEGGAR_FISH));
    locationTable[RC_KAK_BEGGAR_BLUE_FIRE]  = Location::Base(RC_KAK_BEGGAR_BLUE_FIRE,   RCQUEST_BOTH,    RCTYPE_BEGGAR,     RCAREA_KAKARIKO_VILLAGE,    ACTOR_EN_HY,    SCENE_KAKARIKO_VILLAGE,     0x00,   "Beggar Blue Fire",     RHT_BEGGAR_KAKARIKO_VILLAGE,    RG_HUGE_RUPEE,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_BEGGAR_BLUE_FIRE));


    // clang-format on
}

static RegisterShipInitFunc initFunc(RegisterShuffleBeggar, { "IS_RANDO" });
static RegisterShipInitFunc registerSignLocations(Rando::StaticData::RegisterBeggarLocations);