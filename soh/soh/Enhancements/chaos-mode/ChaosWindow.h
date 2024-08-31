#ifndef CHAOSWINDOW_H
#define CHAOSWINDOW_H

#include "GuiWindow.h"
#include "z64.h"
#include <vector>

typedef struct {
    uint32_t eventId;
    const char* eventName;
    uint32_t eventTimer;
} eventObject;

typedef enum {
    EVENT_ACTION_REMOVE,
    EVENT_ACTION_ADD,
};

typedef enum {
    EVENT_INVISIBILITY,
    EVENT_STORMY_WEATHER,
};

class ChaosWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement();
    void DrawElement();
    void UpdateElement() override{};
};

#endif // CHAOSWINDOW_H
