#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
class TimeSplitWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif

typedef struct {
    uint32_t itemID;
    const char* itemName;
    const char* itemImage;
} TimeSplitObject;