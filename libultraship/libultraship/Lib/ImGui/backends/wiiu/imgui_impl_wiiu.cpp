// dear imgui: Platform Backend for the Wii U
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_wiiu.h"
#include <stdlib.h> // malloc/free

// Software keyboard
#include <nn/swkbd.h>

// Wii U Data
struct ImGui_ImplWiiU_Data
{
    nn::swkbd::CreateArg CreateArg;
    nn::swkbd::AppearArg AppearArg;
    nn::swkbd::ControllerType LastController;

    bool WantedTextInput;
    bool WasTouched;

    ImGui_ImplWiiU_Data()   { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData
static ImGui_ImplWiiU_Data* ImGui_ImplWiiU_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplWiiU_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

bool     ImGui_ImplWiiU_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    // Setup backend data
    ImGui_ImplWiiU_Data* bd = IM_NEW(ImGui_ImplWiiU_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_wiiu";
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

    // Initialize and create software keyboard
    nn::swkbd::CreateArg createArg;

    createArg.workMemory = malloc(nn::swkbd::GetWorkMemorySize(0));
    createArg.fsClient = (FSClient*) malloc(sizeof(FSClient));
    if (!createArg.workMemory || !createArg.fsClient)
    {
        free(createArg.workMemory);
        free(createArg.fsClient);
        return false;
    }

    FSAddClient(createArg.fsClient, FS_ERROR_FLAG_NONE);

    if (!nn::swkbd::Create(createArg))
        return false;

    nn::swkbd::AppearArg appearArg;
    bd->CreateArg = createArg;
    bd->AppearArg = appearArg;

    return true;
}

void     ImGui_ImplWiiU_Shutdown()
{
    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    // Destroy software keyboard
    nn::swkbd::Destroy();
    free(bd->CreateArg.workMemory);
    bd->CreateArg.workMemory = NULL;

    if (bd->CreateArg.fsClient)
    {
        FSDelClient(bd->CreateArg.fsClient, FS_ERROR_FLAG_NONE);
        free(bd->CreateArg.fsClient);
        bd->CreateArg.fsClient = NULL;
    }

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    IM_DELETE(bd);
}

static void ImGui_ImplWiiU_UpdateKeyboardInput(ImGui_ImplWiiU_ControllerInput* input)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetActiveID());
    if (!state)
        return;

    if (input->vpad)
        VPADGetTPCalibratedPoint(VPAD_CHAN_0, &input->vpad->tpNormal, &input->vpad->tpNormal);

    nn::swkbd::ControllerInfo controllerInfo;
    controllerInfo.vpad = input->vpad;
    for (int i = 0; i < 4; i++)
        controllerInfo.kpad[i] = input->kpad[i];

    nn::swkbd::Calc(controllerInfo);

    if (nn::swkbd::IsNeedCalcSubThreadFont())
        nn::swkbd::CalcSubThreadFont();

    if (nn::swkbd::IsNeedCalcSubThreadPredict())
        nn::swkbd::CalcSubThreadPredict();

    if (nn::swkbd::IsDecideOkButton(NULL))
    {
        state->ClearText();

        // Add entered text
        const char16_t* string = nn::swkbd::GetInputFormString();
        for (int i = 0; *string; string++)
            io.AddInputCharacterUTF16(string[i]);

        // close keyboard
        nn::swkbd::DisappearInputForm();
    }

    if (nn::swkbd::IsDecideCancelButton(NULL))
        nn::swkbd::DisappearInputForm();
}

static void ImGui_ImplWiiU_UpdateTouchInput(ImGui_ImplWiiU_ControllerInput* input)
{
    if (!input->vpad)
        return;

    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();

    VPADTouchData touch;
    VPADGetTPCalibratedPoint(VPAD_CHAN_0, &touch, &input->vpad->tpNormal);

    if (touch.touched)
    {
        float scale_x = (io.DisplaySize.x / io.DisplayFramebufferScale.x) / 1280.0f;
        float scale_y = (io.DisplaySize.y / io.DisplayFramebufferScale.y) / 720.0f;
        io.AddMousePosEvent(touch.x * scale_x, touch.y * scale_y);
    }

    if (touch.touched != bd->WasTouched)
    {
        io.AddMouseButtonEvent(ImGuiMouseButton_Left, touch.touched);
        bd->WasTouched = touch.touched;
        bd->LastController = nn::swkbd::ControllerType::DrcGamepad;
    }
}

#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

static void ImGui_ImplWiiU_UpdateControllerInput(ImGui_ImplWiiU_ControllerInput* input)
{
    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();

    // SoH removal to make opening the menu easier
    // if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
    //     return;

    uint32_t vpad_buttons = input->vpad ? input->vpad->hold : 0;
    uint32_t wpad_buttons = 0;
    uint32_t classic_buttons = 0;
    uint32_t pro_buttons = 0;

    float stick_l_x = input->vpad ? input->vpad->leftStick.x : 0.0f;
    float stick_l_y = input->vpad ? input->vpad->leftStick.y : 0.0f;
    float stick_r_x = input->vpad ? input->vpad->rightStick.x : 0.0f;
    float stick_r_y = input->vpad ? input->vpad->rightStick.y : 0.0f;

    for (int i = 0; i < 4; i++)
    {
        KPADStatus* kpad = input->kpad[i];
        if (!kpad)
            continue;

        switch (kpad->extensionType)
        {
        case WPAD_EXT_CORE:
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_MPLUS_NUNCHUK:
            wpad_buttons |= kpad->hold;
            break;
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            classic_buttons |= kpad->classic.hold;
            if (classic_buttons & WPAD_CLASSIC_BUTTON_Y)
                bd->LastController = (nn::swkbd::ControllerType) i;

            stick_l_x += kpad->classic.leftStick.x;
            stick_l_y += kpad->classic.leftStick.y;
            stick_r_x += kpad->classic.rightStick.x;
            stick_r_y += kpad->classic.rightStick.y;
            break;
        case WPAD_EXT_PRO_CONTROLLER:
            pro_buttons |= kpad->pro.hold;
            if (pro_buttons & WPAD_PRO_BUTTON_Y)
                bd->LastController = (nn::swkbd::ControllerType) i;

            stick_l_x += kpad->pro.leftStick.x;
            stick_l_y += kpad->pro.leftStick.y;
            stick_r_x += kpad->pro.rightStick.x;
            stick_r_y += kpad->pro.rightStick.y;
            break;
        }
    }

    if (vpad_buttons & VPAD_BUTTON_Y)
        bd->LastController = nn::swkbd::ControllerType::DrcGamepad;

    io.AddKeyEvent(ImGuiKey_GamepadStart, (vpad_buttons & VPAD_BUTTON_PLUS) || (wpad_buttons & WPAD_BUTTON_PLUS) || (classic_buttons & WPAD_CLASSIC_BUTTON_PLUS) || (pro_buttons & WPAD_PRO_BUTTON_PLUS));
    io.AddKeyEvent(ImGuiKey_GamepadBack, (vpad_buttons & VPAD_BUTTON_MINUS) || (wpad_buttons & WPAD_BUTTON_MINUS) || (classic_buttons & WPAD_CLASSIC_BUTTON_MINUS) || (pro_buttons & WPAD_PRO_BUTTON_MINUS));
    io.AddKeyEvent(ImGuiKey_GamepadFaceLeft, (vpad_buttons & VPAD_BUTTON_X) || (classic_buttons & WPAD_CLASSIC_BUTTON_X) || (pro_buttons & WPAD_PRO_BUTTON_X));
    io.AddKeyEvent(ImGuiKey_GamepadFaceRight, (vpad_buttons & VPAD_BUTTON_B) || (wpad_buttons & WPAD_BUTTON_B) || (classic_buttons & WPAD_CLASSIC_BUTTON_B) || (pro_buttons & WPAD_PRO_BUTTON_B));
    io.AddKeyEvent(ImGuiKey_GamepadFaceUp, (vpad_buttons & VPAD_BUTTON_Y) || (classic_buttons & WPAD_CLASSIC_BUTTON_Y) || (pro_buttons & WPAD_PRO_BUTTON_Y));
    io.AddKeyEvent(ImGuiKey_GamepadFaceDown, (vpad_buttons & VPAD_BUTTON_A) || (wpad_buttons & WPAD_BUTTON_A) || (classic_buttons & WPAD_CLASSIC_BUTTON_A) || (pro_buttons & WPAD_PRO_BUTTON_A));
    io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, (vpad_buttons & VPAD_BUTTON_LEFT) || (wpad_buttons & WPAD_BUTTON_LEFT) || (classic_buttons & WPAD_CLASSIC_BUTTON_LEFT) || (pro_buttons & WPAD_PRO_BUTTON_LEFT));
    io.AddKeyEvent(ImGuiKey_GamepadDpadRight, (vpad_buttons & VPAD_BUTTON_RIGHT) || (wpad_buttons & WPAD_BUTTON_RIGHT) || (classic_buttons & WPAD_CLASSIC_BUTTON_RIGHT) || (pro_buttons & WPAD_PRO_BUTTON_RIGHT));
    io.AddKeyEvent(ImGuiKey_GamepadDpadUp, (vpad_buttons & VPAD_BUTTON_UP) || (wpad_buttons & WPAD_BUTTON_UP) || (classic_buttons & WPAD_CLASSIC_BUTTON_UP) || (pro_buttons & WPAD_PRO_BUTTON_UP));
    io.AddKeyEvent(ImGuiKey_GamepadDpadDown, (vpad_buttons & VPAD_BUTTON_DOWN) || (wpad_buttons & WPAD_BUTTON_DOWN) || (classic_buttons & WPAD_CLASSIC_BUTTON_DOWN) || (pro_buttons & WPAD_PRO_BUTTON_DOWN));
    io.AddKeyEvent(ImGuiKey_GamepadL1, (vpad_buttons & VPAD_BUTTON_L) || (classic_buttons & WPAD_CLASSIC_BUTTON_L) || (pro_buttons & WPAD_PRO_TRIGGER_L));
    io.AddKeyEvent(ImGuiKey_GamepadR1, (vpad_buttons & VPAD_BUTTON_R) || (classic_buttons & WPAD_CLASSIC_BUTTON_R) || (pro_buttons & WPAD_PRO_TRIGGER_R));
    io.AddKeyEvent(ImGuiKey_GamepadL2, (vpad_buttons & VPAD_BUTTON_ZL) || (classic_buttons & WPAD_CLASSIC_BUTTON_ZL) || (pro_buttons & WPAD_PRO_TRIGGER_ZL));
    io.AddKeyEvent(ImGuiKey_GamepadR2, (vpad_buttons & VPAD_BUTTON_ZR) || (classic_buttons & WPAD_CLASSIC_BUTTON_ZR) || (pro_buttons & WPAD_PRO_TRIGGER_ZR));
    io.AddKeyEvent(ImGuiKey_GamepadL3, (vpad_buttons & VPAD_BUTTON_STICK_L) || (pro_buttons & WPAD_PRO_BUTTON_STICK_L));
    io.AddKeyEvent(ImGuiKey_GamepadR3, (vpad_buttons & VPAD_BUTTON_STICK_R) || (pro_buttons & WPAD_PRO_BUTTON_STICK_R));

    stick_l_x = IM_CLAMP(stick_l_x, -1.0f, 1.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadLStickLeft, stick_l_x < -0.1f, (stick_l_x < -0.1f) ? (stick_l_x * -1.0f) : 0.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadLStickRight, stick_l_x > 0.1f, (stick_l_x > 0.1f) ? stick_l_x : 0.0f);

    stick_l_y = IM_CLAMP(stick_l_y, -1.0f, 1.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadLStickUp, stick_l_y > 0.1f, (stick_l_y > 0.1f) ? stick_l_y : 0.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadLStickDown, stick_l_y < -0.1f, (stick_l_y < -0.1f) ? (stick_l_y * -1.0f) : 0.0f);

    stick_r_x = IM_CLAMP(stick_r_x, -1.0f, 1.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadRStickLeft, stick_r_x < -0.1f, (stick_r_x < -0.1f) ? (stick_r_x * -1.0f) : 0.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadRStickRight, stick_r_x > 0.1f, (stick_r_x > 0.1f) ? stick_r_x : 0.0f);

    stick_r_y = IM_CLAMP(stick_r_y, -1.0f, 1.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadRStickUp, stick_r_y > 0.1f, (stick_r_y > 0.1f) ? stick_r_y : 0.0f);
    io.AddKeyAnalogEvent(ImGuiKey_GamepadRStickDown, stick_r_y < -0.1f, (stick_r_y < -0.1f) ? (stick_r_y * -1.0f) : 0.0f);
}

bool     ImGui_ImplWiiU_ProcessInput(ImGui_ImplWiiU_ControllerInput* input)
{
    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplWiiU_Init()?");
    ImGuiIO& io = ImGui::GetIO();

    // Show keyboard if wanted
    if (io.WantTextInput && !bd->WantedTextInput) 
    {
        ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetActiveID());
        if (state)
        {
            if (!(state->Flags & ImGuiInputTextFlags_AlwaysOverwrite))
                bd->AppearArg.inputFormArg.initialText = (char16_t*) state->TextW.Data;

            bd->AppearArg.inputFormArg.maxTextLength = state->BufCapacityA;
            bd->AppearArg.inputFormArg.higlightInitialText = !!(state->Flags & ImGuiInputTextFlags_AutoSelectAll);

            if (state->Flags & ImGuiInputTextFlags_Password)
                bd->AppearArg.inputFormArg.passwordMode = nn::swkbd::PasswordMode::Fade;
            else
                bd->AppearArg.inputFormArg.passwordMode = nn::swkbd::PasswordMode::Clear;
        }

        // Open the keyboard for the controller which requested the text input
        bd->AppearArg.keyboardArg.configArg.controllerType = bd->LastController;

        if (nn::swkbd::GetStateInputForm() == nn::swkbd::State::Hidden)
            nn::swkbd::AppearInputForm(bd->AppearArg);
    }
    bd->WantedTextInput = io.WantTextInput;

    // Update keyboard input
    if (nn::swkbd::GetStateInputForm() != nn::swkbd::State::Hidden)
    {
        ImGui_ImplWiiU_UpdateKeyboardInput(input);
        return true;
    }

    // Update touch screen
    ImGui_ImplWiiU_UpdateTouchInput(input);

    // Update gamepads
    ImGui_ImplWiiU_UpdateControllerInput(input);

    return false;
}

void     ImGui_ImplWiiU_DrawKeyboardOverlay(ImGui_ImplWiiU_KeyboardOverlayType type)
{
    ImGui_ImplWiiU_Data* bd = ImGui_ImplWiiU_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplWiiU_Init()?");

    if (nn::swkbd::GetStateInputForm() != nn::swkbd::State::Hidden)
    {
        if (type == ImGui_KeyboardOverlay_Auto)
        {
            if (bd->LastController == nn::swkbd::ControllerType::DrcGamepad)
                nn::swkbd::DrawDRC();
            else
                nn::swkbd::DrawTV();
        }
        else if (type == ImGui_KeyboardOverlay_DRC)
            nn::swkbd::DrawDRC();
        else if (type == ImGui_KeyboardOverlay_TV)
            nn::swkbd::DrawTV();
    }
}
