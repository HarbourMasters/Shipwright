#include <libultraship/libultraship.h>

class TimeDisplayWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override {};
    void Draw() override;
    void UpdateElement() override {};
};

void TimeDisplayUpdateDisplayOptions(uint32_t timeID, bool pushBack);
void TimeDisplayInitSettings();

typedef enum {
    DISPLAY_IN_GAME_TIMER,
    DISPLAY_TIME_OF_DAY
};

typedef struct {
    uint32_t timeID;
    const char* timeEnable;
} TimeObject;