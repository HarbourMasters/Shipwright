#include "ActorAccessibility.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
extern "C" {
s32 func_80839768(PlayState* play, Player* p, Vec3f* arg2, CollisionPoly** arg3, s32* arg4, Vec3f* arg5);
void func_8083E298(CollisionPoly* arg0, Vec3f* arg1, s16* arg2);
void CollisionPoly_GetVertices(CollisionPoly* poly, Vec3s* vtxList, Vec3f* dest);
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "soh/Enhancements/tts/tts.h"
}
# define DETECTION_DISTANCE 500.0
#define MIN_INCLINE_DISTANCE 5.0
#define MIN_DECLINE_DISTANCE 5.0
#define DEFAULT_PROBE_SPEED 5.5

static Player fakePlayer;//Used for wall height detection.
static Vec3f D_80854798 = { 0.0f, 18.0f, 0.0f }; // From z_player.c.

const char* GetLanguageCode();

enum { DISCOVERED_NOTHING = 0,
DISCOVERED_INCLINE,
DISCOVERED_DECLINE,
DISCOVERED_LEDGE,
DISCOVERED_WALL,
DISCOVERED_SPIKE,
DISCOVERED_WATER,
DISCOVERED_CLIMABLE,
};
//Abstract class for terrain cue sound handling. Implementations should not allocate memory. These are always in-place constructed in static memory owned by the TerrainCueDirection object.
class TerrainCueSound {
  protected:
    AccessibleActor* actor;
    Vec3f terrainPos;
    Vec3f terrainProjectedPos;
    f32 currentPitch;
    f32 xzDistToPlayer;
    s16 currentSFX;
    int restFrames;//Used to control how often sounds get played.

    bool shouldLoop;
    // Call to start playback.
    void play() {
        ActorAccessibility_PlaySound(this, 0, currentSFX, shouldLoop);
        ActorAccessibility_SetSoundPos(this, 0, &terrainProjectedPos, xzDistToPlayer, actor->policy.distance);
        ActorAccessibility_SetSoundPitch(this, 0, currentPitch);

    }

    // Call when terrain is no longer present to stop playback.
    virtual void stop() {
        ActorAccessibility_StopSound(this, 0);
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
        ActorAccessibility_SetSoundPos(this, 0, &terrainProjectedPos, xzDistToPlayer, actor->policy.distance);
        ActorAccessibility_SetSoundPitch(this, 0, currentPitch);

    }

  public:
    TerrainCueSound(AccessibleActor* actor, Vec3f pos) {
        this->actor = actor;
        currentPitch = 1.0;
        shouldLoop = false;
        restFrames = 0;
        xzDistToPlayer = 0;
        currentSFX = 0;

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

  public:
    Incline(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 0.5;
        currentSFX = NA_SE_PL_MAGIC_SOUL_FLASH;

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
        ActorAccessibility_SetSoundPitch(this, 0, 0.5 + (1-actor->policy.pitchModifier));
        /*currentPitch += 0.1;
        if (currentPitch >= 2.0) {
            stop();
            currentPitch = 0.5;
            restFrames = 5;
        }*/
    }
};

class Decline : protected TerrainCueSound {

      public:
        Decline(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
            restFrames = 0;
            currentPitch = 2.0;
            currentSFX = NA_SE_PL_MAGIC_SOUL_FLASH;

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
            ActorAccessibility_SetSoundPitch(this, 0, 1.0 + actor->policy.pitchModifier);
            /*currentPitch -= 0.1;
            if (currentPitch < 0.5) {
                stop();
                currentPitch = 2.0;
                restFrames = 5;
            }*/
        }
    };
class Ledge :protected TerrainCueSound {
        bool climbable;//Distinguishes between a ledge link can fall from and one he can climb up.
      public:
    Ledge(AccessibleActor* actor, Vec3f pos, bool above = false) : TerrainCueSound(actor, pos) {
            currentPitch = above? 2.0 : 0.4;
            climbable = above;
            currentSFX = climbable ? NA_SE_EV_WOOD_BOUND : NA_SE_EV_WIND_TRAP;
        shouldLoop = !climbable;
            play();

    }
    virtual ~Ledge() {
    
    }
    bool isClimbable() {
        return climbable;

    }
    void run() {
        if (!climbable)
                return;//Downward ledges play a looping sound and do not need ongoing maintenance.
        if (restFrames == 0)
        {
                play();
                restFrames = 10;
                return;

        }
        restFrames--;

    }
    };
class Platform: protected TerrainCueSound {
  public:
    Platform(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 2.0;
        currentSFX = NA_SE_EV_WOOD_BOUND;
        shouldLoop = false;

    }
    virtual ~Platform() {
    }
    void setActor(AccessibleActor* actor)
    {
        this->actor = actor;

    }
    void setPosition(Vec3f& pos)
    {
        updatePositions(pos);
    }
    void run() {
        if (restFrames == 0) {
                play();
                restFrames = 10;
                return;
        }
        restFrames--;
    }
};

class Wall: protected TerrainCueSound {
        int frames;

  public:
    Wall(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 0.5;
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
class Spike : protected TerrainCueSound {
  public:
    Spike(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 0.5;
        currentSFX = NA_SE_IT_SWORD_PICKOUT;
        play();
    }
    virtual ~Spike() {
    }
    void run() {
        if (restFrames == 0) {
            play();
            restFrames = 10;
            return;
        }
        restFrames--;
    }
};
class Water : protected TerrainCueSound {
  public:
    Water(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 0.5;
        currentSFX = NA_SE_PL_LAND_WATER0; // NA_SE_EN_DAIOCTA_LAND_WATER; // change?
        play();
    }
    virtual ~Water() {
    }
    void run() {
        if (restFrames == 0) {
            play();
            restFrames = 10;
            return;
        }
        restFrames--;
    }
};

class Climable : protected TerrainCueSound {
  public:
    Climable(AccessibleActor* actor, Vec3f pos) : TerrainCueSound(actor, pos) {
        currentPitch = 0.5;
        currentSFX = NA_SE_PL_LAND_LADDER;
        play();
    }
    virtual ~Climable() {
    }
    void run() {
        if (restFrames == 0) {
            play();
            restFrames = 10;
            return;
        }
        restFrames--;
    }
};

    class TerrainCueDirection {
    AccessibleActor* actor;
    int startingBodyPart;//Decides where the probe starts from. Probes going out to the left or right of the player start from the shoulders.
    Vec3f pos;
    Vec3f prevPos;
    Vec3s relRot; // Relative angle.
    Vec3s rot;    // Actual angle.
    f32 wallCheckHeight;
    f32 wallCheckRadius;
    f32 ceilingCheckHeight;
    f32 probeSpeed; // Approximate for now.
    Vec3f velocity;
    Vec3f expectedVelocity;
    int terrainDiscovered;
    CollisionPoly* floorPoly;
    CollisionPoly* wallPoly;
    s32 wallBgId;
    f32 wallHeight;
    s32 floorBgId;
    f32 yDistToWater;
    f32 pushedSpeed;
    bool disabled;     // Only used for debugging.
    bool trackingMode; // A debugging feature which forces Link to move along the probe's path. Used to catch collision
                       // violations and other disagreements between how Link moves and how the probe travels.
    bool trackingModeStarted;

    s16 pushedYaw;
    union {
        Incline incline;
        Decline decline;
        Ledge ledge;
        Wall wall;
        Spike spike;
        Water water;
        Climable climable;
    };
    Platform platform;

    TerrainCueSound* currentSound;
    // Apply an offset b to a Vec3f a.
    Vec3f applyVec3fOffset(Vec3f& a, Vec3f& b) {
        Vec3f c;
        c.x = a.x + b.x;
        c.y = a.y + b.y;
        c.z = a.z + b.z;
        return c;
    }
    // If a sound is currently playing, disable it.
    void destroyCurrentSound() {
        if (currentSound == NULL)
            return;
        currentSound->~TerrainCueSound();
        currentSound = NULL;
        terrainDiscovered = DISCOVERED_NOTHING;
    }
    // Play a sound from the position of a previously discovered incline.

    void discoverIncline(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_INCLINE)
            return;

        destroyCurrentSound();

        new (&incline) Incline(actor, pos);
        currentSound = (TerrainCueSound*)&incline;
        terrainDiscovered = DISCOVERED_INCLINE;
    }
    // Play a sound from the position of a previously discovered decline.

    void discoverDecline(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_DECLINE)
            return;

        destroyCurrentSound();

        new (&decline) Decline(actor, pos);
        
        currentSound = (TerrainCueSound*)&decline;
        terrainDiscovered = DISCOVERED_DECLINE;
    }
    // Play a sound from the position of a previously discovered ledge.

    void discoverLedge(Vec3f pos, bool upper = false) {
        if (terrainDiscovered == DISCOVERED_LEDGE && ledge.isClimbable() == upper)
            return;

        destroyCurrentSound();

        new (&ledge) Ledge(actor, pos, upper);
        currentSound = (TerrainCueSound*)&ledge;
        terrainDiscovered = DISCOVERED_LEDGE;
    }
    // Play a sound from the position of a previously discovered wall.

    void discoverWall(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_WALL)
            return;

        destroyCurrentSound();

        new (&wall) Wall(actor, pos);
        currentSound = (TerrainCueSound*)&wall;
        terrainDiscovered = DISCOVERED_WALL;
    }
    void discoverSpike(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_SPIKE)
            return;
        destroyCurrentSound();
        new (&spike) Spike(actor, pos);
        currentSound = (TerrainCueSound*)&spike;
        terrainDiscovered = DISCOVERED_SPIKE;
    }

    void discoverWater(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_WATER)
            return;
        destroyCurrentSound();
        new (&water) Water(actor, pos);
        currentSound = (TerrainCueSound*)&water;
        terrainDiscovered = DISCOVERED_WATER;
    }

    void discoverClimable(Vec3f pos) {
        if (terrainDiscovered == DISCOVERED_CLIMABLE)
            return;
        destroyCurrentSound();
        new (&climable) Climable(actor, pos);
        currentSound = (TerrainCueSound*)&climable;
        terrainDiscovered = DISCOVERED_CLIMABLE;
    }
    // Find out how high a wall goes.
    f32 findWallHeight(Vec3f& pos, CollisionPoly* poly) {
        Player* player = GET_PLAYER(actor->play);
        f32 wallHeight;
        if (ABS(wallPoly->normal.y) >= 600) {
            wallHeight = 399.96002f;
            return wallHeight;
        }
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
            BgCheck_EntityCheckCeiling(&actor->play->colCtx, &outY, &pos, wallHeight + 20.0f, &testPoly, &bgId, NULL)) {
            wallHeight = 399.96002f;
        } else {
            D_80854798.y = (wallHeight + 5.0f);
            fakePlayer.actor.shape.rot = player->actor.shape.rot;
            fakePlayer.actor.world.pos = pos;
            Vec3f collisionResult;

            // The following replicates some pretty confusing logic in z_player.c (another series of conditions which
            // determines whether wallHeight should be set to the magic number 399.96002f). Rather than copying the
            // relevant functions to eliminate dependency on the player object, or risking weird side effects from
            // passing in the real player with a temporarily modified pos vector, I'm using this fake player instance
            // instead. These functions only need the player's position and shape rotation vectors set.
            if (func_80839768(actor->play, &fakePlayer, &D_80854798, &testPoly, &bgId, &collisionResult) &&
                abs(wallYaw - Math_Atan2S(testPoly->normal.z, testPoly->normal.x)) < 0x4000 &&
                !func_80041E18(&actor->play->colCtx, testPoly, bgId)) {
                wallHeight = 399.96002f;
            }
        }
        return wallHeight;
    }

    // Check if traveling from point A to point B is obstructed by a wall.
    CollisionPoly* checkWall(Vec3f& pos, Vec3f& prevPos, Vec3f& collisionPos) {
        Player* player = GET_PLAYER(actor->play);
        BgCheck_EntitySphVsWall3(&actor->play->colCtx, &collisionPos, &pos, &prevPos, wallCheckRadius, &wallPoly,
                                 &wallBgId, NULL, wallCheckHeight);
        return wallPoly;
    }
    // Another copy/modify job from z_player.c. This function sets windspeed and wind direction, which are used for
    // pushing the player up and down slopes. "Inspired" by func_8083E318.

    s32 computePushedSpeedEtc() {
        s32 pad;
        s16 sp4A;
        Vec3f sp3C;
        s16 sp3A;
        f32 temp1;
        f32 temp2;
        s16 temp3;
        PlayState* play = actor->play;

        if (SurfaceType_GetSlope(&play->colCtx, floorPoly, floorBgId) == 1) {
            sp4A = Math_Atan2S(velocity.z, velocity.x);
            func_8083E298(floorPoly, &sp3C, &sp3A);
            temp3 = sp3A - sp4A;

            if (ABS(temp3) > 16000) {
                temp1 = (1.0f - sp3C.y) * 40.0f;
                temp2 = (temp1 * temp1) * 0.015f;
                if (temp2 < 1.2f) {
                    temp2 = 1.2f;
                }
                pushedYaw = sp3A;
                pushedSpeed = temp1;
                // Math_StepToF(&pushedSpeed / probeSpeed, temp1, temp2);
            }
        } else
            pushedSpeed = 0.0;

        // Math_StepToF(&this->pushedSpeed, 0.0f, 1.0f); // Todo: only step by 0.5F when in water.
        return 0;
    }
    void setVelocity() {

        velocity.x = Math_SinS(rot.y) * probeSpeed;
        velocity.y = 12.0;
        velocity.z = Math_CosS(rot.y) * probeSpeed;
        expectedVelocity = velocity;

        computePushedSpeedEtc();
        if (pushedSpeed == 0.0)
            return;
        velocity.x += pushedSpeed * Math_SinS(pushedYaw);
        velocity.z += pushedSpeed * Math_CosS(pushedYaw);
    }
    // Check if we're being pushed away from our intended destination.
    bool isPushedAway() {
        f32 dist = Math_Vec3f_DistXZ(&velocity, &expectedVelocity);
        if (dist >= probeSpeed)
            return true;
        return false;
    }
    bool proveClimbableStep() {
        setVelocity();
        if (!move())
            return false;
        if (isPushedAway())
            return false;
        Vec3f wallPos;
        if (checkWall(prevPos, pos, wallPos))
            return false;
        return true;
    }
    bool proveClimbable() {
        Vec3s ogRot = rot;
        Vec3f ogPos = pos;
        pos.y += wallHeight;
        // Find the floor up here.
        probeSpeed = 1.0;
        bool foundFloor = false;
        for (int i = 0; i < 100; i++) {
            setVelocity();
            if (!move())
                return false;
            if (pos.y >= ogPos.y + wallHeight - 10) {
                foundFloor = true;
                break;
            } else
                pos.y = ogPos.y + wallHeight;
        }
        probeSpeed = DEFAULT_PROBE_SPEED;

        if (!foundFloor)
            return false;
        prevPos = pos;
        rot.y = ogRot.y + 16384;
        bool clockwiseTest = proveClimbableStep();
        f32 clockwiseY = pos.y;
        rot.y = ogRot.y - 16384;
        pos = prevPos;

        bool counterclockwiseTest = proveClimbableStep();
        f32 counterclockwiseY = pos.y;
        rot.y = ogRot.y;
        pos = ogPos;

        return clockwiseTest && counterclockwiseTest &&
               (fabs(clockwiseY - counterclockwiseY) < 10.0 || fabs(clockwiseY - counterclockwiseY) > wallHeight - 5.0);
    }

  public:
    // Initialize a TerrainCueDirection based on a relative angle and position offset.
    void init(AccessibleActor* actor, Vec3s rot, int startingBodyPart = PLAYER_BODYPART_MAX) {
        this->actor = actor;
        this->relRot = rot;
        this->rot = { 0, 0, 0 };
        this->startingBodyPart = startingBodyPart;

        terrainDiscovered = DISCOVERED_NOTHING;
        currentSound = NULL;
        new (&platform) Platform(actor, { 0.0, 0.0, 0.0 });

        disabled = false;
        trackingMode = false;
        trackingModeStarted = false;
    }
    // Move a probe to its next point along a line, ensuring that it remains on the floor. Returns false if the move
    // would put the probe out of bounds. Does not take walls into account.
    bool move(s8 gravity = true) {
        pos.x += velocity.x;
        pos.y += velocity.y;
        pos.z += velocity.z;
        
        if (gravity) {
            f32 floorHeight = 0;
            floorHeight = BgCheck_EntityRaycastFloor3(&actor->play->colCtx, &floorPoly, &floorBgId, &pos);
            if (floorHeight == BGCHECK_Y_MIN)
                return false; // I'm guessing this means out of bounds?
            pos.y = floorHeight;
        }
        if (!BgCheck_PosInStaticBoundingBox(&actor->play->colCtx, &pos))
            return false; // Out of bounds.

        return true;
    }
    bool isHeadOnCollision(Vec3f& wallPos, Vec3f& velocity) {
        return true;

        Vec3f pos = wallPos;
        if (!move())
            return true; // Arbitrary, but hopefully this can't happen under normal gameplay circumstances.
        Vec3f newWallPos;
        if (!checkWall(pos, wallPos, newWallPos))
            return false;
        f32 dist = Math_Vec3f_DistXYZ(&wallPos, &newWallPos);
        return fabs(dist) < 0.25;
    }

    // Perform all terrain detection and sound book keeping. Call once per frame.

    float rdist(Vec3f pos) {
        Player* player = GET_PLAYER(actor->play);
        float xdist = fabs(pos.x - player->actor.world.pos.x);
        float zdist = fabs(pos.z - player->actor.world.pos.z);
        float r = sqrt((zdist * zdist) + (xdist * xdist));
        return r;
    }
    
    void scan() {

        Player* player = GET_PLAYER(actor->play);
        
        if (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
            destroyCurrentSound();
            return;
        }
        if (disabled)
            return;
        if (trackingMode)
            destroyCurrentSound();
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
        rot = ActorAccessibility_ComputeRelativeAngle(&player->actor.world.rot, &relRot);
        pushedSpeed = 0.0;
        pushedYaw = 0;
        probeSpeed = DEFAULT_PROBE_SPEED;//Experiment with this.
        // Draw a line from Link's position to the max detection distance based on the configured relative angle.
        if (!trackingModeStarted) {
            pos = player->actor.world.pos;
//If a starting body part has been specified, then set the probe's initial X and Z position only.
            if (startingBodyPart != PLAYER_BODYPART_MAX)
            {
                pos.x = player->bodyPartsPos[startingBodyPart].x;
                pos.z = player->bodyPartsPos[startingBodyPart].y;
            }
        }

        if (trackingMode)
            trackingModeStarted = true;

        f32 distToTravel = DETECTION_DISTANCE;
        if (trackingMode)
            distToTravel = 1.0;
        Vec3f collisionResult;
        s32 bgId = 0;
//Don't be fooled: link being in the air does not mean we've found a dropoff. I mean... it could mean that, but it's a little too late to do anything about it at that point anyway.

        if (player->stateFlags3 & PLAYER_STATE3_MIDAIR || player->stateFlags2 & PLAYER_STATE2_HOPPING) {
            f32 floorHeight = 0;
            floorHeight = BgCheck_EntityRaycastFloor3(&actor->play->colCtx, &floorPoly, &floorBgId, &pos);
            if (floorHeight == BGCHECK_Y_MIN)
                return; // Link is about to void out of bounds or something.
            pos.y = floorHeight;
        } else {
            floorPoly = player->actor.floorPoly;
            floorBgId = player->actor.floorBgId;
        }
        while (distToTravel >= 0) {
            prevPos = pos;
            setVelocity();
            f32 step = fabs(velocity.x + velocity.z);

            //checks if link is in the water, needs different logic
            if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
                pos.y = player->actor.prevPos.y;
                if (!move()) {
                    destroyCurrentSound();
                    break; // Probe is out of bounds.
                }
                if (rdist(pos) > 500.00) {
                    destroyCurrentSound();
                    break; // too far too hear
                }
                // is there an incline ahead that leads out of the water
                if (pos.y > player->actor.world.pos.y) {
                    discoverIncline(pos);
                    break; 
                }
                //keeps probe at links feet
                if (pos.y < player->actor.world.pos.y) {
                    pos.y = player->actor.world.pos.y;
                }

                Vec3f wallPos;
                CollisionPoly* wallPoly = checkWall(pos, prevPos, wallPos);
                if (wallPoly == NULL) {
                    continue;
                }

                //sets probe to be at surface of water
                pos.y += player->actor.yDistToWater;
                prevPos.y += player->actor.yDistToWater;

                //checks for new wall poly
                wallPoly = checkWall(pos, prevPos, wallPos);

               
                //if not climable and exists then treats it as a wall
                if (wallPoly != NULL) {
                    discoverWall(pos);
                    break;
                }
                
                //checks for ledges
                pos.y = player->actor.world.pos.y - 10.0;
                f32 ogStep = step;
                step = 1.0;
                while (pos.y < player->actor.world.pos.y + player->actor.yDistToWater) {
                    pos.y = player->actor.world.pos.y - 10.0;
                    pos.y += 50.0;
                    if (!move()) {
                        break; // Probe is out of bounds.
                    }
                }
                step = ogStep;
                if (player->ageProperties->unk_92 == 0) {
                    wallHeight = fabs(pos.y - (player->actor.world.pos.y + player->actor.yDistToWater -
                                               45.5)); // change that number just a guess
                } else {
                    wallHeight = fabs(pos.y - (player->actor.world.pos.y + player->actor.yDistToWater - 30.0));
                }

                prevPos = pos;
                Vec3s ogRot = rot;
                Vec3f ogPos = pos;
                pos.y += 20.0;
                if (!move()) {
                    break; // Probe is out of bounds.
                }
                bool forwardTest = fabs(pos.y - ogPos.y) < 1.0;
                rot.y = ogRot.y + 16384;
                bool clockwiseTest = proveClimbableStep();
                f32 clockwiseY = pos.y;
                rot.y = ogRot.y - 16384;
                pos = prevPos;

                bool counterclockwiseTest = proveClimbableStep();
                f32 counterclockwiseY = pos.y;
                rot.y = ogRot.y;
                pos = ogPos;

                if (clockwiseTest && counterclockwiseTest && (forwardTest || wallHeight < 44.0) &&
                    (fabs(clockwiseY - counterclockwiseY) < 2.0 ||
                     fabs(clockwiseY - counterclockwiseY) > wallHeight - 5.0)) {
                    discoverLedge(pos, true);
                    break;
                } else {
                    discoverWall(pos);
                    break;
                }

            }
            //link isn't in the water
            else {
                if (!move()) {
                    destroyCurrentSound();
                    break; // Probe is out of bounds.
                }

                distToTravel -= (step + fabs(pos.y - pos.y));
                if (isPushedAway() && player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER) {
                    // Call this a wall for now.
                    discoverWall(pos);
                    break;
                }

                if (pos.y < prevPos.y && fabs(pos.y - prevPos.y) >= 20 &&
                    player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER) {
                    // This is a fall.

                    discoverLedge(pos);
                    if (((pos.y - player->actor.prevPos.y) < player->actor.yDistToWater-30) &&
                        (player->actor.yDistToWater < 0)) {
                        discoverWater(pos);
                        break;
                    }
                    testForPlatform();

                    break;
                }

                //checks for water
                if (((pos.y - player->actor.prevPos.y) < player->actor.yDistToWater) &&
                    (player->actor.yDistToWater < 0)) {
                    discoverWater(pos);
                    break;
                }

                if ((player->actor.yDistToWater > 0) &&
                    (fabs(pos.y - (player->actor.world.pos.y + player->actor.yDistToWater)) > 30.0)) {
                    discoverLedge(pos);
                }
                if (pos.y > prevPos.y && fabs(pos.y - prevPos.y) < 20 && fabs(pos.y - prevPos.y) > 2 &&
                    player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER) {
                    // This is an incline.
                    Vec3f_ bottom = pos;

                    while ((pos.y > prevPos.y && fabs(pos.y - prevPos.y) < 20 && fabs(pos.y - prevPos.y) > 2 &&
                            player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER)) {
                        prevPos = pos;
                        if (!move()) {
                            destroyCurrentSound();
                            break; // Probe is out of bounds.
                        }
                    }
                    f32 distToGo = Math_Vec3f_DistXYZ(&bottom, &pos);
                    if (distToGo > 500.0) {
                        distToGo = 500.0;
                    }
                    f32 pitchModifier = distToGo / 500.0;

                    pos = bottom;
                    actor->policy.pitchModifier = pitchModifier;
                    discoverIncline(bottom);
                    break;
                }
                if (pos.y < prevPos.y && fabs(pos.y - prevPos.y) < 20 && fabs(pos.y - prevPos.y) > 2 &&
                    player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER) {
                    // This is a decline.
                    // discorver top
                    Vec3f_ top = pos;

                    while ((pos.y < prevPos.y && fabs(pos.y - prevPos.y) < 20 && fabs(pos.y - prevPos.y) > 2 &&
                            player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER)) {
                        prevPos = pos;
                        if (!move()) {
                            destroyCurrentSound();
                            break; // Probe is out of bounds.
                        }
                    }
                    f32 distToGo = Math_Vec3f_DistXYZ(&top, &pos);
                    if (distToGo > 500.0) {
                        distToGo = 500.0;
                    }
                    f32 pitchModifier = distToGo / 500.0;

                    pos = top;
                    actor->policy.pitchModifier = pitchModifier;
                    discoverDecline(pos);
                    break;
                }
                Vec3f wallPos;
                CollisionPoly* wallPoly = checkWall(pos, prevPos, wallPos);
                if (wallPoly == NULL)
                    continue;
                // Is this a spiked wall?
                Vec3f polyVerts[3];
                CollisionContext* colCtx = &actor->play->colCtx;
                CollisionPoly_GetVertices(wallPoly, colCtx->colHeader->vtxList, polyVerts);
                if (SurfaceType_IsWallDamage(&actor->play->colCtx, wallPoly, BGCHECK_SCENE)) {
                    discoverSpike(pos);
                    break;
                }
                // is this a ladder or vine wall?
                
                wallHeight = findWallHeight(pos, wallPoly);
                if (wallHeight <= player->ageProperties->unk_0C &&
                    player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER) {
                    // Ledge at top of wall can be reached.
                    if (proveClimbable()) {
                        discoverLedge(pos, true);
                    } else {
                        discoverWall(pos);
                    }

                    break;
                }
                
                
                if (isHeadOnCollision(pos, velocity) && player->stateFlags1 != PLAYER_STATE1_CLIMBING_LADDER) {
                    discoverWall(pos);
                    break;
                }
            }
        }
        if (trackingMode)
player->actor.world.pos = pos;
            //Emit sound from the discovered position.
if (currentSound)
            currentSound->update(pos);
if (currentSound && trackingMode) {
disabled = true;
trackingMode = false;
trackingModeStarted = false;
}


    }
    void testForPlatform()
    {
        Player* player = GET_PLAYER(actor->play);

        f32 ledgeCheckDistance = 200.0;
        Vec3f startingPos = pos;
        while (ledgeCheckDistance >= 0) {
            prevPos = pos;
            setVelocity();
            pos.y = player->actor.prevPos.y + 100.0;
            f32 step = fabs(velocity.x + velocity.z);

            if (!move()) {
               break; // Probe is out of bounds.
            }
            ledgeCheckDistance -= (step + fabs(pos.y - pos.y));

            if ((fabs(pos.y - player->actor.prevPos.y) <= 70.00) && fabs(pos.y - prevPos.y) >= 20.0) {
               platform.setPosition(pos);
               platform.run();
               break;
            }
        }
        pos = startingPos;
    }
    };

typedef struct {
    TerrainCueDirection directions[3]; // Directly ahead of Link, 90 degrees to his left and 90 degrees to his right.
    int previousAction; //previous action icon state
}TerrainCueState;

    //Callback for initialization of terrain cue state.
bool ActorAccessibility_InitTerrainCueState(AccessibleActor* actor) {
    TerrainCueState* state = (TerrainCueState*) malloc(sizeof(TerrainCueState));
    if (state == NULL)
        return false;
    state->directions[0].init(actor, { 0, 0, 0 });
    state->directions[1].init(actor, { 0, 16384, 0 });//, PLAYER_BODYPART_L_SHOULDER);
    state->directions[2].init(actor, { 0, -16384, 0 });//, PLAYER_BODYPART_R_SHOULDER);
    state->previousAction = DO_ACTION_NONE;

    actor->userData = state;
    return true;


}
void ActorAccessibility_CleanupTerrainCueState(AccessibleActor* actor) {
    free(actor->userData);
    actor->userData = NULL;

}
//Computes a relative angle based on Link's (or some other actor's) current angle.
Vec3s ActorAccessibility_ComputeRelativeAngle(Vec3s* origin, Vec3s* offset) {
    Vec3s rot = *origin;
    rot.x += offset->x;
    rot.y += offset->y;
    rot.z += offset->z;
    return rot;

}


void accessible_va_terrain_cue(AccessibleActor * actor) {
    TerrainCueState* state = (TerrainCueState*)actor->userData;
            for (int i = 0; i < 3; i++)
            state->directions[i].scan();

    int currentState = actor->play->interfaceCtx.unk_1F0;
    Player* player = GET_PLAYER(actor->play);

    if (state->previousAction != currentState) {
        //Audio_PlaySoundGeneral(NA_SE_EV_DIAMOND_SWITCH, &player->actor.world.pos, 4, &actor->basePitch,
        //                       &actor->baseVolume,
        //                       &actor->currentReverb);
        switch (currentState) { 
            case DO_ACTION_CHECK:
                SpeechSynthesizer::Instance->Speak("Check", GetLanguageCode());
                break;
            case DO_ACTION_CLIMB:
                SpeechSynthesizer::Instance->Speak("Climb", GetLanguageCode());
                break;
            case DO_ACTION_ENTER:
                SpeechSynthesizer::Instance->Speak("Enter", GetLanguageCode());
                break;
            case DO_ACTION_GRAB:
                SpeechSynthesizer::Instance->Speak("Grab", GetLanguageCode());
                break;
            case DO_ACTION_OPEN:
                SpeechSynthesizer::Instance->Speak("Open", GetLanguageCode());
                break;
            case DO_ACTION_SPEAK:
                SpeechSynthesizer::Instance->Speak("Speak", GetLanguageCode());
                break;
            case DO_ACTION_STOP:
                SpeechSynthesizer::Instance->Speak("Stop", GetLanguageCode()); // possibly disable? not sure what it does
                break;
            default:
                break;
        }

        
        
        state->previousAction = currentState;
    } else {
        state->previousAction = currentState;
    }
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
    ActorAccessibility_InitPolicy(&policy, "Terrain cue helper", accessible_va_terrain_cue, 0);
        policy.n = 1;
        policy.runsAlways = true;
        policy.distance = 500;
        policy.initUserData = ActorAccessibility_InitTerrainCueState;
        policy.cleanupUserData = ActorAccessibility_CleanupTerrainCueState;

        ActorAccessibility_AddSupportedActor(VA_TERRAIN_CUE, policy);
        VirtualActorList* list = ActorAccessibility_GetVirtualActorList(EVERYWHERE, 0);
        ActorAccessibility_AddVirtualActor(list, VA_TERRAIN_CUE, { { 0, 0, 0 }, { 0, 0, 0 } });

    
    }