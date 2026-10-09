#include <ship/window/gui/GuiWindow.h>
#include <vector>
#include <map>

class TimesplitsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override{};
    void Draw() override;
    void UpdateElement() override{};
};

typedef enum ExtendedSplitIds {
    SPLIT_KILLED_ODOLWA = 256,
    SPLIT_KILLED_GOHT,
    SPLIT_KILLED_GYORG,
    SPLIT_KILLED_TWINMOLD,
    SPLIT_KILLED_MAJORA,
    SPLIT_SINGLE_MAGIC,
    SPLIT_DOUBLE_MAGIC,
    SPLIT_DOUBLE_DEFENSE,
};

typedef enum SplitSettings {
    SPLIT_HEADERS,
    SPLIT_OPACITY,
    SPLIT_HIGHLIGHT,
    SPLIT_COMPARE,
};

typedef enum SplitFileActions {
    SPLIT_SAVE,
    SPLIT_LOAD,
    SPLIT_RETRIEVE,
};

typedef enum SplitStatus { SPLIT_INACTIVE, SPLIT_ACTIVE, SPLIT_COMPLETE, SPLIT_SKIPPED };
typedef enum SplitTypes { SPLIT_TYPE_NORMAL, SPLIT_TYPE_SCENE };

typedef struct {
    uint32_t splitId;
    std::string splitName;
    uint32_t splitCurrentTime;
    uint32_t splitPreviousBest;
    uint8_t splitStatus;
    uint32_t splitType;
} TimesplitObject;

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
extern std::map<uint32_t, ImVec4> songColorMap;
extern uint32_t comparedIndex;
extern bool shouldPopUpOpen;
extern TimesplitObject GetSplitObjectById(uint32_t itemId);
extern ImVec2 GetItemImageSizeById(uint32_t itemId);
extern void TableCellCenteredText(ImVec4 color, const char* text);
extern const char* GetItemImageById(uint32_t itemId);
extern void SplitsPushImageButtonStyle();
extern void SplitsPopImageButtonStyle();
extern void HandlePopUpContext(uint32_t popupId);
extern void HandleDragAndDrop(size_t i);
extern void UpdateSplitBests();
extern void UpdateSplitSettings(uint32_t settingName);
extern void SkipSplitEntry(uint32_t index);
extern void AddSplitEntryBySceneId(uint32_t sceneId);
extern void AddSplitEntryById(uint32_t itemId);
extern void RemoveSplitEntry(uint32_t splitId, uint32_t index);
extern void SplitSaveFileAction(uint32_t action, std::string listName);
extern void DrawSplitsList(bool isMain);
extern void SplitLoadComparisonList();
