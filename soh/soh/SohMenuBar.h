#pragma once

#include <libultraship/libultraship.h>

namespace SohGui {
class SohMenuBar : public LUS::GuiMenuBar {
  public:
    void Draw() override;
};
} // namespace SohGui