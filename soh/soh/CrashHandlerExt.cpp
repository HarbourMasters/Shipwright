#include "CrashHandlerExp.h"
#include "variables.h"
#include "z64.h"
#include "z64actor.h"
#include <string.h>
#include <stdio.h>
#include <array>

#define WRITE_VAR_LINE(buff, len, varName, varValue) \
    append_str(buff, len, varName);                  \
    append_line(buff, len, varValue);
#define WRITE_VAR(buff, len, varName, varValue) \
    append_str(buff, len, varName);             \
    append_str(buff, len, varValue);

extern "C" PlayState* gPlayState;

static std::array<const char*, ACTORCAT_MAX> sCatToStrArray{
    "SWITCH", "BG", "PLAYER", "EXPLOSIVE", "NPC", "ENEMY", "PROP", "ITEMACTION", "MISC", "BOSS", "DOOR", "CHEST",
};

static std::array<const char*, SCENE_ID_MAX> sSceneIdToStrArray{
    "SCENE_YDAN",
    "SCENE_DDAN",
    "SCENE_BDAN",
    "SCENE_BMORI1",
    "SCENE_HIDAN",
    "SCENE_MIZUSIN",
    "SCENE_JYASINZOU",
    "SCENE_HAKADAN",
    "SCENE_HAKADANCH",
    "SCENE_ICE_DOUKUTO",
    "SCENE_GANON",
    "SCENE_MEN",
    "SCENE_GERUDOWAY",
    "SCENE_GANONTIKA",
    "SCENE_GANON_SONOGO",
    "SCENE_GANONTIKA_SONOGO",
    "SCENE_TAKARAYA",
    "SCENE_YDAN_BOSS",
    "SCENE_DDAN_BOSS",
    "SCENE_BDAN_BOSS",
    "SCENE_MORIBOSSROOM",
    "SCENE_FIRE_BS",
    "SCENE_MIZUSIN_BS",
    "SCENE_JYASINBOSS",
    "SCENE_HAKADAN_BS",
    "SCENE_GANON_BOSS",
    "SCENE_GANON_FINAL",
    "SCENE_ENTRA",
    "SCENE_ENTRA_N",
    "SCENE_ENRUI",
    "SCENE_MARKET_ALLEY",
    "SCENE_MARKET_ALLEY_N",
    "SCENE_MARKET_DAY",
    "SCENE_MARKET_NIGHT",
    "SCENE_MARKET_RUINS",
    "SCENE_SHRINE",
    "SCENE_SHRINE_N",
    "SCENE_SHRINE_R",
    "SCENE_KOKIRI_HOME",
    "SCENE_KOKIRI_HOME3",
    "SCENE_KOKIRI_HOME4",
    "SCENE_KOKIRI_HOME5",
    "SCENE_KAKARIKO",
    "SCENE_KAKARIKO3",
    "SCENE_SHOP1",
    "SCENE_KOKIRI_SHOP",
    "SCENE_GOLON",
    "SCENE_ZOORA",
    "SCENE_DRAG",
    "SCENE_ALLEY_SHOP",
    "SCENE_NIGHT_SHOP",
    "SCENE_FACE_SHOP",
    "SCENE_LINK_HOME",
    "SCENE_IMPA",
    "SCENE_MALON_STABLE",
    "SCENE_LABO",
    "SCENE_HYLIA_LABO",
    "SCENE_TENT",
    "SCENE_HUT",
    "SCENE_DAIYOUSEI_IZUMI",
    "SCENE_YOUSEI_IZUMI_TATE",
    "SCENE_YOUSEI_IZUMI_YOKO",
    "SCENE_KAKUSIANA",
    "SCENE_HAKAANA",
    "SCENE_HAKAANA2",
    "SCENE_HAKAANA_OUKE",
    "SCENE_SYATEKIJYOU",
    "SCENE_TOKINOMA",
    "SCENE_KENJYANOMA",
    "SCENE_HAIRAL_NIWA",
    "SCENE_HAIRAL_NIWA_N",
    "SCENE_HIRAL_DEMO",
    "SCENE_HAKASITARELAY",
    "SCENE_TURIBORI",
    "SCENE_NAKANIWA",
    "SCENE_BOWLING",
    "SCENE_SOUKO",
    "SCENE_MIHARIGOYA",
    "SCENE_MAHOUYA",
    "SCENE_GANON_DEMO",
    "SCENE_KINSUTA",
    "SCENE_SPOT00",
    "SCENE_SPOT01",
    "SCENE_SPOT02",
    "SCENE_SPOT03",
    "SCENE_SPOT04",
    "SCENE_SPOT05",
    "SCENE_SPOT06",
    "SCENE_SPOT07",
    "SCENE_SPOT08",
    "SCENE_SPOT09",
    "SCENE_SPOT10",
    "SCENE_SPOT11",
    "SCENE_SPOT12",
    "SCENE_SPOT13",
    "SCENE_SPOT15",
    "SCENE_SPOT16",
    "SCENE_SPOT17",
    "SCENE_SPOT18",
    "SCENE_SPOT20",
    "SCENE_GANON_TOU",
};

static void append_str(char* buf, size_t* len, const char* str) {
    while (*str != '\0')
        buf[(*len)++] = *str++;
}

static void append_line(char* buf, size_t* len, const char* str) {
    while (*str != '\0')
        buf[(*len)++] = *str++;
    buf[(*len)++] = '\n';
}

static void CrashHandler_WriteActorData(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    append_line(buffer, pos, "Actor Id      Params");
    for (unsigned int i = 0; i < ACTORCAT_MAX; i++) {

        ActorListEntry* entry = &gPlayState->actorCtx.actorLists[i];
        Actor* cur;

        if(entry->length == 0) {
            continue;
        }
        WRITE_VAR_LINE(buffer, pos, "Actor Cat: ", sCatToStrArray[i]);
        cur = entry->head;
        while (cur != nullptr) {
            // Actor ID
            snprintf(intCharBuffer, sizeof(intCharBuffer), "0x%03X       ", cur->id);
            append_str(buffer, pos, intCharBuffer);

            // Actor Params
            snprintf(intCharBuffer, sizeof(intCharBuffer), "0x%04X", cur->params);
            append_line(buffer, pos, intCharBuffer);

            cur = cur->next;
        }
    }
}

extern "C" void CrashHandler_PrintSohData(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    append_line(buffer, pos, "Build Information:");
    WRITE_VAR_LINE(buffer, pos, "Game Version: ", (const char*)gBuildVersion);
    WRITE_VAR_LINE(buffer, pos, "Build Date: ", (const char*)gBuildDate);

    if (gPlayState != nullptr) {
        append_line(buffer, pos, "Actors:");
        CrashHandler_WriteActorData(buffer, pos);
        
        WRITE_VAR_LINE(buffer, pos, "Scene: ", sSceneIdToStrArray[gPlayState->sceneNum]);

        snprintf(intCharBuffer, sizeof(intCharBuffer), "%i", gPlayState->roomCtx.curRoom.num);
        WRITE_VAR_LINE(buffer, pos, "Room: ", intCharBuffer);
    }
}