#include "ChaosWindow.h"
#include "../../UIWidgets.hpp"
#include "../../util.h"
#include "../../OTRGlobals.h"

#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

uint32_t frameCounter = 2000;
std::vector<eventObject> activeEvents = {};


std::vector<eventObject> eventList = {
    { EVENT_INVISIBILITY, "Invisibility", 150 },
    { EVENT_STORMY_WEATHER, "Stormy Weather", 200 },
};

void ChaosEventsManager(uint32_t eventAction, uint32_t eventId) {
    switch (eventAction) {
        case EVENT_ACTION_REMOVE:
            for (int i = 0; i < activeEvents.size(); i++) {
                if (activeEvents[i].eventId == eventId) {
                    activeEvents.erase(activeEvents.begin() + i);
                    break;
                }
            }
            break;
        case EVENT_ACTION_ADD:
            for (auto listCheck : eventList) {
                if (listCheck.eventId == eventId) {
                    activeEvents.push_back(listCheck);
                    break;
                }
            }
            break;
        default:
            break;
    }
}

void ChaosEventsActivator(uint32_t eventId, bool isActive) {
    switch (eventId) {
        case EVENT_INVISIBILITY:
            GameInteractor::RawAction::SetLinkInvisibility(isActive);
            break;
        case EVENT_STORMY_WEATHER:
            GameInteractor::RawAction::SetWeatherStorm(isActive);
            break;
        default:
            break;
    }
}

void ChaosTrackerTimer() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState || gPlayState->pauseCtx.state > 0) {
            return;
        }
        switch (frameCounter) {
            case 1900:
                ChaosEventsManager(EVENT_ACTION_ADD, EVENT_INVISIBILITY);
                ChaosEventsActivator(EVENT_INVISIBILITY, true);
                break;
            case 1800:
                ChaosEventsManager(EVENT_ACTION_ADD, EVENT_STORMY_WEATHER);
                ChaosEventsActivator(EVENT_STORMY_WEATHER, true);
                break;
            case 1500:
                ChaosEventsManager(EVENT_ACTION_ADD, EVENT_INVISIBILITY);
                ChaosEventsActivator(EVENT_INVISIBILITY, true);
                break;
            default:
                break;
        }

        if (activeEvents.size() > 0) {
            for (auto& counter : activeEvents) {
                if (counter.eventTimer > 0) {
                    counter.eventTimer--;
                    
                }
                if (counter.eventTimer == 0) {
                    ChaosEventsActivator(counter.eventId, false);
                    ChaosEventsManager(EVENT_ACTION_REMOVE, counter.eventId);
                    
                }
            }
        }
        if (frameCounter > 0) {
            frameCounter--;
        }
    });
}

void DrawChaosTrackerEvents() {
    if (activeEvents.size() > 0) {
        for (auto obj : activeEvents) {
            ImGui::Text(obj.eventName);
            ImGui::SameLine();
            ImGui::Text(std::to_string(obj.eventTimer).c_str());
        }
    }
}

void ChaosWindow::DrawElement() {
    std::string counter = std::to_string(frameCounter).c_str();
    std::string frameText = "Frame Counter: " + counter;
    ImGui::Text(frameText.c_str());

    ImGui::BeginChild("Chaos Events");
    DrawChaosTrackerEvents();
    ImGui::EndChild();
}

void ChaosWindow::InitElement() {
    ChaosTrackerTimer();
}
