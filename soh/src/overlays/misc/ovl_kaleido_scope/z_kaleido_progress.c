#include "z_kaleido_scope.h"
#include "z_kaleido_progress.h"
#include "SohHooks.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/icon_item_static/icon_item_static.h"

#include "textures/title_static/title_static.h"
#include "textures/place_title_cards/g_pn_01.h" // Forest Temple Title card
#include "textures/place_title_cards/g_pn_02.h" // Shadow Temple Title card
#include "textures/place_title_cards/g_pn_03.h" // Fire Temple Title card
#include "textures/place_title_cards/g_pn_04.h" // Water Temple Title card
#include "textures/place_title_cards/g_pn_05.h" // Spirit Temple Title card
#include "textures/place_title_cards/g_pn_06.h" // Deku Tree Title card
#include "textures/place_title_cards/g_pn_07.h" // Jabu Jabu Title card
#include "textures/place_title_cards/g_pn_08.h" // Dodongo's Cavern Title card
#include "textures/place_title_cards/g_pn_10.h" // Ice Cavern Title card
#include "textures/place_title_cards/g_pn_11.h" // Gerudo Training Ground Title card
#include "textures/place_title_cards/g_pn_16.h" // Temple of time Title card
#include "textures/place_title_cards/g_pn_43.h" // Ganon Castle Title card
#include "textures/place_title_cards/g_pn_54.h" // Bottom of the well Title card
/**
 * Textures
 */
//Background menu Top
Kaleido_sprites Background_Win1_Kspr = { gFileSelWindow1Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win2_Kspr = { gFileSelWindow2Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win3_Kspr = { gFileSelWindow3Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win4_Kspr = { gFileSelWindow4Tex, 48, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };

//Background middle top
Kaleido_sprites Background_Win5_Kspr = { gFileSelWindow5Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win6_Kspr = { gFileSelWindow6Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win7_Kspr = { gFileSelWindow7Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win8_Kspr = { gFileSelWindow8Tex, 48, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };

//Background middle middle
Kaleido_sprites Background_Win9_Kspr = { gFileSelWindow9Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win10_Kspr = { gFileSelWindow10Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win11_Kspr = { gFileSelWindow11Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win12_Kspr = { gFileSelWindow12Tex, 48, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };

//Background middle bottom
Kaleido_sprites Background_Win13_Kspr = { gFileSelWindow13Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win14_Kspr = { gFileSelWindow14Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win15_Kspr = { gFileSelWindow15Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win16_Kspr = { gFileSelWindow16Tex, 48, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };

//Background bottom
Kaleido_sprites Background_Win17_Kspr = { gFileSelWindow17Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win18_Kspr = { gFileSelWindow18Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win19_Kspr = { gFileSelWindow19Tex, 64, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };
Kaleido_sprites Background_Win20_Kspr = { gFileSelWindow20Tex, 48, 32, 1.35f, 1.35f, G_IM_FMT_IA, { 100,150,255,150 }, false };

//Dungeon stuffs like bosses keys, maps compass etc.
Kaleido_sprites BossKey_Kspr = { gBossKeyIconTex, 24, 24, 0.5f, 0.5f, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites Compass_Kspr = { gCompassIconTex, 24, 24, 0.5f, 0.5f, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites Map_Kspr = { gDungeonMapIconTex, 24, 24, 0.5f, 0.5f, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites Small_Kspr = { gSmallKeyIconTex, 24, 24, 0.5f, 0.5f, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites GoldSkulltula_Kspr = { gGoldSkulltulaIconTex, 24, 24, 0.3f, 0.3f, G_IM_FMT_RGBA, { 255,255,255,255 }, false };

//Dungeon name plate
Kaleido_sprites DekuTitle_Kspr = { gDekuTreeTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 110,205,100,255 }, false };
Kaleido_sprites DodongoTitle_Kspr = { gDodongosCavernTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 210,90,90,255 }, false };
Kaleido_sprites JabuTitle_Kspr = { gJabuJabuTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 120,125,255,255 }, false };
Kaleido_sprites ForestTitle_Kspr = { gForestTempleTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 100,185,105,255 }, false };
Kaleido_sprites FireTitle_Kspr = { gFireTempleTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 185,100,100,255 }, false };
Kaleido_sprites WaterTitle_Kspr = { gWaterTempleTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 90,120,195,255 }, false };
Kaleido_sprites SpiritTitle_Kspr = { gSpiritTempleTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 205,160,90,255 }, false };
Kaleido_sprites ShadowTitle_Kspr = { gShadowTempleTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 170,90,210,255 }, false };
Kaleido_sprites ToTTitle_Kspr = { gTempleOfTimeTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 210,210,90,255 }, false };
Kaleido_sprites BotwTitle_Kspr = { gBottomOfTheWellTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 160,130,215,255 }, false };
Kaleido_sprites IceCavernTitle_Kspr = { gIceCavernTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 130,210,215,255 }, false };
Kaleido_sprites GymnaseTitle_Kspr = { gGERudoTrainingGroundTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 240,195,115,255 }, false };
Kaleido_sprites GanonCastleTitle_Kspr = { gGanonsCastleTitleCardENGTex, 144, 24, 0.45f, 0.45f, G_IM_FMT_IA, { 255,255,255,255 }, false };

//For translation purpose I do an array with all the title card and fetch them directly there
const void* TitleCard[] = {
    gDekuTreeTitleCardENGTex,        gDodongosCavernTitleCardENGTex,   gJabuJabuTitleCardENGTex,
    gForestTempleTitleCardENGTex,    gFireTempleTitleCardENGTex,       gWaterTempleTitleCardENGTex,
    gSpiritTempleTitleCardENGTex,    gShadowTempleTitleCardENGTex,     gTempleOfTimeTitleCardENGTex,
    gGanonsCastleTitleCardENGTex,    gBottomOfTheWellTitleCardENGTex,  gIceCavernTitleCardENGTex, 
    gGERudoTrainingGroundTitleCardENGTex,
    gDekuTreeTitleCardGERTex,        gDodongosCavernTitleCardGERTex,   gJabuJabuTitleCardGERTex,
    gForestTempleTitleCardGERTex,    gFireTempleTitleCardGERTex,       gWaterTempleTitleCardGERTex,
    gSpiritTempleTitleCardGERTex,    gShadowTempleTitleCardGERTex,     gTempleOfTimeTitleCardGERTex,
    gGanonsCastleTitleCardGERTex,    gBottomOfTheWellTitleCardGERTex,  gIceCavernTitleCardGERTex,   
    gGERudoTrainingGroundTitleCardGERTex,
    gDekuTreeTitleCardFRATex,        gDodongosCavernTitleCardFRATex,   gJabuJabuTitleCardFRATex,
    gForestTempleTitleCardFRATex,    gFireTempleTitleCardFRATex,       gWaterTempleTitleCardFRATex,
    gSpiritTempleTitleCardFRATex,    gShadowTempleTitleCardFRATex,     gTempleOfTimeTitleCardFRATex,  
    gGanonsCastleTitleCardFRATex,    gBottomOfTheWellTitleCardFRATex,  gIceCavernTitleCardFRATex,
    gGERudoTrainingGroundTitleCardFRATex,
};

//Dungeon tokens
Kaleido_sprites ForestT_Kspr = { gForestMedallionIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites FireT_Kspr = { gFireMedallionIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites WaterT_Kspr = { gWaterMedallionIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites SpritT_Kspr = { gSpiritMedallionIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites ShadowT_Kspr = { gShadowMedallionIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites LightT_Kspr = { gLightMedallionIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites KokiriT_Kspr = { gKokiriEmeraldIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites GoronT_Kspr = { gGoronRubyIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites ZoraT_Kspr = { gZoraSapphireIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
Kaleido_sprites GymT_Kspr = { gGerudosCardIconTex, 24, 24, 0.5f, 0.5f, 0, 0, G_IM_FMT_RGBA, { 255,255,255,255 }, false };
                                        //Right now the structure has a count for the maximumm small key possible but do not use it maybe 
                                        //I could make a system to keep track of how many keys the players had and have and 
                                        //once he/she reach the max amount it change the digit to green?
Kaleido_dgn_container DungeonsSettings[] = {{ &DekuTitle_Kspr, 0, false, false, false, true, true, false, true, &KokiriT_Kspr,false,0,0,0 },
                                    { &DodongoTitle_Kspr, 0, false, false, false, true, true, false, true, &GoronT_Kspr,false,0,1,1 },
                                    { &JabuTitle_Kspr, 0, false, false, false, true, true, false, true, &ZoraT_Kspr,false,0,2,2 },
                                    { &ForestTitle_Kspr, 5, false, false, false, true, true, true, true, &ForestT_Kspr,false,1,0,3}, //if master quest keys = 6 
                                    { &FireTitle_Kspr, 8, false, false, false, true, true, true, true, &FireT_Kspr,false,1,1,4},   //if master quest keys = 5
                                    { &WaterTitle_Kspr, 6, false, false, false, true, true, true, true, &WaterT_Kspr,false,2,0,5},  //if master quest keys = 2 ?
                                    { &SpiritTitle_Kspr, 5, false, false, false, true, true, true, true, &SpritT_Kspr,false,2,1,6}, //if master quest keys = 6
                                    { &ShadowTitle_Kspr, 5, false, false, false, true, true, true, true, &ShadowT_Kspr,false,3,0,7}, //if master quest keys = 7
                                    { &ToTTitle_Kspr, 0, false, false, false, false, false, false, true, &LightT_Kspr,false,3,1,0},
                                    { &GanonCastleTitle_Kspr, 2, false, false, false, false, false, true, false, &GymT_Kspr,false,4,1,13}, //if master quest keys = 3
                                    { &BotwTitle_Kspr, 2, false, false, false, true, true, false, false, &GymT_Kspr,false,1,2,8},
                                    { &IceCavernTitle_Kspr, 0, false, false, false, true, true, false, false, &GymT_Kspr,false,2,2,9 },
                                    { &GymnaseTitle_Kspr, 9, false, false, false, true, true, false, true, &GymT_Kspr,false,3,2,11 } };


/**
 * Sound effects
 */
const s32 Close_SFX = NA_SE_SY_DECIDE;

/**
 * Settings
 */
s16 FrameSinceOpen = 0; //Count the frame since this menu is open. (used to make a little latence before being able to close it.)
const u8 MapIndex_GSFlags[] = { //Array used to detect when ever a player has all gs token from a dungeon.
    0x0F, 0x1F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x07, 0x03,
    0x0F, 0x07, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0x1F, 0x0F, 0x03, 0x0F,
};
const s16 X_Offset_GS[3][13] = {//same order than TitleCard[], this i sthe X position of GS icon next to dungeon label. 
    {-6,-6,-6,
     -4, 2,-2,
     -1,-5,-3,
     -2,-6, 4,
     -8},//ENG.

    {-3,-5,-5,
      1, 2,-2,
     -2,-4,-5,
     -3,-6, 8,
     -2},//GER.

    {1,-6,-6,
    -5,-2,-4,
    -5,-4,-5,
    -5,13,-6,
    -6},//FRA.
};

void MenuBackground_Draw(GlobalContext* globalCtx, Kaleido_sprites* sprite, s16 line, s16 col) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 alpha = sprite->color.a;

    s16 Size_W = BG_KAL_PROG_W; //Used to ajust script to the last image.
    s16 Size_H = BG_KAL_PROG_H;
    if(col == 3) {
        Size_W = BG_KAL_PROG_W_L;
    }

    int height = Size_H * sprite->scale_h;
    int width = Size_W * sprite->scale_w;

    int width_factor = (1 << 10) * Size_W / width;
    int height_factor = (1 << 10) * Size_H / height;

    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);

    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a);

    s16 AjustXPos = width + (width * col);
    s16 AjustYPos = height + (height * line);
    if(col == 3) {
        AjustXPos = (BG_KAL_PROG_W*sprite->scale_w)+(BG_KAL_PROG_W * col) + (((BG_KAL_PROG_W-BG_KAL_PROG_W_L) * sprite->scale_w)*col+1);
    }

    s16 PosX = MENU_POS_X + AjustXPos;
    s16 PosY = MENU_POS_Y + AjustYPos;

    gDPLoadTextureBlock(POLY_KAL_DISP++, sprite->tex, sprite->im_fmt, G_IM_SIZ_16b, Size_W, BG_KAL_PROG_H, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPWideTextureRectangle(POLY_KAL_DISP++,PosX << 2, PosY << 2, (PosX + width) << 2, (PosY + height) << 2, G_TX_RENDERTILE, 0, 0, width_factor, height_factor);
    
    gsSPGrayscale(POLY_KAL_DISP++, false);
    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}

const char* Kaleido_ProgressDigits_tex[] = { //Array holding digit texture for small keys counter
    gCounterDigit0Tex, gCounterDigit1Tex, gCounterDigit2Tex, gCounterDigit3Tex,
    gCounterDigit4Tex, gCounterDigit5Tex, gCounterDigit6Tex, gCounterDigit7Tex, gCounterDigit8Tex,
    gCounterDigit9Tex, gCounterColonTex
};

void Kaleido_DigitsDraw(GlobalContext* globalCtx, s16 number, f32 scale, s16 Line, s16 Col){
    s16 PosX = MENU_LEFT_INNER + (MENU_CELLS_SPACE_X * Col)+7; //General Pos
    s16 PosY = MENU_TOP_INNER + (MENU_CELLS_SPACE_Y * Line)+13; //General Pos
    int height = 16 * scale; //Adjust Height with scale
    int width = 16 * scale; //Adjust Width with scale
    int height_factor = (1 << 10) * 16 / height;
    int width_factor = (1 << 10) * 16 / width;

    //We open our Disp that will allow graphic draw
    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    //We set color to match the one we set in the Sprite.
    if (number > 0) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);
    } else {
        //No key draw it a bit darker.
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 150, 150, 150, 255);
    }

    //Loading texture with proper settings (settings like size if it clamp mirror etc.)
    gDPLoadTextureBlock(POLY_KAL_DISP++, Kaleido_ProgressDigits_tex[number], G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    //Making a "room" for our loaded graphic, it's like making a size where our graphic will draw, it has a top left corner at one postion and bottom right corner at another and make a rectangle from it, and we draw in that rectangle
    gSPTextureRectangle(POLY_KAL_DISP++, PosX << 2, PosY << 2, (PosX + width) << 2, (PosY + height) << 2, G_TX_RENDERTILE, 0, 0, width_factor, height_factor);

    //We close the DISP has there no more GFX to be draw there.
    gDPPipeSync(POLY_KAL_DISP++);
    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}

void KaleidoScope_BackgroundDraw(GlobalContext* globalCtx){
    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
    //We want settings for proper color and textures display, this allow colour etc. , in short if I get it right we define our shader here.
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetRenderMode(POLY_KAL_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    //Top line
    MenuBackground_Draw(globalCtx,&Background_Win1_Kspr,0,0);
    MenuBackground_Draw(globalCtx,&Background_Win2_Kspr,0,1);
    MenuBackground_Draw(globalCtx,&Background_Win3_Kspr,0,2);
    MenuBackground_Draw(globalCtx,&Background_Win4_Kspr,0,3);

    //Middle top line
    MenuBackground_Draw(globalCtx,&Background_Win5_Kspr,1,0);
    MenuBackground_Draw(globalCtx,&Background_Win6_Kspr,1,1);
    MenuBackground_Draw(globalCtx,&Background_Win7_Kspr,1,2);
    MenuBackground_Draw(globalCtx,&Background_Win8_Kspr,1,3);

    //Middle ... middle line
    MenuBackground_Draw(globalCtx,&Background_Win9_Kspr,2,0);
    MenuBackground_Draw(globalCtx,&Background_Win10_Kspr,2,1);
    MenuBackground_Draw(globalCtx,&Background_Win11_Kspr,2,2);
    MenuBackground_Draw(globalCtx,&Background_Win12_Kspr,2,3);

    //Middle bottom line
    MenuBackground_Draw(globalCtx,&Background_Win13_Kspr,3,0);
    MenuBackground_Draw(globalCtx,&Background_Win14_Kspr,3,1);
    MenuBackground_Draw(globalCtx,&Background_Win15_Kspr,3,2);
    MenuBackground_Draw(globalCtx,&Background_Win16_Kspr,3,3);

    //Bottom line
    MenuBackground_Draw(globalCtx,&Background_Win17_Kspr,4,0);
    MenuBackground_Draw(globalCtx,&Background_Win18_Kspr,4,1);
    MenuBackground_Draw(globalCtx,&Background_Win19_Kspr,4,2);
    MenuBackground_Draw(globalCtx,&Background_Win20_Kspr,4,3);

    gDPPipeSync(POLY_KAL_DISP++);
    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}

void KaleidoScope_DungeonsLabelsDraw(GlobalContext* globalCtx, Kaleido_sprites* sprite, s16 Line, s16 Col){
    s16 PosX = MENU_LEFT_INNER + (MENU_CELLS_SPACE_X * Col); //General Pos
    s16 PosY = MENU_TOP_INNER + (MENU_CELLS_SPACE_Y * Line); //General Pos
    int height = sprite->height * sprite->scale_h; //Adjust Height with scale
    int width = sprite->width * sprite->scale_w; //Adjust Width with scale
    int height_factor = (1 << 10) * sprite->height / height;
    int width_factor = (1 << 10) * sprite->width / width;

    //We open our Disp that will allow graphic draw
    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetRenderMode(POLY_KAL_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    //We set color to match the one we set in the Sprite.
    if (CVar_GetS32("gKaleidoProgressColored", 0) == 1) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, sprite->color.r, sprite->color.g, sprite->color.b, sprite->color.a);
    } else {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, sprite->color.a);
    }

    gDPLoadTextureBlock(POLY_KAL_DISP++, sprite->tex, sprite->im_fmt, G_IM_SIZ_8b, sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    //Making a "room" for our loaded graphic, it's like making a size where our graphic will draw, it has a top left corner at one postion and bottom right corner at another and make a rectangle from it, and we draw in that rectangle
    gSPTextureRectangle(POLY_KAL_DISP++, PosX << 2, PosY << 2, (PosX + width) << 2, (PosY + height) << 2, G_TX_RENDERTILE, 0, 0, width_factor, height_factor);

    //We close the DISP has there no more GFX to be draw there.
    gDPPipeSync(POLY_KAL_DISP++);
    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}

void KaleidoScope_DungeonsIcons24Draw(GlobalContext* globalCtx, Kaleido_sprites* sprite, s16 Line, s16 Col, s16 Icon_Type, bool greyscale, s16 ID){
    s16 X_Offset = 0;
    s16 Y_Offset = 12;
    if (Icon_Type == TYPE_SKULLTULA) {
        X_Offset = X_Offset_GS[gSaveContext.language][ID];
        Y_Offset = 1;
    }
    if (Icon_Type == TYPE_SMALL_KEYS) {
        X_Offset = -4;
    }
    if (Icon_Type == TYPE_MAP) {
        X_Offset = 15;
    }
    if (Icon_Type == TYPE_COMPASS) {
        X_Offset = 30;
    }
    if (Icon_Type == TYPE_BOSS_KEY) {
        X_Offset = 45;
    }
    if (Icon_Type == TYPE_TOKEN) {
        X_Offset = 60;
    }
    s16 PosX = MENU_LEFT_INNER + (MENU_CELLS_SPACE_X * Col)+X_Offset; //General Pos
    s16 PosY = MENU_TOP_INNER + (MENU_CELLS_SPACE_Y * Line)+Y_Offset; //General Pos
    int height = sprite->height * sprite->scale_h; //Adjust Height with scale
    int width = sprite->width * sprite->scale_w; //Adjust Width with scale
    int height_factor = (1 << 10) * sprite->height / height;
    int width_factor = (1 << 10) * sprite->width / width;

    //We open our Disp that will allow graphic draw
    OPEN_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    //We set color to match the one we set in the Sprite.
    if(greyscale) {
        gsDPSetGrayscaleColor(POLY_KAL_DISP++, 109, 109, 109, 220);
        gsSPGrayscale(POLY_KAL_DISP++, true);
    } else {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, 255);
        gsSPGrayscale(POLY_KAL_DISP++, false);
    }

    //Loading texture with proper settings (settings like size if it clamp mirror etc.)
    gDPLoadTextureBlock(POLY_KAL_DISP++, sprite->tex, G_IM_FMT_RGBA, G_IM_SIZ_32b, sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    //Making a "room" for our loaded graphic, it's like making a size where our graphic will draw, it has a top left corner at one postion and bottom right corner at another and make a rectangle from it, and we draw in that rectangle
    gSPTextureRectangle(POLY_KAL_DISP++, PosX << 2, PosY << 2, (PosX + width) << 2, (PosY + height) << 2, G_TX_RENDERTILE, 0, 0, width_factor, height_factor);

    //We close the DISP has there no more GFX to be draw there.
    gsSPGrayscale(POLY_KAL_DISP++, false);
    gDPPipeSync(POLY_KAL_DISP++);
    CLOSE_DISPS(globalCtx->state.gfxCtx, __FILE__, __LINE__);
}

void KeleidoScope_DungeonContainerDraw(GlobalContext* globalCtx) {

    for (s16 i =0; i < ARRAY_COUNT(DungeonsSettings); i++){
        //Dungeon name draw
        DungeonsSettings[i].name_sptr->tex = TitleCard[i+(13*gSaveContext.language)]; //Translation fetch the tex from the array.
        KaleidoScope_DungeonsLabelsDraw(globalCtx, DungeonsSettings[i].name_sptr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol);
        
        bool not_all_gs_found = true;
        if (GET_GS_FLAGS(DungeonsSettings[i].MapIndex) == MapIndex_GSFlags[DungeonsSettings[i].MapIndex]) {
            //if all skulltula has been found make it appear.
            not_all_gs_found = false;
        }
        
        KaleidoScope_DungeonsIcons24Draw(globalCtx, &GoldSkulltula_Kspr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol, TYPE_SKULLTULA, not_all_gs_found, i);

        if (DungeonsSettings[i].smallkeys_count > 0) {
            //If dungeon has at least one key draw
            KaleidoScope_DungeonsIcons24Draw(globalCtx, &Small_Kspr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol, TYPE_SMALL_KEYS, false, i);
            //We set the color for our Digits here
            s16 KeyCount = gSaveContext.inventory.dungeonKeys[DungeonsSettings[i].MapIndex];
            if (KeyCount < 0) {
                KeyCount = 0; //Prevent -1 crash but the function do not support 10 or more key be careful.
            }
            Kaleido_DigitsDraw(globalCtx,KeyCount,0.8f,DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol);
        }
        if (DungeonsSettings[i].has_map) {
            //If dungeon has a map let draw it here.
            bool is_greyscale = true;
            if (CHECK_DUNGEON_ITEM(2, DungeonsSettings[i].MapIndex)){
                is_greyscale = false;
            }
            KaleidoScope_DungeonsIcons24Draw(globalCtx, &Map_Kspr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol, TYPE_MAP, is_greyscale, i);
        }
        if (DungeonsSettings[i].has_compass) {
            //If dungeon has a compass let draw it here.
            bool is_greyscale = true;
            if (CHECK_DUNGEON_ITEM(1, DungeonsSettings[i].MapIndex)){
                is_greyscale = false;
            }
            KaleidoScope_DungeonsIcons24Draw(globalCtx, &Compass_Kspr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol, TYPE_COMPASS, is_greyscale, i);
        }
        if (DungeonsSettings[i].has_bosskey) {
            //If dungeon has a boss key let draw it here.
            bool is_greyscale = true;
            if (CHECK_DUNGEON_ITEM(0, DungeonsSettings[i].MapIndex)){
                is_greyscale = false;
            }
            KaleidoScope_DungeonsIcons24Draw(globalCtx, &BossKey_Kspr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol, TYPE_BOSS_KEY, is_greyscale, i);
        }
        if (DungeonsSettings[i].has_token) {
            //If dungeon has a token let draw it here.
            bool is_greyscale = true;
            if (i <= 2) {
                if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD + i)) {
                    is_greyscale = false;
                }
            } else if (i > 2) {
                if (CHECK_QUEST_ITEM(i - 3)){
                    is_greyscale = false;
                }
            }
            if (DungeonsSettings[i].MapIndex == 11) {
                if (CHECK_QUEST_ITEM(QUEST_GERUDO_CARD)){
                    is_greyscale = false;
                }
            }
            KaleidoScope_DungeonsIcons24Draw(globalCtx, DungeonsSettings[i].tokens_sptr, DungeonsSettings[i].DrawListLine, DungeonsSettings[i].DrawListCol, TYPE_TOKEN, is_greyscale, i);
        }
    }
}

void KaleidoScope_ProgressClose(PauseContext* pauseCtx){
    //Since I use a debugstate to enter here we reset it to bring the player where he/she was on CUP button
    //Start will remove this state and the one on background menu so that fine.
    //And B will bring save menu, both them will enter here and remove the debug state and reset FrameSinceOpen.

    pauseCtx->debugState = 0;
    FrameSinceOpen = 0;
    Audio_PlaySoundGeneral(Close_SFX, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    return;
}

void KaleidoScope_ProgressPage(GlobalContext* globalCtx){
    PauseContext* pauseCtx = &globalCtx->pauseCtx;
    Input* input = &globalCtx->state.input[0];
    Gfx* gfx;
    Gfx* gfxRef;
    FrameSinceOpen++;

    //Draw all that is required to have a background
    KaleidoScope_BackgroundDraw(globalCtx);

    //Now we draw Dungeon Cells
    KeleidoScope_DungeonContainerDraw(globalCtx);

    if (FrameSinceOpen >= 10) { //Wait 10 frames before being able to close the menu
        if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_B)) { 
            //Un pause game and close menu
            KaleidoScope_ProgressClose(pauseCtx);
        }
    }
}