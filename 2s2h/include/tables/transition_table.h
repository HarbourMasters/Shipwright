/**
 * Transition Table
 *
 * DEFINE_TRANSITION should be used for normal transitions
 *    - Argument 0: Enum value for this transition
 *    - Argument 1: Name of the effect struct
 *    - Argument 2: Instance name
 *    - Argument 3: Name of the effect (without the ovl_ part)
 *
 * DEFINE_TRANSITION_INTERNAL should be used for transitions that aren't an overlay
 *    - Argument 0: Enum value for this transition
 *    - Argument 1: Name of the effect struct
 *    - Argument 2: Instance name
 */
/* 0 */ DEFINE_TRANSITION_INTERNAL(FBDEMO_FADE, TransitionFade, fade)
/* 1 */ DEFINE_TRANSITION(FBDEMO_TRIFORCE, TransitionTriforce, triforce, fbdemo_triforce)
/* 2 */ DEFINE_TRANSITION(FBDEMO_WIPE1, TransitionWipe1, wipe1, fbdemo_wipe1)
/* 3 */ DEFINE_TRANSITION(FBDEMO_WIPE3, TransitionWipe3, wipe3, fbdemo_wipe3)
/* 4 */ DEFINE_TRANSITION(FBDEMO_WIPE4, TransitionWipe4, wipe4, fbdemo_wipe4)
/* 5 */ DEFINE_TRANSITION_INTERNAL(FBDEMO_CIRCLE, TransitionCircle, circle)
/* 6 */ DEFINE_TRANSITION(FBDEMO_WIPE5, TransitionWipe5, wipe5, fbdemo_wipe5)
