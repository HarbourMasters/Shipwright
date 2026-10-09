/**
 * Sfx Ocarina Bank
 *
 * DEFINE_SFX should be used for all sfx define in the ocarina bank from sequence 0
 *    - Argument 0: Enum value for this sfx
 *    - Argument 1: Importance for deciding which sfx to prioritize. Higher values have greater importance
 *    - Argument 2: Slows the decay of volume with distance (a 3-bit number ranging from 0-7)
 *    - Argument 3: Applies increasingly random offsets to frequency (a 2-bit number ranging from 0-3)
 *    - Argument 4: Various flags to add properties to the sfx
 *    - Argument 5: Various flags to add properties to the sfx
 *
 * WARNING: entries must align with the table defined for the ocarina bank in sequence 0
 */
/* 0x5800 */ DEFINE_SFX(NA_SE_OC_OCARINA, 0x30, 0, 0, 0, 0)
/* 0x5801 */ DEFINE_SFX(NA_SE_OC_ABYSS, 0x30, 0, 0, 0, SFX_FLAG_BLOCK_EQUAL_IMPORTANCE)
/* 0x5802 */ DEFINE_SFX(NA_SE_OC_DOOR_OPEN, 0x30, 2, 1, 0, SFX_FLAG_BEHIND_SCREEN_Z_INDEX | SFX_FLAG_SURROUND_LOWPASS_FILTER)
/* 0x5803 */ DEFINE_SFX(NA_SE_OC_SECRET_WARP_IN, 0x30, 0, 0, 0, 0)
/* 0x5804 */ DEFINE_SFX(NA_SE_OC_SECRET_WARP_OUT, 0x30, 0, 0, 0, 0)
/* 0x5805 */ DEFINE_SFX(NA_SE_OC_SECRET_HOLE_OUT, 0x30, 0, 0, 0, 0)
/* 0x5806 */ DEFINE_SFX(NA_SE_OC_REVENGE, 0x30, 0, 0, 0, 0)
/* 0x5807 */ DEFINE_SFX(NA_SE_OC_TUNAMI, 0x30, 0, 0, 0, 0)
/* 0x5808 */ DEFINE_SFX(NA_SE_OC_TELOP_IMPACT, 0x30, 0, 0, 0, 0)
/* 0x5809 */ DEFINE_SFX(NA_SE_OC_WOOD_GATE_OPEN, 0x30, 0, 0, 0, 0)
/* 0x580A */ DEFINE_SFX(NA_SE_OC_FIREWORKS, 0x30, 0, 0, 0, SFX_FLAG_VOLUME_NO_DIST)
/* 0x580B */ DEFINE_SFX(NA_SE_OC_WHITE_OUT_INTO_KYOJIN, 0x30, 0, 0, 0, SFX_FLAG_VOLUME_NO_DIST)
/* 0x580C */ DEFINE_SFX(NA_SE_OC_12, 0x30, 0, 0, 0, 0)
/* 0x580D */ DEFINE_SFX(NA_SE_OC_13, 0x30, 0, 0, 0, 0)
/* 0x580E */ DEFINE_SFX(NA_SE_OC_14, 0x30, 0, 0, 0, 0)
/* 0x580F */ DEFINE_SFX(NA_SE_OC_15, 0x30, 0, 0, 0, 0)
