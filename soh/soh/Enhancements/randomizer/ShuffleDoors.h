#pragma once

#define DOOR_KEY(scene, transitionIdx) (((scene) << 8) | (u8)(transitionIdx))

struct Door {
    s16 scene;
    s8 srcRoom;
    s8 dstRoom;
    // index into the scene's transition actor list, same in vanilla & MQ
    s8 transitionIdx;
    // where Link stands on srcRoom's side, facing away from the door
    s16 linkX;
    s16 linkY;
    s16 linkZ;
    s16 rotY;
};
