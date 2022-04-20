/*
 * File: z_scene_exit.c
 * Overlay: ovl_Scene_Exit
 * Description: Placeholder/marker for exit collision in a scene
 */

#include "z_scene_exit.h"
#include "vt.h"

#define FLAGS 0 //ACTOR_FLAG_0

void SceneExit_Init(Actor* thisx, PlayState* play);
void SceneExit_Destroy(Actor* thisx, PlayState* play);
void SceneExit_Update(Actor* thisx, PlayState* play);
void SceneExit_Draw(Actor* thisx, PlayState* play);

const ActorInit Scene_Exit_InitVars = {
    ACTOR_SCENE_EXIT,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(SceneExit),
    (ActorFunc)SceneExit_Init,
    (ActorFunc)SceneExit_Destroy,
    (ActorFunc)SceneExit_Update,
    (ActorFunc)SceneExit_Draw,
    NULL,
};

void SceneExit_Destroy(Actor* thisx, PlayState* play) {

}

void SceneExit_Init(Actor* thisx, PlayState* play) {
    SceneExit* this = (SceneExit*)thisx;
    this->bbMax.x = this->bbMax.y = this->bbMax.z = -FLT_MAX;
    this->bbMin.x = this->bbMin.y = this->bbMin.z = FLT_MAX;
    this->actor.room = -1; //prevent despawn on room change
}

void SceneExit_Update(Actor* thisx, PlayState* play) {
    SceneExit* this = (SceneExit*)thisx;

    if (false) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y + 50.0f, this->actor.world.pos.z,
                               this->actor.world.rot.x + 0x4000, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}

void SceneExit_Draw(Actor* thisx, PlayState* play) {
    if (play->state.gfxCtx->unk_014 == 0) {
        //only draw when rendering the scene info pass
        return;
    }

    SceneExit* this = (SceneExit*)thisx;

    Vec3f vecA;
    Vec3f vecB;
    Vec3f vecC;

    const f32 height = 100.0f;

    for (int i = 0; i < this->polyCount; i++) {
        for (int v = 0; v < 3; v++) {
            vecA = this->triangles[i].vtx[v];
            vecB = vecA;
            vecB.y += height;
            vecC = this->triangles[i].vtx[(v + 1) % 3];
            Collider_DrawPoly(play->state.gfxCtx, &vecA, &vecB, &vecC, 255, 0, 0);

            vecA = this->triangles[i].vtx[v];
            vecA.y += height;
            vecB = this->triangles[i].vtx[(v + 1) % 3];
            vecB.y += height;
            vecC = this->triangles[i].vtx[(v + 1) % 3];
            Collider_DrawPoly(play->state.gfxCtx, &vecA, &vecB, &vecC, 255, 0, 0);
        }
    }
}

void SceneExit_AddTriangle(SceneExit* this, Vec3f triangleVerts[3]) {
    struct SceneExitTriangle* triangle = &this->triangles[this->polyCount++];

    for (int v = 0; v < 3; v++) {
        Vec3f vtx = triangleVerts[v];
        triangle->vtx[v] = vtx;
        this->bbMax.x = MAX(vtx.x, this->bbMax.x);
        this->bbMax.y = MAX(vtx.y, this->bbMax.y);
        this->bbMax.z = MAX(vtx.z, this->bbMax.z);
        this->bbMin.x = MIN(vtx.x, this->bbMin.x);
        this->bbMin.y = MIN(vtx.y, this->bbMin.y);
        this->bbMin.z = MIN(vtx.z, this->bbMin.z);
    }
    this->center.x = 0.5 * (this->bbMax.x + this->bbMin.x);
    this->center.y = 0.5 * (this->bbMax.y + this->bbMin.y);
    this->center.z = 0.5 * (this->bbMax.z + this->bbMin.z);
    this->actor.home.pos = this->actor.focus.pos = this->actor.world.pos = this->center;
}
