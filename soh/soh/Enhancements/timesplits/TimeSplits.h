#pragma once

#include <ship/window/gui/GuiWindow.h>

#include <vector>
#include <map>

#ifdef __cplusplus
namespace TimeSplits {

class TimesplitsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override{};
    void Draw() override;
    void UpdateElement() override{};
};

typedef enum {
    SPLIT_HEADERS,
    SPLIT_OPACITY,
    SPLIT_HIGHLIGHT,
    SPLIT_COMPARE,
} SplitSettings;

typedef enum {
    SPLIT_SAVE,
    SPLIT_LOAD,
    SPLIT_RETRIEVE,
    SPLIT_CONVERT,
} SplitFileActions;

typedef enum { SPLIT_INACTIVE, SPLIT_ACTIVE, SPLIT_COMPLETE, SPLIT_SKIPPED } SplitStatus;

// Split ids come from several enums, the type says which one an id belongs to.
typedef enum {
    SPLIT_TYPE_ITEM,  // ITEM_*
    SPLIT_TYPE_SCENE, // SCENE_*
    SPLIT_TYPE_BOSS,  // ACTOR_* of the boss
} SplitTypes;

typedef struct {
    uint32_t splitId;
    std::string splitName;
    uint32_t splitCurrentTime;
    uint32_t splitPreviousBest;
    uint8_t splitStatus;
    uint32_t splitType;
} TimesplitObject;

#define SPLIT_BOSS(actorId, name) \
    { actorId, name, 0, 0, SPLIT_INACTIVE, SPLIT_TYPE_BOSS }

typedef struct {
    uint32_t timeDisplay;
    ImVec4 colorDisplay;
} SplitTextObject;

typedef struct {
    uint32_t startIndex;
    uint32_t endIndex;
} IndexRangeObject;

extern std::vector<TimesplitObject> splitList;
extern std::vector<TimesplitObject> comparisonList;
extern std::vector<TimesplitObject> splitObjectList;
extern std::vector<TimesplitObject> sceneObjectList;
extern std::vector<std::string> savedLists;
extern std::map<uint32_t, std::vector<uint32_t>> itemSubMenuList;
extern uint32_t comparedIndex;
extern bool shouldPopUpOpen;
extern uint32_t GetTotalTime();
extern ImVec4 GetSplitColor(const TimesplitObject& split);
extern TimesplitObject GetSplitObject(uint32_t splitType, uint32_t splitId);
extern ImVec2 GetSplitImageSize(const TimesplitObject& split);
extern void TableCellCenteredText(ImVec4 color, const char* text);
extern const char* GetSplitImage(const TimesplitObject& split);
extern void SplitsPushImageButtonStyle();
extern void SplitsPopImageButtonStyle();
extern void HandlePopUpContext(uint32_t popupId);
extern void HandleDragAndDrop(size_t i);
extern void UpdateSplitBests();
extern void UpdateSplitSettings(uint32_t settingName);
extern void SkipSplitEntry(uint32_t index);
extern void AddSplitEntryBySceneId(uint32_t sceneId);
extern void AddSplitEntry(uint32_t splitType, uint32_t splitId);
extern void RemoveSplitEntry(uint32_t index);
extern void SplitSaveFileAction(uint32_t action, std::string listName);
extern void DrawSplitsList(bool isMain);
extern void SplitLoadComparisonList();

} // namespace TimeSplits

#endif
