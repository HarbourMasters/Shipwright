#pragma once
#include "../libultraship/ImGuiImpl.h"
#define SECTION_SIZE(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define CATEGORY_NPC 0
#define CATEGORY_ITEMS 1
#define CATEGORY_MENU 2
#define CATEGORY_HUD 3
#define CATEGORY_MISC 4

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
static ImVec4 hearts_colors;       static ImVec4 hearts_dd_colors;        static ImVec4 hearts_ddi_colors;
static ImVec4 a_btn_colors;        static ImVec4 b_btn_colors;            static ImVec4 c_btn_colors;        static ImVec4 start_btn_colors;
static ImVec4 c_btn_u_colors;      static ImVec4 c_btn_l_colors;          static ImVec4 c_btn_d_colors;      static ImVec4 c_btn_r_colors;
static ImVec4 magic_border_colors; static ImVec4 magic_remaining_colors;  static ImVec4 magic_use_colors;
static ImVec4 minimap_colors;      static ImVec4 dgn_minimap_colors;
static ImVec4 cp_minimap_colors;   static ImVec4 le_minimap_colors;
static ImVec4 rupee_colors;        static ImVec4 smolekey_colors;         static ImVec4 magic_bordern_colors;
static ImVec4 fileselect_colors;   static ImVec4 fileselect_text_colors;
static ImVec4 kokiri_col;          static ImVec4 goron_col;               static ImVec4 zora_col;
static ImVec4 navi_idle_i_col;     static ImVec4 navi_idle_o_col;
static ImVec4 navi_npc_i_col;      static ImVec4 navi_npc_o_col;
static ImVec4 navi_enemy_i_col;    static ImVec4 navi_enemy_o_col;
static ImVec4 navi_prop_i_col;     static ImVec4 navi_prop_o_col;
static ImVec4 trailscol;
static ImVec4 firearrow_col;       static ImVec4 icearrow_col;            static ImVec4 lightarrow_col;
static ImVec4 firearrow_colenv;    static ImVec4 icearrow_colenv;         static ImVec4 lightarrow_colenv;
static ImVec4 charged1_col;        static ImVec4 charged2_col;
static ImVec4 charged1_colenv;     static ImVec4 charged2_colenv;
static ImVec4 Keese1_primcol;      static ImVec4 Keese2_primcol;
static ImVec4 Keese1_envcol;       static ImVec4 Keese2_envcol;
static ImVec4 doggo1col;           static ImVec4 doggo2col;
static ImVec4 df_col;              static ImVec4 df_colenv;
static ImVec4 nl_diam_col;         static ImVec4 nl_diam_colenv;
static ImVec4 nl_orb_col;          static ImVec4 nl_orb_colenv;
static ImVec4 tc_ou_colors;        static ImVec4 tc_bu_colors;
static ImVec4 dpad_colors;
static ImVec4 visualagony_colors;
/*ImVec4 menu_equips_colors;
ImVec4 menu_items_colors;
ImVec4 menu_map_colors;
ImVec4 menu_quest_colors;
ImVec4 menu_save_colors;
ImVec4 menu_gameover_colors;*/

//Navi
static CosmeticsColorIndividual Navi_Idle_Inner = { "Navi Idle (Primary)", "Inner color for Navi (idle flying around)", "gNavi_Idle_Inner_", navi_idle_i_col, ImVec4(255, 255, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Idle_Outer = { "Navi Idle (Secondary)", "Outer color for Navi (idle flying around)", "gNavi_Idle_Outer_", navi_idle_o_col, ImVec4(0, 0, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Npc_Inner = { "Navi NPC (Primary)", "Inner color for Navi (when Navi fly around NPCs)", "gNavi_NPC_Inner_", navi_npc_i_col, ImVec4(150, 150, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Npc_Outer = { "Navi NPC (Secondary)", "Outer color for Navi (when Navi fly around NPCs)", "gNavi_NPC_Outer_", navi_npc_o_col, ImVec4(150, 150, 255, 255), false, false, false };
static CosmeticsColorIndividual Navi_Enemy_Inner = { "Navi Enemy (Primary)", "Inner color for Navi (when Navi fly around Enemies or Bosses)", "gNavi_Enemy_Inner_", navi_enemy_i_col, ImVec4(255, 255, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Enemy_Outer = { "Navi Enemy (Secondary)", "Outer color for Navi (when Navi fly around Enemies or Bosses)", "gNavi_Enemy_Outer_", navi_enemy_o_col, ImVec4(220, 155, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Prop_Inner = { "Navi Enemy (Primary)", "Inner color for Navi (when Navi fly around props (signs etc))", "gNavi_Prop_Inner_", navi_prop_i_col, ImVec4(0, 255, 0, 255), false, false, false };
static CosmeticsColorIndividual Navi_Prop_Outer = { "Navi Enemy (Secondary)", "Outer color for Navi (when Navi fly around props (signs etc))", "gNavi_Prop_Outer_", navi_prop_o_col, ImVec4(0, 255, 0, 255), false, false, false };

//Keese
static CosmeticsColorIndividual Keese1_prim = { "Fire Primary color", "Affects the primary color of the Fire itself of the Keese", "gKeese1_Ef_Prim", Keese1_primcol, ImVec4(255, 255, 100, 255), true, false, false };
static CosmeticsColorIndividual Keese2_prim = { "Ice Primary color", "Affects the primary color of the Ice itself of the Keese", "gKeese2_Ef_Prim", Keese2_primcol, ImVec4(100, 200, 255, 255), true, false, false };
static CosmeticsColorIndividual Keese1_env = { "Fire Secondary color", "Affects the secondary color of the Fire itself of the Keese", "gKeese1_Ef_Env", Keese1_envcol, ImVec4(255, 50, 0, 255), true, false, false };
static CosmeticsColorIndividual Keese2_env = { "Ice Secondary color", "Affects the secondary color of the Ice itself of the Keese", "gKeese2_Ef_Env", Keese2_envcol, ImVec4(0, 0, 255, 255), true, false, false };

//Good old Dogs, we all love them
static CosmeticsColorIndividual DogN1 = { "Dog white", "Affects the colors of the white dog", "gDog1Col", doggo1col, ImVec4(255,255,200,255), true, false, true };
static CosmeticsColorIndividual DogN2 = { "Dog brown", "Affects the colors of the brown dog", "gDog2Col", doggo2col, ImVec4(150,100,50,255), true, false, true };


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

void InitCosmeticsEditor();//Init the menu itself
void LoadRainbowColor();
void NewSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0);