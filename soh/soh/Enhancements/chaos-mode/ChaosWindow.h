#ifndef CHAOSWINDOW_H
#define CHAOSWINDOW_H

#include "GuiWindow.h"
#include "z64.h"
#include <vector>
#include "Fonts.h"

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

typedef struct {
    uint32_t eventTimer;
    const char* eventVariable;
} eventTimerObject;

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
    EVENT_MIDO_SUCKS,
    EVENT_THROWN_IN_THE_PAST,
    EVENT_ERASURE,
    EVENT_FALLING_CEILING,
    EVENT_FORCE_STOP_HEARTS,
    EVENT_SPIKE_TRAP,
    EVENT_FLOATING_STUFF,
    EVENT_DROP_RUPEES,
    EVENT_FIRE_MAZE,
    EVENT_CARDINALS_REVENGE,
    EVENT_MAX_CHAOS,
} ChaosEvents;

typedef enum {
    VOTE_OPTION_A,
    VOTE_OPTION_B,
    VOTE_OPTION_C,
};

typedef enum {
    BUTTON_CLEFT = 1,
    BUTTON_CDOWN,
    BUTTON_CRIGHT,
    BUTTON_DUP,
    BUTTON_DDOWN,
    BUTTON_DLEFT,
    BUTTON_DRIGHT,
};

class ChaosWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement();
    void DrawElement();
    void UpdateElement() override{};
};

#endif // CHAOSWINDOW_H
