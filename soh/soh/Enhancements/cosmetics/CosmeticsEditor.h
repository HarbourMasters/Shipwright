#pragma once
#include <ImGuiImpl.h>

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

static float TablesCellsWidth = 300.0f;
static ImGuiTableColumnFlags FlagsTable = ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV;
static ImGuiTableColumnFlags FlagsCell = ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort;
static ImVec4 navi_idle_i_col;        static ImVec4 navi_idle_o_col;
static ImVec4 navi_npc_i_col;         static ImVec4 navi_npc_o_col;
static ImVec4 navi_enemy_i_col;       static ImVec4 navi_enemy_o_col;
static ImVec4 navi_prop_i_col;        static ImVec4 navi_prop_o_col;
static ImVec4 swordtrailtop_col;      static ImVec4 swordtrailbottom_col;
static ImVec4 boomtrailstart_col;     static ImVec4 boomtrailend_col;
static ImVec4 bombtrail_col;           
static ImVec4 tc_ou_colors;           static ImVec4 tc_bu_colors;

//Navi
static CosmeticsColorIndividual Navi_Idle_Inner = { "Navi Idle (Primary)", "Inner color for Navi (idle flying around)", "gNavi_Idle_Inner", navi_idle_i_col, ImVec4(255, 255, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Idle_Outer = { "Navi Idle (Secondary)", "Outer color for Navi (idle flying around)", "gNavi_Idle_Outer", navi_idle_o_col, ImVec4(0, 0, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Npc_Inner = { "Navi NPC (Primary)", "Inner color for Navi (when Navi fly around NPCs)", "gNavi_NPC_Inner", navi_npc_i_col, ImVec4(150, 150, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Npc_Outer = { "Navi NPC (Secondary)", "Outer color for Navi (when Navi fly around NPCs)", "gNavi_NPC_Outer", navi_npc_o_col, ImVec4(150, 150, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Enemy_Inner = { "Navi Enemy (Primary)", "Inner color for Navi (when Navi fly around Enemies or Bosses)", "gNavi_Enemy_Inner", navi_enemy_i_col, ImVec4(255, 255, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Enemy_Outer = { "Navi Enemy (Secondary)", "Outer color for Navi (when Navi fly around Enemies or Bosses)", "gNavi_Enemy_Outer", navi_enemy_o_col, ImVec4(220, 155, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Prop_Inner = { "Navi Props (Primary)", "Inner color for Navi (when Navi fly around props (signs etc))", "gNavi_Prop_Inner", navi_prop_i_col, ImVec4(0, 255, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Prop_Outer = { "Navi Props (Secondary)", "Outer color for Navi (when Navi fly around props (signs etc))", "gNavi_Prop_Outer", navi_prop_o_col, ImVec4(0, 255, 0, 255), false, false, false };

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

//Title Cards colors
static CosmeticsColorIndividual TitleCards_OW = { "Main color (Overworld)", "Affects all the overworld title cards color, white is the default value", "gCCTC_OW_U_Prim", tc_ou_colors, ImVec4(255, 255, 255, 255), false, false, false };
static CosmeticsColorIndividual TitleCards_DG = { "Main color (Dungeon)", "Affects all the bosses title cards color, white is the default value", "gCCTC_B_U_Prim", tc_bu_colors, ImVec4(255, 255, 255, 255), false, false, false };

void InitCosmeticsEditor();//Init the menu itself
ImVec4 GetRandomValue(int MaximumPossible);
