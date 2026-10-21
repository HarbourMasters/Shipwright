#pragma once

struct PlayState;

#ifdef __cplusplus
extern "C" {
#endif

// Places the branch point of the star field in the polyOpa list. It must happen after the skybox is drawn and
// before the sun and moon, so that the stars show through the sky while the moon still covers them.
void MajoraStarField_Setup(struct PlayState* play);

// Fills the branch point placed by MajoraStarField_Setup, once the view matrix of the frame is final.
void MajoraStarField_Draw(struct PlayState* play);

#ifdef __cplusplus
}
#endif
