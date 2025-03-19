#pragma once
#ifndef PLANDOMIZER_H
#define PLANDOMIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // PLANDOMIZER_H

#include <libultraship/libultraship.h>
#include "soh/Enhancements/randomizer/item.h"

#ifdef __cplusplus
class PlandomizerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

typedef struct {
    std::string checkName;
    Rando::Item checkRewardItem;
    int32_t shopPrice;
    Rando::Item iceTrapModel;
    std::string iceTrapName;
} SpoilerCheckObject;

typedef struct {
    std::string hintName;
    std::string hintType;
    std::string hintText;
} SpoilerHintObject;

typedef enum PlandoTabs {
    TAB_HINTS,
    TAB_LOCATIONS
} PlandoTabs;

typedef enum PlandoHints {
    HINT_SINGLE,
    HINT_ALL,
} PlandoHints;

#endif