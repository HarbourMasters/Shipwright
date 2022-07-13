#include "ControllerHud.h"
#include "Controller.h"
#include "Window.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Utils/StringHelper.h"
#include "Lib/ImGui/imgui_internal.h"

namespace Ship {

	extern "C" uint8_t __enableGameInput;
	#define SEPARATION() ImGui::Dummy(ImVec2(0, 5))

	void ControllerHud::Init() {

	}

	std::shared_ptr<Controller> GetControllerPerSlot(int slot) {
		const std::vector<int> vDevices = Window::ControllerApi->virtualDevices;
		return Window::ControllerApi->physicalDevices[vDevices[slot]];
	}

	void ControllerHud::DrawButton(const char* label, int n64Btn) {
		const std::shared_ptr<Controller> backend = GetControllerPerSlot(CurrentPort);

		float size = 40;
		bool readingMode = BtnReading == n64Btn;
		bool disabled = BtnReading != -1 && !readingMode || !backend->Connected();
		ImVec2 len = ImGui::CalcTextSize(label);
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPosY(pos.y + len.y / 4);
		ImGui::SetCursorPosX(pos.x + abs(len.x - size));
		ImGui::Text("%s", label);
		ImGui::SameLine();
		ImGui::SetCursorPosY(pos.y);

		if(disabled) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if(readingMode) {
			const int32_t btn = backend->ReadRawPress();
			if(btn != -1) {
				backend->SetButtonMapping(CurrentPort, n64Btn, btn);
				BtnReading = -1;
			}
		}

		if(backend->Connected()) {
			int bp = 0;
		}
		
		const char* BtnName = backend->GetButtonName(CurrentPort, n64Btn);

		if (ImGui::Button( readingMode ? "Press a Key..." : BtnName)) {
			BtnReading = n64Btn;
		}

		if(disabled) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	void ControllerHud::DrawVirtualStick(const char* label, ImVec2 stick) {
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 5, ImGui::GetCursorPos().y));
		ImGui::BeginChild(label, ImVec2(68, 75), false);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImVec2 p = ImGui::GetCursorScreenPos();

		float sz = 45.0f;
		float rad = sz * 0.5f;
		ImVec2 pos = ImVec2(p.x + sz * 0.5f + 12, p.y + sz * 0.5f + 11);

		float stickX =  (stick.x / 83.0f) * (rad * 0.5f);
		float stickY = -(stick.y / 83.0f) * (rad * 0.5f);

		ImVec4 rect = ImVec4(p.x + 2, p.y + 2, 65, 65);
		draw_list->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.z, rect.y + rect.w), ImColor(100, 100, 100, 255), 0.0f, 0, 1.5f);
		draw_list->AddCircleFilled(pos, rad, ImColor(130, 130, 130, 255), 8);
		draw_list->AddCircleFilled(ImVec2(pos.x + stickX, pos.y + stickY), 5, ImColor(15, 15, 15, 255), 7);
		ImGui::EndChild();
	}

	void ControllerHud::DrawControllerSchema() {

		const std::vector<int> vDevices = Window::ControllerApi->virtualDevices;
		const std::vector<std::shared_ptr<Controller>> devices = Window::ControllerApi->physicalDevices;

		std::shared_ptr<Controller> Backend = devices[vDevices[CurrentPort]];
		DeviceProfile& profile =Backend->profiles[CurrentPort];

		if (ImGui::BeginCombo("##ControllerEntries", Backend->GetControllerName())) {
			for (uint8_t i = 0; i < devices.size(); i++) {
				if (ImGui::Selectable(devices[i]->GetControllerName(), i == vDevices[CurrentPort])) {
					Window::ControllerApi->SetPhysicalDevice(CurrentPort, i);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if(ImGui::Button("Refresh")) {
			Window::ControllerApi->ScanPhysicalDevices();
		}

		SohImGui::BeginGroupPanel("Buttons", ImVec2(150, 20));
			DrawButton("A", BTN_A);
			DrawButton("B", BTN_B);
			DrawButton("L", BTN_L);
			DrawButton("R", BTN_R);
			DrawButton("Z", BTN_Z);
			DrawButton("START", BTN_START);
			SEPARATION();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Digital Pad", ImVec2(150, 20));
			DrawButton("Up", BTN_DUP);
			DrawButton("Down", BTN_DDOWN);
			DrawButton("Left", BTN_DLEFT);
			DrawButton("Right", BTN_DRIGHT);
			SEPARATION();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Analog Stick", ImVec2(150, 20));
			DrawButton("Up", BTN_STICKUP);
			DrawButton("Down", BTN_STICKDOWN);
			DrawButton("Left", BTN_STICKLEFT);
			DrawButton("Right", BTN_STICKRIGHT);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
			DrawVirtualStick("##MainVirtualStick", ImVec2(Backend->wStickX, Backend->wStickY));
			ImGui::SameLine();

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

			ImGui::BeginChild("##MSInput", ImVec2(90, 50), false);
				ImGui::Text("Deadzone");
				ImGui::PushItemWidth(80);
				ImGui::InputInt("##MDZone", &profile.Thresholds[LEFT_STICK]);
				ImGui::PopItemWidth();
			ImGui::EndChild();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();

		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Camera Stick", ImVec2(150, 20));
			DrawButton("Up", BTN_VSTICKUP);
			DrawButton("Down", BTN_VSTICKDOWN);
			DrawButton("Left", BTN_VSTICKLEFT);
			DrawButton("Right", BTN_VSTICKRIGHT);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
			DrawVirtualStick("##CameraVirtualStick", ImVec2(0, 0));

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::BeginChild("##CSInput", ImVec2(90, 50), false);
				ImGui::Text("Deadzone");
				ImGui::PushItemWidth(80);
				ImGui::InputInt("##MDZone", &profile.Thresholds[RIGHT_STICK]);
				ImGui::PopItemWidth();
			ImGui::EndChild();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();


		ImVec2 cursor = ImGui::GetCursorPos();

		SohImGui::BeginGroupPanel("C-Buttons", ImVec2(150, 20));
			DrawButton("Up", BTN_CUP);
			DrawButton("Down", BTN_CDOWN);
			DrawButton("Left", BTN_CLEFT);
			DrawButton("Right", BTN_CRIGHT);
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel();

		ImGui::SetCursorPosX(cursor.x);
		ImGui::SetCursorPosY(cursor.y + 120);
		SohImGui::BeginGroupPanel("Options", ImVec2(150, 20));
			float cursorX = ImGui::GetCursorPosX() + 5;
			ImGui::SetCursorPosX(cursorX);
			ImGui::Checkbox("Rumble Enabled", &profile.UseRumble);
			ImGui::SetCursorPosX(cursorX);
			ImGui::Checkbox("Show Inputs", &profile.ShowInputs);
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel();
	}

	void ControllerHud::DrawHud() {

		__enableGameInput = true;

		if (!this->Opened) return;

		ImGui::SetNextWindowSizeConstraints(ImVec2(681, 290), ImVec2(990, 290));
		ImGui::Begin("Controller Configuration", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::BeginTabBar("##Controllers");

		for (int i = 0; i < 4; i++) {
			if (ImGui::BeginTabItem(StringHelper::Sprintf("Port %d", i + 1).c_str())) {
				CurrentPort = i;
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();

		// Draw current cfg

		DrawControllerSchema();

		ImGui::End();
	}
}
