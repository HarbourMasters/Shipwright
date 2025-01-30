#include <libultraship/libultraship.h>

class TimeDisplayWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override {};
    void Draw() override;
    void UpdateElement() override {};
};

void TimeDisplayUpdateDisplayOptions();
void TimeDisplayInitSettings();

typedef enum {
    DISPLAY_IN_GAME_TIMER,
    DISPLAY_TIME_OF_DAY,
    DISPLAY_CONDITIONAL_TIMER,
    DISPLAY_NAVI_TIMER
};

typedef enum {
    NAVI_PREPARE = 600,
    NAVI_ACTIVE = 3000,
    NAVI_COOLDOWN = 25800,
    DAY_BEGINS = 17759,
    NIGHT_BEGINS = 49155
};

typedef struct {
    uint32_t timeID;
    std::string timeLabel;
    const char* timeEnable;
} TimeObject;

extern const std::vector<TimeObject> timeDisplayList;