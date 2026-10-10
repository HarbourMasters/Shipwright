#include "2s2h/ShipUtils.h"
#include <libultraship/bridge.h>
#include <string.h>
#include <stdio.h>
#include <array>
#include <fast/interpreter.h>

extern "C" {
#include "build.h"
#include "variables.h"
}

#define WRITE_VAR_LINE(buff, len, varName, varValue) \
    append_str(buff, len, varName);                  \
    append_line(buff, len, varValue);
#define WRITE_VAR(buff, len, varName, varValue) \
    append_str(buff, len, varName);             \
    append_str(buff, len, varValue);

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
        WRITE_VAR_LINE(buffer, pos, "  Category: ", GetActorCategoryName(i).c_str());
        cur = entry->first;
        while (cur != nullptr) {
            std::string actorLine = "    " + GetActorDescription(cur->id);
            actorLine += "(";
            actorLine += std::to_string(cur->params);
            actorLine += ")";
            append_line(buffer, pos, actorLine.c_str());

            cur = cur->next;
        }
    }
}

extern "C" void CrashHandler_PrintExt(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    append_line(buffer, pos, "Build Information:");
    WRITE_VAR_LINE(buffer, pos, "  Game Version: ", (const char*)gBuildVersion);
    WRITE_VAR_LINE(buffer, pos, "  Git Branch: ", (const char*)gGitBranch);
    WRITE_VAR_LINE(buffer, pos, "  Git Commit: ", (const char*)gGitCommitHash);
    WRITE_VAR_LINE(buffer, pos, "  Build Date: ", (const char*)gBuildDate);

    if (gPlayState != nullptr) {
        WRITE_VAR_LINE(buffer, pos, "Scene: ", Ship_GetSceneName(gPlayState->sceneId));

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
