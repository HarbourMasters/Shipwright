#ifndef _NAMETAG_H_
#define _NAMETAG_H_
#include <z64.h>

typedef struct {
    const char* tag; // Tag identifier to filter/remove multiple tags 
    int16_t yOffset; // Additional Y offset to apply for the name tag
    Color_RGBA8 textColor; // Text color override. Global color is used if alpha is 0
} NameTagOptions;

// Register required hooks for nametags on startup
void NameTag_RegisterHooks();

#ifdef __cplusplus
extern "C" {
#endif

// Registers a name tag to an actor with additional options applied
void NameTag_RegisterForActorWithOptions(Actor* actor, const char* text, NameTagOptions options);
// Registers a name tag to an actor. Multiple name tags can exist for the same actor
void NameTag_RegisterForActor(Actor* actor, const char* text);
// Remove all name tags registered to a specific actor
void NameTag_RemoveAllForActor(Actor* actor);
// Remove all name tags that share the same tag identifier
void NameTag_RemoveAllByTag(const char* tag);

#ifdef __cplusplus
}
#endif

#endif // _NAMETAG_H_
