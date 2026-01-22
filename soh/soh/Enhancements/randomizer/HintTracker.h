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
    void LoadHintTable(int32_t fileNum);

  private:
    std::array<Rando::Hint, RH_MAX>* mHintTable = nullptr;
};
} // namespace HintTracker