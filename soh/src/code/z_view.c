#include "global.h"
#include "vt.h"

#include <string.h>
#include <math.h>

#include "soh/frame_interpolation.h"

vu32 D_8012ABF0 = true;

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
}

View* View_New(GraphicsContext* gfxCtx) {
    View* view = SystemArena_MallocDebug(sizeof(View), "../z_view.c", 285);

    if (view != NULL) {
        memset(view, 0, sizeof(View));
        View_Init(view, gfxCtx);
    }

    return view;
}

void View_Free(View* view) {
    SystemArena_FreeDebug(view, "../z_view.c", 297);
}

void View_Init(View* view, GraphicsContext* gfxCtx) {
    view->gfxCtx = gfxCtx;
    view->viewport.topY = 0;
    view->viewport.bottomY = SCREEN_HEIGHT;
    view->viewport.leftX = 0;
    view->viewport.rightX = SCREEN_WIDTH;
    view->magic = 0x56494557; // "VIEW"
    view->eye.x = 0.0f;
    view->eye.y = 0.0f;
    view->scale = 1.0f;
    view->fovy = 60.0f;
    view->zNear = 10.0f;
    view->zFar = 12800.0f;
    view->lookAt.x = 0.0f;
    view->up.x = 0.0f;
    view->up.y = 1.0f;
    view->up.z = 0.0f;
    view->eye.z = -1.0f;

    if (D_8012ABF0) {
        if (D_8012ABF0 == 0) {}
        osSyncPrintf("\nview: initialize ---\n");
        D_8012ABF0 = false;
    }

    view->unk_124 = 0;
    view->flags = 1 | 2 | 4;
    func_800AA7B8(view);
}

void View_GetParams(View* view, Vec3f* eye, Vec3f* lookAt, Vec3f* up) {
    /*if (eye->x == lookAt->x && eye->z == lookAt->z) {
        eye->x += 0.1f;
    }*/

    *eye = view->eye;
    *lookAt = view->lookAt;
    *up = view->up;
    //view->flags |= 1;
}


void func_800AA358(View* view, Vec3f* eye, Vec3f* lookAt, Vec3f* up) {
    if (eye->x == lookAt->x && eye->z == lookAt->z) {
        eye->x += 0.1f;
    }

    view->eye = *eye;
    view->lookAt = *lookAt;
    view->up = *up;
    view->flags |= 1;
}

void func_800AA3F0(View* view, Vec3f* eye, Vec3f* lookAt, Vec3f* up) {
    view->eye = *eye;
    view->lookAt = *lookAt;
    view->up = *up;
}

void View_SetScale(View* view, f32 scale) {
    view->flags |= 4;
    view->scale = scale;
}

void View_GetScale(View* view, f32* scale) {
    *scale = view->scale;
}

void func_800AA460(View* view, f32 fovy, f32 near, f32 far) {
    view->fovy = fovy;
    view->zNear = near;
    //view->zNear = -30;
    view->zFar = far;
    view->flags |= 4;
}

void func_800AA48C(View* view, f32* fovy, f32* near, f32* far) {
    *fovy = view->fovy;
    *near = view->zNear;
    *far = view->zFar;
}

void func_800AA4A8(View* view, f32 fovy, f32 near, f32 far) {
    view->fovy = fovy;
    view->zNear = near;
    //view->zNear = -30;
    view->zFar = far;
    view->flags |= 8;
    view->scale = 1.0f;
}

void func_800AA4E0(View* view, f32* fovy, f32* near, f32* far) {
    *fovy = view->fovy;
    *near = view->zNear;
    *far = view->zFar;
}

void View_SetViewport(View* view, Viewport* viewport) {
    view->viewport = *viewport;
    view->flags |= 2;
}

void View_GetViewport(View* view, Viewport* viewport) {
    *viewport = view->viewport;
}

void func_800AA550(View* view) {
    s32 varY;
    s32 varX;
    s32 pad;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    GraphicsContext* gfxCtx = view->gfxCtx;

    varY = ShrinkWindow_GetCurrentVal();

    varX = -1; // The following is optimized to varX = 0 but affects codegen

    if (varX < 0) {
        varX = 0;
    }
    if (varX > SCREEN_WIDTH / 2) {
        varX = SCREEN_WIDTH / 2;
    }

    if (varY < 0) {
        varY = 0;
    }
    if (varY > SCREEN_HEIGHT / 2) {
        varY = SCREEN_HEIGHT / 2;
    }

    ulx = view->viewport.leftX + varX;
    uly = view->viewport.topY + varY;
    lrx = view->viewport.rightX - varX;
    lry = view->viewport.bottomY - varY;

    ASSERT(ulx >= 0, "ulx >= 0", "../z_view.c", 454);
    ASSERT(uly >= 0, "uly >= 0", "../z_view.c", 455);
    ASSERT(lrx <= SCREEN_WIDTH, "lrx <= SCREEN_WD", "../z_view.c", 456);
    ASSERT(lry <= SCREEN_HEIGHT, "lry <= SCREEN_HT", "../z_view.c", 457);

    OPEN_DISPS(gfxCtx, "../z_view.c", 459);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetScissor(POLY_OPA_DISP++, G_SC_NON_INTERLACE, ulx, uly, lrx, lry);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetScissor(POLY_XLU_DISP++, G_SC_NON_INTERLACE, ulx, uly, lrx, lry);
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetScissor(POLY_KAL_DISP++, G_SC_NON_INTERLACE, ulx, uly, lrx, lry);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 472);
}

void func_800AA76C(View* view, f32 x, f32 y, f32 z) {
    view->unk_E8.x = x;
    view->unk_E8.y = y;
    view->unk_E8.z = z;
}

void func_800AA78C(View* view, f32 x, f32 y, f32 z) {
    view->unk_F4.x = x;
    view->unk_F4.y = y;
    view->unk_F4.z = z;
}

s32 func_800AA7AC(View* view, f32 arg1) {
    view->unk_100 = arg1;
}

void func_800AA7B8(View* view) {
    view->unk_E8.x = 0.0f;
    view->unk_E8.y = 0.0f;
    view->unk_E8.z = 0.0f;
    view->unk_F4.x = 1.0f;
    view->unk_F4.y = 1.0f;
    view->unk_F4.z = 1.0f;
    view->unk_104 = view->unk_E8;
    view->unk_110 = view->unk_F4;
    view->unk_100 = 0.0f;
}

void func_800AA814(View* view) {
    view->unk_E8.x = 0.0f;
    view->unk_E8.y = 0.0f;
    view->unk_E8.z = 0.0f;
    view->unk_F4.x = 1.0f;
    view->unk_F4.y = 1.0f;
    view->unk_F4.z = 1.0f;
    view->unk_100 = 1.0f;
}

void func_800AA840(View* view, Vec3f vec1, Vec3f vec2, f32 arg3) {
    view->unk_E8 = vec1;
    view->unk_F4 = vec2;
    view->unk_100 = arg3;
}

s32 func_800AA890(View* view, Mtx* mtx) {
    MtxF mf;

    if (view->unk_100 == 0.0f) {
        return 0;
    } else if (view->unk_100 == 1.0f) {
        view->unk_104 = view->unk_E8;
        view->unk_110 = view->unk_F4;
        view->unk_100 = 0.0f;
    } else {
        view->unk_104.x += ((view->unk_E8.x - view->unk_104.x) * view->unk_100);
        view->unk_104.y += ((view->unk_E8.y - view->unk_104.y) * view->unk_100);
        view->unk_104.z += ((view->unk_E8.z - view->unk_104.z) * view->unk_100);

        view->unk_110.x += ((view->unk_F4.x - view->unk_110.x) * view->unk_100);
        view->unk_110.y += ((view->unk_F4.y - view->unk_110.y) * view->unk_100);
        view->unk_110.z += ((view->unk_F4.z - view->unk_110.z) * view->unk_100);
    }

    Matrix_MtxToMtxF(mtx, &mf);
    Matrix_Put(&mf);
    Matrix_RotateX(view->unk_104.x, MTXMODE_APPLY);
    Matrix_RotateY(view->unk_104.y, MTXMODE_APPLY);
    Matrix_RotateZ(view->unk_104.z, MTXMODE_APPLY);
    Matrix_Scale(view->unk_110.x, view->unk_110.y, view->unk_110.z, MTXMODE_APPLY);
    Matrix_RotateZ(-view->unk_104.z, MTXMODE_APPLY);
    Matrix_RotateY(-view->unk_104.y, MTXMODE_APPLY);
    Matrix_RotateX(-view->unk_104.x, MTXMODE_APPLY);
    Matrix_ToMtx(mtx, "../z_view.c", 566);

    return 1;
}

void func_800AAA50(View* view, s32 arg1) {
    arg1 = (view->flags & arg1) | (arg1 >> 4);

    if (arg1 & 8) {
        func_800AB0A8(view);
    } else {
        func_800AAA9C(view);
    }
}

static float sqr(float a) {
    return a * a;
}

s32 func_800AAA9C(View* view) {
    f32 aspect;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;
    GraphicsContext* gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_view.c", 596);

    vp = Graph_Alloc(gfxCtx, sizeof(Vp));
    LogUtils_CheckNullPointer("vp", vp, "../z_view.c", 601);
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    func_800AA550(view);

    gSPViewport(POLY_OPA_DISP++, vp);
    gSPViewport(POLY_XLU_DISP++, vp);
    gSPViewport(POLY_KAL_DISP++, vp);

    projection = Graph_Alloc(gfxCtx, sizeof(Mtx));
    LogUtils_CheckNullPointer("projection", projection, "../z_view.c", 616);
    view->projectionPtr = projection;

    width = view->viewport.rightX - view->viewport.leftX;
    height = view->viewport.bottomY - view->viewport.topY;
    aspect = (f32)width / (f32)height;

    viewing = Graph_Alloc(gfxCtx, sizeof(Mtx));
    LogUtils_CheckNullPointer("viewing", viewing, "../z_view.c", 667);
    view->viewingPtr = viewing;

    if (view->eye.x == view->lookAt.x && view->eye.y == view->lookAt.y && view->eye.z == view->lookAt.z) {
        view->eye.x += 1.0f;
        view->eye.y += 1.0f;
        view->eye.z += 1.0f;
    }

    func_800ABE74(view->eye.x, view->eye.y, view->eye.z);
    MtxF viewingF;
    guLookAtF(viewingF.mf, view->eye.x, view->eye.y, view->eye.z, view->lookAt.x, view->lookAt.y, view->lookAt.z, view->up.x,
             view->up.y, view->up.z);

    // Some heuristics to identify instant camera movements and skip interpolation in that case

    static View old_view;

    float dirx = view->eye.x - view->lookAt.x;
    float diry = view->eye.y - view->lookAt.y;
    float dirz = view->eye.z - view->lookAt.z;
    float dir_dist = sqrtf(sqr(dirx) + sqr(diry) + sqr(dirz));
    dirx /= dir_dist;
    diry /= dir_dist;
    dirz /= dir_dist;

    float odirx = old_view.eye.x - old_view.lookAt.x;
    float odiry = old_view.eye.y - old_view.lookAt.y;
    float odirz = old_view.eye.z - old_view.lookAt.z;
    float odir_dist = sqrtf(sqr(odirx) + sqr(odiry) + sqr(odirz));
    odirx /= odir_dist;
    odiry /= odir_dist;
    odirz /= odir_dist;

    float eye_dist = sqrtf(sqr(view->eye.x - old_view.eye.x) + sqr(view->eye.y - old_view.eye.y) + sqr(view->eye.z - old_view.eye.z));
    float look_dist = sqrtf(sqr(view->lookAt.x - old_view.lookAt.x) + sqr(view->lookAt.y - old_view.lookAt.y) + sqr(view->lookAt.z - old_view.lookAt.z));
    float up_dist = sqrtf(sqr(view->up.x - old_view.up.x) + sqr(view->up.y - old_view.up.y) + sqr(view->up.z - old_view.up.z));
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

    if (dont_interpolate) {
        FrameInterpolation_DontInterpolateCamera();
    }

    FrameInterpolation_RecordOpenChild(NULL, FrameInterpolation_GetCameraEpoch());

    if (HREG(80) == 11) {
        if (HREG(94) != 11) {
            HREG(94) = 11;
            HREG(83) = 60;
            HREG(84) = 13333;
            HREG(85) = 10;
            HREG(86) = 12800;
            HREG(87) = 100;
        }
        guPerspective(projection, &view->normal, HREG(83), HREG(84) / 10000.0f, HREG(85), HREG(86), HREG(87) / 100.0f);
    } else {
        guPerspective(projection, &view->normal, view->fovy, aspect, view->zNear, view->zFar, view->scale);
    }

    if (QREG(88) & 1) {
        s32 i;
        MtxF mf;

        osSyncPrintf("fovy %f near %f far %f scale %f aspect %f normal %08x\n", view->fovy, view->zNear, view->zFar,
                     view->scale, aspect, view->normal);

        Matrix_MtxToMtxF(projection, &mf);
        osSyncPrintf("projection\n");
        for (i = 0; i < 4; i++) {
            osSyncPrintf("	%f	%f	%f	%f\n", mf.mf[i][0], mf.mf[i][1], mf.mf[i][2], mf.mf[i][3]);
        }
        osSyncPrintf("\n");
    }

    view->projection = *projection;

    func_800AA890(view, projection);

    gSPPerspNormalize(POLY_OPA_DISP++, view->normal);
    gSPMatrix(POLY_OPA_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(POLY_XLU_DISP++, view->normal);
    gSPMatrix(POLY_XLU_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(POLY_KAL_DISP++, view->normal);
    gSPMatrix(POLY_KAL_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    Matrix_MtxFToMtx(viewingF.mf, viewing);

    view->viewing = *viewing;


    /*if (eye_dist > 1 || look_dist > 1 || abs(up_dist) > 0.1 || abs(d_dist) > 0.1)
        printf("%d %f %f %f, %f %f %f, %f %f %f, %f %f %f %f %d\n", (int)view->fovy, view->eye.x, view->eye.y, view->eye.z, view->lookAt.x, view->lookAt.y, view->lookAt.z,
           view->up.x, view->up.y, view->up.z, eye_dist, look_dist, up_dist, d_dist, dont_interpolate);*/

    old_view = *view;

    if (QREG(88) & 2) {
        s32 i;
        MtxF mf;

        Matrix_MtxToMtxF(view->viewingPtr, &mf);
        osSyncPrintf("viewing\n");
        for (i = 0; i < 4; i++) {
            osSyncPrintf("	%f	%f	%f	%f\n", mf.mf[i][0], mf.mf[i][1], mf.mf[i][2], mf.mf[i][3]);
        }
        osSyncPrintf("\n");
    }
    gSPMatrix(POLY_OPA_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(POLY_XLU_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(POLY_KAL_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    FrameInterpolation_RecordCloseChild();

    CLOSE_DISPS(gfxCtx, "../z_view.c", 711);

    return 1;
}

s32 func_800AB0A8(View* view) {
    Vp* vp;
    Mtx* projection;
    GraphicsContext* gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_view.c", 726);

    vp = Graph_Alloc(gfxCtx, sizeof(Vp));
    LogUtils_CheckNullPointer("vp", vp, "../z_view.c", 730);
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    func_800AA550(view);

    gSPViewport(POLY_OPA_DISP++, vp);
    gSPViewport(POLY_XLU_DISP++, vp);
    gSPViewport(POLY_KAL_DISP++, vp);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = Graph_Alloc(gfxCtx, sizeof(Mtx));
    LogUtils_CheckNullPointer("projection", projection, "../z_view.c", 744);
    view->projectionPtr = projection;

    guOrtho(projection, -(f32)gScreenWidth * 0.5f, (f32)gScreenWidth * 0.5f, -(f32)gScreenHeight * 0.5f,
            (f32)gScreenHeight * 0.5f, view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPMatrix(POLY_OPA_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(POLY_XLU_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(POLY_KAL_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 762);

    return 1;
}

s32 func_800AB2C4(View* view) {
    Vp* vp;
    Mtx* projection;
    GraphicsContext* gfxCtx;

    gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_view.c", 777);

    vp = Graph_Alloc(gfxCtx, sizeof(Vp));
    LogUtils_CheckNullPointer("vp", vp, "../z_view.c", 781);
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                  view->viewport.bottomY);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = Graph_Alloc(gfxCtx, sizeof(Mtx));
    LogUtils_CheckNullPointer("projection", projection, "../z_view.c", 791);
    view->projectionPtr = projection;

    guOrtho(projection, -(f32)gScreenWidth * 0.5f, (f32)gScreenWidth * 0.5f, -(f32)gScreenHeight * 0.5f,
            (f32)gScreenHeight * 0.5f, -30, view->zFar, view->scale);

    view->projection = *projection;

    gSPMatrix(OVERLAY_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 801);

    return 1;
}

s32 func_800AB560(View* view) {
    s32 pad[2];
    f32 aspect;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;
    GraphicsContext* gfxCtx = view->gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_view.c", 816);

    vp = Graph_Alloc(gfxCtx, sizeof(Vp));
    LogUtils_CheckNullPointer("vp", vp, "../z_view.c", 821);
    View_ViewportToVp(vp, &view->viewport);
    view->vp = *vp;

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                  view->viewport.bottomY);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = Graph_Alloc(gfxCtx, sizeof(Mtx));
    LogUtils_CheckNullPointer("projection", projection, "../z_view.c", 833);
    view->projectionPtr = projection;

    width = view->viewport.rightX - view->viewport.leftX;
    height = view->viewport.bottomY - view->viewport.topY;

    aspect = (f32)width / (f32)height;
    guPerspective(projection, &view->normal, view->fovy, aspect, view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPPerspNormalize(OVERLAY_DISP++, view->normal);
    gSPMatrix(OVERLAY_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    viewing = Graph_Alloc(gfxCtx, sizeof(Mtx));
    LogUtils_CheckNullPointer("viewing", viewing, "../z_view.c", 848);
    view->viewingPtr = viewing;

    if (view->eye.x == view->lookAt.x && view->eye.y == view->lookAt.y && view->eye.z == view->lookAt.z) {
        view->eye.x += 1.0f;
        view->eye.y += 1.0f;
        view->eye.z += 1.0f;
    }

    func_800ABE74(view->eye.x, view->eye.y, view->eye.z);
    guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->lookAt.x, view->lookAt.y, view->lookAt.z, view->up.x,
             view->up.y, view->up.z);

    view->viewing = *viewing;

    gSPMatrix(OVERLAY_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 871);

    return 1;
}

s32 func_800AB944(View* view) {
    OPEN_DISPS(view->gfxCtx, "../z_view.c", 878);

    func_800ABE74(view->eye.x, view->eye.y, view->eye.z);
    guLookAt(view->viewingPtr, view->eye.x, view->eye.y, view->eye.z, view->lookAt.x, view->lookAt.y, view->lookAt.z,
             view->up.x, view->up.y, view->up.z);

    CLOSE_DISPS(view->gfxCtx, "../z_view.c", 886);

    return 1;
}

s32 func_800AB9EC(View* view, s32 arg1, Gfx** gfxp) {
    Gfx* gfx = *gfxp;
    GraphicsContext* gfxCtx = view->gfxCtx;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;

    arg1 = (view->flags & arg1) | (arg1 >> 4);

    if (arg1 & 2) {
        vp = Graph_Alloc(gfxCtx, sizeof(Vp));
        LogUtils_CheckNullPointer("vp", vp, "../z_view.c", 910);
        View_ViewportToVp(vp, &view->viewport);

        view->vp = *vp;

        gDPPipeSync(gfx++);
        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                      view->viewport.bottomY);
        gSPViewport(gfx++, vp);
    }

    if (arg1 & 8) {
        projection = Graph_Alloc(gfxCtx, sizeof(Mtx));
        LogUtils_CheckNullPointer("projection", projection, "../z_view.c", 921);
        view->projectionPtr = projection;

        guOrtho(projection, -(f32)gScreenWidth * 0.5f, (f32)gScreenWidth * 0.5f, -(f32)gScreenHeight * 0.5f,
                (f32)gScreenHeight * 0.5f, view->zNear, view->zFar, view->scale);

        view->projection = *projection;

        gSPMatrix(gfx++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    } else if (arg1 & 6) {
        projection = Graph_Alloc(gfxCtx, sizeof(Mtx));
        LogUtils_CheckNullPointer("projection", projection, "../z_view.c", 932);
        view->projectionPtr = projection;

        width = view->viewport.rightX - view->viewport.leftX;
        height = view->viewport.bottomY - view->viewport.topY;

        guPerspective(projection, &view->normal, view->fovy, (f32)width / (f32)height, view->zNear, view->zFar,
                      view->scale);

        view->projection = *projection;

        gSPPerspNormalize(gfx++, view->normal);
        gSPMatrix(gfx++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    }

    if (arg1 & 1) {
        viewing = Graph_Alloc(gfxCtx, sizeof(Mtx));
        LogUtils_CheckNullPointer("viewing", viewing, "../z_view.c", 948);
        view->viewingPtr = viewing;

        func_800ABE74(view->eye.x, view->eye.y, view->eye.z);
        guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->lookAt.x, view->lookAt.y, view->lookAt.z,
                 view->up.x, view->up.y, view->up.z);

        view->viewing = *viewing;

        gSPMatrix(gfx++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    }

    view->flags = 0;
    *gfxp = gfx;

    return 1;
}

s32 func_800ABE74(f32 eyeX, f32 eyeY, f32 eyeZ) {
    s32 error = 0;

    if (SQ(eyeX) + SQ(eyeY) + SQ(eyeZ) > SQ(32767.0f)) {
        error = 3;
    } else {
        f32 absEyeX = ABS(eyeX);
        f32 absEyeY = ABS(eyeY);
        f32 absEyeZ = ABS(eyeZ);

        if (((18900.0f < absEyeX) || (18900.0f < absEyeY)) || (18900.0f < absEyeZ)) {
            error = 2;
        } else if (((16000.0f < absEyeX) || (16000.0f < absEyeY)) || (16000.0f < absEyeZ)) {
            error = 1;
        }
    }

    if (error != 0) {
        osSyncPrintf(VT_FGCOL(RED));
        // "Is too large"
        osSyncPrintf("eye が大きすぎます eye=[%8.3f %8.3f %8.3f] error=%d\n", eyeX, eyeY, eyeZ, error);
        osSyncPrintf(VT_RST);
    }

    return error;
}
