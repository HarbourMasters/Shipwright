#include "ActorAccessibility.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"

static const f32 detectionDistance = 500.0;
static const f32 speedXZ = 2.0;
static const f32 speedRate = 500.0;
static const s16 wallCueSound = NA_SE_IT_BOMB_EXPLOSION;

static int framesUntilCue = 10;

/*
Computes a relative angle based on Link's (or some other actor's) current angle.
*/
Vec3s computeRelativeAngle(Vec3s& origin, Vec3s& offset) {
    Vec3s rot = origin;
    rot.x += offset.x;
    rot.y += offset.y;
    rot.z += offset.z;
    return rot;

}
    void accessible_va_ledge_cue(AccessibleActor * actor) {
    Player* player = GET_PLAYER(actor->play);

}
void accessible_va_wall_cue(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
//The virtual cue actors travel in lines relative to Link's angle.
Vec3s rot = computeRelativeAngle(player->actor.world.rot, actor->world.rot);
    Vec3f velocity;
    velocity.x = Math_SinS(rot.y);
    velocity.z = Math_CosS(rot.y);
//Draw a line from Link's position to the max detection distance based on the configured relative angle.
    Vec3f pos = player->actor.world.pos;
    Vec3f headPos = player->bodyPartsPos[PLAYER_BODYPART_TORSO];
    f32 headDistY = headPos.y - pos.y;

    f32 step = fabs(velocity.x + velocity.z);
    f32 distToTravel = detectionDistance;
    CollisionPoly* poly = NULL;
    Vec3f collisionResult;
    s32 bgId = 0;
    while (true) {
        Vec3f prevPos = pos;
        Vec3f prevHeadPos = headPos;

        pos.x += velocity.x;
        pos.y += 1;
        pos.z += velocity.z;
        headPos.x += velocity.x;
        headPos.y += 1;
        headPos.z += velocity.z;

        distToTravel -= step;
        f32 floorHeight = 0;
        CollisionPoly tempPoly;
        floorHeight = BgCheck_AnyRaycastFloor1(&actor->play->colCtx, &tempPoly, &pos);
            pos.y = floorHeight;
        headPos.y = floorHeight + headDistY;

        if (BgCheck_AnyLineTest3(&actor->play->colCtx, &prevPos, &pos, &collisionResult, &poly, 1, 0, 0, 0, &bgId)) {
                s16 wallYaw = Math_Atan2S(poly->normal.z, poly->normal.x);
            CollisionPoly* headPoly = NULL;

                BgCheck_AnyLineTest3(&actor->play->colCtx, &prevHeadPos, &headPos, &collisionResult, &headPoly, 1, 0, 0, 0,
                                 &bgId);
            if (poly == headPoly)
                break;

            poly = NULL;

        }
        
        if (distToTravel <= 0)
            break;

    }
    if (poly == NULL) {
        Audio_StopSfxByPosAndId(&actor->world.pos, wallCueSound);
        return; // Nothing found.

    }
    // Move the virtual actor to the position of the discovered wall so we can use preexisting sound
                   // logic.
    actor->world.pos = collisionResult;
    framesUntilCue--;
    if (framesUntilCue < 1) {
        framesUntilCue = 10;
        ActorAccessibility_PlaySpecialSound(actor, wallCueSound);

    }

    }
