#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/util.h"

extern "C" {
#include "z64.h"
#include "global.h"
#include "soh/Enhancements/enhancementTypes.h"
void Sram_InitDebugSave(void);
void Select_LoadGame(SelectContext* selectContext, s32 entranceIndex);
}

struct InputPlayback {
    bool isRecording = false;
    bool isPlaying = false;
    size_t playbackFrameIndex = 0;
    std::vector<uint64_t> frames;
};

std::map<std::string, InputPlayback> inputPlaybacks;

void SaveInputPlaybacks() {
    auto inputPlaybacksJson = nlohmann::json::object();
    for (auto& [playbackName, playback] : inputPlaybacks) {
        inputPlaybacksJson[playbackName] = playback.frames;
    }
    Ship::Context::GetInstance()->GetConfig()->SetBlock("InputPlaybacks", inputPlaybacksJson);
    Ship::Context::GetInstance()->GetConfig()->Save();
}

void LoadInputPlaybacks() {
    auto allConfig = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();

    // Verify that the config has InputPlaybacks and its an object
    if (allConfig.find("InputPlaybacks") != allConfig.end() && allConfig["InputPlaybacks"].is_object()) {
        inputPlaybacks.clear();
        auto& inputPlaybacksJson = allConfig["InputPlaybacks"];
        for (auto& [playbackName, framesJson] : inputPlaybacksJson.items()) {
            std::vector<uint64_t> frames = framesJson.get<std::vector<uint64_t>>();
            inputPlaybacks[playbackName] = { false, false, 0, frames };
        }
    }
}

static std::string inputPlaybackNameInput = "";

void InputPlaybacksWidget(WidgetInfo& info) {
    ImGui::PushID("InputPlaybacks");
    ImGui::SeparatorText("Input Playbacks");
    UIWidgets::InputString("##InputPlaybacksNameInput", &inputPlaybackNameInput,
                           {
                               .size = ImVec2(ImGui::GetContentRegionAvail().x - 50.0f, 0.0f),
                               .placeholder = "Enter name...",
                           });

    ImGui::SameLine();
    bool isEmpty = inputPlaybackNameInput.empty();
    if (isEmpty) {
        ImGui::BeginDisabled();
    }

    if (UIWidgets::Button(ICON_FA_PLUS)) {
        inputPlaybacks[inputPlaybackNameInput] = { false, false, 0, std::vector<uint64_t>() };
        SaveInputPlaybacks();
        inputPlaybackNameInput = "";
    }
    if (isEmpty) {
        ImGui::EndDisabled();
    }

    // List of warp points, showing just their name, a button to record, play, and delete
    for (auto it = inputPlaybacks.begin(); it != inputPlaybacks.end();) {
        auto& [name, playback] = *it;
        ImGui::PushID(name.c_str());

        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", name.c_str());

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 115.0f);

        if (playback.isRecording)
            ImGui::BeginDisabled();
        if (playback.isPlaying) {
            if (UIWidgets::Button(ICON_FA_STOP,
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green })) {
                playback.isPlaying = false;
            }
        } else {
            if (UIWidgets::Button(ICON_FA_PLAY,
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green })) {
                playback.isPlaying = true;
                playback.playbackFrameIndex = 0;
            }
        }
        if (playback.isRecording)
            ImGui::EndDisabled();

        ImGui::SameLine();

        if (playback.isPlaying)
            ImGui::BeginDisabled();
        if (playback.isRecording) {
            if (UIWidgets::Button(ICON_FA_MICROPHONE_SLASH,
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Orange })) {
                playback.isRecording = false;
                // Trim 0 frames from beginning and end of the recording
                std::vector<uint64_t> frames;
                size_t emptyFramesInARow = 0;
                for (auto& frame : playback.frames) {
                    if (frame == 0) {
                        // Don't even store empty frames till we see a non-empty frame
                        if (frames.empty()) {
                            continue;
                        }
                        emptyFramesInARow++;
                    } else {
                        emptyFramesInARow = 0;
                    }
                    frames.push_back(frame);
                }
                frames.resize(frames.size() - emptyFramesInARow);
                playback.frames = frames;

                SaveInputPlaybacks();
            }
        } else {
            if (UIWidgets::Button(ICON_FA_MICROPHONE,
                                  { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Orange })) {
                playback.isRecording = true;
                playback.frames.clear();
            }
        }
        if (playback.isPlaying)
            ImGui::EndDisabled();

        ImGui::SameLine();

        if (playback.isRecording || playback.isPlaying)
            ImGui::BeginDisabled();
        if (UIWidgets::Button(ICON_FA_TRASH, { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red })) {
            it = inputPlaybacks.erase(it);
            SaveInputPlaybacks();
            ImGui::PopID();
            continue;
            ;
        }
        if (playback.isRecording || playback.isPlaying)
            ImGui::EndDisabled();

        ImGui::PopID();

        ++it;
    }
    ImGui::PopID();
}

void RegisterInputPlaybacks() {
    static bool loadedConfig = false;
    if (!loadedConfig) {
        LoadInputPlaybacks();
        loadedConfig = true;
    }

    COND_HOOK(OnGameStateMainStart, true, []() {
        for (auto& [name, playback] : inputPlaybacks) {
            if (playback.isPlaying) {
                // Apply input for current frame
                if (playback.playbackFrameIndex < playback.frames.size()) {
                    uint64_t value = playback.frames[playback.playbackFrameIndex];
                    uint32_t curValue = (uint32_t)(value & 0xFFFFFFFF);
                    uint32_t pressValue = (uint32_t)((value >> 32) & 0xFFFFFFFF);

                    SPDLOG_INFO("Playing input - cur: {:08x}, press: {:08x}", curValue, pressValue);

                    gGameState->input[0].cur.button |= curValue & 0xFFFF;
                    gGameState->input[0].cur.stick_x += (int8_t)((curValue >> 16) & 0xFF);
                    gGameState->input[0].cur.stick_y += (int8_t)((curValue >> 24) & 0xFF);
                    gGameState->input[0].press.button |= pressValue & 0xFFFF;
                    gGameState->input[0].rel.stick_x += (int8_t)((pressValue >> 16) & 0xFF);
                    gGameState->input[0].rel.stick_y += (int8_t)((pressValue >> 24) & 0xFF);

                    playback.playbackFrameIndex++;
                } else {
                    // Stop playback when we reach the end
                    playback.isPlaying = false;
                }
            }

            if (playback.isRecording) {
                // Capture current input
                uint64_t currentInput = 0;
                uint32_t curValue = (uint32_t)(gGameState->input[0].cur.button & 0xFFFF) |
                                    ((uint32_t)(uint8_t)gGameState->input[0].cur.stick_x << 16) |
                                    ((uint32_t)(uint8_t)gGameState->input[0].cur.stick_y << 24);
                uint32_t pressValue = (uint32_t)(gGameState->input[0].press.button & 0xFFFF) |
                                      ((uint32_t)(uint8_t)gGameState->input[0].rel.stick_x << 16) |
                                      ((uint32_t)(uint8_t)gGameState->input[0].rel.stick_y << 24);

                SPDLOG_INFO("Recording input - cur: {:08x}, press: {:08x}", curValue, pressValue);
                currentInput = curValue | ((uint64_t)pressValue << 32);

                playback.frames.push_back(currentInput);
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterInputPlaybacks, {});
