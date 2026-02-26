#include "CrashHandlerExt.h"
#include "variables.h"
#include "z64.h"
#include "z64actor.h"
#include <string.h>
#include <stdio.h>
#include <array>
#include "soh/ActorDB.h"
#include <fast/interpreter.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define WRITE_VAR_LINE(buff, len, varName, varValue) \
    append_str(buff, len, varName);                  \
    append_line(buff, len, varValue);
#define WRITE_VAR(buff, len, varName, varValue) \
    append_str(buff, len, varName);             \
    append_str(buff, len, varValue);

extern "C" PlayState* gPlayState;

namespace {
// Keep this in sync with Ship::CrashHandler::gMaxBufferSize.
constexpr size_t kCrashHandlerBufferLimit = 32768;
constexpr int32_t kMaxSaneActorCount = 2048;

inline void ClampAndTerminate(char* buf, size_t* len) {
    if (buf == nullptr || len == nullptr) {
        return;
    }

    if (*len >= kCrashHandlerBufferLimit) {
        *len = kCrashHandlerBufferLimit - 1;
    }
    buf[*len] = '\0';
}
} // namespace

static std::array<const char*, ACTORCAT_MAX> sCatToStrArray{
    "SWITCH", "BG", "PLAYER", "EXPLOSIVE", "NPC", "ENEMY", "PROP", "ITEMACTION", "MISC", "BOSS", "DOOR", "CHEST",
};

#define DEFINE_SCENE(_1, _2, enumName, _4, _5, _6) #enumName,

static std::array<const char*, SCENE_ID_MAX> sSceneIdToStrArray{
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE

static void append_str(char* buf, size_t* len, const char* str) {
    if (buf == nullptr || len == nullptr || str == nullptr) {
        return;
    }

    if (*len >= kCrashHandlerBufferLimit) {
        ClampAndTerminate(buf, len);
        return;
    }

    while (*str != '\0' && *len + 1 < kCrashHandlerBufferLimit) {
        buf[(*len)++] = *str++;
    }
    ClampAndTerminate(buf, len);
}

static void append_line(char* buf, size_t* len, const char* str) {
    append_str(buf, len, str);
    if (buf == nullptr || len == nullptr) {
        return;
    }
    if (*len + 1 < kCrashHandlerBufferLimit) {
        buf[(*len)++] = '\n';
        ClampAndTerminate(buf, len);
    }
}

static void CrashHandler_WriteActorData(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    if (buffer == nullptr || pos == nullptr || gPlayState == nullptr || ActorDB::Instance == nullptr) {
        append_line(buffer, pos, "  <actor context unavailable>");
        return;
    }

    for (unsigned int i = 0; i < ACTORCAT_MAX; i++) {
        if (*pos >= kCrashHandlerBufferLimit - 1) {
            return;
        }

        if (i >= sCatToStrArray.size()) {
            append_line(buffer, pos, "  Category: UNKNOWN");
            continue;
        }

        ActorListEntry* entry = &gPlayState->actorCtx.actorLists[i];
        if (entry == nullptr) {
            append_line(buffer, pos, "  Category: <null>");
            append_line(buffer, pos, "    <actor entry unavailable>");
            continue;
        }

        const int32_t actorCount = static_cast<int32_t>(entry->length);
        if (actorCount == 0) {
            continue;
        }
        WRITE_VAR_LINE(buffer, pos, "  Category: ", sCatToStrArray[i]);

        // Crash-safe mode: do not traverse actor linked lists here.
        // The original crash can corrupt list pointers and cause a second crash during logging.
        if (actorCount < 0 || actorCount > kMaxSaneActorCount) {
            append_line(buffer, pos, "    <actor list unavailable>");
            continue;
        }
        snprintf(intCharBuffer, sizeof(intCharBuffer), "%d", actorCount);
        WRITE_VAR_LINE(buffer, pos, "    count=", intCharBuffer);
        if (entry->head == nullptr) {
            append_line(buffer, pos, "    <head=null>");
        }
    }
}

extern "C" void CrashHandler_PrintSohData(char* buffer, size_t* pos) {
    if (buffer == nullptr || pos == nullptr) {
        return;
    }
    ClampAndTerminate(buffer, pos);

    char intCharBuffer[16];
    append_line(buffer, pos, "Build Information:");
    WRITE_VAR_LINE(buffer, pos, "  Game Version: ", (const char*)gBuildVersion);
    WRITE_VAR_LINE(buffer, pos, "  Git Branch: ", (const char*)gGitBranch);
    WRITE_VAR_LINE(buffer, pos, "  Git Commit: ", (const char*)gGitCommitHash);
    WRITE_VAR_LINE(buffer, pos, "  Build Date: ", (const char*)gBuildDate);

    if (gPlayState != nullptr) {
        const int32_t sceneNum = static_cast<int32_t>(gPlayState->sceneNum);
        if (sceneNum >= 0 && sceneNum < static_cast<int32_t>(sSceneIdToStrArray.size())) {
            WRITE_VAR_LINE(buffer, pos, "Scene: ", sSceneIdToStrArray[sceneNum]);
        } else {
            snprintf(intCharBuffer, sizeof(intCharBuffer), "%d", sceneNum);
            WRITE_VAR_LINE(buffer, pos, "Scene: <invalid id ", intCharBuffer);
            append_line(buffer, pos, ">");
        }

        snprintf(intCharBuffer, sizeof(intCharBuffer), "%i", gPlayState->roomCtx.curRoom.num);
        WRITE_VAR_LINE(buffer, pos, "Room: ", intCharBuffer);

        append_line(buffer, pos, "Actors:");
        CrashHandler_WriteActorData(buffer, pos);

        append_line(buffer, pos, "GFX Stack:");
        append_line(buffer, pos, "  <skipped in crash-safe mode>");
    }

    ClampAndTerminate(buffer, pos);
}
