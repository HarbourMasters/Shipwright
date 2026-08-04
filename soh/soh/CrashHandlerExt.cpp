#include "CrashHandlerExt.h"
#include "variables.h"
#include "z64.h"
#include "z64actor.h"
#include <string.h>
#include <stdio.h>
#include <array>
#include "soh/ActorDB.h"
#include <fast/interpreter.h>

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

#define DEFINE_SCENE(_1, _2, enumName, _4, _5, _6) #enumName,

static std::array<const char*, SCENE_ID_MAX> sSceneIdToStrArray{
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE

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
    for (unsigned int i = 0; i < ACTORCAT_MAX; i++) {

        ActorListEntry* entry = &gPlayState->actorCtx.actorLists[i];
        Actor* cur;

        if (entry->length == 0) {
            continue;
        }
        WRITE_VAR_LINE(buffer, pos, "  Category: ", sCatToStrArray[i]);
        cur = entry->head;
        while (cur != nullptr) {
            std::string actorLine = "    ";
            actorLine += ActorDB::Instance->RetrieveEntry(cur->id).entry.valid
                             ? ActorDB::Instance->RetrieveEntry(cur->id).entry.desc
                             : "???";
            actorLine += " (" + std::to_string(cur->params) + ")";
            append_line(buffer, pos, actorLine.c_str());

            cur = cur->next;
        }
    }
}

extern "C" void CrashHandler_PrintSohData(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    append_line(buffer, pos, "Build Information:");
    WRITE_VAR_LINE(buffer, pos, "  Game Version: ", (const char*)gBuildVersion);
    WRITE_VAR_LINE(buffer, pos, "  Git Branch: ", (const char*)gGitBranch);
    WRITE_VAR_LINE(buffer, pos, "  Git Commit: ", (const char*)gGitCommitHash);
    WRITE_VAR_LINE(buffer, pos, "  Build Date: ", (const char*)gBuildDate);

    if (gPlayState != nullptr) {
        WRITE_VAR_LINE(buffer, pos, "Scene: ", sSceneIdToStrArray[gPlayState->sceneNum]);

        snprintf(intCharBuffer, sizeof(intCharBuffer), "%i", gPlayState->roomCtx.curRoom.num);
        WRITE_VAR_LINE(buffer, pos, "Room: ", intCharBuffer);

        append_line(buffer, pos, "Actors:");
        CrashHandler_WriteActorData(buffer, pos);

        append_line(buffer, pos, "GFX Stack:");
        for (auto& disp : Fast::g_exec_stack.disp_stack) {
            std::string line = "  ";
            line += disp.file;
            line += ":";
            line += std::to_string(disp.line);
            append_line(buffer, pos, line.c_str());
        }
    }
}
