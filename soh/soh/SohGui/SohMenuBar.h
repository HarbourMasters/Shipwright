#pragma once

#include <libultraship/libultraship.h>
#include "window/gui/GuiMenuBar.h"
#include "window/gui/GuiElement.h"

namespace SohGui {
class SohMenuBar : public Ship::GuiMenuBar {
  public:
    using Ship::GuiMenuBar::GuiMenuBar;
  protected:
    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override {};
};
} // namespace SohGui