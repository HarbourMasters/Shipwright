#include "InputEditor.h"
#include "Controller.h"
#include "Window.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Utils/StringHelper.h"
#include "Lib/ImGui/imgui_internal.h"
#include "Cvar.h"

namespace Ship {

	extern "C" uint8_t __enableGameInput;
	#define SEPARATION() ImGui::Dummy(ImVec2(0, 5))

	void InputEditor::Init() {
		BtnReading = -1;
	}

	std::shared_ptr<Controller> GetControllerPerSlot(int slot) {
		const std::vector<int> vDevices = Window::ControllerApi->virtualDevices;
		return Window::ControllerApi->physicalDevices[vDevices[slot]];
	}

	void InputEditor::DrawButton(const char* label, int n64Btn) {
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

		const char* BtnName = backend->GetButtonName(CurrentPort, n64Btn);

		if (ImGui::Button(StringHelper::Sprintf("%s##HBTNID_%d", readingMode ? "Press a Key..." : BtnName, n64Btn).c_str())) {
			BtnReading = n64Btn;
			backend->ClearRawPress();
		}

		if(disabled) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	void InputEditor::DrawVirtualStick(const char* label, ImVec2 stick) {
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

	void InputEditor::DrawControllerSchema() {

		const std::vector<int> vDevices = Window::ControllerApi->virtualDevices;
		const std::vector<std::shared_ptr<Controller>> devices = Window::ControllerApi->physicalDevices;

		std::shared_ptr<Controller> Backend = devices[vDevices[CurrentPort]];
		DeviceProfile& profile =Backend->profiles[CurrentPort];
		float sensitivity = profile.Thresholds[SENSITIVITY];
		bool IsKeyboard = Backend->GetGuid() == "Keyboard" || !Backend->Connected();
		const char* ControllerName = Backend->GetControllerName();

		if (ControllerName != nullptr && ImGui::BeginCombo("##ControllerEntries", ControllerName)) {
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
		SohImGui::EndGroupPanel(IsKeyboard ? 7.0f : 48.0f);
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Digital Pad", ImVec2(150, 20));
			DrawButton("Up", BTN_DUP);
			DrawButton("Down", BTN_DDOWN);
			DrawButton("Left", BTN_DLEFT);
			DrawButton("Right", BTN_DRIGHT);
			SEPARATION();
		SohImGui::EndGroupPanel(IsKeyboard ? 53.0f : 94.0f);
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Analog Stick", ImVec2(150, 20));
			DrawButton("Up", BTN_STICKUP);
			DrawButton("Down", BTN_STICKDOWN);
			DrawButton("Left", BTN_STICKLEFT);
			DrawButton("Right", BTN_STICKRIGHT);

			if (!IsKeyboard) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
				DrawVirtualStick("##MainVirtualStick", ImVec2(Backend->wStickX, Backend->wStickY));
				ImGui::SameLine();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

				ImGui::BeginChild("##MSInput", ImVec2(90, 50), false);
				ImGui::Text("Deadzone");
				ImGui::PushItemWidth(80);
				ImGui::InputFloat("##MDZone", &profile.Thresholds[LEFT_STICK], 1.0f, 0.0f, "%.0f");
				ImGui::PopItemWidth();
				ImGui::EndChild();
			} else {
				ImGui::Dummy(ImVec2(0, 6));
			}
		SohImGui::EndGroupPanel(IsKeyboard ? 52.0f : 24.0f);
		ImGui::SameLine();

		if (!IsKeyboard) {
			ImGui::SameLine();
			SohImGui::BeginGroupPanel("Camera Stick", ImVec2(150, 20));
				DrawButton("Up", BTN_VSTICKUP);
				DrawButton("Down", BTN_VSTICKDOWN);
				DrawButton("Left", BTN_VSTICKLEFT);
				DrawButton("Right", BTN_VSTICKRIGHT);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
				DrawVirtualStick("##CameraVirtualStick", ImVec2(Backend->wCamX / sensitivity, Backend->wCamY / sensitivity));

				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
				ImGui::BeginChild("##CSInput", ImVec2(90, 85), false);
					ImGui::Text("Deadzone");
					ImGui::PushItemWidth(80);
					ImGui::InputFloat("##MDZone", &profile.Thresholds[RIGHT_STICK], 1.0f, 0.0f, "%.0f");
					ImGui::PopItemWidth();
					ImGui::Text("Sensitivity");
					ImGui::PushItemWidth(80);
					ImGui::InputFloat("##MSensitivity", &profile.Thresholds[SENSITIVITY], 1.0f, 0.0f, "%.0f");
					ImGui::PopItemWidth();
				ImGui::EndChild();
			SohImGui::EndGroupPanel(14.0f);
		}

		if(Backend->CanGyro()) {
			ImGui::SameLine();

			SohImGui::BeginGroupPanel("Gyro Options", ImVec2(175, 20));
			float cursorX = ImGui::GetCursorPosX() + 5;
			ImGui::SetCursorPosX(cursorX);
			ImGui::Checkbox("Enable Gyro", &profile.UseGyro);
			ImGui::SetCursorPosX(cursorX);
			ImGui::Text("Gyro Sensitivity: %d%%", static_cast<int>(100.0f * profile.Thresholds[GYRO_SENSITIVITY]));
			ImGui::PushItemWidth(135.0f);
			ImGui::SetCursorPosX(cursorX);
			ImGui::SliderFloat("##GSensitivity", &profile.Thresholds[GYRO_SENSITIVITY], 0.0f, 1.0f, "");
			ImGui::PopItemWidth();
			ImGui::Dummy(ImVec2(0, 1));
			ImGui::SetCursorPosX(cursorX);
			if (ImGui::Button("Recalibrate Gyro##RGyro")) {
				profile.Thresholds[DRIFT_X] = 0.0f;
				profile.Thresholds[DRIFT_Y] = 0.0f;
			}
			ImGui::SetCursorPosX(cursorX);
			DrawVirtualStick("##GyroPreview", ImVec2(-10.0f * Backend->wGyroY, 10.0f * Backend->wGyroX));

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::BeginChild("##GyInput", ImVec2(90, 85), false);
			ImGui::Text("Drift X");
			ImGui::PushItemWidth(80);
			ImGui::InputFloat("##GDriftX", &profile.Thresholds[DRIFT_X], 1.0f, 0.0f, "%.1f");
			ImGui::PopItemWidth();
			ImGui::Text("Drift Y");
			ImGui::PushItemWidth(80);
			ImGui::InputFloat("##GDriftY", &profile.Thresholds[DRIFT_Y], 1.0f, 0.0f, "%.1f");
			ImGui::PopItemWidth();
			ImGui::EndChild();
			SohImGui::EndGroupPanel(14.0f);
		}

		ImGui::SameLine();

		const ImVec2 cursor = ImGui::GetCursorPos();

		SohImGui::BeginGroupPanel("C-Buttons", ImVec2(158, 20));
			DrawButton("Up", BTN_CUP);
			DrawButton("Down", BTN_CDOWN);
			DrawButton("Left", BTN_CLEFT);
			DrawButton("Right", BTN_CRIGHT);
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel();

		ImGui::SetCursorPosX(cursor.x);
		ImGui::SetCursorPosY(cursor.y + 120);
		SohImGui::BeginGroupPanel("Options", ImVec2(158, 20));
			float cursorX = ImGui::GetCursorPosX() + 5;
			ImGui::SetCursorPosX(cursorX);
			ImGui::Checkbox("Rumble Enabled", &profile.UseRumble);
			if (Backend->CanRumble()) {
				ImGui::SetCursorPosX(cursorX);
				ImGui::Text("Rumble Force: %d%%", static_cast<int>(100.0f * profile.RumbleStrength));
				ImGui::SetCursorPosX(cursorX);
				ImGui::PushItemWidth(135.0f);
				ImGui::SliderFloat("##RStrength", &profile.RumbleStrength, 0.0f, 1.0f, "");
				ImGui::PopItemWidth();
			}
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel(IsKeyboard ? 0.0f : 2.0f);
	}

	void InputEditor::DrawHud() {

		__enableGameInput = true;

		if (!this->Opened) {
			BtnReading = -1;
			CVar_SetS32("gControllerConfigurationEnabled", 0);
			return;
		}

		ImGui::SetNextWindowSizeConstraints(ImVec2(641, 250), ImVec2(1200, 290));
		//OTRTODO: Disable this stupid workaround ( ReadRawPress() only works when the window is on the main viewport )
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
		ImGui::Begin("Controller Configuration", &this->Opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

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
