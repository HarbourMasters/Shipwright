#include "InputEditor.h"
#include "Controller.h"
#include "Window.h"
#include "Lib/ImGui/imgui.h"
#include "ImGuiImpl.h"
#include "Utils/StringHelper.h"
#include "Lib/ImGui/imgui_internal.h"
#include "Cvar.h"

namespace Ship {

	#define SEPARATION() ImGui::Dummy(ImVec2(0, 5))

	void InputEditor::Init() {
		BtnReading = -1;
	}

	std::shared_ptr<Controller> GetControllerPerSlot(int virtualSlot) {
		auto controlDeck = Ship::GlobalCtx2::GetInstance()->GetWindow()->GetControlDeck();
		return controlDeck->GetPhysicalDeviceFromVirtualSlot(virtualSlot);
	}

	void InputEditor::DrawButton(const char* label, int n64Btn) {
		const std::shared_ptr<Controller> backend = GetControllerPerSlot(CurrentPort);

		float size = 40;
		bool readingMode = BtnReading == n64Btn;
		bool disabled = (BtnReading != -1 && !readingMode) || !backend->Connected() || backend->GetGuid() == "Auto";
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

		const std::string BtnName = backend->GetButtonName(CurrentPort, n64Btn);

		if (ImGui::Button(StringHelper::Sprintf("%s##HBTNID_%d", readingMode ? "Press a Key..." : BtnName.c_str(), n64Btn).c_str())) {
			BtnReading = n64Btn;
			backend->ClearRawPress();
		}

		if(disabled) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	void InputEditor::DrawVirtualStick(const char* label,
									   int32_t CurrentPort,
							  		   InputAxisDirection yAxisUp,
							  		   InputAxisDirection yAxisDown,
							  		   InputAxisDirection xAxisLeft,
							  		   InputAxisDirection xAxisRight) {
		// Get the controller based on the current virtualSlot (port)
		const std::shared_ptr<Controller> physicalDevice = GetControllerPerSlot(CurrentPort);
		
		int16_t inputX = 0;
		int16_t inputY = 0;

		// For each mapped axis direction
		const InputAxisDirection directions[] = { yAxisUp, yAxisDown, xAxisLeft, xAxisRight };
		for(auto direction : directions) {
			// get the mapped SDL button code
			auto mappedDirection = physicalDevice->GetMappedButton(CurrentPort, direction);

			if (mappedDirection == -1) {
				// SDL_CONTROLLER_BUTTON_INVALID is -1
				continue;
			}

			if (abs(mappedDirection) < AXIS_SCANCODE_BIT) {
				// if it's absolute value is less than the scancode bit,
				// this is a button and not an axis
				if (physicalDevice->ButtonIsPressedRaw(mappedDirection)) {
					if (direction == yAxisUp) {
						inputY -= 32767;
					} else if (direction == yAxisDown) {
						inputY += 32767;
					} else if (direction == xAxisLeft) {
						inputX -= 32767;
					} else if (direction == xAxisRight) {
						inputX += 32767;
					}
				}
			} else {
				// this is an axis, so we subtract the scancode to figure out which one
				auto sdlAxis = abs(mappedDirection) - AXIS_SCANCODE_BIT;

				// we only want to change the input in the direction we've mapped, so
				// * only read positive values when mappedDirection > 0
				// * only read negative values when mappedDirection < 0
				// subtracting 1 from each value to prevents overflow issues
				if ((mappedDirection > 0 && physicalDevice->ReadRawAxis(sdlAxis) > 0) ||
					(mappedDirection < 0 && physicalDevice->ReadRawAxis(sdlAxis) < 0)) {
					if (direction == yAxisUp) {
						inputY -= (abs(physicalDevice->ReadRawAxis(sdlAxis)) - 1);
					} else if (direction == yAxisDown) {
						inputY += (abs(physicalDevice->ReadRawAxis(sdlAxis)) - 1);
					} else if (direction == xAxisLeft) {
						inputX -= (abs(physicalDevice->ReadRawAxis(sdlAxis)) - 1);
					} else if (direction == xAxisRight) {
						inputX += (abs(physicalDevice->ReadRawAxis(sdlAxis)) - 1);
					}
				}
			}
		}

		// now that we have the input values, do some math and draw them
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 5, ImGui::GetCursorPos().y));
		ImGui::BeginChild(label, ImVec2(68, 75), false);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImVec2 p = ImGui::GetCursorScreenPos();

		float sz = 45.0f;
		float rad = sz * 0.5f;
		ImVec2 pos = ImVec2(p.x + sz * 0.5f + 12, p.y + sz * 0.5f + 11);

		float stickX = (inputX / 32767.0f) * (rad * 0.5f);
		float stickY = (inputY / 32767.0f) * (rad * 0.5f);

		ImVec4 rect = ImVec4(p.x + 2, p.y + 2, 65, 65);
		draw_list->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.z, rect.y + rect.w), ImColor(100, 100, 100, 255), 0.0f, 0, 1.5f);
		draw_list->AddCircleFilled(pos, rad, ImColor(130, 130, 130, 255), 8);
		draw_list->AddCircleFilled(ImVec2(pos.x + stickX, pos.y + stickY), 5, ImColor(15, 15, 15, 255), 7);
		ImGui::EndChild();
	}

	void InputEditor::DrawControllerSchema() {
		auto controlDeck = Ship::GlobalCtx2::GetInstance()->GetWindow()->GetControlDeck();
		auto Backend = controlDeck->GetPhysicalDeviceFromVirtualSlot(CurrentPort);
		auto profile = Backend->getProfile(CurrentPort);
		bool IsKeyboard = Backend->GetGuid() == "Keyboard"  || Backend->GetGuid() == "Auto" || !Backend->Connected();
		std::string ControllerName = Backend->GetControllerName();

		if (ImGui::BeginCombo("##ControllerEntries", ControllerName.c_str())) {
			for (uint8_t i = 0; i < controlDeck->GetNumPhysicalDevices(); i++) {
				if (ControllerName != "Keyboard" && ControllerName != "Auto") {
					ControllerName += "##"+std::to_string(i);
				}
				if (ImGui::Selectable(controlDeck->GetPhysicalDevice(i)->GetControllerName().c_str(), i == controlDeck->GetVirtualDevice(CurrentPort))) {
					controlDeck->SetPhysicalDevice(CurrentPort, i);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if(ImGui::Button("Refresh")) {
			controlDeck->ScanPhysicalDevices();
		}

		SohImGui::BeginGroupPanel("Buttons", ImVec2(150, 20));
			DrawButton("A", BTN_A);
			DrawButton("B", BTN_B);
			DrawButton("L", BTN_L);
			DrawButton("R", BTN_R);
			DrawButton("Z", BTN_Z);
			DrawButton("START", BTN_START);
			SEPARATION();
	#ifdef __SWITCH__
		SohImGui::EndGroupPanel(IsKeyboard ? 7.0f : 56.0f);
	#else
		SohImGui::EndGroupPanel(IsKeyboard ? 7.0f : 48.0f);
	#endif
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Digital Pad", ImVec2(150, 20));
			DrawButton("Up", BTN_DUP);
			DrawButton("Down", BTN_DDOWN);
			DrawButton("Left", BTN_DLEFT);
			DrawButton("Right", BTN_DRIGHT);
			SEPARATION();
	#ifdef __SWITCH__
		SohImGui::EndGroupPanel(IsKeyboard ? 53.0f : 122.0f);
	#else
		SohImGui::EndGroupPanel(IsKeyboard ? 53.0f : 94.0f);
	#endif
		ImGui::SameLine();
		SohImGui::BeginGroupPanel("Analog Stick", ImVec2(150, 20));
			DrawButton("Up", BTN_STICKUP);
			DrawButton("Down", BTN_STICKDOWN);
			DrawButton("Left", BTN_STICKLEFT);
			DrawButton("Right", BTN_STICKRIGHT);

			if (!IsKeyboard) {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
				DrawVirtualStick("##MainVirtualStick",
								 CurrentPort,
								 PRIMARY_ANALOG_STICK_UP,
								 PRIMARY_ANALOG_STICK_DOWN,
								 PRIMARY_ANALOG_STICK_LEFT,
								 PRIMARY_ANALOG_STICK_RIGHT);
				ImGui::SameLine();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

				ImGui::BeginChild("##MSInput", ImVec2(90, 50), false);
				ImGui::Text("Deadzone");
				ImGui::PushItemWidth(80);
				ImGui::InputFloat("##MDZone", &profile->AxisDeadzones[0] /* This is the SDL value for left stick X axis */, 1.0f, 0.0f, "%.0f");
				ImGui::PopItemWidth();
				ImGui::EndChild();
			} else {
				ImGui::Dummy(ImVec2(0, 6));
			}
		#ifdef __SWITCH__
			SohImGui::EndGroupPanel(IsKeyboard ? 52.0f : 52.0f);
		#else
			SohImGui::EndGroupPanel(IsKeyboard ? 52.0f : 24.0f);
		#endif
		ImGui::SameLine();

		if (!IsKeyboard) {
			ImGui::SameLine();
			SohImGui::BeginGroupPanel("Camera Stick", ImVec2(150, 20));
				DrawButton("Up", BTN_VSTICKUP);
				DrawButton("Down", BTN_VSTICKDOWN);
				DrawButton("Left", BTN_VSTICKLEFT);
				DrawButton("Right", BTN_VSTICKRIGHT);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
				DrawVirtualStick("##CameraVirtualStick",
								 CurrentPort,
								 SECONDARY_ANALOG_STICK_UP,
								 SECONDARY_ANALOG_STICK_DOWN,
								 SECONDARY_ANALOG_STICK_LEFT,
								 SECONDARY_ANALOG_STICK_RIGHT);

				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
				ImGui::BeginChild("##CSInput", ImVec2(90, 85), false);
					ImGui::Text("Deadzone");
					ImGui::PushItemWidth(80);
					ImGui::InputFloat("##MDZone", &profile->AxisDeadzones[2] /* This is the SDL value for left stick X axis */, 1.0f, 0.0f, "%.0f");
					ImGui::PopItemWidth();
					ImGui::Text("Sensitivity");
					ImGui::PushItemWidth(80);
					ImGui::InputFloat("##MSensitivity", &profile->AxisSensitivities[2] /* This is the SDL value for right stick X axis */, 1.0f, 0.0f, "%.0f");
					profile->AxisSensitivities[3] = profile->AxisSensitivities[2];
					ImGui::PopItemWidth();
				ImGui::EndChild();
		#ifdef __SWITCH__
			SohImGui::EndGroupPanel(43.0f);
		#else
			SohImGui::EndGroupPanel(14.0f);
		#endif
		}

		if(Backend->CanGyro()) {
			ImGui::SameLine();

			SohImGui::BeginGroupPanel("Gyro Options", ImVec2(175, 20));
				float cursorX = ImGui::GetCursorPosX() + 5;
				ImGui::SetCursorPosX(cursorX);
				ImGui::Checkbox("Enable Gyro", &profile->UseGyro);
				ImGui::SetCursorPosX(cursorX);
				ImGui::Text("Gyro Sensitivity: %d%%", static_cast<int>(100.0f * profile->GyroData[GYRO_SENSITIVITY]));
				ImGui::PushItemWidth(135.0f);
				ImGui::SetCursorPosX(cursorX);
				ImGui::SliderFloat("##GSensitivity", &profile->GyroData[GYRO_SENSITIVITY], 0.0f, 1.0f, "");
				ImGui::PopItemWidth();
				ImGui::Dummy(ImVec2(0, 1));
				ImGui::SetCursorPosX(cursorX);
				if (ImGui::Button("Recalibrate Gyro##RGyro")) {
					profile->GyroData[DRIFT_X] = 0.0f;
					profile->GyroData[DRIFT_Y] = 0.0f;
				}
				ImGui::SetCursorPosX(cursorX);
				// TODO: implement/test gyro preview
				DrawVirtualStick("##GyroPreview", CurrentPort, GYRO_Y, GYRO_Y, GYRO_X, GYRO_X);

				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
				ImGui::BeginChild("##GyInput", ImVec2(90, 85), false);
				ImGui::Text("Drift X");
				ImGui::PushItemWidth(80);
				ImGui::InputFloat("##GDriftX", &profile->GyroData[DRIFT_X], 1.0f, 0.0f, "%.1f");
				ImGui::PopItemWidth();
				ImGui::Text("Drift Y");
				ImGui::PushItemWidth(80);
				ImGui::InputFloat("##GDriftY", &profile->GyroData[DRIFT_Y], 1.0f, 0.0f, "%.1f");
				ImGui::PopItemWidth();
				ImGui::EndChild();
		#ifdef __SWITCH__
			SohImGui::EndGroupPanel(46.0f);
		#else
			SohImGui::EndGroupPanel(14.0f);
		#endif
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
	#ifdef __SWITCH__
		ImGui::SetCursorPosY(cursor.y + 167);
	#else
		ImGui::SetCursorPosY(cursor.y + 120);
	#endif
		SohImGui::BeginGroupPanel("Options", ImVec2(158, 20));
			float cursorX = ImGui::GetCursorPosX() + 5;
			ImGui::SetCursorPosX(cursorX);
			ImGui::Checkbox("Rumble Enabled", &profile->UseRumble);
			if (Backend->CanRumble()) {
				ImGui::SetCursorPosX(cursorX);
				ImGui::Text("Rumble Force: %d%%", static_cast<int>(100.0f * profile->RumbleStrength));
				ImGui::SetCursorPosX(cursorX);
				ImGui::PushItemWidth(135.0f);
				ImGui::SliderFloat("##RStrength", &profile->RumbleStrength, 0.0f, 1.0f, "");
				ImGui::PopItemWidth();
			}
			ImGui::Dummy(ImVec2(0, 5));
		SohImGui::EndGroupPanel(IsKeyboard ? 0.0f : 2.0f);
	}

	void InputEditor::DrawHud() {
		if (!this->Opened) {
			BtnReading = -1;
			CVar_SetS32("gControllerConfigurationEnabled", 0);
			return;
		}

#ifdef __SWITCH__
		ImVec2 minSize = ImVec2(641, 250);
		ImVec2 maxSize = ImVec2(2200, 505);
#else
		ImVec2 minSize = ImVec2(641, 250);
		ImVec2 maxSize = ImVec2(1200, 290);
#endif

		ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
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
