#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Syateki_Itm/z_en_syateki_itm.h"
extern PlayState* gPlayState;
}

#define CVAR_GALLERY_NAME CVAR_ENHANCEMENT("CustomizeShootingGallery")
#define CVAR_GALLERY_VALUE CVarGetInteger(CVAR_GALLERY_NAME, 0)

#define CVAR_INSTANT_WIN_NAME CVAR_ENHANCEMENT("InstantShootingGalleryWin")
#define CVAR_INSTANT_WIN_VALUE CVarGetInteger(CVAR_INSTANT_WIN_NAME, 0)

#define CVAR_CONSTANT_ADULT_NAME CVAR_ENHANCEMENT("ConstantAdultGallery")
#define CVAR_CONSTANT_ADULT_VALUE CVarGetInteger(CVAR_CONSTANT_ADULT_NAME, 0)

static constexpr s32 SHOOTING_GALLERY_AMMO_DEFAULT = 15;

#define CVAR_GALLERY_AMMO_CHILD_NAME CVAR_ENHANCEMENT("ShootingGalleryAmmoChild")
#define CVAR_GALLERY_AMMO_CHILD_VALUE CVarGetInteger(CVAR_GALLERY_AMMO_CHILD_NAME, SHOOTING_GALLERY_AMMO_DEFAULT)

#define CVAR_GALLERY_AMMO_ADULT_NAME CVAR_ENHANCEMENT("ShootingGalleryAmmoAdult")
#define CVAR_GALLERY_AMMO_ADULT_VALUE CVarGetInteger(CVAR_GALLERY_AMMO_ADULT_NAME, SHOOTING_GALLERY_AMMO_DEFAULT)

static void RegisterShootingGalleryInstantWin() {
    COND_VB_SHOULD(VB_PLAY_SHOOTING_GALLERY, CVAR_GALLERY_VALUE && CVAR_INSTANT_WIN_VALUE, {
        EnSyatekiItm* gallery = va_arg(args, EnSyatekiItm*);
        gallery->hitCount = 10;
        gallery->signal = ENSYATEKI_END;
        *should = false;
    });
}

static void RegisterShootingGalleryConstantAdult() {
    COND_VB_SHOULD(VB_SHOOTING_GALLERY_SHUFFLE_ADULT_RUPEES, CVAR_GALLERY_VALUE && CVAR_CONSTANT_ADULT_VALUE,
                   { *should = false; });
}

static void RegisterShootingGalleryAmmoChild() {
    COND_VB_SHOULD(VB_SET_SHOOTING_GALLERY_AMMO,
                   CVAR_GALLERY_VALUE && (CVAR_GALLERY_AMMO_CHILD_VALUE != SHOOTING_GALLERY_AMMO_DEFAULT), {
                       if (LINK_IS_CHILD) {
                           s32* ammo = va_arg(args, s32*);
                           *ammo = CVAR_GALLERY_AMMO_CHILD_VALUE;
                       }
                   });
}

static void RegisterShootingGalleryAmmoAdult() {
    COND_VB_SHOULD(VB_SET_SHOOTING_GALLERY_AMMO,
                   CVAR_GALLERY_VALUE && (CVAR_GALLERY_AMMO_ADULT_VALUE != SHOOTING_GALLERY_AMMO_DEFAULT), {
                       if (LINK_IS_ADULT) {
                           s32* ammo = va_arg(args, s32*);
                           *ammo = CVAR_GALLERY_AMMO_ADULT_VALUE;
                       }
                   });
}

static RegisterShipInitFunc initFunc_InstantWin(RegisterShootingGalleryInstantWin,
                                                { CVAR_GALLERY_NAME, CVAR_INSTANT_WIN_NAME });
static RegisterShipInitFunc initFunc_Constant(RegisterShootingGalleryConstantAdult,
                                              { CVAR_GALLERY_NAME, CVAR_CONSTANT_ADULT_NAME });
static RegisterShipInitFunc initFunc_AmmoChild(RegisterShootingGalleryAmmoChild,
                                               { CVAR_GALLERY_NAME, CVAR_GALLERY_AMMO_CHILD_NAME });
static RegisterShipInitFunc initFunc_AmmoAdult(RegisterShootingGalleryAmmoAdult,
                                               { CVAR_GALLERY_NAME, CVAR_GALLERY_AMMO_ADULT_NAME });
