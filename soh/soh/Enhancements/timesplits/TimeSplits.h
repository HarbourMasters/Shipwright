#pragma once
#ifndef TIMESPLITS_H
#define TIMESPLITS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void TimeSplitSplitsHandlerS(uint32_t itemID);

#ifdef __cplusplus
}
#endif

#endif // TIMESPLITS_H

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

typedef enum {
    START_VALUE = 656,
    BOSS_QUEEN_GOHMA,
    BOSS_KING_DODONGO,
    BOSS_BARINADE,
    BOSS_PHANTOM_GANON,
    BOSS_VOLVAGIA,
    BOSS_MORPHA,
    BOSS_TWINROVA,
    BOSS_BONGO_BONGO,
    BOSS_GANONDORF,
    BOSS_GANON
};