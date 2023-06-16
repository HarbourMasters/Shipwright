#include "ActorAccessibility.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"


static const f32 detectionDistance = 500.0;
static const f32 minInclineDistance = 1.0;
static const f32 minDeclineDistance = 1.0;

enum { DISCOVERED_NOTHING = 0,
DISCOVERED_INCLINE,
DISCOVERED_DECLINE,
DISCOVERED_LEDGE,
DISCOVERED_WALL,
};
//Abstract class for terrain cue sound handling. Implementations should not allocate memory. These are always in-place constructed in static memory owned by the TerrainCueDirection object.
class TerrainCueSound {
  protected:
    AccessibleActor* actor;
    Vec3f terrainPos;
    Vec3f terrainProjectedPos;
    f32 currentPitch;
    f32 currentVolume;
    f32 xzDistToPlayer;
    s16 currentSFX;
    s8 currentReverb;
    // Call to start playback.
    void play() {
        Audio_PlaySoundGeneral(currentSFX, &terrainProjectedPos, 255, &currentPitch, &currentVolume, &currentReverb);
    }

    // Call when terrain is no longer present to stop playback.
    virtual void stop() {
        Audio_StopSfxByTokenAndId(255, currentSFX);
    }

    //Custom terrain sound behaviour.
    virtual void run() = 0;
    // Update sound position and volume once per frame.
    virtual void updatePositions(Vec3f& pos) {
        terrainPos = pos;
        Player* player = GET_PLAYER(actor->play);

        f32 w = 0.0f;
        // Set projectedPos.
        SkinMatrix_Vec3fMtxFMultXYZW(&actor->play->viewProjectionMtxF, &terrainPos, &terrainProjectedPos, &w);

        // Set xzDistToPlayer.
        xzDistToPlayer = Math_Vec3f_DistXZ(&terrainPos, &player->actor.world.pos);
        currentVolume = ActorAccessibility_ComputeCurrentVolume(actor->policy.distance, xzDistToPlayer);
    }

  public:
    TerrainCueSound(AccessibleActor* actor, Vec3f pos) {
        this->actor = actor;
        currentPitch = 1.0;
        currentVolume = 1.0;
        currentReverb = 0;
        xzDistToPlayer = 0;
        currentSFX = 0;
        updatePositions(pos);

    }
    virtual ~TerrainCueSound() {
        stop();

    }
    void update(Vec3f& pos) {
        updatePositions(pos);
        run();

    }
};
class Incline : protected TerrainCueSound {
    int restFrames;

  public:
    Incline(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        restFrames = 0;
        currentPitch = 0.5;
        currentSFX = NA_SE_EV_WIND_TRAP;

        play();
    }
    virtual ~Incline() {
    }
    virtual void run() {
        if (restFrames > 0) {
            restFrames--;
            if (restFrames == 0)
                play();

            return;
        }
        currentPitch += 0.1;
        if (currentPitch >= 2.0) {
            stop();
            currentPitch = 0.5;
            restFrames = 5;
        }
    }
};

class Decline : protected TerrainCueSound {
        int restFrames;

      public:
        Decline(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
            restFrames = 0;
            currentPitch = 2.0;
            currentSFX = NA_SE_EV_WIND_TRAP;

            play();
        }
        virtual ~Decline() {
        }
        virtual void run() {
            if (restFrames > 0) {
                restFrames--;
                if (restFrames == 0)
                    play();

                return;
            }
            currentPitch -= 0.1;
            if (currentPitch < 0.5) {
                stop();
                currentPitch = 2.0;
                restFrames = 5;
            }
        }
    };
class Ledge :protected TerrainCueSound {
      public:
    Ledge(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
            currentPitch = 0.4;
        currentReverb = 1;
            currentSFX = NA_SE_EV_WIND_TRAP;
            play();

    }
    virtual ~Ledge() {
    
    }
    void run() {
    //No implementation.

    }
    };
class Wall: protected TerrainCueSound {
        int frames;

  public:
    Wall(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 0.5;
        currentReverb = 1;
        currentSFX = NA_SE_IT_SWORD_CHARGE;
        frames = 0;

        play();
    }
    virtual ~Wall() {
    }
    void run() {
        frames++;
        if (frames == 20) {
            frames = 0;
            play();

        }
    }
};

    class TerrainCueDirection
{
    AccessibleActor* actor;
    Vec3s relRot;//Relative angle.
    Vec3s rot;//Actual angle.
    int terrainDiscovered;

    union {
        Incline incline;
        Decline decline;
        Ledge ledge;
        Wall wall;

    };
    TerrainCueSound* currentSound;
  public:
    // Initialize a TerrainCueDirection based on a relative angle.
    void init(AccessibleActor* actor, Vec3s rot) {
        this->actor = actor;
        this->relRot = rot;
        this->rot = { 0, 0, 0 };
        terrainDiscovered = DISCOVERED_NOTHING;
        currentSound = NULL;

    }
    void destroyCurrentSound() {
        if (currentSound == NULL)
            return;
            currentSound->~TerrainCueSound();
            currentSound = NULL;
            terrainDiscovered = DISCOVERED_NOTHING;

        }
    void discoverIncline(Vec3f pos) {
            if (terrainDiscovered == DISCOVERED_INCLINE)
                return;

        destroyCurrentSound();

        new (&incline) Incline(actor, pos);
        currentSound = (TerrainCueSound*) & incline;
        terrainDiscovered = DISCOVERED_INCLINE;
    }
    void discoverDecline(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_DECLINE)
            return;

        destroyCurrentSound();

        new (&decline) Decline(actor, pos);
        currentSound = (TerrainCueSound*)&decline;
        terrainDiscovered = DISCOVERED_DECLINE;

    }
    void discoverLedge(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_LEDGE)
            return;

        destroyCurrentSound();

        new (&ledge) Ledge(actor, pos);
        currentSound = (TerrainCueSound*)&ledge;
        terrainDiscovered = DISCOVERED_LEDGE;

    }
    void discoverWall(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_WALL)
            return;

        destroyCurrentSound();

        new (&wall) Wall(actor, pos);
        currentSound = (TerrainCueSound*)&wall;
        terrainDiscovered = DISCOVERED_WALL;


    }
    void scan() {
        Player* player = GET_PLAYER(actor->play);
        if (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            destroyCurrentSound();
            return;

        }
        // The virtual cue actors travel in lines relative to Link's angle.
        rot = ActorAccessibility_ComputeRelativeAngle(player->actor.world.rot, relRot);
        Vec3f velocity;
        velocity.x = Math_SinS(rot.y);
        velocity.z = Math_CosS(rot.y);
        // Draw a line from Link's position to the max detection distance based on the configured relative angle.
        Vec3f pos = player->actor.world.pos;

        CollisionPoly poly;
        CollisionPoly* wallPoly;
        f32 step = fabs(velocity.x + velocity.z);
        f32 distToTravel = detectionDistance;
        Vec3f collisionResult;
        s32 bgId = 0;
//Don't be fooled: link being in the air does not mean we've found a dropoff. I mean... it could mean that, but it's a little too late to do anything about it at that point anyway.

        if (player->stateFlags3 & PLAYER_STATE3_MIDAIR || player->stateFlags2&PLAYER_STATE2_HOPPING) {
            f32 floorHeight = 0;
            floorHeight = BgCheck_AnyRaycastFloor1(&actor->play->colCtx, &poly, &pos);
            if (floorHeight == BGCHECK_Y_MIN)
                return;//Link is about to void out of bounds or something.
            pos.y = floorHeight;

        }
        Vec3f startingPos = pos;

        while (distToTravel >= 0) {
            Vec3f prevPos = pos;
            pos.x += velocity.x;
            pos.y += 10;
            pos.z += velocity.z;
            distToTravel -= step;
            f32 floorHeight = 0;
            floorHeight = BgCheck_AnyRaycastFloor1(&actor->play->colCtx, &poly, &pos);
            if (floorHeight == BGCHECK_Y_MIN) {
                destroyCurrentSound();
                break; // I'm guessing this means out of bounds?
            }

            pos.y = floorHeight;
            if (pos.y > prevPos.y && fabs(pos.y - prevPos.y) > minInclineDistance)
                // Player is on an incline.
            {

                discoverIncline(pos);
                break;

            } else if (pos.y < prevPos.y && fabs(pos.y - prevPos.y) > minDeclineDistance)
            {
                // Is this a big drop or just a slope?
                if (fabs(pos.y - prevPos.y) < 10) {
                    discoverDecline(pos);
                    break;

                } else // It's a bigger drop, possibly the edge of a platform.
                {
                    discoverLedge(pos);
                    break;

                }
            }
            if (BgCheck_AnyLineTest3(&actor->play->colCtx, &prevPos, &pos, &collisionResult, &wallPoly, 1, 0, 0, 0,
                                     &bgId)) {
                discoverWall(pos);
                break;
            }

        }
//Emit sound from the discovered position.
if (currentSound)
            currentSound->update(pos);
    }

    };

typedef struct {
    TerrainCueDirection directions[3]; // Directly ahead of Link, 90 degrees to his left and 90 degrees to his right.

}TerrainCueState;

    //Callback for initialization of terrain cue state.
bool ActorAccessibility_InitTerrainCueState(AccessibleActor* actor) {
    TerrainCueState* state = (TerrainCueState*) malloc(sizeof(TerrainCueState));
    if (state == NULL)
        return false;
    state->directions[0].init(actor, { 0, 0, 0 });
    state->directions[1].init(actor, { 0, 16384, 0 });
    state->directions[2].init(actor, { 0, -16384, 0 });

    actor->userData = state;
    return true;


}
void ActorAccessibility_CleanupTerrainCueState(AccessibleActor* actor) {
    free(actor->userData);
    actor->userData = NULL;

}
//Computes a relative angle based on Link's (or some other actor's) current angle.
Vec3s ActorAccessibility_ComputeRelativeAngle(Vec3s& origin, Vec3s& offset) {
    Vec3s rot = origin;
    rot.x += offset.x;
    rot.y += offset.y;
    rot.z += offset.z;
    return rot;

}
        void accessible_va_terrain_cue(AccessibleActor * actor) {

    TerrainCueState* state = (TerrainCueState*)actor->userData;
            state->directions[0].scan();

    }
        /*
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
            if (headPoly != NULL)
                break;

            poly = NULL;

        }
        
        if (distToTravel <= 0)
            break;

    }
    if (poly == NULL) {
        //Audio_StopSfxByPosAndId(&actor->world.pos, wallCueSound);
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
    */

void ActorAccessibility_InitCues() {

ActorAccessibilityPolicy policy;
    ActorAccessibility_InitPolicy(policy, "Terrain cue helper", accessible_va_terrain_cue);
        policy.n = 1;
        policy.runsAlways = true;
        policy.distance = 500;
        policy.initUserData = ActorAccessibility_InitTerrainCueState;
        policy.cleanupUserData = ActorAccessibility_CleanupTerrainCueState;

        ActorAccessibility_AddSupportedActor(VA_TERRAIN_CUE, policy);
        VirtualActorList* list = ActorAccessibility_GetVirtualActorList(EVERYWHERE, 0);
        ActorAccessibility_AddVirtualActor(list, VA_TERRAIN_CUE, { { 0, 0, 0 }, { 0, 0, 0 } });
    }