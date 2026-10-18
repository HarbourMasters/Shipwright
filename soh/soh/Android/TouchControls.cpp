#include "soh/Android/TouchControls.h"

#ifdef __ANDROID__

#include <SDL2/SDL.h>
#include <imgui.h>
#include <ship/Context.h>
#include <ship/controller/controldeck/ControlDeck.h>
#include <ship/controller/controldevice/controller/mapping/keyboard/KeyboardScancodes.h>
#include <ship/window/Window.h>
#include <ship/window/gui/Gui.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <limits>
#include <map>
#include <mutex>
#include <set>
#include <utility>
#include <vector>

namespace SohAndroid::TouchControls {
namespace {
enum class Control {
    None,
    Menu,
    Toggle,
    Analog,
    A,
    B,
    Z,
    Start,
    L,
    R,
    CUp,
    CDown,
    CLeft,
    CRight,
    DUp,
    DDown,
    DLeft,
    DRight,
};

using TouchId = std::pair<SDL_TouchID, SDL_FingerID>;

struct FingerState {
    float x = 0.0f;
    float y = 0.0f;
    float startX = 0.0f;
    float startY = 0.0f;
    bool down = false;
    bool justPressed = false;
};

struct ButtonVisual {
    Control control;
    const char* label;
    ImVec2 center;
    float radius;
    ImU32 color;
};

struct Layout {
    ImVec2 size;
    ImVec2 analogCenter;
    float analogRadius;
    std::array<ButtonVisual, 16> buttons;
};

std::mutex gTouchMutex;
std::map<TouchId, FingerState> gTouches;
std::map<TouchId, Control> gAssignments;
std::set<Ship::KbScancode> gPressedKeys;
std::atomic<bool> gMenuToggleRequested = false;
bool gInitialized = false;
bool gControlsVisible = true;
bool gEscapeReleasePending = false;
bool gResetTouchesPending = false;

int SDLCALL OnSdlEvent(void*, SDL_Event* event) {
    if (event == nullptr) {
        return 1;
    }

    std::lock_guard<std::mutex> lock(gTouchMutex);
    switch (event->type) {
        case SDL_FINGERDOWN: {
            const TouchId id{ event->tfinger.touchId, event->tfinger.fingerId };
            FingerState& finger = gTouches[id];
            finger.x = finger.startX = event->tfinger.x;
            finger.y = finger.startY = event->tfinger.y;
            finger.down = true;
            finger.justPressed = true;
            break;
        }
        case SDL_FINGERMOTION: {
            const TouchId id{ event->tfinger.touchId, event->tfinger.fingerId };
            FingerState& finger = gTouches[id];
            if (!finger.down && !finger.justPressed) {
                finger.startX = event->tfinger.x;
                finger.startY = event->tfinger.y;
                finger.justPressed = true;
            }
            finger.x = event->tfinger.x;
            finger.y = event->tfinger.y;
            finger.down = true;
            break;
        }
        case SDL_FINGERUP: {
            const TouchId id{ event->tfinger.touchId, event->tfinger.fingerId };
            auto finger = gTouches.find(id);
            if (finger != gTouches.end()) {
                finger->second.x = event->tfinger.x;
                finger->second.y = event->tfinger.y;
                finger->second.down = false;
            }
            break;
        }
        case SDL_WINDOWEVENT:
            if (event->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                gTouches.clear();
                gResetTouchesPending = true;
            }
            break;
        case SDL_APP_WILLENTERBACKGROUND:
        case SDL_APP_TERMINATING:
            gTouches.clear();
            gResetTouchesPending = true;
            break;
        default:
            break;
    }
    return 1;
}

Layout MakeLayout(ImVec2 size) {
    const float shortSide = std::max(1.0f, std::min(size.x, size.y));
    const float buttonRadius = std::clamp(shortSide * 0.055f, 24.0f, 58.0f);
    const float smallRadius = std::clamp(shortSide * 0.038f, 18.0f, 34.0f);
    const ImVec2 analogCenter(size.x * 0.17f, size.y * 0.78f);
    const float analogRadius = std::clamp(shortSide * 0.15f, 60.0f, 142.0f);
    const float dpadOffset = buttonRadius * 1.8f;
    const float cOffset = buttonRadius * 1.82f;
    const float cX = size.x * 0.86f;
    const float cY = size.y * 0.42f;
    const ImU32 neutral = IM_COL32(14, 23, 37, 145);

    return {
        size,
        analogCenter,
        analogRadius,
        { ButtonVisual{ Control::Menu, "M", ImVec2(size.x * 0.055f, size.y * 0.08f), smallRadius, neutral },
          ButtonVisual{ Control::Toggle, "T", ImVec2(size.x * 0.135f, size.y * 0.08f), smallRadius, neutral },
          ButtonVisual{ Control::L, "L", ImVec2(size.x * 0.075f, size.y * 0.20f), buttonRadius * 0.86f, neutral },
          ButtonVisual{ Control::R, "R", ImVec2(size.x * 0.925f, size.y * 0.20f), buttonRadius * 0.86f, neutral },
          ButtonVisual{ Control::A, "A", ImVec2(size.x * 0.86f, size.y * 0.77f), buttonRadius * 1.16f,
                        IM_COL32(48, 155, 90, 170) },
          ButtonVisual{ Control::B, "B", ImVec2(size.x * 0.72f, size.y * 0.87f), buttonRadius,
                        IM_COL32(59, 101, 184, 170) },
          ButtonVisual{ Control::Z, "Z", ImVec2(size.x * 0.59f, size.y * 0.67f), buttonRadius * 0.92f,
                        IM_COL32(154, 83, 71, 170) },
          ButtonVisual{ Control::Start, "START", ImVec2(size.x * 0.49f, size.y * 0.92f), buttonRadius * 0.78f,
                        IM_COL32(104, 111, 126, 175) },
          ButtonVisual{ Control::CUp, "C^", ImVec2(cX, cY - cOffset), buttonRadius * 0.75f,
                        IM_COL32(166, 143, 54, 175) },
          ButtonVisual{ Control::CDown, "Cv", ImVec2(cX, cY + cOffset), buttonRadius * 0.75f,
                        IM_COL32(166, 143, 54, 175) },
          ButtonVisual{ Control::CLeft, "C<", ImVec2(cX - cOffset, cY), buttonRadius * 0.75f,
                        IM_COL32(166, 143, 54, 175) },
          ButtonVisual{ Control::CRight, "C>", ImVec2(cX + cOffset, cY), buttonRadius * 0.75f,
                        IM_COL32(166, 143, 54, 175) },
          ButtonVisual{ Control::DUp, "^", ImVec2(size.x * 0.16f, size.y * 0.42f - dpadOffset),
                        buttonRadius * 0.72f, neutral },
          ButtonVisual{ Control::DDown, "v", ImVec2(size.x * 0.16f, size.y * 0.42f + dpadOffset),
                        buttonRadius * 0.72f, neutral },
          ButtonVisual{ Control::DLeft, "<", ImVec2(size.x * 0.16f - dpadOffset, size.y * 0.42f),
                        buttonRadius * 0.72f, neutral },
          ButtonVisual{ Control::DRight, ">", ImVec2(size.x * 0.16f + dpadOffset, size.y * 0.42f),
                        buttonRadius * 0.72f, neutral } }
    };
}

float DistanceSquared(ImVec2 left, ImVec2 right) {
    const float x = left.x - right.x;
    const float y = left.y - right.y;
    return x * x + y * y;
}

Control HitTest(ImVec2 position, const Layout& layout, bool controlsVisible) {
    // The two small utility controls remain available when the gamepad overlay is hidden.
    for (const ButtonVisual& button : layout.buttons) {
        if (button.control != Control::Menu && button.control != Control::Toggle) {
            continue;
        }
        const float radius = button.radius * 1.45f;
        if (DistanceSquared(position, button.center) <= radius * radius) {
            return button.control;
        }
    }

    if (!controlsVisible) {
        return Control::None;
    }

    // Prefer the nearest visible button when hit regions overlap slightly.
    Control nearest = Control::None;
    float nearestDistance = std::numeric_limits<float>::max();
    for (const ButtonVisual& button : layout.buttons) {
        if (button.control == Control::Menu || button.control == Control::Toggle) {
            continue;
        }
        const float radius = button.radius * 1.25f;
        const float distance = DistanceSquared(position, button.center);
        if (distance <= radius * radius && distance < nearestDistance) {
            nearest = button.control;
            nearestDistance = distance;
        }
    }
    if (nearest != Control::None) {
        return nearest;
    }

    const float analogHitRadius = layout.analogRadius * 1.42f;
    return DistanceSquared(position, layout.analogCenter) <= analogHitRadius * analogHitRadius ? Control::Analog
                                                                                              : Control::None;
}

void AddAnalogKeys(std::set<Ship::KbScancode>& keys, ImVec2 position, const Layout& layout) {
    const float dx = position.x - layout.analogCenter.x;
    const float dy = position.y - layout.analogCenter.y;
    const float deadZone = layout.analogRadius * 0.27f;
    if (dx < -deadZone) {
        keys.insert(Ship::LUS_KB_A);
    } else if (dx > deadZone) {
        keys.insert(Ship::LUS_KB_D);
    }
    if (dy < -deadZone) {
        keys.insert(Ship::LUS_KB_W);
    } else if (dy > deadZone) {
        keys.insert(Ship::LUS_KB_S);
    }
}

void AddMappedKey(std::set<Ship::KbScancode>& keys, Control control, ImVec2 position, const Layout& layout) {
    switch (control) {
        case Control::Analog:
            AddAnalogKeys(keys, position, layout);
            break;
        case Control::A:
            keys.insert(Ship::LUS_KB_X);
            break;
        case Control::B:
            keys.insert(Ship::LUS_KB_C);
            break;
        case Control::Z:
            keys.insert(Ship::LUS_KB_Z);
            break;
        case Control::Start:
            keys.insert(Ship::LUS_KB_SPACE);
            break;
        case Control::L:
            keys.insert(Ship::LUS_KB_E);
            break;
        case Control::R:
            keys.insert(Ship::LUS_KB_R);
            break;
        case Control::CUp:
            keys.insert(Ship::LUS_KB_ARROWKEY_UP);
            break;
        case Control::CDown:
            keys.insert(Ship::LUS_KB_ARROWKEY_DOWN);
            break;
        case Control::CLeft:
            keys.insert(Ship::LUS_KB_ARROWKEY_LEFT);
            break;
        case Control::CRight:
            keys.insert(Ship::LUS_KB_ARROWKEY_RIGHT);
            break;
        case Control::DUp:
            keys.insert(Ship::LUS_KB_T);
            break;
        case Control::DDown:
            keys.insert(Ship::LUS_KB_G);
            break;
        case Control::DLeft:
            keys.insert(Ship::LUS_KB_F);
            break;
        case Control::DRight:
            keys.insert(Ship::LUS_KB_H);
            break;
        default:
            break;
    }
}

void QueueEscapeEvent(bool pressed) {
    SDL_Event event{};
    event.type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = pressed ? SDL_PRESSED : SDL_RELEASED;
    event.key.repeat = 0;
    event.key.keysym.scancode = SDL_SCANCODE_ESCAPE;
    event.key.keysym.sym = SDLK_ESCAPE;
    SDL_PushEvent(&event);
}

void DrawButton(ImDrawList* drawList, const ButtonVisual& button, bool pressed, bool controlsVisible) {
    const ImU32 fill = pressed ? IM_COL32(70, 157, 232, 215) : button.color;
    drawList->AddCircleFilled(button.center, button.radius, fill, 32);
    drawList->AddCircle(button.center, button.radius, IM_COL32(255, 255, 255, pressed ? 235 : 130), 32, 2.0f);

    const ImVec2 labelSize = ImGui::CalcTextSize(button.label);
    const ImVec2 labelPosition(button.center.x - labelSize.x * 0.5f, button.center.y - labelSize.y * 0.5f);
    drawList->AddText(ImVec2(labelPosition.x + 1.0f, labelPosition.y + 1.0f), IM_COL32(0, 0, 0, 220), button.label);
    drawList->AddText(labelPosition, IM_COL32(255, 255, 255, 245), button.label);

    if (button.control == Control::Menu || button.control == Control::Toggle) {
        const char* caption = button.control == Control::Menu ? "MENU" : (controlsVisible ? "TOUCH ON" : "TOUCH OFF");
        const ImVec2 captionPosition(button.center.x - button.radius * 0.35f, button.center.y + button.radius + 2.0f);
        drawList->AddText(captionPosition, IM_COL32(255, 255, 255, 205), caption);
    }
}
} // namespace

void Initialize() {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
    SDL_DelEventWatch(OnSdlEvent, nullptr);
    SDL_AddEventWatch(OnSdlEvent, nullptr);
    {
        std::lock_guard<std::mutex> lock(gTouchMutex);
        gTouches.clear();
        gResetTouchesPending = false;
    }
    gAssignments.clear();
    gPressedKeys.clear();
    gEscapeReleasePending = false;
    gMenuToggleRequested.store(false, std::memory_order_relaxed);
    gControlsVisible = true;
    gInitialized = true;
}

void RequestMenuToggle() {
    gMenuToggleRequested.store(true, std::memory_order_relaxed);
}

void Draw() {
    if (!gInitialized || ImGui::GetCurrentContext() == nullptr) {
        return;
    }

    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    if (displaySize.x <= 0.0f || displaySize.y <= 0.0f) {
        return;
    }
    const Layout layout = MakeLayout(displaySize);

    std::vector<std::pair<TouchId, FingerState>> fingers;
    bool resetTouches = false;
    {
        std::lock_guard<std::mutex> lock(gTouchMutex);
        resetTouches = gResetTouchesPending;
        gResetTouchesPending = false;
        for (auto finger = gTouches.begin(); finger != gTouches.end();) {
            fingers.emplace_back(finger->first, finger->second);
            if (finger->second.down) {
                finger->second.justPressed = false;
                ++finger;
            } else {
                finger = gTouches.erase(finger);
            }
        }
    }
    if (resetTouches) {
        gAssignments.clear();
    }

    auto context = Ship::Context::GetRawInstance();
    if (context == nullptr || context->GetWindow() == nullptr || context->GetWindow()->GetGui() == nullptr) {
        return;
    }
    const auto gui = context->GetWindow()->GetGui();
    const bool popupOpen = ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
    bool uiOpen = gui->GetMenuOrMenubarVisible() || popupOpen;
    if (uiOpen) {
        // Let SDL's touch-to-mouse events navigate ImGui without assigning those fingers to game controls.
        gAssignments.clear();
        fingers.clear();
    }

    std::set<Ship::KbScancode> desiredKeys;
    std::set<Control> activeControls;
    bool analogActive = false;
    ImVec2 analogPosition = layout.analogCenter;

    // Keep a synthesized Escape press across frames when a popup needs dismissing.
    if (gEscapeReleasePending) {
        QueueEscapeEvent(false);
        gEscapeReleasePending = false;
    }
    bool toggleMenu = gMenuToggleRequested.exchange(false, std::memory_order_relaxed);

    for (const auto& [id, finger] : fingers) {
        auto assignment = gAssignments.find(id);
        if (assignment == gAssignments.end() && (finger.down || finger.justPressed)) {
            const ImVec2 startPosition(finger.startX * displaySize.x, finger.startY * displaySize.y);
            const Control control = HitTest(startPosition, layout, gControlsVisible);
            if (control != Control::None) {
                assignment = gAssignments.emplace(id, control).first;
            }
        }
        if (assignment == gAssignments.end()) {
            continue;
        }

        const Control control = assignment->second;
        if (finger.justPressed && control == Control::Menu) {
            toggleMenu = true;
        } else if (finger.justPressed && control == Control::Toggle) {
            gControlsVisible = !gControlsVisible;
        }

        const ImVec2 position(finger.x * displaySize.x, finger.y * displaySize.y);
        const bool visibleControl = gControlsVisible || control == Control::Menu || control == Control::Toggle;
        if (!uiOpen && visibleControl && (finger.down || finger.justPressed)) {
            AddMappedKey(desiredKeys, control, finger.down ? position : ImVec2(finger.startX * displaySize.x,
                                                                                finger.startY * displaySize.y),
                         layout);
            activeControls.insert(control);
            if (control == Control::Analog) {
                analogActive = true;
                analogPosition = finger.down ? position : ImVec2(finger.startX * displaySize.x,
                                                                 finger.startY * displaySize.y);
            }
        }

        if (!finger.down) {
            gAssignments.erase(assignment);
        }
    }

    if (toggleMenu) {
        if (popupOpen) {
            // Let Escape dismiss modal popups before changing the underlying menu state.
            QueueEscapeEvent(true);
            gEscapeReleasePending = true;
        } else if (const auto menu = gui->GetMenu(); menu != nullptr) {
            // Toggle directly on the render thread instead of routing this touch through a
            // synthetic SDL key event before it reaches the ImGui menu handler.
            menu->ToggleVisibility();
            context->GetWindow()->GetMouseStateManager()->UpdateMouseCapture();
        } else {
            QueueEscapeEvent(true);
            gEscapeReleasePending = true;
        }
        uiOpen = gui->GetMenuOrMenubarVisible() ||
                 ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
    }

    if (uiOpen) {
        desiredKeys.clear();
        activeControls.clear();
        analogActive = false;
    }

    const auto controlDeck = context->GetControlDeck();
    if (controlDeck != nullptr) {
        for (const Ship::KbScancode key : gPressedKeys) {
            if (!desiredKeys.contains(key)) {
                controlDeck->ProcessKeyboardEvent(Ship::LUS_KB_EVENT_KEY_UP, key);
            }
        }
        for (const Ship::KbScancode key : desiredKeys) {
            if (!gPressedKeys.contains(key)) {
                controlDeck->ProcessKeyboardEvent(Ship::LUS_KB_EVENT_KEY_DOWN, key);
            }
        }
    }
    gPressedKeys = desiredKeys;

    if (uiOpen) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    if (drawList == nullptr) {
        return;
    }

    for (const ButtonVisual& button : layout.buttons) {
        const bool isUtilityButton = button.control == Control::Menu || button.control == Control::Toggle;
        if (!gControlsVisible && !isUtilityButton) {
            continue;
        }
        DrawButton(drawList, button, activeControls.contains(button.control), gControlsVisible);
    }

    if (gControlsVisible) {
        const ImU32 baseColor = IM_COL32(14, 23, 37, 125);
        drawList->AddCircleFilled(layout.analogCenter, layout.analogRadius, baseColor, 48);
        drawList->AddCircle(layout.analogCenter, layout.analogRadius, IM_COL32(255, 255, 255, 115), 48, 2.0f);

        ImVec2 knobPosition = layout.analogCenter;
        if (analogActive) {
            float dx = analogPosition.x - layout.analogCenter.x;
            float dy = analogPosition.y - layout.analogCenter.y;
            const float distance = std::sqrt(dx * dx + dy * dy);
            const float maxDistance = layout.analogRadius * 0.78f;
            if (distance > maxDistance && distance > 0.0f) {
                dx *= maxDistance / distance;
                dy *= maxDistance / distance;
            }
            knobPosition = ImVec2(layout.analogCenter.x + dx, layout.analogCenter.y + dy);
        }
        drawList->AddCircleFilled(knobPosition, layout.analogRadius * 0.39f,
                                  analogActive ? IM_COL32(70, 157, 232, 220) : IM_COL32(235, 240, 248, 145), 36);
        drawList->AddCircle(knobPosition, layout.analogRadius * 0.39f, IM_COL32(255, 255, 255, 185), 36, 2.0f);
    }
}
} // namespace SohAndroid::TouchControls

namespace SohAndroid {
TouchControlsWindow::TouchControlsWindow() : Ship::GuiWindow("", true, "AndroidTouchControls") {
}

void TouchControlsWindow::Draw() {
    if (IsVisible()) {
        TouchControls::Draw();
    }
}

void TouchControlsWindow::InitElement() {
}

void TouchControlsWindow::UpdateElement() {
}

void TouchControlsWindow::DrawElement() {
}
} // namespace SohAndroid

#endif // __ANDROID__
