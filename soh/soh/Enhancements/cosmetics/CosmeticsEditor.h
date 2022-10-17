#pragma once
#include <libultraship/ImGuiImpl.h>
#define SECTION_SIZE(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define RANDOMIZE_32(Max) GetRandomValue(Max);
#define CATEGORY_NPC 0
#define CATEGORY_ITEMS 1
#define CATEGORY_MENU 2
#define CATEGORY_HUD 3
#define CATEGORY_MISC 4

#define PATCH_GFX(path, name, cvar, index, instruction) \
    if (CVar_GetS32(cvar, 0)) { \
        ResourceMgr_PatchGfxByName(path, name, index, instruction); \
    } else { \
        ResourceMgr_UnpatchGfxByName(path, name); \
    }

typedef struct {
    const std::string Name;
    const std::string ToolTip;
    const std::string CvarName;
    ImVec4 ModifiedColor;
    ImVec4 DefaultColor;
    bool canRainbow;
    bool hasAlpha;
    bool sameLine;
} CosmeticsColorIndividual;

typedef struct {
    CosmeticsColorIndividual* Element;
    bool Nextcol;
    bool NextLine;
} CosmeticsColorSection;

static float TablesCellsWidth = 300.0f;
static ImGuiTableColumnFlags FlagsTable = ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV;
static ImGuiTableColumnFlags FlagsCell = ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort;
static ImVec4 hearts_colors;          static ImVec4 hearts_dd_colors;        static ImVec4 hearts_ddi_colors;
static ImVec4 a_btn_colors;           static ImVec4 b_btn_colors;            static ImVec4 c_btn_colors;        static ImVec4 start_btn_colors;
static ImVec4 c_btn_u_colors;         static ImVec4 c_btn_l_colors;          static ImVec4 c_btn_d_colors;      static ImVec4 c_btn_r_colors;
static ImVec4 magic_border_colors;    static ImVec4 magic_remaining_colors;  static ImVec4 magic_use_colors;
static ImVec4 minimap_colors;         static ImVec4 dgn_minimap_colors;
static ImVec4 cp_minimap_colors;      static ImVec4 le_minimap_colors;
static ImVec4 rupee_colors;           static ImVec4 smolekey_colors;         static ImVec4 magic_bordern_colors;
static ImVec4 fileselect_colors;      static ImVec4 fileselect_text_colors;
static ImVec4 kokiri_col;             static ImVec4 goron_col;               static ImVec4 zora_col;
static ImVec4 silvergaunts_col;       static ImVec4 goldengaunts_col;
static ImVec4 mirrorshield_border_col;static ImVec4 mirrorshield_mirror_col; static ImVec4 mirrorshield_emblem_col;
static ImVec4 navi_idle_i_col;        static ImVec4 navi_idle_o_col;
static ImVec4 navi_npc_i_col;         static ImVec4 navi_npc_o_col;
static ImVec4 navi_enemy_i_col;       static ImVec4 navi_enemy_o_col;
static ImVec4 navi_prop_i_col;        static ImVec4 navi_prop_o_col;
static ImVec4 swordtrailtop_col;      static ImVec4 swordtrailbottom_col;
static ImVec4 boomtrailstart_col;     static ImVec4 boomtrailend_col;
static ImVec4 bombtrail_col;           
static ImVec4 crtfilter;
static ImVec4 normalarrow_col;        static ImVec4 firearrow_col;           static ImVec4 icearrow_col;            static ImVec4 lightarrow_col;
static ImVec4 normalarrow_colenv;     static ImVec4 firearrow_colenv;        static ImVec4 icearrow_colenv;         static ImVec4 lightarrow_colenv;
static ImVec4 charged1_col;           static ImVec4 charged2_col;
static ImVec4 charged1_colenv;        static ImVec4 charged2_colenv;
static ImVec4 Keese1_primcol;         static ImVec4 Keese2_primcol;
static ImVec4 Keese1_envcol;          static ImVec4 Keese2_envcol;
static ImVec4 doggo1col;              static ImVec4 doggo2col;
static ImVec4 df_col;                 static ImVec4 df_colenv;
static ImVec4 nl_diam_col;            static ImVec4 nl_diam_colenv;
static ImVec4 nl_orb_col;             static ImVec4 nl_orb_colenv;
static ImVec4 tc_ou_colors;           static ImVec4 tc_bu_colors;
static ImVec4 dpad_colors;
static ImVec4 visualagony_colors;
static ImVec4 tc_fire_colors;         static ImVec4 tc_fire_colors_env; 
/*ImVec4 menu_equips_colors;
ImVec4 menu_items_colors;
ImVec4 menu_map_colors;
ImVec4 menu_quest_colors;
ImVec4 menu_save_colors;
ImVec4 menu_gameover_colors;*/

//Navi
static CosmeticsColorIndividual Navi_Idle_Inner = { "Navi Idle (Primary)", "Inner color for Navi (idle flying around)", "gNavi_Idle_Inner", navi_idle_i_col, ImVec4(255, 255, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Idle_Outer = { "Navi Idle (Secondary)", "Outer color for Navi (idle flying around)", "gNavi_Idle_Outer", navi_idle_o_col, ImVec4(0, 0, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Npc_Inner = { "Navi NPC (Primary)", "Inner color for Navi (when Navi fly around NPCs)", "gNavi_NPC_Inner", navi_npc_i_col, ImVec4(150, 150, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Npc_Outer = { "Navi NPC (Secondary)", "Outer color for Navi (when Navi fly around NPCs)", "gNavi_NPC_Outer", navi_npc_o_col, ImVec4(150, 150, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Enemy_Inner = { "Navi Enemy (Primary)", "Inner color for Navi (when Navi fly around Enemies or Bosses)", "gNavi_Enemy_Inner", navi_enemy_i_col, ImVec4(255, 255, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Enemy_Outer = { "Navi Enemy (Secondary)", "Outer color for Navi (when Navi fly around Enemies or Bosses)", "gNavi_Enemy_Outer", navi_enemy_o_col, ImVec4(220, 155, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Prop_Inner = { "Navi Props (Primary)", "Inner color for Navi (when Navi fly around props (signs etc))", "gNavi_Prop_Inner", navi_prop_i_col, ImVec4(0, 255, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Prop_Outer = { "Navi Props (Secondary)", "Outer color for Navi (when Navi fly around props (signs etc))", "gNavi_Prop_Outer", navi_prop_o_col, ImVec4(0, 255, 0, 255), false, false, false };

//Keese
static CosmeticsColorIndividual Keese1_prim = { "Fire Primary color", "Affects the primary color of the Fire itself of the Keese", "gKeese1_Ef_Prim", Keese1_primcol, ImVec4(255, 255, 100, 255), true, false, false };
static CosmeticsColorIndividual Keese2_prim = { "Ice Primary color", "Affects the primary color of the Ice itself of the Keese", "gKeese2_Ef_Prim", Keese2_primcol, ImVec4(100, 200, 255, 255), true, false, false };
static CosmeticsColorIndividual Keese1_env = { "Fire Secondary color", "Affects the secondary color of the Fire itself of the Keese", "gKeese1_Ef_Env", Keese1_envcol, ImVec4(255, 50, 0, 255), true, false, false };
static CosmeticsColorIndividual Keese2_env = { "Ice Secondary color", "Affects the secondary color of the Ice itself of the Keese", "gKeese2_Ef_Env", Keese2_envcol, ImVec4(0, 0, 255, 255), true, false, false };

//Good old Dogs, we all love them
static CosmeticsColorIndividual DogN1 = { "Dog white", "Affects the colors of the white dog", "gDog1Col", doggo1col, ImVec4(255,255,200,255), true, false, true };
static CosmeticsColorIndividual DogN2 = { "Dog brown", "Affects the colors of the brown dog", "gDog2Col", doggo2col, ImVec4(150,100,50,255), true, false, true };

//Tunic colors
static CosmeticsColorIndividual KokiriTunic = { "Kokiri Tunic", "Affects Kokiri Tunic color", "gTunic_Kokiri", kokiri_col, ImVec4(30, 105, 27, 255), true, false, true };
static CosmeticsColorIndividual GoronTunic = { "Goron Tunic", "Affects Goron Tunic color", "gTunic_Goron", goron_col, ImVec4(100, 20, 0, 255), true, false, true };
static CosmeticsColorIndividual ZoraTunic = { "Zora Tunic", "Affects Zora Tunic color", "gTunic_Zora", zora_col, ImVec4(0, 60, 100, 255), true, false, true };

//Gauntlet colors
static CosmeticsColorIndividual SilverGauntlets = { "Silver Gauntlets", "Affects Silver Gauntlets color", "gGauntlets_Silver", silvergaunts_col, ImVec4(255, 255, 255, 255), true, false, true };
static CosmeticsColorIndividual GoldenGauntlets = { "Golden Gauntlets", "Affects Golden Gauntlets color", "gGauntlets_Golden", goldengaunts_col, ImVec4(254, 207, 15, 255), true, false, true };

//Mirror Shield colors
static CosmeticsColorIndividual MirrorShieldBorder = { "Mirror Shield Border", "Affects Mirror Shield Border color", "gMirrorShield_Border", mirrorshield_border_col, ImVec4(215,   0,   0, 255), false, false, true };
static CosmeticsColorIndividual MirrorShieldMirror = { "Mirror Shield Mirror", "Affects Mirror Shield Mirror color", "gMirrorShield_Mirror", mirrorshield_mirror_col, ImVec4(255, 255, 255, 255), false, false, true };
static CosmeticsColorIndividual MirrorShieldEmblem = { "Mirror Shield Emblem", "Affects Mirror Shield Emblem color", "gMirrorShield_Emblem", mirrorshield_emblem_col, ImVec4(205, 225, 255, 255), false, false, true };

//Arrows (Fire -> Ice -> Light)
static CosmeticsColorIndividual Normal_Arrow_Prim = { "Normal Arrows (primary)", "Affects Primary color", "gNormalArrowCol", normalarrow_col, ImVec4(255, 255, 170, 255), true, false, false };
static CosmeticsColorIndividual Normal_Arrow_Env = { "Normal Arrows (Secondary)", "Affects Secondary color", "gNormalArrowColEnv", normalarrow_colenv, ImVec4(0, 150, 0, 0), true, false, false };
static CosmeticsColorIndividual Fire_Arrow_Prim = { "Fire Arrows (primary)", "Affects Primary color", "gFireArrowCol", firearrow_col, ImVec4(255,200,0,255), true, false, false };
static CosmeticsColorIndividual Fire_Arrow_Env = { "Fire Arrows (Secondary)", "Affects Secondary color", "gFireArrowColEnv", firearrow_colenv, ImVec4(255,0,0,255), true, false, false };
static CosmeticsColorIndividual Ice_Arrow_Prim = { "Ice Arrows (Primary)", "Affects Primary color", "gIceArrowCol", icearrow_col, ImVec4(170,255,255,255), true, false, false };
static CosmeticsColorIndividual Ice_Arrow_Env = { "Ice Arrows (Secondary)", "Affects Secondary color", "gIceArrowColEnv", icearrow_colenv, ImVec4(0,0,255,255), true, false, false };
static CosmeticsColorIndividual Light_Arrow_Prim = { "Light Arrows (Primary)", "Affects Primary color", "gLightArrowCol", lightarrow_col, ImVec4(255,255,170,255), true, false, false };
static CosmeticsColorIndividual Light_Arrow_Env = { "Light Arrows (Secondary)", "Affects Secondary color", "gLightArrowColEnv", lightarrow_colenv, ImVec4(255,255,0,255), true, false, false };

//Spells
static CosmeticsColorIndividual Din_Fire_Prim = { "Din's Fire (primary)", "Affects Primary color", "gDF_Col", df_col, ImVec4(255,200,0,255), true, false, false };
static CosmeticsColorIndividual Din_Fire_Env = { "Din's Fire (Secondary)", "Affects Secondary color", "gDF_Env", df_colenv, ImVec4(255,0,0,255), true, false, false };
static CosmeticsColorIndividual Nayru_Diamond_Prim = { "Nayru's Love Diamond (primary)", "Affects Primary color", "gNL_Diamond_Col", nl_diam_col, ImVec4(170,255,255,255), true, false, false };
static CosmeticsColorIndividual Nayru_Diamond_Env = { "Nayru's Love Diamond (Secondary)", "Affects Secondary color", "gNL_Diamond_Env", nl_diam_colenv, ImVec4(100,255,128,255), true, false, false };
static CosmeticsColorIndividual Nayru_Orb_Prim = { "Nayru's Love Orb (primary)", "Affects Primary color", "gNL_Orb_Col", nl_orb_col, ImVec4(170,255,255,255), true, false, false };
static CosmeticsColorIndividual Nayru_Orb_Env = { "Nayru's Love Orb (Secondary)", "Affects Secondary color", "gNL_Orb_Env", nl_orb_colenv, ImVec4(150,255,255,255), true, false, false };

//Spin attacks colors
static CosmeticsColorIndividual Spin_Lv1_Prim = { "Level 1 (primary)", "Affects Primary color", "gCharged1Col", charged1_col, ImVec4(170,255,255,255), true, false, false };
static CosmeticsColorIndividual Spin_Lv1_Env = { "Level 1 (Secondary)", "Affects Secondary color", "gCharged1ColEnv", charged1_colenv, ImVec4(0,100,255,255), true, false, false };
static CosmeticsColorIndividual Spin_Lv2_Prim = { "Level 2 (primary)", "Affects Primary color", "gCharged2Col", charged2_col, ImVec4(255,255,170,255), true, false, false };
static CosmeticsColorIndividual Spin_Lv2_Env = { "Level 2 (Secondary)", "Affects Secondary color", "gCharged2ColEnv", charged2_colenv, ImVec4(255,100,0,255), true, false, false };

//Trails
static CosmeticsColorIndividual Sword_Trails_Top_col = { "Sword Trail Top Color", "Affects top of sword slash", "gSwordTrailTopCol", swordtrailtop_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual Sword_Trails_Bottom_col = { "Sword Trail End Color", "Affects bottom of sword slash", "gSwordTrailBottomCol", swordtrailbottom_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual Boom_Trails_Start_col = { "Boomerang Trail Start Color", "Affects start of boomerang trail", "gBoomTrailStartCol", boomtrailstart_col, ImVec4(255,255,100,255), true, false, false };
static CosmeticsColorIndividual Boom_Trails_End_col = { "Boomerang Trail End Color", "Affects end of boomerang trail", "gBoomTrailEndCol", boomtrailend_col, ImVec4(255,255,100,255), true, false, false };
static CosmeticsColorIndividual Bomb_Trails_col = { "Bombchu Trail Color", "Affects Bomchus", "gBombTrailCol", bombtrail_col, ImVec4(250,0,0,255), true, false, false };


static CosmeticsColorIndividual KSword_Trails_Top_col = { "Kokiri Sword Top Color", "Affects top of sword slash", "gKSwordTrailTopCol", swordtrailtop_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual KSword_Trails_Bottom_col = { "Kokiri Sword End Color", "Affects bottom of sword slash", "gKSwordTrailBottomCol", swordtrailbottom_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual MSword_Trails_Top_col = { "Master Sword Top Color", "Affects top of sword slash", "gMSwordTrailTopCol", swordtrailtop_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual MSword_Trails_Bottom_col = { "Master Sword End Color", "Affects bottom of sword slash", "gMSwordTrailBottomCol", swordtrailbottom_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual BSword_Trails_Top_col = { "BG Sword Top Color", "Affects top of sword slash", "gBSwordTrailTopCol", swordtrailtop_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual BSword_Trails_Bottom_col = { "BG Sword End Color", "Affects bottom of sword slash", "gBSwordTrailBottomCol", swordtrailbottom_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual Stick_Trails_Top_col = { "Stick Top Color", "Affects top of stick swing", "gStickTrailTopCol", swordtrailtop_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual Stick_Trails_Bottom_col = { "Stick End Color", "Affects bottom of stick swing", "gStickTrailBottomCol", swordtrailbottom_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual Hammer_Trails_Top_col = { "Hammer Top Color", "Affects top of hammer swing", "gHammerTrailTopCol", swordtrailtop_col, ImVec4(255,255,255,255), true, false, false };
static CosmeticsColorIndividual Hammer_Trails_Bottom_col = { "Hammer End Color", "Affects bottom of hammer swing", "gHammerTrailBottomCol", swordtrailbottom_col, ImVec4(255,255,255,255), true, false, false };

//Menus - File Choose
static CosmeticsColorIndividual FileChoose_Background = { "Main menu color", "Affects the File Select menu background.", "gCCFileChoosePrim", fileselect_colors, ImVec4(100, 150, 255, 255), true, false, false };
static CosmeticsColorIndividual FileChoose_BottomText = { "Bottom texts color", "Affects the File Select texts.", "gCCFileChooseTextPrim", fileselect_text_colors, ImVec4(100, 255, 255, 255), true, false, false };

//Hud Stuff (Dpad, Buttons etc)
static CosmeticsColorIndividual A_Btn = { "A Button", "Affects the A button colors (and various cursors that use the same theme)", "gCCABtnPrim", a_btn_colors, ImVec4(0, 200, 50, 255), true, false, false };
static CosmeticsColorIndividual B_Btn = { "B Button", "Affects the B button color", "gCCBBtnPrim", b_btn_colors, ImVec4(255, 30, 30, 255), true, false, false };
static CosmeticsColorIndividual C_Btn = { "C-Buttons", "Affects the C Buttons' color (if not using separate colors)\nAnd various cursor that use C-Buttons colors", "gCCCBtnPrim", c_btn_colors, ImVec4(255, 160, 0, 255), true, false, false };
static CosmeticsColorIndividual C_Btn_U = { "C-Buttons Up", "Affects C-Buttons Up colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top", "gCCCUBtnPrim", c_btn_u_colors, ImVec4(255,160,0,255), true, false, false };
static CosmeticsColorIndividual C_Btn_D = { "C-Buttons Down", "Affects C-Buttons Down colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top", "gCCCDBtnPrim", c_btn_d_colors, ImVec4(255,160,0,255), true, false, false };
static CosmeticsColorIndividual C_Btn_L = { "C-Buttons Left", "Affects C-Buttons Left colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top", "gCCCLBtnPrim", c_btn_l_colors, ImVec4(255,160,0,255), true, false, false };
static CosmeticsColorIndividual C_Btn_R = { "C-Buttons Right", "Affects C-Buttons Right colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top", "gCCCRBtnPrim", c_btn_r_colors, ImVec4(255,160,0,255), true, false, false };
static CosmeticsColorIndividual Start_btn = { "Start Button", "Affects the Start button color", "gCCStartBtnPrim", start_btn_colors, ImVec4(200, 0, 0, 255), true, false, false };
static CosmeticsColorIndividual DPad_Items = { "DPad background", "DPad background color, White is the default value", "gCCDpadPrim", dpad_colors, ImVec4(255, 255, 255, 255), true, false, false };

//Hearts colors
static CosmeticsColorIndividual Hearts_NInner = { "Inner (Normal)", "Affects the inner color", "gCCHeartsPrim", hearts_colors, ImVec4(255,70,50,255), true, false, false };
static CosmeticsColorIndividual Hearts_DDInner = { "Inner (Double defense)", "Affects the inner color", "gCCDDHeartsPrim", hearts_ddi_colors, ImVec4(200,00,00,255), true, false, false };
static CosmeticsColorIndividual Hearts_DDOutline = { "Outline (Double defense)", "Affects the outline color of hearts when you have Double Defense\nWhite is the default value", "gDDCCHeartsPrim", hearts_dd_colors, ImVec4(255,255,255,255), true, false, false };

//Magic Meter colors
static CosmeticsColorIndividual Magic_Borders = { "Borders", "Affects the border of the magic meter\nWhite is the default value, color change only when used one time", "gCCMagicBorderNormPrim", magic_bordern_colors, ImVec4(255,255,255,255), false, false, false };
static CosmeticsColorIndividual Magic_InUse_Borders = { "Borders in use", "Affects the border of the magic meter when being used\nWhite is the default value", "gCCMagicBorderPrim", magic_border_colors, ImVec4(255,255,255,255), false, false, false };
static CosmeticsColorIndividual Magic_Meter = { "Main color", "Affects the magic meter color\nGreen is the default value", "gCCMagicPrim", magic_remaining_colors, ImVec4(0,200,0,255), true, false, false };
static CosmeticsColorIndividual Magic_InUse_Meter = { "Main color in use", "Affects the magic meter when being used\nYellow is the default value", "gCCMagicUsePrim", magic_use_colors, ImVec4(250,250,0,255), true, false, false };

//Minimap and it's arrow colors
static CosmeticsColorIndividual Minimap_OW = { "Overworlds", "Affects the Overworld minimaps", "gCCMinimapPrim", minimap_colors, ImVec4(0, 255, 255, 255), true, false, false };
static CosmeticsColorIndividual Minimap_DG = { "Dungeons", "Affects the Dungeon minimaps", "gCCMinimapDGNPrim", dgn_minimap_colors, ImVec4(100, 255, 255, 255), true, false, false };
static CosmeticsColorIndividual Minimap_CP = { "Current position arrow", "Affects the current position arrow on the minimap\nYellow is the default value", "gCCMinimapCPPrim", cp_minimap_colors, ImVec4(200, 255, 0, 255), true, false, false };
static CosmeticsColorIndividual Minimap_LE = { "Last entrance arrow", "Affects the last entrance position arrow on the minimap\nRed is the default value", "gCCMinimapLEPrim", le_minimap_colors, ImVec4(200, 0, 0, 255), true, false, false };

//Title Cards colors
static CosmeticsColorIndividual TitleCards_OW = { "Main color (Overworld)", "Affects all the overworld title cards color, white is the default value", "gCCTC_OW_U_Prim", tc_ou_colors, ImVec4(255, 255, 255, 255), false, false, false };
static CosmeticsColorIndividual TitleCards_DG = { "Main color (Dungeon)", "Affects all the bosses title cards color, white is the default value", "gCCTC_B_U_Prim", tc_bu_colors, ImVec4(255, 255, 255, 255), false, false, false };

//Misc. colors, the one I have no clue where to put nor that aren't that important
#ifdef MASTER_QUEST
static CosmeticsColorIndividual TitleScreen_fire_Prim = { "Opening logo fire (Primary)", "Affects the fire behind the title screen logo", "gCCTCFirePrim", tc_fire_colors, ImVec4(170, 255, 255, 255), false, false, false };
static CosmeticsColorIndividual TitleScreen_fire_Env = { "Opening logo fire (Secondary)", "Affects the fire behind the title screen logo", "gCCTCFireEnv", tc_fire_colors_env, ImVec4(200, 255, 0, 255), false, false, false };
#else
static CosmeticsColorIndividual TitleScreen_fire_Prim = { "Opening logo fire (Primary)", "Affects the fire behind the title screen logo", "gCCTCFirePrim", tc_fire_colors, ImVec4(255, 255, 170, 255), false, false, false };
static CosmeticsColorIndividual TitleScreen_fire_Env = { "Opening logo fire (Secondary)", "Affects the fire behind the title screen logo", "gCCTCFireEnv", tc_fire_colors_env, ImVec4(255, 100, 0, 255), false, false, false };
#endif
static CosmeticsColorIndividual Rupee_Icon = { "Rupee icon", "Affects the Rupee icon on interface\nGreen is the default value", "gCCRupeePrim", rupee_colors, ImVec4(200, 255, 100, 255), true, false, false };
static CosmeticsColorIndividual SmallKeys_Icon = { "Small Keys icon", "Affects the Small keys icon on interface\nGray is the default value", "gCCKeysPrim", smolekey_colors, ImVec4(200, 230, 255, 255), true, false, false };
static CosmeticsColorIndividual VSOA_Icon = { "Visual Stone of agony icon", "Affects the Stone of Agony icon on interface\nWhite is the default value\nRequire room reload or activation", "gCCVSOAPrim", visualagony_colors, ImVec4(255, 255, 255, 255), true, false, false };

//Sections creation, here I regroup them in some sort of a Theme, all navi stuff together etc. It will be used to create the menu.
static CosmeticsColorSection Navi_Section[] = {
    { &Navi_Idle_Inner, false, false },
    { &Navi_Idle_Outer, true, false },
    { &Navi_Npc_Inner, false, true },
    { &Navi_Npc_Outer, true, false },
    { &Navi_Enemy_Inner, false, true },
    { &Navi_Enemy_Outer, true, false },
    { &Navi_Prop_Inner, false, true },
    { &Navi_Prop_Outer, true, false }
};
static CosmeticsColorSection Keese_Section[] = {
    { &Keese1_prim, false, false },
    { &Keese2_prim, true, false },
    { &Keese1_env, false, true },
    { &Keese2_env, true, false }
};
static CosmeticsColorSection Dogs_Section[] = {
    { &DogN1, false, false },
    { &DogN2, true, false }
};
static CosmeticsColorSection Tunics_Section[] = {
    { &KokiriTunic, false, false },
    { &GoronTunic, true, false },
    { &ZoraTunic, true, false }
};
static CosmeticsColorSection Gauntlets_Section[] = {
    { &SilverGauntlets, false, false },
    { &GoldenGauntlets, true, false },
};
static CosmeticsColorSection MirrorShield_Section[] = {
    { &MirrorShieldBorder, false, false },
    { &MirrorShieldMirror, true, false },
    { &MirrorShieldEmblem, true, false },
};
static CosmeticsColorSection Arrows_section[] = {
    { &Normal_Arrow_Prim, false, false },
    { &Normal_Arrow_Env, true, false },
    { &Fire_Arrow_Prim, false, true },
    { &Fire_Arrow_Env, true, false },
    { &Ice_Arrow_Prim, false, true },
    { &Ice_Arrow_Env, true, false },
    { &Light_Arrow_Prim, false, true },
    { &Light_Arrow_Env, true, false }
};
static CosmeticsColorSection Spells_section[] = {
    { &Din_Fire_Prim, false, false },
    { &Din_Fire_Env, true, false },
    { &Nayru_Diamond_Prim, false, true },
    { &Nayru_Diamond_Env, true, false },
    { &Nayru_Orb_Prim, false, true },
    { &Nayru_Orb_Env, true, false }
};
static CosmeticsColorSection SpinAtk_section[] = {
    { &Spin_Lv1_Prim, false, false },
    { &Spin_Lv1_Env, true, false },
    { &Spin_Lv2_Prim, false, true },
    { &Spin_Lv2_Env, true, false } };

static CosmeticsColorSection Trail_section[] = {
    { &Sword_Trails_Top_col, false, false },
    { &Sword_Trails_Bottom_col, false, false },
    { &Boom_Trails_Start_col, true, false },
    { &Boom_Trails_End_col, false, false },
    { &Bomb_Trails_col, true, false }
};
static CosmeticsColorSection SwordTrail_section[] = {
    { &KSword_Trails_Top_col, false, false },
    { &KSword_Trails_Bottom_col, false, false },
    { &MSword_Trails_Top_col, true, false },
    { &MSword_Trails_Bottom_col, false, false },
    { &BSword_Trails_Top_col, true, false },
    { &BSword_Trails_Bottom_col, false, false },
    { &Stick_Trails_Top_col, true, false },
    { &Stick_Trails_Bottom_col, false, false },
    { &Hammer_Trails_Top_col, true, false },
    { &Hammer_Trails_Bottom_col, false, false },
};
static CosmeticsColorSection AllTrail_section[] = {
    { &Sword_Trails_Top_col, false, false },
    { &Sword_Trails_Bottom_col, false, false },
    { &Boom_Trails_Start_col, true, false },
    { &Boom_Trails_End_col, false, false },
    { &Bomb_Trails_col, true, false },
    { &KSword_Trails_Top_col, false, false },
    { &KSword_Trails_Bottom_col, false, false },
    { &MSword_Trails_Top_col, true, false },
    { &MSword_Trails_Bottom_col, false, false },
    { &BSword_Trails_Top_col, true, false },
    { &BSword_Trails_Bottom_col, false, false },
    { &Stick_Trails_Top_col, true, false },
    { &Stick_Trails_Bottom_col, false, false },
    { &Hammer_Trails_Top_col, true, false },
    { &Hammer_Trails_Bottom_col, false, false },
};
static CosmeticsColorSection FileChoose_section[] = {
    { &FileChoose_Background, false, false },
    { &FileChoose_BottomText, true, false }
};
static CosmeticsColorSection A_Btn_section[] = {
    { &A_Btn, false, false }
};
static CosmeticsColorSection B_Btn_section[] = {
    { &B_Btn, false, false }
};
static CosmeticsColorSection C_Btn_Unified_section[] = {
    { &C_Btn, false, false }
};
static CosmeticsColorSection C_Btn_Separated_section[] = {
    { &C_Btn_U, false, false },
    { &C_Btn_D, false, true },
    { &C_Btn_L, false, true },
    { &C_Btn_R, false, true }
};
static CosmeticsColorSection Start_Btn_section[] = {
    { &Start_btn, false, false }
};
static CosmeticsColorSection DPad_section[] = {
    { &DPad_Items, false, false }
};
static CosmeticsColorSection Hearts_section[] = {
    { &Hearts_NInner, false, false },
    { &Hearts_DDInner, true, false },
    { &Hearts_DDOutline, true, false }
};
static CosmeticsColorSection Magic_Meter_section[] = {
    { &Magic_Borders, false, false },
    { &Magic_InUse_Borders, true, false },
    { &Magic_Meter, false, true },
    { &Magic_InUse_Meter, true, false }
};
static CosmeticsColorSection Minimap_section[] = {
    { &Minimap_OW, false, false },
    { &Minimap_DG, false, true },
    { &Minimap_CP, false, true },
    { &Minimap_LE, false, true }
};
static CosmeticsColorSection TitleCards_section[] = {
    { &TitleCards_OW, false, false },
    { &TitleCards_DG, true, false }
};
static CosmeticsColorSection Misc_section[] = {
    { &Rupee_Icon, false, false },
    { &SmallKeys_Icon, false, true },
    { &VSOA_Icon, false, true },
    { &TitleScreen_fire_Prim, false, true },
    { &TitleScreen_fire_Env, false, true }
};

//Randomizer specific stuff (by randomizer I mean within cosmetics, these section are there just for the purpose of randomizing their colors)
static CosmeticsColorSection Everything_Section[] = {
    { &TitleScreen_fire_Prim, false, true },
    { &TitleScreen_fire_Env, false, true },
    { &Navi_Idle_Inner, false, false },
    { &Navi_Idle_Outer, true, false },
    { &Navi_Npc_Inner, false, true },
    { &Navi_Npc_Outer, true, false },
    { &Navi_Enemy_Inner, false, true },
    { &Navi_Enemy_Outer, true, false },
    { &Navi_Prop_Inner, false, true },
    { &Navi_Prop_Outer, true, false },
    { &Keese1_prim, false, false },
    { &Keese2_prim, true, false },
    { &Keese1_env, false, true },
    { &Keese2_env, true, false },
    { &DogN1, false, false },
    { &DogN2, true, false },
    { &KokiriTunic, false, false },
    { &GoronTunic, true, false },
    { &ZoraTunic, true, false },
    { &SilverGauntlets, true, false },
    { &GoldenGauntlets, true, false },
    { &MirrorShieldBorder, false, false },
    { &MirrorShieldMirror, true, false },
    { &MirrorShieldEmblem, true, false },
    { &Normal_Arrow_Prim, false, false },
    { &Normal_Arrow_Env, true, false },
    { &Fire_Arrow_Prim, false, true },
    { &Fire_Arrow_Env, true, false },
    { &Ice_Arrow_Prim, false, true },
    { &Ice_Arrow_Env, true, false },
    { &Light_Arrow_Prim, false, true },
    { &Light_Arrow_Env, true, false },
    { &Din_Fire_Prim, false, false },
    { &Din_Fire_Env, true, false },
    { &Nayru_Diamond_Prim, false, true },
    { &Nayru_Diamond_Env, true, false },
    { &Nayru_Orb_Prim, false, true },
    { &Nayru_Orb_Env, true, false },
    { &Spin_Lv1_Prim, false, false },
    { &Spin_Lv1_Env, true, false },
    { &Spin_Lv2_Prim, false, true },
    { &Spin_Lv2_Env, true, false },
    { &Sword_Trails_Top_col, false, false },
    { &Sword_Trails_Bottom_col, false, false },
    { &Boom_Trails_Start_col, true, false },
    { &Boom_Trails_End_col, false, false },
    { &Bomb_Trails_col, true, false },
    { &KSword_Trails_Top_col, false, false },
    { &KSword_Trails_Bottom_col, false, false },
    { &MSword_Trails_Top_col, true, false },
    { &MSword_Trails_Bottom_col, false, false },
    { &BSword_Trails_Top_col, true, false },
    { &BSword_Trails_Bottom_col, false, false },
    { &Stick_Trails_Top_col, true, false },
    { &Stick_Trails_Bottom_col, false, false },
    { &Hammer_Trails_Top_col, true, false },
    { &Hammer_Trails_Bottom_col, false, false },
    { &FileChoose_Background, false, false },
    { &FileChoose_BottomText, true, false },
    { &A_Btn, false, false },
    { &B_Btn, false, false },
    { &C_Btn, false, false },
    { &C_Btn_U, false, false },
    { &C_Btn_D, false, true },
    { &C_Btn_L, false, true },
    { &C_Btn_R, false, true },
    { &Start_btn, false, false },
    { &DPad_Items, false, false },
    { &Hearts_NInner, false, false },
    { &Hearts_DDInner, true, false },
    { &Hearts_DDOutline, true, false },
    { &Magic_Borders, false, false },
    { &Magic_InUse_Borders, true, false },
    { &Magic_Meter, false, true },
    { &Magic_InUse_Meter, true, false },
    { &Minimap_OW, false, false },
    { &Minimap_DG, false, true },
    { &Minimap_CP, false, true },
    { &Minimap_LE, false, true },
    { &TitleCards_OW, false, false },
    { &TitleCards_DG, true, false },
    { &Rupee_Icon, false, false },
    { &SmallKeys_Icon, false, true },
    { &VSOA_Icon, false, true }
};
static CosmeticsColorSection Buttons_section[]{
    { &DPad_Items, false, false },
    { &C_Btn_U, false, false },
    { &C_Btn_D, false, true },
    { &C_Btn_L, false, true },
    { &C_Btn_R, false, true },
    { &C_Btn, false, false },
    { &B_Btn, false, false },
    { &A_Btn, false, false },
    { &Start_btn, false, false }
};
static CosmeticsColorSection Misc_Interface_section[]{
    { &Hearts_NInner, false, false },
    { &Hearts_DDInner, true, false },
    { &Hearts_DDOutline, true, false },
    { &Magic_Borders, false, false },
    { &Magic_InUse_Borders, true, false },
    { &Magic_Meter, false, true },
    { &Magic_InUse_Meter, true, false },
    { &Minimap_OW, false, false },
    { &Minimap_DG, false, true },
    { &Minimap_CP, false, true },
    { &Minimap_LE, false, true },
    { &TitleCards_OW, false, false },
    { &TitleCards_DG, true, false },
    { &Rupee_Icon, false, false },
    { &SmallKeys_Icon, false, true },
    { &VSOA_Icon, false, true },
    { &TitleScreen_fire_Prim, false, true },
    { &TitleScreen_fire_Env, false, true }
};
static CosmeticsColorSection NPCs_section[]{
    { &Navi_Idle_Inner, false, false },
    { &Navi_Idle_Outer, true, false },
    { &Navi_Npc_Inner, false, true },
    { &Navi_Npc_Outer, true, false },
    { &Navi_Enemy_Inner, false, true },
    { &Navi_Enemy_Outer, true, false },
    { &Navi_Prop_Inner, false, true },
    { &Navi_Prop_Outer, true, false },
    { &Keese1_prim, false, false },
    { &Keese2_prim, true, false },
    { &Keese1_env, false, true },
    { &Keese2_env, true, false },
    { &DogN1, false, false },
    { &DogN2, true, false }
};
static CosmeticsColorSection AllItemsSkills_section[]{
    { &KokiriTunic, false, false },
    { &GoronTunic, true, false },
    { &ZoraTunic, true, false },
    { &SilverGauntlets, true, false },
    { &GoldenGauntlets, true, false },
    { &MirrorShieldBorder, false, false },
    { &MirrorShieldMirror, true, false },
    { &MirrorShieldEmblem, true, false },
    { &Normal_Arrow_Prim, false, false },
    { &Normal_Arrow_Env, true, false },
    { &Fire_Arrow_Prim, false, true },
    { &Fire_Arrow_Env, true, false },
    { &Ice_Arrow_Prim, false, true },
    { &Ice_Arrow_Env, true, false },
    { &Light_Arrow_Prim, false, true },
    { &Light_Arrow_Env, true, false },
    { &Din_Fire_Prim, false, false },
    { &Din_Fire_Env, true, false },
    { &Nayru_Diamond_Prim, false, true },
    { &Nayru_Diamond_Env, true, false },
    { &Nayru_Orb_Prim, false, true },
    { &Nayru_Orb_Env, true, false },
    { &Spin_Lv1_Prim, false, false },
    { &Spin_Lv1_Env, true, false },
    { &Spin_Lv2_Prim, false, true },
    { &Spin_Lv2_Env, true, false },
    { &Sword_Trails_Top_col, false, false},
    { &Sword_Trails_Bottom_col, false, false},
    { &Boom_Trails_Start_col, true, false },
    { &Boom_Trails_Start_col, true, false },
    { &Boom_Trails_End_col, false, false },
    { &Bomb_Trails_col, true, false },
    { &KSword_Trails_Top_col, false, false },
    { &KSword_Trails_Bottom_col, false, false },
    { &MSword_Trails_Top_col, true, false },
    { &MSword_Trails_Bottom_col, false, false },
    { &BSword_Trails_Top_col, true, false },
    { &BSword_Trails_Bottom_col, false, false },
    { &Stick_Trails_Top_col, true, false },
    { &Stick_Trails_Bottom_col, false, false },
    { &Hammer_Trails_Top_col, true, false },
    { &Hammer_Trails_Bottom_col, false, false }
};

void InitCosmeticsEditor();//Init the menu itself
void LoadRainbowColor();
void NewSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0);
ImVec4 GetRandomValue(int MaximumPossible);
