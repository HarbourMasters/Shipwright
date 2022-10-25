#pragma once
#include "Controller.h"
#include <string>

namespace Ship {
    class WiiUGamepad : public Controller {
        public:
            WiiUGamepad();

            bool Open();
            void Close();

            void ReadFromSource(int32_t virtualSlot) override;
            void WriteToSource(int32_t virtualSlot, ControllerCallback* controller) override;
            bool Connected() const override { return connected; };
            bool CanGyro() const override { return true; }
            bool CanRumble() const override { return true; };

            void ClearRawPress() override;
            int32_t ReadRawPress() override;

            const std::string GetButtonName(int32_t virtualSlot, int n64Button) override;
            const std::string GetControllerName() override;

        protected:
            void NormalizeStickAxis(int32_t virtualSlot, float x, float y, uint16_t threshold, bool isRightStick);
            void CreateDefaultBinding(int32_t virtualSlot) override;

        private:
            bool connected = true;
            float rumblePatternStrength;
            uint8_t rumblePattern[15];
    };
}
