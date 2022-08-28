#include "DummyController.h"

namespace Ship {
	DummyController::DummyController(const std::string& CUID, const std::string& KeyName, bool Connected) {
		GUID = CUID;
		isConnected = Connected;
		ButtonName = KeyName;
	}

	void DummyController::ReadFromSource(int32_t virtualSlot) {
		
	}

	const std::string DummyController::GetControllerName() {
		return GUID;
	}

	const std::string DummyController::GetButtonName(int32_t virtualSlot, int32_t n64Button) {
		return ButtonName;
	}

	void DummyController::WriteToSource(int32_t virtualSlot, ControllerCallback* controller){
		
	}

	bool DummyController::Connected() const {
		return isConnected;
	}

	bool DummyController::CanRumble() const {
		return false;
	}

	bool DummyController::CanGyro() const {
		return false;
	}

	void DummyController::CreateDefaultBinding(int32_t slot) {
		
	}

	std::string DummyController::GetControllerType() {
		return "Unk";
	}

	std::string DummyController::GetConfSection() {
		return "Unk";
	}

	std::string DummyController::GetBindingConfSection() {
		return "Unk";
	}

	void DummyController::ClearRawPress() {
		
	}

	int32_t DummyController::ReadRawPress() {
		return -1;
	}
}
