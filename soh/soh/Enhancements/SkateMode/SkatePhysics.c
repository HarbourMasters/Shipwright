#include "SkatePhysics.h"

#include <math.h>

#define SKATE_PI 3.14159265358979f
#define BINANG_TO_RAD(a) ((float)(a) * (SKATE_PI / 32768.0f))
#define DEG_TO_BINANG_F(d) ((d) * (65536.0f / 360.0f))

#define STICK_DEADZONE 0.15f
// Below this speed the board pivots on the spot instead of carving.
#define PIVOT_SPEED 1.5f
// A slope has to pull harder than this before a stationary board starts rolling.
#define STATIC_HOLD_ACCEL 0.1f
// At a standstill, a stick direction further than this from the heading (120 degrees) asks for a 180.
#define TURNAROUND_ZONE 0x5555

static float SinB(int16_t a) {
    return sinf(BINANG_TO_RAD(a));
}

static float CosB(int16_t a) {
    return cosf(BINANG_TO_RAD(a));
}

static int16_t YawOf(float x, float z) {
    int32_t binang = (int32_t)lroundf(atan2f(x, z) * (32768.0f / SKATE_PI));
    return (int16_t)(uint16_t)(binang & 0xFFFF);
}

static float ClampF(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

static float SignF(float v) {
    return (v < 0.0f) ? -1.0f : 1.0f;
}

static int32_t AbsYawDiff(int16_t a, int16_t b) {
    int32_t d = (int16_t)(a - b);
    return (d < 0) ? -d : d;
}

void SkatePhysics_DefaultTuning(SkateTuning* tuning) {
    tuning->pushImpulse = 2.4f;
    tuning->pushTopSpeed = 11.0f;
    tuning->maxSpeed = 18.0f;
    tuning->rollFriction = 0.03f;
    tuning->drag = 0.0035f;
    tuning->brakeDecel = 0.7f;
    tuning->grip = 0.6f;
    tuning->slideGrip = 0.12f;
    tuning->carveRetention = 0.7f;
    tuning->slopeGravity = 1.0f;
    tuning->turnRateSlow = 7.0f;
    tuning->turnRateFast = 2.5f;
    tuning->airSpinRate = 16.0f;
    tuning->ollieMin = 5.5f;
    tuning->ollieMax = 8.5f;
    tuning->bailWallSpeed = 8.0f;
    tuning->bailLandSpeed = 6.0f;
    tuning->pushCooldown = 9;
    tuning->ollieChargeTime = 10;
}

void SkatePhysics_Reset(SkateState* state, int16_t heading, float speed, int16_t travelYaw) {
    state->vx = speed * SinB(travelYaw);
    state->vz = speed * CosB(travelYaw);
    state->heading = heading;
    state->lean = 0.0f;
    state->pushTimer = 0;
    state->ollieCharge = 0;
    state->fakie = 0;
    state->turnaround = 0;
    state->turnaroundArmed = 1;
    state->turnaroundTarget = heading;
}

float SkatePhysics_Speed(const SkateState* state) {
    return sqrtf(state->vx * state->vx + state->vz * state->vz);
}

int16_t SkatePhysics_TravelYaw(const SkateState* state) {
    if (SkatePhysics_Speed(state) > 0.3f) {
        return YawOf(state->vx, state->vz);
    }
    return state->fakie ? (int16_t)(state->heading + 0x8000) : state->heading;
}

int SkatePhysics_StepGround(SkateState* state, const SkateInput* input, const SkateTuning* tuning) {
    float speed = SkatePhysics_Speed(state);
    float fwdX = SinB(state->heading);
    float fwdZ = CosB(state->heading);
    float forward = state->vx * fwdX + state->vz * fwdZ;
    float lateral;
    float lateralKept;
    float friction;
    float turnedDeg = 0.0f;
    float ax;
    float az;
    float leanTarget;
    int stickBrake = 0;
    int pushed = 0;

    // Fakie has a little hysteresis so it does not flicker around a standstill.
    if (forward < -0.5f) {
        state->fakie = 1;
    } else if (forward > 0.5f) {
        state->fakie = 0;
    }

    // --- Steering -----------------------------------------------------------------------------------------------
    if (input->stickMag > STICK_DEADZONE) {
        if (speed < PIVOT_SPEED) {
            // Nearly stopped: kick-turn on the spot.
            float rate = DEG_TO_BINANG_F(tuning->turnRateSlow * 1.5f * input->stickMag);
            float diff = (float)(int16_t)(input->stickYaw - state->heading);
            float step = 0.0f;

            if (!state->turnaround) {
                if (fabsf(diff) > (float)TURNAROUND_ZONE) {
                    // Stick pulled back. The camera swings round behind Link as he turns, so chasing the stick
                    // direction would spin him forever. Commit to one half turn instead.
                    if (state->turnaroundArmed) {
                        state->turnaround = 1;
                        state->turnaroundArmed = 0;
                        state->turnaroundTarget = (int16_t)(state->heading + ((diff > 0.0f) ? 0x7FFF : -0x7FFF));
                    } else {
                        // Still holding back after braking: keep dragging the foot until the board stops.
                        stickBrake = 1;
                    }
                } else {
                    state->turnaroundArmed = 1;
                    step = ClampF(diff, -rate, rate);
                }
            }

            if (state->turnaround) {
                rate = DEG_TO_BINANG_F(tuning->turnRateSlow * 2.5f);
                diff = (float)(int16_t)(state->turnaroundTarget - state->heading);
                step = ClampF(diff, -rate, rate);
                if (fabsf(diff) <= rate) {
                    state->turnaround = 0;
                }
            }

            state->heading += (int16_t)step;
            turnedDeg = step * (360.0f / 65536.0f);
        } else {
            int16_t travelYaw = SkatePhysics_TravelYaw(state);
            int32_t stickToTravel = AbsYawDiff(input->stickYaw, travelYaw);

            state->turnaround = 0;

            if (stickToTravel > 0x6000) {
                // Stick pulled against the direction of travel: drag a foot.
                // Coming to a stop like this should not roll straight into a standing 180.
                stickBrake = 1;
                state->turnaroundArmed = 0;
            } else if (state->fakie && (stickToTravel < 0x2AAA)) {
                // Rolling backwards and steering the way we are going: revert so the nose leads again.
                state->heading += 0x8000;
                state->vx *= 0.9f;
                state->vz *= 0.9f;
                state->fakie = 0;
            } else {
                float speedFrac = ClampF(speed / tuning->maxSpeed, 0.0f, 1.0f);
                float rateDeg = tuning->turnRateSlow + (tuning->turnRateFast - tuning->turnRateSlow) * speedFrac;
                float rate;
                float diff;
                float step;
                int16_t target = input->stickYaw;

                if (state->fakie) {
                    target += 0x8000;
                }
                if (input->brake) {
                    rateDeg *= 1.6f;
                }

                rate = DEG_TO_BINANG_F(rateDeg * input->stickMag);
                diff = (float)(int16_t)(target - state->heading);
                step = ClampF(diff, -rate, rate);

                state->heading += (int16_t)step;
                turnedDeg = step * (360.0f / 65536.0f);
            }
        }

        fwdX = SinB(state->heading);
        fwdZ = CosB(state->heading);
    } else {
        state->turnaroundArmed = 1;
        if (state->turnaround) {
            // Let go mid-turn: finish it anyway so Link does not end up sideways.
            float rate = DEG_TO_BINANG_F(tuning->turnRateSlow * 2.5f);
            float diff = (float)(int16_t)(state->turnaroundTarget - state->heading);
            float step = ClampF(diff, -rate, rate);

            if ((fabsf(diff) <= rate) || (speed >= PIVOT_SPEED)) {
                state->turnaround = 0;
            }
            state->heading += (int16_t)step;
            turnedDeg = step * (360.0f / 65536.0f);
            fwdX = SinB(state->heading);
            fwdZ = CosB(state->heading);
        }
    }

    // --- Slope --------------------------------------------------------------------------------------------------
    // Acceleration along the slope is g*sin(a); its horizontal part is g*sin(a)*cos(a), which is exactly
    // (horizontal part of the normal) * normal.y.
    ax = tuning->slopeGravity * input->nx * input->ny;
    az = tuning->slopeGravity * input->nz * input->ny;

    if ((speed < 0.3f) && (sqrtf(ax * ax + az * az) < STATIC_HOLD_ACCEL)) {
        // Gentle ground does not set a parked board rolling.
        state->vx = 0.0f;
        state->vz = 0.0f;
    } else {
        state->vx += ax;
        state->vz += az;
    }

    // --- Grip: split velocity into along-the-board and across-the-board ------------------------------------------
    forward = state->vx * fwdX + state->vz * fwdZ;
    // (fwdZ, -fwdX) is the heading rotated +90 degrees, i.e. the rider's left.
    lateral = state->vx * fwdZ - state->vz * fwdX;

    lateralKept = lateral * (1.0f - ClampF(input->brake ? tuning->slideGrip : tuning->grip, 0.0f, 1.0f));

    if (!input->brake) {
        // The wheels bite and redirect momentum along the board instead of just throwing it away.
        float scrubbedEnergy = (lateral * lateral) - (lateralKept * lateralKept);
        forward =
            SignF(forward) * sqrtf(forward * forward + ClampF(tuning->carveRetention, 0.0f, 1.0f) * scrubbedEnergy);
    }
    lateral = lateralKept;

    // --- Friction and braking -------------------------------------------------------------------------------------
    friction = tuning->rollFriction * input->surfaceFriction + tuning->drag * fabsf(forward);
    if (input->brake || stickBrake) {
        friction += tuning->brakeDecel;
    }

    if (fabsf(forward) <= friction) {
        forward = 0.0f;
    } else {
        forward -= SignF(forward) * friction;
    }

    // --- Pushing --------------------------------------------------------------------------------------------------
    if (state->pushTimer > 0) {
        state->pushTimer--;
    }

    if (input->push && !input->brake && !stickBrake && (state->pushTimer == 0) &&
        (fabsf(forward) < tuning->pushTopSpeed)) {
        float dir = state->fakie ? -1.0f : 1.0f;

        forward += dir * tuning->pushImpulse;
        forward = ClampF(forward, -tuning->pushTopSpeed, tuning->pushTopSpeed);
        state->pushTimer = tuning->pushCooldown;
        pushed = 1;
    }

    // --- Recompose ------------------------------------------------------------------------------------------------
    state->vx = forward * fwdX + lateral * fwdZ;
    state->vz = forward * fwdZ - lateral * fwdX;

    speed = SkatePhysics_Speed(state);
    if (speed > tuning->maxSpeed) {
        float scale = tuning->maxSpeed / speed;
        state->vx *= scale;
        state->vz *= scale;
        speed = tuning->maxSpeed;
    } else if (speed < 0.05f) {
        state->vx = 0.0f;
        state->vz = 0.0f;
        speed = 0.0f;
    }

    // --- Lean into the turn ---------------------------------------------------------------------------------------
    leanTarget =
        ClampF(turnedDeg / tuning->turnRateSlow, -1.0f, 1.0f) * ClampF(speed / tuning->pushTopSpeed, 0.0f, 1.0f);
    state->lean += (leanTarget - state->lean) * 0.3f;

    return pushed;
}

void SkatePhysics_StepAir(SkateState* state, const SkateInput* input, const SkateTuning* tuning) {
    if (fabsf(input->stickSide) > STICK_DEADZONE) {
        state->heading += (int16_t)DEG_TO_BINANG_F(tuning->airSpinRate * ClampF(input->stickSide, -1.0f, 1.0f));
    }

    if (state->pushTimer > 0) {
        state->pushTimer--;
    }

    state->ollieCharge = 0;
    state->lean += (0.0f - state->lean) * 0.3f;
}

float SkatePhysics_Ollie(SkateState* state, const SkateTuning* tuning) {
    float charge = 1.0f;

    if (tuning->ollieChargeTime > 0) {
        charge = ClampF((float)state->ollieCharge / (float)tuning->ollieChargeTime, 0.0f, 1.0f);
    }

    state->ollieCharge = 0;

    return tuning->ollieMin + (tuning->ollieMax - tuning->ollieMin) * charge;
}

SkateLanding SkatePhysics_Land(SkateState* state, const SkateTuning* tuning) {
    float speed = SkatePhysics_Speed(state);
    int32_t offAxis;

    if (speed < PIVOT_SPEED) {
        state->fakie = 0;
        return SKATE_LANDING_CLEAN;
    }

    offAxis = AbsYawDiff(state->heading, YawOf(state->vx, state->vz));

    if (offAxis <= 0x2800) { // within ~56 degrees of straight
        state->fakie = 0;
        return SKATE_LANDING_CLEAN;
    }

    if (offAxis >= 0x5800) { // within ~56 degrees of backwards
        state->fakie = 1;
        return SKATE_LANDING_FAKIE;
    }

    if (speed > tuning->bailLandSpeed) {
        return SKATE_LANDING_BAIL;
    }

    state->vx *= 0.4f;
    state->vz *= 0.4f;
    return SKATE_LANDING_SKETCHY;
}

SkateWallHit SkatePhysics_HitWall(SkateState* state, int16_t wallYaw, const SkateTuning* tuning) {
    float nx = SinB(wallYaw);
    float nz = CosB(wallYaw);
    float intoWall = -(state->vx * nx + state->vz * nz);
    float along;

    if (intoWall < 0.2f) {
        return SKATE_WALL_NONE;
    }

    // Remove the part of the velocity that points into the wall, keep sliding along it.
    state->vx += intoWall * nx;
    state->vz += intoWall * nz;

    if (intoWall > tuning->bailWallSpeed) {
        return SKATE_WALL_BAIL;
    }

    // The harder the hit, the more speed the scrape costs.
    along = SkatePhysics_Speed(state);
    if (along > 0.0f) {
        float kept = ClampF((along - 0.3f * intoWall) / along, 0.0f, 1.0f);

        state->vx *= kept;
        state->vz *= kept;
        along *= kept;
    }

    if (along > 0.15f * intoWall + 0.5f) {
        // Mostly a glancing hit: the wall turns the board to run along it. Without this the wheels would keep
        // steering back into the wall every frame and grind to a halt.
        int16_t alongYaw = YawOf(state->vx, state->vz);

        state->heading = (AbsYawDiff(alongYaw, state->heading) > 0x4000) ? (int16_t)(alongYaw + 0x8000) : alongYaw;
    } else {
        // Mostly head on: a soft bump rolls the board back off the wall a little.
        state->vx += nx * intoWall * 0.15f;
        state->vz += nz * intoWall * 0.15f;
    }

    return SKATE_WALL_SCRUB;
}
