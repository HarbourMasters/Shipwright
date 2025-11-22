#ifndef SHIP_UTILS_H
#define SHIP_UTILS_H

#include <libultraship/libultraship.h>
//#include "PR/ultratypes.h"

#ifdef __cplusplus

void LoadGuiTextures();

extern "C" {
#endif

struct PlayState;
struct Actor;

f32 Ship_GetExtendedAspectRatioMultiplier();
void Ship_ExtendedCullingActorAdjustProjectedZ(Actor* actor);
void Ship_ExtendedCullingActorAdjustProjectedX(Actor* actor);
void Ship_ExtendedCullingActorRestoreProjectedPos(PlayState* play, Actor* actor);

bool Ship_IsCStringEmpty(const char* str);
void Ship_CreateQuadVertexGroup(Vtx* vtxList, s32 xStart, s32 yStart, s32 width, s32 height, u8 flippedH);
f32 Ship_GetCharFontWidth(u8 character);
void* Ship_GetCharFontTexture(u8 character);

#ifdef __cplusplus
}

namespace ShipUtils {
    void RandInit(uint64_t seed);
    uint32_t Random(uint32_t min, uint32_t max);
    double RandomDouble();

    // Get a random element from a vector or array
    template <typename T> T RandomElement(std::vector<T>& vector, bool erase) {
        const auto idx = Random(0, static_cast<uint32_t>(vector.size()));
        const T selected = vector[idx];
        if (erase) {
            vector.erase(vector.begin() + idx);
        }
        return selected;
    }
    template <typename Container> auto& RandomElement(Container& container) {
        return container[Random(0, static_cast<uint32_t>(std::size(container)))];
    }
    template <typename Container> const auto& RandomElement(const Container& container) {
        return container[Random(0, static_cast<uint32_t>(std::size(container)))];
    }

    template <typename T> const T RandomElementFromSet(const std::set<T>& set) {
        if (set.size() == 1) {
            return *set.begin();
        }
        uint32_t rand = Random(0, static_cast<uint32_t>(set.size()));
        auto it = set.begin();
        for (uint32_t i = 0; i < rand; i++) {
            it++;
        }
        auto test = *it;
        return *it;
    }
}
#endif

#endif // SHIP_UTILS_H
