#ifndef KALEIDO_H
#define KALEIDO_H
#include <z64.h>


#ifdef __cplusplus
#include <vector>
#include <string>
namespace Rando {

class KaleidoEntry {
public:
    KaleidoEntry(int16_t x, int16_t y, std::string text = "");
    virtual void Draw(PlayState* play, std::vector<Gfx>* mEntryDl) = 0;
    virtual void Update(PlayState* play) = 0;
protected:
    int16_t mX;
    int16_t mY;
    int16_t mHeight;
    int16_t mWidth;
    Vtx* vtx;
    std::string mText;
    bool mAchieved = false;
};

class KaleidoEntryIcon : public KaleidoEntry {
public:
    KaleidoEntryIcon(const char* iconResourcename, int iconFormat, int iconSize, int iconWidth, int iconHeight,
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
     * @param mName name to draw under the icon. Leave blank to omit.
     */
    KaleidoEntryIconFlag(const char* iconResourceName, int iconFormat, int iconSize, int iconWidth, int iconHeight,
                         Color_RGBA8 iconColor, FlagType flagType, int flag, int16_t x, int16_t y,
                         std::string name = "");
    void Update(PlayState* play) override;
private:
    FlagType mFlagType;
    int mFlag;
};

class KaleidoEntryIconCountRequired : public KaleidoEntryIcon {
public:
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

class Kaleido {
public:
    Kaleido();
    void Draw(PlayState* play);
    void Update(PlayState* play);
private:
    std::vector<std::shared_ptr<KaleidoEntry>> mEntries;
    std::vector<Gfx> mEntryDl;
};
} // Rando

extern "C" {
#endif
void RandoKaleido_DrawMiscCollectibles(PlayState* play);
void RandoKaleido_UpdateMiscCollectibles(PlayState* play);
#ifdef __cplusplus
}
#endif

#endif //KALEIDO_H
