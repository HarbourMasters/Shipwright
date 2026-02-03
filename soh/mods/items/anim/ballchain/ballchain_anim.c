/**
 * Ball and Chain Animation/Pose Implementation
 */

#include "ballchain_anim.h"
#include "ballchain_anim_data.h"

void BallChain_ResetPose(Player* p) {
    p->upperLimbRot.x = 0;
    p->upperLimbRot.y = 0;
    p->upperLimbRot.z = 0;
}

void BallChain_SetEquipPose(Player* p) {
    // Left arm
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].x = BC_EQUIP_L_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].y = BC_EQUIP_L_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].z = BC_EQUIP_L_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].x = BC_EQUIP_L_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].y = BC_EQUIP_L_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].z = BC_EQUIP_L_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].x = BC_EQUIP_L_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].y = BC_EQUIP_L_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].z = BC_EQUIP_L_HAND_Z;

    // Right arm
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].x = BC_EQUIP_R_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].y = BC_EQUIP_R_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].z = BC_EQUIP_R_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].x = BC_EQUIP_R_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].y = BC_EQUIP_R_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].z = BC_EQUIP_R_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].x = BC_EQUIP_R_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].y = BC_EQUIP_R_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].z = BC_EQUIP_R_HAND_Z;

    // Reset torso lean
    p->upperLimbRot.x = 0;
    p->upperLimbRot.y = 0;
    p->upperLimbRot.z = 0;
}

void BallChain_SetSpinPose(Player* p, f32 stickX, f32 stickY) {
    // Left arm
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].x = BC_SPIN_L_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].y = BC_SPIN_L_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_SHOULDER].z = BC_SPIN_L_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].x = BC_SPIN_L_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].y = BC_SPIN_L_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_FOREARM].z = BC_SPIN_L_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].x = BC_SPIN_L_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].y = BC_SPIN_L_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_L_HAND].z = BC_SPIN_L_HAND_Z;

    // Right arm
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].x = BC_SPIN_R_SHOULDER_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].y = BC_SPIN_R_SHOULDER_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_SHOULDER].z = BC_SPIN_R_SHOULDER_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].x = BC_SPIN_R_FOREARM_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].y = BC_SPIN_R_FOREARM_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_FOREARM].z = BC_SPIN_R_FOREARM_Z;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].x = BC_SPIN_R_HAND_X;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].y = BC_SPIN_R_HAND_Y;
    p->skelAnime.jointTable[PLAYER_LIMB_R_HAND].z = BC_SPIN_R_HAND_Z;

    // Lean - inverted: stick right = lean left (resisting the weight)
    p->upperLimbRot.x = (s16)(-stickY * BALLCHAIN_POSE_LEAN_MULT);
    p->upperLimbRot.y = 0;
    p->upperLimbRot.z = (s16)(stickX * BALLCHAIN_POSE_LEAN_MULT);
}
