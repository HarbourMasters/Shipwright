#pragma once

#include <libultraship/libultraship.h>

namespace SohGui {
class SohMenuBar : public LUS::GuiMenuBar {
  public:
    void DrawElement() override;
    void InitElement() override {};
    void UpdateElement() override {};
};
} // namespace SohGui