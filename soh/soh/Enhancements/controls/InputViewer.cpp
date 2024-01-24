#include "InputViewer.h"

#include "public/bridge/consolevariablebridge.h"
#include "libultraship/libultra/controller.h"
#include "Context.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include <spdlog/spdlog.h>

#define BindButton(btn, status)                                                          \
    ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(btn),   \
                 ImVec2(16.0f * scale, 16.0f * scale), ImVec2(0, 0), ImVec2(1.0f, 1.0f), \
                 ImVec4(255, 255, 255, (status) ? 255 : 0));

InputViewer::~InputViewer() {
    SPDLOG_TRACE("destruct input viewer");
}

void InputViewer::DrawElement() {
    static bool sButtonTexturesLoaded = false;
    if (!sButtonTexturesLoaded) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("A-Btn", "textures/buttons/ABtn.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("B-Btn", "textures/buttons/BBtn.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("L-Btn", "textures/buttons/LBtn.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("R-Btn", "textures/buttons/RBtn.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("Z-Btn", "textures/buttons/ZBtn.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("Start-Btn", "textures/buttons/StartBtn.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("C-Left", "textures/buttons/CLeft.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("C-Right", "textures/buttons/CRight.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("C-Up", "textures/buttons/CUp.png");
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("C-Down", "textures/buttons/CDown.png");
        sButtonTexturesLoaded = true;
    }

    ImVec2 mainPos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetContentRegionAvail();

#ifdef __WIIU__
    const float scale = CVarGetFloat("gInputScale", 1.0f) * 2.0f;
#else
    const float scale = CVarGetFloat("gInputScale", 1.0f);
#endif
    ImVec2 btnPos = ImVec2(160 * scale, 85 * scale);

    ImGui::SetNextWindowSize(btnPos);
    ImGui::SetNextWindowPos(ImVec2(mainPos.x + size.x - btnPos.x - 20, mainPos.y + size.y - btnPos.y - 20));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

    OSContPad* pads = LUS::Context::GetInstance()->GetControlDeck()->GetPads();

    if (pads != nullptr && ImGui::Begin("Game Buttons", nullptr,
                                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground)) {
        ImGui::SetCursorPosY(32 * scale);

        ImGui::BeginGroup();
        const ImVec2 cPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cPos.x + 10 * scale, cPos.y - 20 * scale));
        BindButton("L-Btn", pads[0].button & BTN_L);
        ImGui::SetCursorPos(ImVec2(cPos.x + 16 * scale, cPos.y));
        BindButton("C-Up", pads[0].button & BTN_CUP);
        ImGui::SetCursorPos(ImVec2(cPos.x, cPos.y + 16 * scale));
        BindButton("C-Left", pads[0].button & BTN_CLEFT);
        ImGui::SetCursorPos(ImVec2(cPos.x + 32 * scale, cPos.y + 16 * scale));
        BindButton("C-Right", pads[0].button & BTN_CRIGHT);
        ImGui::SetCursorPos(ImVec2(cPos.x + 16 * scale, cPos.y + 32 * scale));
        BindButton("C-Down", pads[0].button & BTN_CDOWN);
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        const ImVec2 sPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(sPos.x + 21, sPos.y - 20 * scale));
        BindButton("Z-Btn", pads[0].button & BTN_Z);
        ImGui::SetCursorPos(ImVec2(sPos.x + 22, sPos.y + 16 * scale));
        BindButton("Start-Btn", pads[0].button & BTN_START);
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        const ImVec2 bPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(bPos.x + 20 * scale, bPos.y - 20 * scale));
        BindButton("R-Btn", pads[0].button & BTN_R);
        ImGui::SetCursorPos(ImVec2(bPos.x + 12 * scale, bPos.y + 8 * scale));
        BindButton("B-Btn", pads[0].button & BTN_B);
        ImGui::SetCursorPos(ImVec2(bPos.x + 28 * scale, bPos.y + 24 * scale));
        BindButton("A-Btn", pads[0].button & BTN_A);
        ImGui::EndGroup();

        ImGui::End();
    }

    ImGui::PopStyleColor();
}
