#pragma once

#include <cstdint>
#include <string>

#include <libultraship/libultraship.h>

namespace SOH {

class ExternalModInventoryWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {
    }
    void DrawElement() override;
    void UpdateElement() override {
    }

  private:
    int32_t mSelectedCell = -1;
    std::string mStatusMessage;
    bool mStatusIsError = false;
};

} // namespace SOH
