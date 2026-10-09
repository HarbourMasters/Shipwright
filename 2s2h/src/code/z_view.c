#include "global.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

s32 View_ApplyPerspective(View* view);
s32 View_ApplyOrtho(View* view);

void View_ViewportToVp(Vp* dest, Viewport* src) {
    s32 width = src->rightX - src->leftX;
    s32 height = src->bottomY - src->topY;

    dest->vp.vscale[0] = width * 2;
    dest->vp.vscale[1] = height * 2;
    dest->vp.vscale[2] = 0x01FF;
    dest->vp.vscale[3] = 0;
    dest->vp.vtrans[0] = ((src->leftX * 2) + width) * 2;
    dest->vp.vtrans[1] = ((src->topY * 2) + height) * 2;
    dest->vp.vtrans[2] = 0x01FF;
    dest->vp.vtrans[3] = 0;

    if ((src->leftX == 0) && (src->rightX == HIRES_BUFFER_WIDTH) && (src->topY == 0) &&
        (src->bottomY == HIRES_BUFFER_HEIGHT)) {}
}

void View_Init(View* view, GraphicsContext* gfxCtx) {
    view->gfxCtx = gfxCtx;

    view->viewport.topY = 0;
    view->viewport.bottomY = SCREEN_HEIGHT;
    view->viewport.leftX = 0;
    view->viewport.rightX = SCREEN_WIDTH;

    view->magic = 0x56494557; // "VIEW"

    if (1) {}

    view->scale = 1.0f;
    view->fovy = 60.0f;
    view->zNear = 10.0f;
    view->zFar = 12800.0f;

    view->eye.x = 0.0f;
    view->eye.y = 0.0f;
    view->eye.z = -1.0f;

    view->at.x = 0.0f;

    view->up.x = 0.0f;
    view->up.y = 1.0f;
    view->up.z = 0.0f;

    view->unk164 = 0;
    view->flags = VIEW_VIEWING | VIEW_VIEWPORT | VIEW_PROJECTION_PERSPECTIVE;

    View_InitDistortion(view);
}

void View_LookAt(View* view, Vec3f* eye, Vec3f* at, Vec3f* up) {
    if ((eye->x == at->x) && (eye->z == at->z)) {
        eye->z += 0.1f;
        up->z = 0.0f;
        up->x = 0.0f;
        up->y = 1.0f;
    }

    view->eye = *eye;
    view->at = *at;
    view->up = *up;
    view->flags |= VIEW_VIEWING;
}

/*
 * Unused. View_LookAt is always used instead. This version is similar but
 * is missing the input sanitization and the update to the flags.
 */
void View_LookAtUnsafe(View* view, Vec3f* eye, Vec3f* at, Vec3f* up) {
    view->eye = *eye;
    view->at = *at;
    view->up = *up;
}

void View_SetScale(View* view, f32 scale) {
    view->flags |= VIEW_PROJECTION_PERSPECTIVE;
    view->scale = scale;
}

void View_GetScale(View* view, f32* scale) {
    *scale = view->scale;
}

void View_SetPerspective(View* view, f32 fovy, f32 zNear, f32 zFar) {
    view->fovy = fovy;
    view->zNear = zNear;
    view->zFar = zFar;
    view->flags |= VIEW_PROJECTION_PERSPECTIVE;
}

void View_GetPerspective(View* view, f32* fovy, f32* zNear, f32* zFar) {
    *fovy = view->fovy;
    *zNear = view->zNear;
    *zFar = view->zFar;
}

void View_SetOrtho(View* view, f32 fovy, f32 zNear, f32 zFar) {
    view->fovy = fovy;
    view->zNear = zNear;
    view->zFar = zFar;
    view->flags |= VIEW_PROJECTION_ORTHO;
    view->scale = 1.0f;
}

/*
 * Identical to View_GetPerspective, and never called.
 * Named as it seems to fit the "set, get" pattern.
 */
void View_GetOrtho(View* view, f32* fovy, f32* zNear, f32* zFar) {
    *fovy = view->fovy;
    *zNear = view->zNear;
    *zFar = view->zFar;
}

void View_SetViewport(View* view, Viewport* viewport) {
    view->viewport = *viewport;
    view->flags |= VIEW_VIEWPORT;
}

void View_GetViewport(View* view, Viewport* viewport) {
    *viewport = view->viewport;
}

void View_SetScissor(Gfx** gfx, s32 ulx, s32 uly, s32 lrx, s32 lry) {
    Gfx* gfxP = *gfx;

    gDPSetScissor(gfxP++, G_SC_NON_INTERLACE, ulx, uly, lrx, lry);

    *gfx = gfxP;
}

void View_ClearScissor(View* view, Gfx** gfx) {
    Gfx* gfxP = *gfx;
    s32 ulx = view->viewport.leftX;
    s32 uly = view->viewport.topY;
    s32 lrx = view->viewport.rightX;
    s32 lry = view->viewport.bottomY;

    gDPPipeSync(gfxP++);
    View_SetScissor(&gfxP, ulx, uly, lrx, lry);

    *gfx = gfxP;
}

void View_ApplyLetterbox(View* view) {
    s32 letterboxY;
    s32 letterboxX;
    s32 pad1;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;

    if (GameInteractor_Should(VB_DISABLE_LETTERBOX, false)) {
        return;
    }

    OPEN_DISPS(view->gfxCtx);

    letterboxY = ShrinkWindow_Letterbox_GetSize();

    letterboxX = -1; // The following is optimized to varX = 0 but affects codegen

    if (letterboxX < 0) {
        letterboxX = 0;
    }
    if (letterboxX > (SCREEN_WIDTH / 2)) {
        letterboxX = SCREEN_WIDTH / 2;
    }

    if (letterboxY < 0) {
        letterboxY = 0;
    } else if (letterboxY > (SCREEN_HEIGHT / 2)) {
        letterboxY = SCREEN_HEIGHT / 2;
    }

    ulx = view->viewport.leftX + letterboxX;
    uly = view->viewport.topY + letterboxY;
    lrx = view->viewport.rightX - letterboxX;
    lry = view->viewport.bottomY - letterboxY;

    gDPPipeSync(POLY_OPA_DISP++);
    {
        s32 pad2;
        Gfx* polyOpa;

        polyOpa = POLY_OPA_DISP;
        View_SetScissor(&polyOpa, ulx, uly, lrx, lry);
        POLY_OPA_DISP = polyOpa;
    }

    gDPPipeSync(POLY_XLU_DISP++);
    {
        Gfx* polyXlu;
        s32 pad3;

        polyXlu = POLY_XLU_DISP;
        View_SetScissor(&polyXlu, ulx, uly, lrx, lry);
        POLY_XLU_DISP = polyXlu;
    }

    CLOSE_DISPS(view->gfxCtx);
}

s32 View_SetDistortionOrientation(View* view, f32 rotX, f32 rotY, f32 rotZ) {
    view->distortionOrientation.x = rotX;
    view->distortionOrientation.y = rotY;
    view->distortionOrientation.z = rotZ;
    return 1;
}

s32 View_SetDistortionScale(View* view, f32 scaleX, f32 scaleY, f32 scaleZ) {
    view->distortionScale.x = scaleX;
    view->distortionScale.y = scaleY;
    view->distortionScale.z = scaleZ;
    return 1;
}

s32 View_SetDistortionSpeed(View* view, f32 speed) {
    view->distortionSpeed = speed;
    return 1;
}

s32 View_InitDistortion(View* view) {
    view->distortionOrientation.x = 0.0f;
    view->distortionOrientation.y = 0.0f;
    view->distortionOrientation.z = 0.0f;
    view->distortionScale.x = 1.0f;
    view->distortionScale.y = 1.0f;
    view->distortionScale.z = 1.0f;
    view->curDistortionOrientation = view->distortionOrientation;
    view->curDistortionScale = view->distortionScale;
    view->distortionSpeed = 0.0f;
    return 1;
}

s32 View_ClearDistortion(View* view) {
    view->distortionOrientation.x = 0.0f;
    view->distortionOrientation.y = 0.0f;
    view->distortionOrientation.z = 0.0f;
    view->distortionScale.x = 1.0f;
    view->distortionScale.y = 1.0f;
    view->distortionScale.z = 1.0f;
    view->distortionSpeed = 1.0f;
    return 1;
}

s32 View_SetDistortion(View* view, Vec3f orientation, Vec3f scale, f32 speed) {
    view->distortionOrientation = orientation;
    view->distortionScale = scale;
    view->distortionSpeed = speed;
    return 1;
}

s32 View_StepDistortion(View* view, Mtx* projectionMtx) {
    MtxF projectionMtxF;

    if (view->distortionSpeed == 0.0f) {
        return false;
    }

    if (view->distortionSpeed == 1.0f) {
        view->curDistortionOrientation = view->distortionOrientation;
        view->curDistortionScale = view->distortionScale;
        view->distortionSpeed = 0.0f;
    } else {
        view->curDistortionOrientation.x =
            F32_LERPIMP(view->curDistortionOrientation.x, view->distortionOrientation.x, view->distortionSpeed);
        view->curDistortionOrientation.y =
            F32_LERPIMP(view->curDistortionOrientation.y, view->distortionOrientation.y, view->distortionSpeed);
        view->curDistortionOrientation.z =
            F32_LERPIMP(view->curDistortionOrientation.z, view->distortionOrientation.z, view->distortionSpeed);

        view->curDistortionScale.x =
            F32_LERPIMP(view->curDistortionScale.x, view->distortionScale.x, view->distortionSpeed);
        view->curDistortionScale.y =
            F32_LERPIMP(view->curDistortionScale.y, view->distortionScale.y, view->distortionSpeed);
        view->curDistortionScale.z =
            F32_LERPIMP(view->curDistortionScale.z, view->distortionScale.z, view->distortionSpeed);
    }

    Matrix_MtxToMtxF(projectionMtx, &projectionMtxF);
    Matrix_Put(&projectionMtxF);
    Matrix_RotateXFApply(view->curDistortionOrientation.x);
    Matrix_RotateYF(view->curDistortionOrientation.y, MTXMODE_APPLY);
    Matrix_RotateZF(view->curDistortionOrientation.z, MTXMODE_APPLY);
    Matrix_Scale(view->curDistortionScale.x, view->curDistortionScale.y, view->curDistortionScale.z, MTXMODE_APPLY);
    Matrix_RotateZF(-view->curDistortionOrientation.z, MTXMODE_APPLY);
    Matrix_RotateYF(-view->curDistortionOrientation.y, MTXMODE_APPLY);
    Matrix_RotateXFApply(-view->curDistortionOrientation.x);
    Matrix_ToMtx(projectionMtx);

    return true;
}

/**
 * Apply view to POLY_OPA_DISP, POLY_XLU_DISP (and OVERLAY_DISP if ortho)
 */
s32 View_Apply(View* view, s32 mask) {
    mask = (view->flags & mask) | (mask >> 4);

    if (mask & VIEW_PROJECTION_ORTHO) {
        return View_ApplyOrtho(view);
    } else {
        return View_ApplyPerspective(view);
    }
}

static float sqr(float a) {
    return a * a;
}

s32 View_ApplyPerspective(View* view) {
    f32 aspect;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* projectionFlipped;
    Mtx* viewing;
    GraphicsContext* gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx);

    // Viewport
    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    View_ApplyLetterbox(view);

    gSPViewport(POLY_OPA_DISP++, vp);
    gSPViewport(POLY_XLU_DISP++, vp);

    // Perspective projection
    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    projectionFlipped = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->projectionPtr = projection;
    view->shipMirrorProjectionPtr = projectionFlipped;

    width = view->viewport.rightX - view->viewport.leftX;
    height = view->viewport.bottomY - view->viewport.topY;
    aspect = (f32)width / (f32)height;

    guPerspective(projection, &view->perspNorm, view->fovy, aspect, view->zNear, view->zFar, view->scale);

    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        MtxF flipF;
        SkinMatrix_Clear(&flipF);
        flipF.xx = -1.0;
        MtxF projectionF;
        Matrix_MtxToMtxF(projection, &projectionF);
        SkinMatrix_MtxFMtxFMult(&projectionF, &flipF, &projectionF);
        Matrix_MtxFToMtx(&projectionF, projectionFlipped);
    }

    view->projection = *projection;

    View_StepDistortion(view, projection);

    gSPPerspNormalize(POLY_OPA_DISP++, view->perspNorm);
    gSPMatrix(POLY_OPA_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(POLY_XLU_DISP++, view->perspNorm);
    gSPMatrix(POLY_XLU_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    // View matrix (look-at)
    viewing = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->viewingPtr = viewing;

    if ((view->eye.x == view->at.x) && (view->eye.y == view->at.y) && (view->eye.z == view->at.z)) {
        view->eye.z += 2.0f;
    }

    guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x, view->up.y,
             view->up.z);

    // Some heuristics to identify instant camera movements and skip interpolation in that case

    static View old_view;

    float dirx = view->eye.x - view->at.x;
    float diry = view->eye.y - view->at.y;
    float dirz = view->eye.z - view->at.z;
    float dir_dist = sqrtf(sqr(dirx) + sqr(diry) + sqr(dirz));
    dirx /= dir_dist;
    diry /= dir_dist;
    dirz /= dir_dist;

    float odirx = old_view.eye.x - old_view.at.x;
    float odiry = old_view.eye.y - old_view.at.y;
    float odirz = old_view.eye.z - old_view.at.z;
    float odir_dist = sqrtf(sqr(odirx) + sqr(odiry) + sqr(odirz));
    odirx /= odir_dist;
    odiry /= odir_dist;
    odirz /= odir_dist;

    float eye_dist = sqrtf(sqr(view->eye.x - old_view.eye.x) + sqr(view->eye.y - old_view.eye.y) +
                           sqr(view->eye.z - old_view.eye.z));
    float look_dist =
        sqrtf(sqr(view->at.x - old_view.at.x) + sqr(view->at.y - old_view.at.y) + sqr(view->at.z - old_view.at.z));
    float up_dist =
        sqrtf(sqr(view->up.x - old_view.up.x) + sqr(view->up.y - old_view.up.y) + sqr(view->up.z - old_view.up.z));
    float d_dist = sqrtf(sqr(dirx - odirx) + sqr(diry - odiry) + sqr(dirz - odirz));

    bool dont_interpolate = false;

    if (up_dist < 0.01 && d_dist < 0.01) {
        if (eye_dist + look_dist > 300) {
            dont_interpolate = true;
        }
    } else {
        if (eye_dist >= 400) {
            dont_interpolate = true;
        }
        if (look_dist >= 100) {
            dont_interpolate = true;
        }
        if (up_dist >= 1.50f) {
            dont_interpolate = true;
        }
        if (d_dist >= 1.414f && look_dist >= 15) {
            dont_interpolate = true;
        }
        if (d_dist >= 1.414f && up_dist >= 0.31f && look_dist >= 1 && eye_dist >= 300) {
            dont_interpolate = true;
        }
        if (d_dist >= 0.5f && up_dist >= 0.31f && look_dist >= 3 && eye_dist >= 170) {
            dont_interpolate = true;
        }
        if (look_dist >= 52 && eye_dist >= 52) {
            dont_interpolate = true;
        }
        if (look_dist >= 30 && eye_dist >= 90) {
            dont_interpolate = true;
        }
    }

    // Ignore camera heuristics when not in the PlayState or the game is paused as the camera moves a lot in Kaleido
    if (dont_interpolate && gPlayState != NULL && R_PAUSE_BG_PRERENDER_STATE == PRERENDER_FILTER_STATE_NONE) {
        FrameInterpolation_DontInterpolateCamera();
    }

    FrameInterpolation_RecordOpenChild(NULL, FrameInterpolation_GetCameraEpoch());

    view->viewing = *viewing;
    old_view = *view;
    gSPMatrix(POLY_OPA_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(POLY_XLU_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    FrameInterpolation_RecordCloseChild();

    CLOSE_DISPS(gfxCtx);

    return 1;
}

s32 View_ApplyOrtho(View* view) {
    Vp* vp;
    Mtx* projection;
    Mtx* projectionFlipped;
    GraphicsContext* gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx);

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    View_ApplyLetterbox(view);

    gSPViewport(POLY_OPA_DISP++, vp);
    gSPViewport(POLY_XLU_DISP++, vp);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    projectionFlipped = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->projectionPtr = projection;
    view->shipMirrorProjectionPtr = projectionFlipped;

    guOrtho(projection, gScreenWidth * -0.5f, gScreenWidth * 0.5f, gScreenHeight * -0.5f, gScreenHeight * 0.5f,
            view->zNear, view->zFar, view->scale);

    // if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
    //     MtxF flipF;
    //     SkinMatrix_Clear(&flipF);
    //     flipF.xx = -1.0;
    //     MtxF projectionF;
    //     Matrix_MtxToMtxF(projection, &projectionF);
    //     SkinMatrix_MtxFMtxFMult(&projectionF, &flipF, &projectionF);
    //     Matrix_MtxFToMtx(&projectionF, projectionFlipped);
    // }

    view->projection = *projection;

    gSPMatrix(POLY_OPA_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(POLY_XLU_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx);

    return 1;
}

/**
 * Apply scissor, viewport and projection (ortho) to OVERLAY_DISP.
 */
s32 View_ApplyOrthoToOverlay(View* view) {
    Vp* vp;
    Mtx* projection;
    Mtx* projectionFlipped;
    GraphicsContext* gfxCtx;

    gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx);

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    gDPPipeSync(OVERLAY_DISP++);
    {
        Gfx* overlay;
        s32 pad;

        overlay = OVERLAY_DISP;
        View_SetScissor(&overlay, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                        view->viewport.bottomY);
        OVERLAY_DISP = overlay;
    }

    gSPViewport(OVERLAY_DISP++, vp);
    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    projectionFlipped = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->projectionPtr = projection;
    view->shipMirrorProjectionPtr = projectionFlipped;

    guOrtho(projection, gScreenWidth * -0.5f, gScreenWidth * 0.5f, gScreenHeight * -0.5f, gScreenHeight * 0.5f,
            view->zNear, view->zFar, view->scale);

    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        MtxF flipF;
        SkinMatrix_Clear(&flipF);
        flipF.xx = -1.0;
        MtxF projectionF;
        Matrix_MtxToMtxF(projection, &projectionF);
        SkinMatrix_MtxFMtxFMult(&projectionF, &flipF, &projectionF);
        Matrix_MtxFToMtx(&projectionF, projectionFlipped);
    }

    view->projection = *projection;

    gSPMatrix(OVERLAY_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx);
    return 1;
}

/**
 * Apply scissor, viewport, view and projection (perspective) to OVERLAY_DISP.
 */
s32 View_ApplyPerspectiveToOverlay(View* view) {
    f32 aspect;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;
    GraphicsContext* gfxCtx;
    s32 pad;

    gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx);

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    gDPPipeSync(OVERLAY_DISP++);
    {
        s32 pad;
        Gfx* overlay;

        overlay = OVERLAY_DISP;
        View_SetScissor(&overlay, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                        view->viewport.bottomY);
        OVERLAY_DISP = overlay;
    }

    gSPViewport(OVERLAY_DISP++, vp);
    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->projectionPtr = projection;

    width = view->viewport.rightX - view->viewport.leftX;
    height = view->viewport.bottomY - view->viewport.topY;
    aspect = (f32)width / (f32)height;

    guPerspective(projection, &view->perspNorm, view->fovy, aspect, view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPPerspNormalize(OVERLAY_DISP++, view->perspNorm);
    gSPMatrix(OVERLAY_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    viewing = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->viewingPtr = viewing;

    // This check avoids a divide-by-zero in guLookAt if eye == at
    if (view->eye.x == view->at.x && view->eye.y == view->at.y && view->eye.z == view->at.z) {
        view->eye.z += 2.0f;
    }

    guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x, view->up.y,
             view->up.z);

    view->viewing = *viewing;

    gSPMatrix(OVERLAY_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx);

    return 1;
}

/**
 * Just updates view's view matrix from its eye/at/up vectors.
 */
s32 View_UpdateViewingMatrix(View* view) {
    guLookAt(view->viewingPtr, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x,
             view->up.y, view->up.z);

    view->unkE0 = *view->viewingPtr;
    view->viewingPtr = &view->unkE0;

    return 1;
}

s32 View_ApplyTo(View* view, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    GraphicsContext* gfxCtx = view->gfxCtx;
    Viewport* viewport = &view->viewport;
    Mtx* projection;
    Vp* vp;

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    View_ViewportToVp(vp, viewport);
    view->vp = *vp;

    View_ClearScissor(view, &gfx);

    gSPViewport(gfx++, vp);

    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    view->projectionPtr = projection;

    guOrtho(projection, gScreenWidth * -0.5f, gScreenWidth * 0.5f, gScreenHeight * -0.5f, gScreenHeight * 0.5f,
            view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPMatrix(gfx++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    *gfxP = gfx;

    return 1;
}
