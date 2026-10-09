#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
void ModMenu_LoadArchives();

class ModMenuWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
#endif