#ifndef KALEIDO_H
#define KALEIDO_H
#include <z64.h>


#ifdef __cplusplus
#include <vector>
#include <string>
namespace Rando {

class KaleidoEntry {
public:
    KaleidoEntry(const char* iconResourceName, std::string name, std::string value, int16_t x, int16_t y);
    void Draw(PlayState* play, std::vector<Gfx>* mEntryDl);
private:
    const char* mIconResourceName;
    std::string mName;
    std::string mValue;
    int16_t mX;
    int16_t mY;
    int16_t mHeight;
    int16_t mWidth;
//    Mtx* mtx;
    Vtx* vtx;
};

class Kaleido {
public:
    Kaleido();
    void Draw(PlayState* play);
private:
    std::vector<KaleidoEntry> mEntries;
    std::vector<Gfx> mEntryDl;
};
} // Rando

extern "C" {
#endif
void RandoKaleido_DrawMiscCollectibles(PlayState* play);
#ifdef __cplusplus
}
#endif

#endif //KALEIDO_H
