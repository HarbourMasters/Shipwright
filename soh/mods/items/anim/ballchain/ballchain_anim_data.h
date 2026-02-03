/**
 * Ball and Chain Pose Data
 * Joint values for equip and spin poses
 */

#ifndef BALLCHAIN_ANIM_DATA_H
#define BALLCHAIN_ANIM_DATA_H

#include "z64.h"

// Equip pose - arms extended forward holding the ball
// Left arm
#define BC_EQUIP_L_SHOULDER_X  0x255e
#define BC_EQUIP_L_SHOULDER_Y  0xecc6
#define BC_EQUIP_L_SHOULDER_Z  0x5e6d
#define BC_EQUIP_L_FOREARM_X   0xe207
#define BC_EQUIP_L_FOREARM_Y   0x097b
#define BC_EQUIP_L_FOREARM_Z   0xd19c
#define BC_EQUIP_L_HAND_X      0xa012
#define BC_EQUIP_L_HAND_Y      0xa74b
#define BC_EQUIP_L_HAND_Z      0x0651

// Right arm
#define BC_EQUIP_R_SHOULDER_X  0xdaa1
#define BC_EQUIP_R_SHOULDER_Y  0x1339
#define BC_EQUIP_R_SHOULDER_Z  0x5e6d
#define BC_EQUIP_R_FOREARM_X   0x1df8
#define BC_EQUIP_R_FOREARM_Y   0xf684
#define BC_EQUIP_R_FOREARM_Z   0xd19c
#define BC_EQUIP_R_HAND_X      0x5fed
#define BC_EQUIP_R_HAND_Y      0x58b4
#define BC_EQUIP_R_HAND_Z      0x0651

// Spin pose - arms raised for spinning motion
// Left arm
#define BC_SPIN_L_SHOULDER_X   0x052b
#define BC_SPIN_L_SHOULDER_Y   0xed4b
#define BC_SPIN_L_SHOULDER_Z   0xfc8b
#define BC_SPIN_L_FOREARM_X    0xf742
#define BC_SPIN_L_FOREARM_Y    0x29ec
#define BC_SPIN_L_FOREARM_Z    0xf6de
#define BC_SPIN_L_HAND_X       0xf74c
#define BC_SPIN_L_HAND_Y       0xe70a
#define BC_SPIN_L_HAND_Z       0xb74f

// Right arm
#define BC_SPIN_R_SHOULDER_X   0xfcb3
#define BC_SPIN_R_SHOULDER_Y   0x1232
#define BC_SPIN_R_SHOULDER_Z   0xff47
#define BC_SPIN_R_FOREARM_X    0xfa6e
#define BC_SPIN_R_FOREARM_Y    0xd605
#define BC_SPIN_R_FOREARM_Z    0x040d
#define BC_SPIN_R_HAND_X       0x0d24
#define BC_SPIN_R_HAND_Y       0x16e7
#define BC_SPIN_R_HAND_Z       0xb7eb

#endif // BALLCHAIN_ANIM_DATA_H
