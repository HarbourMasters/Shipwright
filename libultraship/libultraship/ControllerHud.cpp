#include "ControllerHud.h"
#include "Controller.h"
#include "Window.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Utils/StringHelper.h"
#include <iostream>

#include "KeyboardController.h"
#include "Lib/ImGui/imgui_internal.h"
#include "Hooks.h"

namespace Ship {

	extern "C" uint8_t __enableGameInput;
	#define SEPARATION() ImGui::Dummy(ImVec2(0, 5))

	void ControllerHud::Init() {
		ModInternal::RegisterHook<ModInternal::ControllerRawInput>([](Controller* backend, uint32_t raw) {
			std::cout << raw << std::endl;
		});
	}

	void ControllerHud::DrawButton(const char* label, int n64Btn) {
		const std::vector<int> vDevices = Window::ControllerApi->virtualDevices;
		const std::vector<std::shared_ptr<DeviceEntry>> devices = Window::ControllerApi->physicalDevices;
		const auto pad = dynamic_cast<KeyboardController*>(Window::ControllerApi->physicalDevices[Window::ControllerApi->physicalDevices.size() - 2]->Backend.get());

		std::shared_ptr<Controller> backend = devices[vDevices[CurrentPort]]->Backend;

		float size = 40;
		bool readingMode = BtnReading == n64Btn;
		bool disabled = BtnReading != -1 && !readingMode || !backend->Connected();
		ImVec2 len = ImGui::CalcTextSize(label);
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPosY(pos.y + len.y / 4);
		ImGui::SetCursorPosX(pos.x + abs(len.x - size));
		ImGui::Text(label);
		ImGui::SameLine();
		ImGui::SetCursorPosY(pos.y);
		if(disabled) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		if(readingMode) {
			// int32_t btn = backend->ReadRawPress();
			// std::cout << btn << std::endl;
			// if(btn > -1) {
			// 	std::cout << "Pressed: " << btn << std::endl;
			// 	BtnReading = -1;
			// }
		}

		DeviceProfile profile = backend->GetDefaultMapping();

		if (ImGui::Button(readingMode ? "Press a Key" : backend->GetButtonName(profile.Mappings[n64Btn]))) {
			BtnReading = n64Btn;
		}
		if(disabled) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	void ControllerHud::DrawVirtualStick(ImVec2 stick) {
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 5, ImGui::GetCursorPos().y));
		ImGui::BeginChild("VirtualStick", ImVec2(68, 75), false);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImVec2 p = ImGui::GetCursorScreenPos();

		float sz = 45.0f;
		ImVec2 pos = ImVec2(p.x + sz * 0.5f + 12, p.y + sz * 0.5f + 11);

		ImVec4 rect = ImVec4(p.x + 2, p.y + 2, 65, 65);
		draw_list->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.z, rect.y + rect.w), ImColor(100, 100, 100, 255), 0.0f, 0, 1.5f);
		draw_list->AddCircleFilled(pos, sz * 0.5f, ImColor(130, 130, 130, 255), 8);
		draw_list->AddCircleFilled(pos, 5, ImColor(15, 15, 15, 255), 7);
		ImGui::EndChild();
	}

	void ControllerHud::DrawControllerSchema() {

		int dm = 0;
		const std::vector<int> vDevices = Window::ControllerApi->virtualDevices;
		const std::vector<std::shared_ptr<DeviceEntry>> devices = Window::ControllerApi->physicalDevices;

		if (ImGui::BeginCombo("##ControllerEntries", devices[vDevices[CurrentPort]]->Name)) {
			for (uint8_t i = 0; i < devices.size(); i++) {
				if (ImGui::Selectable(devices[i]->Name, i == vDevices[CurrentPort])) {
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
			DrawButton("A", B_A);
			DrawButton("B", B_B);
			DrawButton("L",  B_L);
			DrawButton("R",  B_R);
			DrawButton("Z", B_Z);
			DrawButton("START",  B_Start);
			SEPARATION();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Digital Pad", ImVec2(150, 20));
			DrawButton("Up", D_Up);
			DrawButton("Down", D_Down);
			DrawButton("Left", D_Left);
			DrawButton("Right", D_Right);
			SEPARATION();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Analog Stick", ImVec2(150, 20));
			DrawButton("Up", S_Up);
			DrawButton("Down", S_Down);
			DrawButton("Left", S_Left);
			DrawButton("Right", S_Right);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);

			ImGui::Text("Main Stick");
			DrawVirtualStick(ImVec2(0, 0));

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 17);
			ImGui::BeginChild("Input", ImVec2(90, 50), false);
				ImGui::Text("Deadzone");
				ImGui::PushItemWidth(80);
				if (ImGui::InputInt("##x", &dm)) {

				}
				ImGui::PopItemWidth();
			ImGui::EndChild();
		SohImGui::EndGroupPanel();
		ImGui::SameLine();

		ImVec2 cursor = ImGui::GetCursorPos();

		SohImGui::BeginGroupPanel("C-Pad", ImVec2(150, 20));
			DrawButton("Up", C_Up);
			DrawButton("Down", C_Down);
			DrawButton("Left", C_Left);
			DrawButton("Right", C_Right);
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel();

		ImGui::SetCursorPosX(cursor.x);
		ImGui::SetCursorPosY(cursor.y + 120);
		SohImGui::BeginGroupPanel("Options", ImVec2(150, 20));
			float cursorX = ImGui::GetCursorPosX() + 5;
			ImGui::SetCursorPosX(cursorX);
			bool dummy = true;
			if (ImGui::Checkbox("Rumble Enabled", &dummy)) {

			}
			ImGui::SetCursorPosX(cursorX);
			if (ImGui::Checkbox("Show Inputs", &dummy)) {

			}
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel();
	}

	void ControllerHud::DrawHud() {

		__enableGameInput = true;

		if (!this->Opened) return;

		ImGui::SetNextWindowSizeConstraints(ImVec2(681, 290), ImVec2(900, 290));
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
