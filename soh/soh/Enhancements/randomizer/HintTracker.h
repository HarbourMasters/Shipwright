#pragma once

#include "soh/Enhancements/randomizer/hint.h"
#include <libultraship/libultraship.h>

namespace HintTracker {
class HintTrackerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void Draw() override;
    ~HintTrackerWindow(){};

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
} // namespace HintTracker