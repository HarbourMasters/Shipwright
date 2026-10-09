#ifndef DEVELOPER_TOOLS_H
#define DEVELOPER_TOOLS_H

enum DebugSaveInfo {
    DEBUG_SAVE_INFO_NONE,
    DEBUG_SAVE_INFO_VANILLA_DEBUG,
    DEBUG_SAVE_INFO_COMPLETE,
};

void RegisterDebugSaveCreate();
void RegisterPreventActorUpdateHooks();
void RegisterPreventActorDrawHooks();
void RegisterPreventActorInitHooks();
void RenderWarpPointSection();

#endif // DEVELOPER_TOOLS_H
