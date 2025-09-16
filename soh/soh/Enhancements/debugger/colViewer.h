#pragma once

#include <libultraship/libultraship.h>

typedef enum { COLVIEW_DISABLED, COLVIEW_SOLID, COLVIEW_TRANSPARENT } ColViewerRenderSetting;

#ifdef __cplusplus
class ColViewerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif
