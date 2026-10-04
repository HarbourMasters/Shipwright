#ifndef SOH_SKATE_PHYSICS_H
#define SOH_SKATE_PHYSICS_H

/**
 * Skate Mode physics core.
 *
 * This file has no dependency on the game. It only knows about a board with a heading, a horizontal
 * velocity, a floor normal and a handful of inputs. z_player.c feeds it and applies the result to Link.
 *
 * Units match the OoT engine: speeds are "units per frame" at the 20Hz game tick, angles are binary
 * angles (0x10000 == 360 degrees, 0 == +Z, 0x4000 == +X).
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SkateTuning {
    float pushImpulse;    // speed added by one push
    float pushTopSpeed;   // pushing does nothing above this speed
    float maxSpeed;       // hard cap, only reachable downhill
    float rollFriction;   // constant deceleration per frame
    float drag;           // speed-proportional deceleration per frame
    float brakeDecel;     // deceleration per frame while braking / powersliding
    float grip;           // fraction of sideways velocity removed per frame (0..1)
    float slideGrip;      // same, while powersliding
    float carveRetention; // fraction of the speed scrubbed by a carve that is given back (0..1)
    float slopeGravity;   // acceleration down a slope, per frame, for a vertical wall
    float turnRateSlow;   // degrees per frame at standstill
    float turnRateFast;   // degrees per frame at maxSpeed
    float airSpinRate;    // degrees per frame of spin with the stick fully sideways
    float ollieMin;       // vertical launch speed for a tapped ollie
    float ollieMax;       // vertical launch speed for a fully charged ollie
    float bailWallSpeed;  // speed into a wall that causes a bail
    float bailLandSpeed;  // sideways landing above this speed causes a bail
    int pushCooldown;     // frames between pushes
    int ollieChargeTime;  // frames to fully charge an ollie
} SkateTuning;

typedef struct SkateState {
    float vx, vz;             // horizontal velocity in world space
    int16_t heading;          // direction the nose of the board points
    float lean;               // -1..1, positive leans to the rider's left
    int pushTimer;            // frames until the next push is allowed
    int ollieCharge;          // frames the ollie has been held
    uint8_t fakie;            // rolling backwards relative to the heading
    uint8_t turnaround;       // a standing 180 is in progress
    uint8_t turnaroundArmed;  // the stick has been let go since the last standing 180 or stick brake
    int16_t turnaroundTarget; // heading the standing 180 ends on
} SkateState;

typedef struct SkateInput {
    float stickMag;        // 0..1
    int16_t stickYaw;      // world direction the stick points
    float stickSide;       // -1..1, positive is stick left as seen by the camera (used for air spin)
    uint8_t push;          // push button held
    uint8_t brake;         // brake / powerslide button held
    float nx, ny, nz;      // floor normal (ignored in the air)
    float surfaceFriction; // multiplier on rollFriction, 1 == smooth stone
} SkateInput;

typedef enum SkateLanding {
    SKATE_LANDING_CLEAN,   // rolled away forwards
    SKATE_LANDING_FAKIE,   // rolled away backwards
    SKATE_LANDING_SKETCHY, // landed sideways slowly, lost most speed
    SKATE_LANDING_BAIL     // landed sideways at speed
} SkateLanding;

typedef enum SkateWallHit {
    SKATE_WALL_NONE,  // not moving into the wall
    SKATE_WALL_SCRUB, // slid along / bumped it
    SKATE_WALL_BAIL   // slammed it
} SkateWallHit;

void SkatePhysics_DefaultTuning(SkateTuning* tuning);
void SkatePhysics_Reset(SkateState* state, int16_t heading, float speed, int16_t travelYaw);

float SkatePhysics_Speed(const SkateState* state);
/** Direction of travel. Falls back to the heading (or its opposite in fakie) when nearly stopped. */
int16_t SkatePhysics_TravelYaw(const SkateState* state);

/** One frame of rolling on a floor. Returns 1 if a push happened this frame. */
int SkatePhysics_StepGround(SkateState* state, const SkateInput* input, const SkateTuning* tuning);
/** One frame in the air: momentum is kept, the stick spins the board. */
void SkatePhysics_StepAir(SkateState* state, const SkateInput* input, const SkateTuning* tuning);

/** Launch speed for an ollie given how long it was charged. Resets the charge. */
float SkatePhysics_Ollie(SkateState* state, const SkateTuning* tuning);

SkateLanding SkatePhysics_Land(SkateState* state, const SkateTuning* tuning);
/** wallYaw is the direction the wall faces (pointing away from the wall). */
SkateWallHit SkatePhysics_HitWall(SkateState* state, int16_t wallYaw, const SkateTuning* tuning);

#ifdef __cplusplus
}
#endif

#endif
