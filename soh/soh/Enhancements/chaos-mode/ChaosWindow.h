#ifndef CHAOSWINDOW_H
#define CHAOSWINDOW_H

#include "GuiWindow.h"
#include "z64.h"
#include <vector>

extern void ChaosUpdateFrameMultiplier();
extern void ChaosUpdateVotingInterval();

typedef enum {
    COLOR_WHITE,
    COLOR_GRAY,
    COLOR_DARK_GRAY,
    COLOR_INDIGO,
    COLOR_LIGHT_RED,
    COLOR_RED,
    COLOR_DARK_RED,
    COLOR_LIGHT_GREEN,
    COLOR_GREEN,
    COLOR_DARK_GREEN,
    COLOR_YELLOW,
};

typedef struct {
    uint32_t colorName;
    ImVec4 colorCode;
} colorObject;

typedef struct {
    uint32_t eventId;
    const char* eventName;
    const char* eventVariable;
    uint32_t eventTimer;
    const char* eventDescription;
} eventObject;

typedef struct {
    uint32_t votingOption;
    uint32_t votingCount;
} voteObject;

typedef enum {
    EVENT_ACTION_REMOVE,
    EVENT_ACTION_ADD,
};

typedef enum {
    EVENT_INVISIBILITY,
    EVENT_STORMY_WEATHER,
    EVENT_FORCE_IRON_BOOTS,
    EVENT_FORCE_HOVER_BOOTS,
    EVENT_BOMBERMAN_MODE,
    EVENT_FLOOR_IS_LAVA,
    EVENT_FAKE_TELEPORT,
    EVENT_REAL_TELEPORT,
    EVENT_ACTOR_MAGNET,
    EVENT_DEATH_SWITCH,
    EVENT_KNUCKLE_RING,
};

typedef enum {
    VOTE_OPTION_A,
    VOTE_OPTION_B,
    VOTE_OPTION_C,
};

class ChaosWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement();
    void DrawElement();
    void UpdateElement() override{};
};

#endif // CHAOSWINDOW_H
