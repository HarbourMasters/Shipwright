#include "ActorAccessibility.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
extern "C" {
s32 func_80839768(PlayState* play, Player* p, Vec3f* arg2, CollisionPoly** arg3, s32* arg4, Vec3f* arg5);

}

static const f32 detectionDistance = 500.0;
static const f32 minInclineDistance = 5.0;
static const f32 minDeclineDistance = 5.0;
static Player fakePlayer;//Used for wall height detection.
static Vec3f D_80854798 = { 0.0f, 18.0f, 0.0f }; // From z_player.c.

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
        bool climbable;//Distinguishes between a ledge link can fall from and one he can climb up.
      public:
    Ledge(AccessibleActor* actor, Vec3f pos, bool above = false) : TerrainCueSound(actor, pos) {
            currentPitch = above? 2.0 : 0.4;
            climbable = above;
        currentReverb = 1;
            currentSFX = NA_SE_EV_WIND_TRAP;
            play();

    }
    virtual ~Ledge() {
    
    }
    bool isClimbable() {
        return climbable;

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
    f32 wallCheckHeight;
    f32 wallCheckRadius;
    f32 ceilingCheckHeight;
    int terrainDiscovered;

    union {
        Incline incline;
        Decline decline;
        Ledge ledge;
        Wall wall;

    };
    TerrainCueSound* currentSound;
//Apply an offset b to a Vec3f a.
    Vec3f applyVec3fOffset(Vec3f& a, Vec3f& b) {
        Vec3f c;
        c.x = a.x + b.x;
        c.y = a.y + b.y;
        c.z = a.z + b.z;
        return c;

    }
    //If a sound is currently playing, disable it.
    void destroyCurrentSound() {
        if (currentSound == NULL)
            return;
        currentSound->~TerrainCueSound();
        currentSound = NULL;
        terrainDiscovered = DISCOVERED_NOTHING;
    }
//Play a sound from the position of a previously discovered incline.

    void discoverIncline(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_INCLINE)
            return;

        destroyCurrentSound();

        new (&incline) Incline(actor, pos);
        currentSound = (TerrainCueSound*)&incline;
        terrainDiscovered = DISCOVERED_INCLINE;
    }
    //Play a sound from the position of a previously discovered decline.

    void discoverDecline(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_DECLINE)
            return;

        destroyCurrentSound();

        new (&decline) Decline(actor, pos);
        currentSound = (TerrainCueSound*)&decline;
        terrainDiscovered = DISCOVERED_DECLINE;
    }
    //Play a sound from the position of a previously discovered ledge.

    void discoverLedge(Vec3f pos, bool upper = false) {
        if (terrainDiscovered == DISCOVERED_LEDGE && ledge.isClimbable() == upper)
            return;

        destroyCurrentSound();

        new (&ledge) Ledge(actor, pos, upper);
        currentSound = (TerrainCueSound*)&ledge;
        terrainDiscovered = DISCOVERED_LEDGE;
    }
    //Play a sound from the position of a previously discovered wall.

    void discoverWall(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_WALL)
            return;

        destroyCurrentSound();

        new (&wall) Wall(actor, pos);
        currentSound = (TerrainCueSound*)&wall;
        terrainDiscovered = DISCOVERED_WALL;
    }
        //Find out how high a wall goes.
    f32 findWallHeight(Vec3f& pos, CollisionPoly* poly) {
        Player* player = GET_PLAYER(actor->play);
        f32 wallHeight;
        D_80854798.y = 18.0f;
        D_80854798.z = player->ageProperties->unk_38 + 10.0f;
        f32 wallYaw = Math_Atan2S(poly->normal.z, poly->normal.x);
        f32 nx = COLPOLY_GET_NORMAL(poly->normal.x);
        f32 ny = COLPOLY_GET_NORMAL(poly->normal.y);

        f32 nz = COLPOLY_GET_NORMAL(poly->normal.z);

        // Logic adapted from contents of z_player.c (beginning around line 10148).
        f32 wallDistance = Math3D_UDistPlaneToPos(nx, ny, nz, poly->dist, &pos);
        f32 wd10 = wallDistance + 10.0f;

        Vec3f raycast;
        raycast.x = pos.x - (wd10 * nx);
        raycast.z = pos.z - (wd10 * nz);
        raycast.y = pos.y + player->ageProperties->unk_0C;
        CollisionPoly* testPoly;
        wallHeight = BgCheck_EntityRaycastFloor1(&actor->play->colCtx, &testPoly, &raycast) - pos.y;
        f32 outY;
        s32 bgId;
        if ((wallHeight < 18.0f) ||
            BgCheck_EntityCheckCeiling(&actor->play->colCtx, &outY, &pos,
                                       wallHeight + 20.0f, &testPoly, &bgId, NULL)) {
            wallHeight = 399.96002f;
        } 
else {
            D_80854798.y = (wallHeight + 5.0f);
            fakePlayer.actor.shape.rot = player->actor.shape.rot;
            fakePlayer.actor.world.pos = pos;
            Vec3f collisionResult;

            //The following replicates some pretty confusing logic in z_player.c (another series of conditions which determines whether wallHeight should be set to the magic number 399.96002f).
            //Rather than copying the relevant functions to eliminate dependency on the player object, or risking weird side effects from passing in the real player with a temporarily modified pos vector, I'm using this fake player instance instead. These functions only need the player's position and shape rotation vectors set.
            if (func_80839768(actor->play, &fakePlayer, &D_80854798, &testPoly, &bgId, &collisionResult) &&
                abs(wallYaw - Math_Atan2S(testPoly->normal.z, testPoly->normal.x)) < 0x4000 &&
                !func_80041E18(&actor->play->colCtx, testPoly, bgId)) {
                wallHeight = 399.96002f;
            }

        }
        return wallHeight;

    }

    // choose to discover either a wall or a climbable ledge depending on wall height.
    void discoverWallOrLedge(Vec3f pos, CollisionPoly* poly) {
        Player* player = GET_PLAYER(actor->play);
        if (findWallHeight(pos, poly) <= player->ageProperties->unk_0C) // Ledge at top of wall can be reached.
            discoverLedge(pos, true);
        else
            discoverWall(pos);
    }

     //Check if traveling from point A to point B is obstructed by a wall.
    CollisionPoly* checkWall(Vec3f& pos, Vec3f& prevPos, Vec3f& collisionPos) {
        Player* player = GET_PLAYER(actor->play);
        CollisionPoly* poly;
        s32 bgId;
        BgCheck_EntitySphVsWall3(&actor->play->colCtx, &collisionPos, &pos, &prevPos, wallCheckRadius, &poly, &bgId,
                                 NULL, wallCheckHeight);
        return poly;

    }

  public:
    // Initialize a TerrainCueDirection based on a relative angle.
    void init(AccessibleActor* actor, Vec3s rot) {
        this->actor = actor;
        this->relRot = rot;
        this->rot = { 0, 0, 0 };
        terrainDiscovered = DISCOVERED_NOTHING;
        currentSound = NULL;

    }
//Move a probe to its next point along a line, ensuring that it remains on the floor. Returns false if the move would put the probe out of bounds. Does not take walls into account.
    bool move(Vec3f& pos, Vec3f& velocity) {
        CollisionPoly poly;
        pos.x += velocity.x;
        pos.y += velocity.y;
        pos.z += velocity.z;
        f32 floorHeight = 0;
        floorHeight = BgCheck_AnyRaycastFloor1(&actor->play->colCtx, &poly, &pos);
        if (floorHeight == BGCHECK_Y_MIN)
            return false;// I'm guessing this means out of bounds?
        pos.y = floorHeight;
        if (!BgCheck_PosInStaticBoundingBox(&actor->play->colCtx, &pos))
            return false;//Out of bounds.
        return true;
    }
    bool isHeadOnCollision(Vec3f& wallPos, Vec3f& velocity) {
        Vec3f pos = wallPos;
        if (!move(pos, velocity))
            return true;//Arbitrary, but hopefully this can't happen under normal gameplay circumstances.
        Vec3f newWallPos;
        if (!checkWall(pos, wallPos, newWallPos))
            return false;
        f32 dist = Math_Vec3f_DistXYZ(&wallPos, &newWallPos);
        return fabs(dist) < 0.25;
    }
        //Perform all terrain detection and sound book keeping. Call once per frame.
    void scan() {
        Player* player = GET_PLAYER(actor->play);
        if (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            destroyCurrentSound();
            return;
        }
        // Adapted from code in z_player.c, lines 10000 - 10008.
        if (player->stateFlags2 & PLAYER_STATE2_CRAWLING) {
            wallCheckRadius = 10.0f;
            wallCheckHeight = 15.0f;
            ceilingCheckHeight = 30.0f;
        } else {
            wallCheckRadius = player->ageProperties->unk_38;
            wallCheckHeight = 26.0f;
            ceilingCheckHeight = player->ageProperties->unk_00;
        }

        // The virtual cue actors travel in lines relative to Link's angle.
        rot = ActorAccessibility_ComputeRelativeAngle(player->actor.world.rot, relRot);
        Vec3f velocity;

        velocity.x = Math_SinS(rot.y);
        velocity.y = 10.0;
        velocity.z = Math_CosS(rot.y);
        // Draw a line from Link's position to the max detection distance based on the configured relative angle.
        Vec3f pos = player->actor.world.pos;
        CollisionPoly* wallPoly;
        f32 step = fabs(velocity.x + velocity.z);
        f32 distToTravel = detectionDistance;
        Vec3f collisionResult;
        s32 bgId = 0;
//Don't be fooled: link being in the air does not mean we've found a dropoff. I mean... it could mean that, but it's a little too late to do anything about it at that point anyway.

        if (player->stateFlags3 & PLAYER_STATE3_MIDAIR || player->stateFlags2&PLAYER_STATE2_HOPPING) {
            f32 floorHeight = 0;
            CollisionPoly poly;
            floorHeight = BgCheck_AnyRaycastFloor1(&actor->play->colCtx, &poly, &pos);
            if (floorHeight == BGCHECK_Y_MIN)
                return;//Link is about to void out of bounds or something.
            pos.y = floorHeight;

        }

        while (distToTravel >= 0) {
            Vec3f prevPos = pos;
            if (!move(pos, velocity)) {
                destroyCurrentSound();
                break;//Probe is out of bounds.
            }
            distToTravel -= (step + fabs(pos.y - pos.y));
            if (pos.y < prevPos.y && fabs(pos.y - prevPos.y) >= 10)
            {
                    //This is a fall.
                    discoverLedge(pos);
                    break;

                }
            Vec3f wallPos;
                CollisionPoly* wallPoly = checkWall(pos, prevPos, wallPos);
            if (wallPoly) {

//Ignore wall collisions that aren't *almost* head-on.
                if (isHeadOnCollision (wallPos, velocity))
                {
                        discoverWallOrLedge(wallPos, wallPoly);

                        break;
                    }

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
            for (int i = 0; i < 3; i++)
            state->directions[i].scan();

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