#pragma once

typedef enum { COLVIEW_DISABLED, COLVIEW_SOLID, COLVIEW_TRANSPARENT } ColViewerRenderSetting;

#ifdef __cplusplus
#include "soh/SohGui/LocalizedWindow.h"
class ColViewerWindow final : public SohGui::LocalizedWindow {
  public:
    using LocalizedWindow::LocalizedWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif
