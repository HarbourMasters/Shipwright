#include "global.h"

void Gfx_DrawDListOpa(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);

     if (ResourceMgr_OTRSigCheck(dlist) == 1)
        gsSPPushCD(POLY_OPA_DISP++, dlist);
    
     gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Gfx_DrawDListXlu(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    
    if (ResourceMgr_OTRSigCheck(dlist) == 1)
        gsSPPushCD(POLY_XLU_DISP++, dlist);
    
    gSPDisplayList(POLY_XLU_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx);
}
