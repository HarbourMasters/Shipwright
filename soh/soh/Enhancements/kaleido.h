#ifndef KALEIDO_H
#define KALEIDO_H
#include <z64.h>


#ifdef __cplusplus
#include <vector>
#include <string>
#include <array>

namespace Rando {

/**
 * Base class for all Kaleido Entries in our Rando Collectibles list.
 * Stores the common aspects such as the line of text, positioning,
 * vertices, and whether or not the goal has been achieved (subclasses
 * can use this to change their rendering). Also sets an interface for
 * subclasses to declare their Draw and Update functions.
 */
class KaleidoEntry {
public:
    /**
     * @brief Constructor for Base KaleidoEntry class. Sets the position and
     * initial value of the line of text.
     * @param x x coordinate relative to the current matrix origin.
     * @param y y coordinate relative to the current matrix origin.
     * @param text the initial value of the line of text. Can be omitted for an
     * empty string.
     */
    KaleidoEntry(int16_t x, int16_t y, std::string text = "");
    virtual void Draw(PlayState* play, std::vector<Gfx>* mEntryDl) = 0;
    virtual void Update(PlayState* play) = 0;
    void SetYOffset(int yOffset);
protected:
    int16_t mX;
    int16_t mY;
    int16_t mHeight;
    int16_t mWidth;
    Vtx* vtx;
    std::string mText;
    bool mAchieved = false;

};

/**
 * Subclass of KaleidoEntry intended to be a parent class for any KaleidoEntry subclasses
 * that wish to render an Icon at the start of their line.
 */
class KaleidoEntryIcon : public KaleidoEntry {
public:
    /**
     * @param iconResourceName resource name of the icon to draw
     * @param iconFormat flag representing the format of the icon (i.e. G_IM_FMT_IA)
     * @param iconSize flag representing the size of the icon in bytes (i.e. G_IM_SIZ_8b)
     * @param iconWidth pixel width of the source icon image
     * @param iconHeight pixel height of the source icon image
     * @param iconColor Color to shade the icon with. This may be ignored for certain icon formats
     * @param x x coordinate of the location to draw this relative to the parent matrix's origin.
     * @param y y coordinate of the location to draw this relative to the parent matrix's origin.
     * @param text text to draw to the right of the icon.
     */
    KaleidoEntryIcon(const char* iconResourceName, int iconFormat, int iconSize, int iconWidth, int iconHeight,
                     Color_RGBA8 iconColor, int16_t x, int16_t y, std::string text = "");
    void Draw(PlayState* play, std::vector<Gfx>* mEntryDl) override;
    void RebuildVertices();
protected:
    const char* mIconResourceName;
    int mIconFormat;
    int mIconSize;
    int mIconWidth;
    int mIconHeight;
    Color_RGBA8 mIconColor;

    void BuildVertices();
    void LoadIconTex(std::vector<Gfx>* mEntryDl);
};

/**
 * Class representing a Kaleido Entry that can be represented with an icon
 * that is either colored in or Grayscale according to a flag
 */
class KaleidoEntryIconFlag : public KaleidoEntryIcon {
public :
    /**
     * @param iconResourceName resource name of the icon to draw
     * @param iconFormat flag representing the format of the icon (i.e. G_IM_FMT_IA)
     * @param iconSize flag representing the size of the icon in bytes (i.e. G_IM_SIZ_8b)
     * @param iconWidth pixel width of the source icon image
     * @param iconHeight pixel height of the source icon image
     * @param iconColor Color to shade the icon with. This may be ignored for certain icon formats
     * @param flagType FlagType of the flag to check for (i.e. FlagType::FLAG_RANDOMIZER_INF
     * @param flag flag to check for. An integer can be provided but enum values should be preferred
     * @param x x coordinate of the location to draw this relative to the parent matrix's origin.
     * @param y y coordinate of the location to draw this relative to the parent matrix's origin.
     * @param mName name to draw to the right of the icon. Leave blank to omit.
     */
    KaleidoEntryIconFlag(const char* iconResourceName, int iconFormat, int iconSize, int iconWidth, int iconHeight,
                         Color_RGBA8 iconColor, FlagType flagType, int flag, int16_t x, int16_t y,
                         std::string name = "");
    void Update(PlayState* play) override;
private:
    FlagType mFlagType;
    int mFlag;
};

/**
 * KaleidoEntryIcon subclass to render a count of how many collectibles have been collected.
 * The `required` and `total` values can be omitted from the constructor or set to 0 to only
 * render the count and not show progress towards a required amount or a total.
 */
class KaleidoEntryIconCountRequired : public KaleidoEntryIcon {
public:
    /**
     * @param iconResourceName resource name of the icon to draw
     * @param iconFormat flag representing the format of the icon (i.e. G_IM_FMT_IA)
     * @param iconSize flag representing the size of the icon in bytes (i.e. G_IM_SIZ_8b)
     * @param iconWidth pixel width of the source icon image
     * @param iconHeight pixel height of the source icon image
     * @param iconColor Color to shade the icon with. This may be ignored for certain icon formats
     * @param flagType FlagType of the flag to check for (i.e. FlagType::FLAG_RANDOMIZER_INF
     * @param flag flag to check for. An integer can be provided but enum values should be preferred
     * @param x x coordinate of the location to draw this relative to the parent matrix's origin.
     * @param y y coordinate of the location to draw this relative to the parent matrix's origin.
     * @param watch a pointer to an integer value to watch. Update will check this value to update
     * a local count variable.
     * @param required The amount of this collectible required to beat the seed. Set to 0 to not render.
     * @param total The amount of this collectible available in the seed. Set to 0 to not render.
     */
    KaleidoEntryIconCountRequired(const char* iconResourceName, int iconFormat, int iconSize, int iconWidth, int iconHeight,
                                  Color_RGBA8 iconColor, int16_t x, int16_t y, int* watch, int required = 0, int total = 0);
    void Update(PlayState* play) override;
private:
    int* mWatch;
    int mRequired;
    int mTotal;
    int mCount;

    void BuildText();
};

class KaleidoEntryOcarinaButtons : public KaleidoEntryIcon {
public:
    KaleidoEntryOcarinaButtons(int16_t x, int16_t y);
    void Update(PlayState* play) override;
    void Draw(PlayState* play, std::vector<Gfx>* mEntryDl) override;
private:
    void CalculateColors();

    std::array<Color_RGBA8, 5> mButtonColors = {};
    std::array<bool, 5> mButtonCollected = {};
};

class Kaleido {
public:
    Kaleido();
    void Draw(PlayState* play);
    void Update(PlayState* play);
private:
    std::vector<std::shared_ptr<KaleidoEntry>> mEntries;
    std::vector<Gfx> mEntryDl;
    int mTopIndex = 0;
    int mNumVisible = 7;
};
} // Rando

extern "C" {
#endif
void RandoKaleido_DrawMiscCollectibles(PlayState* play);
void RandoKaleido_UpdateMiscCollectibles(int16_t inDungeonScene);
#ifdef __cplusplus
}
#endif
void RandoKaleido_RegisterHooks();


#endif //KALEIDO_H
