#include "CutsceneTime.h"

#include <string>
#include <vector>

#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

#include "soh/ResourceManagerHelpers.h"
#include "soh/resource/type/Scene.h"
#include "soh/resource/type/scenecommand/SetAlternateHeaders.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"

extern "C" {
#include "variables.h"
}

static SOH::Scene* LoadScene(const std::string& path) {
    return (SOH::Scene*)Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path).get();
}

template <typename T> static T* FindCommand(SOH::Scene* scene, SOH::SceneCommandID id) {
    for (auto& cmd : scene->commands) {
        if (cmd->cmdId == id) {
            return (T*)cmd.get();
        }
    }
    return nullptr;
}

// Same header pick as Scene_CommandAlternateHeaderList
static SOH::Scene* LayerHeader(SOH::Scene* scene, int32_t sceneLayer) {
    auto headers = FindCommand<SOH::SetAlternateHeaders>(scene, SOH::SceneCommandID::SetAlternateHeaders);
    if (headers == nullptr || sceneLayer - 1 >= (int32_t)headers->headers.size()) {
        return nullptr;
    }
    return headers->headers[sceneLayer - 1].get();
}

// Same path as OTRPlay_SpawnScene
static std::string ScenePath(int32_t sceneNum) {
    std::string version = "shared";
    if ((sceneNum >= SCENE_DEKU_TREE && sceneNum <= SCENE_ICE_CAVERN) || sceneNum == SCENE_GERUDO_TRAINING_GROUND ||
        sceneNum == SCENE_INSIDE_GANONS_CASTLE) {
        version = ResourceMgr_IsGameMasterQuest() ? "mq" : "nonmq";
    }
    const char* file = gSceneTable[sceneNum].sceneFile.fileName;
    return "scenes/" + version + "/" + file + "/" + file;
}

static bool IsNight(uint16_t time) {
    return time > 0xC000 || time < 0x4555;
}

// Sun's Song speeds time up to the next of these
static bool IsSunsSongDay(uint16_t time) {
    return time >= 0x4555 && time <= 0xC001;
}

struct CutsceneTimeScript {
    std::vector<CsCmdBase> misc;
    std::vector<CsCmdBase> textboxes;
    std::vector<CsCmdDayTime> setTimes;
    int32_t lastFrame = 0;
};

// Walks the command list the same way as Cutscene_ProcessCommands, keeping only what touches the clock
static CutsceneTimeScript ReadScript(const uint32_t* data) {
    CutsceneTimeScript script;
    int32_t totalEntries = data[0];
    script.lastFrame = data[1];
    const uint32_t* ptr = data + 2;

    for (int32_t i = 0; i < totalEntries; i++) {
        int32_t cmdType = *ptr++;
        if (cmdType == CS_CMD_STOP) {
            break;
        }

        switch (cmdType) {
            case CS_CMD_CAM_EYE:
            case CS_CMD_CAM_EYE_REL_TO_PLAYER:
            case CS_CMD_CAM_AT:
            case CS_CMD_CAM_AT_REL_TO_PLAYER: {
                ptr += 2;
                bool more = true;
                while (more) {
                    more = ((CutsceneCameraPoint*)ptr)->continueFlag != CS_CMD_STOP;
                    ptr += 4;
                }
                break;
            }
            case CS_CMD_07:
            case CS_CMD_08:
                ptr += 6;
                break;
            case CS_CMD_TERMINATOR:
                script.lastFrame = ((CsCmdBase*)(ptr + 1))->startFrame;
                ptr += 3;
                break;
            case CS_CMD_SCENE_TRANS_FX:
                ptr += 3;
                break;
            case CS_CMD_TEXTBOX:
            case CS_CMD_09:
            case CS_CMD_SETTIME: {
                int32_t entries = *ptr++;
                for (int32_t j = 0; j < entries; j++, ptr += 3) {
                    if (cmdType == CS_CMD_TEXTBOX && ((CsCmdBase*)ptr)->base != 0xFFFF) {
                        script.textboxes.push_back(*(CsCmdBase*)ptr);
                    } else if (cmdType == CS_CMD_SETTIME) {
                        script.setTimes.push_back(*(CsCmdDayTime*)ptr);
                    }
                }
                break;
            }
            default: {
                int32_t entries = *ptr++;
                for (int32_t j = 0; j < entries; j++, ptr += 12) {
                    if (cmdType == CS_CMD_MISC) {
                        script.misc.push_back(*(CsCmdBase*)ptr);
                    }
                }
                break;
            }
        }
    }

    return script;
}

uint16_t CutsceneTime_Simulate(int32_t sceneNum, uint16_t cutsceneIndex, uint16_t dayTime, int32_t fadeInFrames,
                               const CutsceneTimeActorUpdate& actorUpdate) {
    int32_t sceneLayer = SCENE_LAYER_CUTSCENE_FIRST + (cutsceneIndex & 0xF);
    SOH::Scene* scene = LoadScene(ScenePath(sceneNum));
    SOH::Scene* layer = scene != nullptr ? LayerHeader(scene, sceneLayer) : nullptr;
    auto cutscene =
        layer != nullptr ? FindCommand<SOH::SetCutscenes>(layer, SOH::SceneCommandID::SetCutscenes) : nullptr;
    auto rooms = FindCommand<SOH::SetRoomList>(scene, SOH::SceneCommandID::SetRoomList);
    if (cutscene == nullptr || rooms == nullptr || rooms->fileNames.empty()) {
        return dayTime;
    }

    // Room time settings, as in Scene_CommandTimeSettings. Assumes the cutscene starts in the first room.
    uint16_t timeSpeed = 0;
    SOH::Scene* room = LoadScene(rooms->fileNames[0]);
    SOH::Scene* roomLayer = room != nullptr ? LayerHeader(room, sceneLayer) : nullptr;
    auto timeSettings = roomLayer != nullptr
                            ? FindCommand<SOH::SetTimeSettings>(roomLayer, SOH::SceneCommandID::SetTimeSettings)
                            : nullptr;
    if (timeSettings != nullptr) {
        auto& settings = timeSettings->settings;
        if (settings.hour != 0xFF && settings.minute != 0xFF) {
            dayTime = (uint16_t)(((settings.hour + (settings.minute / 60.0f)) * 60.0f) / ((f32)(24 * 60) / 0x10000));
        }
        timeSpeed = settings.timeIncrement != 0xFF ? settings.timeIncrement : 0;
    }
    uint16_t roomTimeSpeed = timeSpeed;

    CutsceneTimeScript script = ReadScript(cutscene->cutscene->commands.data());
    bool night = IsNight(dayTime);
    bool sunsSongStarted = false;
    bool sunsSongSpeeding = false;
    bool sunsSongToDusk = false;
    uint16_t sunsSongPrevSpeed = 0;

    for (int32_t frame = 1; frame <= script.lastFrame; frame++) {
        if (actorUpdate) {
            actorUpdate(dayTime, timeSpeed);
        }

        // func_80064824, only the cases that touch the clock
        for (auto& cmd : script.misc) {
            if (frame < cmd.startFrame || (frame >= cmd.endFrame && cmd.endFrame != cmd.startFrame)) {
                continue;
            }
            switch (cmd.base) {
                case 18:
                    if (dayTime < 0x4AAB) {
                        dayTime += 30;
                    }
                    break;
                case 25:
                    dayTime += 30;
                    if (dayTime > 0xCAAA) {
                        dayTime = 0xCAAA;
                    }
                    break;
                case 33:
                    sunsSongStarted = true;
                    break;
                case 34:
                    dayTime -= night ? timeSpeed * 2 : timeSpeed;
                    break;
            }
        }

        // func_80065134
        for (auto& cmd : script.setTimes) {
            if (frame == cmd.startFrame) {
                s16 hours = (cmd.hour * 60.0f) / (360.0f / 0x4000);
                s16 minutes = (cmd.minute + 1) / (360.0f / 0x4000);
                dayTime = hours + minutes;
            }
        }

        // Sun's Song handling in Interface_Update
        if (sunsSongStarted && roomTimeSpeed != 0) {
            if (!sunsSongSpeeding) {
                sunsSongToDusk = IsSunsSongDay(dayTime);
                sunsSongSpeeding = true;
                sunsSongPrevSpeed = timeSpeed;
                timeSpeed = 400;
            } else if (sunsSongToDusk ? dayTime > 0xC001 : IsSunsSongDay(dayTime)) {
                sunsSongStarted = sunsSongSpeeding = false;
                timeSpeed = sunsSongPrevSpeed;
            }
        }

        // Environment_Update pauses time during the fade in and while a textbox is up.
        // A textbox is up after its start frame, and the cutscene stalls on its end frame until it closes.
        bool paused = frame <= fadeInFrames;
        for (auto& cmd : script.textboxes) {
            paused |= frame > cmd.startFrame && frame < cmd.endFrame;
        }
        if (!paused) {
            dayTime += night && timeSpeed < 400 ? timeSpeed * 2 : timeSpeed;
        }
        night = IsNight(dayTime);
    }

    return dayTime;
}
