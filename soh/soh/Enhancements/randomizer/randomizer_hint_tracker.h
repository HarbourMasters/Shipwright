#pragma once

#include <ship/window/gui/GuiWindow.h>
#include "randomizerTypes.h"

typedef enum {
    // Every hint location grouped by area, with unread hint text masked.
    HINT_TRACKER_VIEW_LOCATIONS,
    // Only hints the player has read, grouped by hint type by usefulness.
    HINT_TRACKER_VIEW_JOURNAL,
} HintTrackerViewMode;

namespace HintTracker {

class HintTrackerSettingsWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

  protected:
    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
};

class HintTrackerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void Draw() override;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
} // namespace HintTracker
