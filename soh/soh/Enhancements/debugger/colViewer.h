#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
extern "C"
#endif
void DrawColViewer();

typedef enum {
  COLVIEW_DISABLED,
  COLVIEW_SOLID,
  COLVIEW_TRANSPARENT
} ColViewerRenderSetting;

#ifdef __cplusplus
class ColViewerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};

#endif