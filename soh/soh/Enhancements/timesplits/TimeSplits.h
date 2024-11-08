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

typedef enum SplitAction {
    SPLIT_ACTION_SAVE,
    SPLIT_ACTION_LOAD,
    SPLIT_ACTION_UPDATE,
    SPLIT_ACTION_COLLECT,
    SPLIT_ACTION_DELETE
} SplitAction;

typedef enum SplitStatus {
    SPLIT_STATUS_ACTIVE,
    SPLIT_STATUS_INACTIVE,
    SPLIT_STATUS_COLLECTED,
    SPLIT_STATUS_SKIPPED
} SplitStatus;

typedef enum SplitType {
    SPLIT_TYPE_ITEM,
    SPLIT_TYPE_UPGRADE,
    SPLIT_TYPE_EQUIPMENT,
    SPLIT_TYPE_QUEST,
    SPLIT_TYPE_BOSS,
    SPLIT_TYPE_ENTRANCE,
    SPLIT_TYPE_MISC
} SplitType;

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
