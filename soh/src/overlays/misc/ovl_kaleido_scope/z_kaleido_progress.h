#include "z_kaleido_scope.h"
#include "Hooks.h"
#include "textures/parameter_static/parameter_static.h"

#define MENU_POS_X -100
#define MENU_POS_Y -25
#define BG_KAL_PROG_H 32
#define BG_KAL_PROG_W 64

#define MENU_TOP_POS  MENU_POS_Y + 70   //Most Top of the menu
#define MENU_LEFT_POS MENU_POS_X + 100  //Most Left of the menu

#define MENU_TOP_INNER  MENU_TOP_POS + 5 //Could be useful later on
#define MENU_LEFT_INNER MENU_LEFT_POS + 25

#define MENU_CELLS_SPACE_Y 32       //This is Height a cell will take from top of the label to the top of the next one
#define MENU_CELLS_SPACE_X 100       //This is Width a cell will take from left of the label to the left of the next one

#define TYPE_SMALL_KEYS 0
#define TYPE_MAP 1
#define TYPE_COMPASS 2
#define TYPE_BOSS_KEY 3
#define TYPE_SKULLTULA 4
#define TYPE_TOKEN 5

typedef struct { //This structure help me to draw icon 
    void *tex;
    uint16_t width;
    uint16_t height;
    f32 scale_w;
    f32 scale_h;
    uint8_t im_fmt;
    Color_RGBA8 color;
    bool isgreyscale;
} Kaleido_sprites;

typedef struct {
    Kaleido_sprites* name_sptr; //This structure hold the dungeon informations
    s16 smallkeys_count;
    bool compass_opt;
    bool map_opt;
    bool bosskey_opt;
    bool has_compass;
    bool has_map;
    bool has_bosskey;
    bool has_token;
    Kaleido_sprites* tokens_sptr;
    bool is_masterquest; //Bool added in case later on we support MQ now it does nothing.
    s16 DrawListLine; //This ajust the Y axe
    s16 DrawListCol; ///0 Left /1 Middle /2 Right
    s16 MapIndex;
} Kaleido_dgn_container;