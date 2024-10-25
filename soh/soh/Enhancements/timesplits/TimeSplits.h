#pragma once
#ifndef TIMESPLITS_H
#define TIMESPLITS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // TIMESPLITS_H

#include <libultraship/libultraship.h>

#ifdef __cplusplus
class TimeSplitWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void Draw() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

typedef enum {
    ACTION_SAVE,
    ACTION_LOAD,
    ACTION_UPDATE,
    ACTION_COLLECT,
    ACTION_DELETE
};

typedef enum {
    SPLIT_ACTIVE,
    SPLIT_INACTIVE,
    SPLIT_COLLECTED,
    SPLIT_SKIPPED
};

typedef enum {
    SPLIT_ITEM,
    SPLIT_UPGRADE,
    SPLIT_EQUIPMENT,
    SPLIT_QUEST,
    SPLIT_BOSS,
    SPLIT_ENTRANCE,
    SPLIT_MISC
};

typedef struct {
    uint32_t splitType;
    uint32_t splitID;
    std::string splitName;
    std::string splitImage;
    ImVec4 splitTint;
    uint32_t splitTimeCurrent;
    uint32_t splitTimeBest;
    uint32_t splitTimePreviousBest;
    uint32_t splitTimeStatus;
    uint32_t splitSkullTokenCount;
} SplitObject;

#endif
