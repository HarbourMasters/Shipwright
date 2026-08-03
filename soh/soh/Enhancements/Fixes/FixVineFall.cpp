#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "z64bgcheck.h"
}

static void RegisterFixVineFall() {
    // conflicts arise from these two being enabled at once, and with ClimbEverything on, FixVineFall is redundant
    // anyway
    COND_VB_SHOULD(
        VB_REVALIDATE_CLIMBED_WALL,
        CVarGetInteger(CVAR_ENHANCEMENT("FixVineFall"), 0) && !CVarGetInteger(CVAR_CHEAT("ClimbEverything"), 0), {
            PlayState* play = va_arg(args, PlayState*);
            Player* player = va_arg(args, Player*);
            u32* touchedWallFlags = va_arg(args, u32*);
            s16* yawDiff = va_arg(args, s16*);

            /* This fixes the "started climbing a wall and then immediately fell off" bug.
             * The main idea is if a climbing wall is detected, double-check that it will
             * still be valid once climbing begins by doing a second raycast with a small
             * margin to make sure it still hits a climbable poly. Then update the flags
             * in touchedWallFlags again and proceed as normal.
             */
            if (*touchedWallFlags & WALL_FLAG_CLIMBABLE) {
                Vec3f checkPosA;
                Vec3f checkPosB;
                Vec3f raycastResult;
                CollisionPoly* wallPoly;
                s32 wallBgId;
                f32 yawCos;
                f32 yawSin;
                s32 hitWall;

                /* Angle the raycast slightly out towards the side based on the angle of
                 * attack the player takes coming at the climb wall. This is necessary because
                 * the player's XZ position actually wobbles very slightly while climbing
                 * due to small rounding errors in the sin/cos lookup tables. This wobble
                 * can cause wall checks while climbing to be slightly left or right of
                 * the wall check to start the climb. By adding this buffer it accounts for
                 * any possible wobble. The end result is the player has to be further than
                 * some epsilon distance from the edge of the climbing poly to actually
                 * start the climb. I divide it by 2 to make that epsilon slightly smaller,
                 * mainly for visuals. Using the full yawDiff leaves a noticeable gap on
                 * the edges that can't be climbed. But with the half distance it looks like
                 * the player is climbing right on the edge, and still works.
                 */
                yawCos = Math_CosS(player->actor.wallYaw - (*yawDiff / 2) + 0x8000);
                yawSin = Math_SinS(player->actor.wallYaw - (*yawDiff / 2) + 0x8000);
                checkPosA.x = player->actor.world.pos.x + (-20.0f * yawSin);
                checkPosA.z = player->actor.world.pos.z + (-20.0f * yawCos);
                checkPosB.x = player->actor.world.pos.x + (50.0f * yawSin);
                checkPosB.z = player->actor.world.pos.z + (50.0f * yawCos);
                checkPosB.y = checkPosA.y = player->actor.world.pos.y + 26.0f;

                hitWall = BgCheck_EntityLineTest1(&play->colCtx, &checkPosA, &checkPosB, &raycastResult, &wallPoly,
                                                  true, false, false, true, &wallBgId);

                if (hitWall) {
                    player->actor.wallPoly = wallPoly;
                    player->actor.wallBgId = wallBgId;
                    player->actor.wallYaw = Math_Atan2S(wallPoly->normal.z, wallPoly->normal.x);
                    *yawDiff = player->actor.shape.rot.y - (s16)(player->actor.wallYaw + 0x8000);

                    *touchedWallFlags =
                        SurfaceType_GetWallFlags(&play->colCtx, player->actor.wallPoly, player->actor.wallBgId);
                }
            }
        });
}

static RegisterShipInitFunc initFunc(RegisterFixVineFall,
                                     { CVAR_ENHANCEMENT("FixVineFall"), CVAR_CHEAT("ClimbEverything") });
