#include "CosmeticsEditor.h"
#include "../../util.h"
#include "../libultraship/SohImGuiImpl.h"

#include <array>
#include <bit>
#include <map>
#include <string>
#include <Cvar.h>

/**
 * Colors variables
 */
float TablesCellsWidth = 300.0f; //1 Col
float Tables1CellsWidth = 300.0f; //1 Col
float Tables2CellsWidth = 200.0f; //2 Col
float Tables3CellsWidth = 100.0f; //3 Col
ImVec4 hearts_colors;
ImVec4 hearts_dd_colors;
ImVec4 hearts_ddi_colors; //DD inner colors
ImVec4 a_btn_colors;
ImVec4 b_btn_colors;
ImVec4 c_btn_colors;
ImVec4 start_btn_colors;
ImVec4 magic_border_colors;
ImVec4 magic_remaining_colors;
ImVec4 magic_use_colors;
ImVec4 minimap_colors;
ImVec4 rupee_colors;
ImVec4 smolekey_colors;
ImVec4 fileselect_colors;
ImVec4 fileselect_text_colors;
ImVec4 kokiri_col;
ImVec4 goron_col;
ImVec4 zora_col;
ImVec4 navi_idle_i_col;
ImVec4 navi_idle_o_col;
ImVec4 navi_npc_i_col;
ImVec4 navi_npc_o_col;
ImVec4 navi_enemy_i_col;
ImVec4 navi_enemy_o_col;
ImVec4 navi_prop_i_col;
ImVec4 navi_prop_o_col;
ImVec4 trailscol;
ImVec4 c_btn_u_colors;
ImVec4 c_btn_l_colors;
ImVec4 c_btn_d_colors;
ImVec4 c_btn_r_colors;
ImVec4 magic_bordern_colors;
ImVec4 firearrow_col;
ImVec4 icearrow_col;
ImVec4 lightarrow_col;
ImVec4 firearrow_colenv;
ImVec4 icearrow_colenv;
ImVec4 lightarrow_colenv;
ImVec4 charged1_col;
ImVec4 charged2_col;
ImVec4 charged1_colenv;
ImVec4 charged2_colenv;
ImVec4 doggo1col;
ImVec4 doggo2col;
ImVec4 df_col;
ImVec4 df_colenv;
ImVec4 nl_diam_col;
ImVec4 nl_diam_colenv;
ImVec4 nl_orb_col;
ImVec4 nl_orb_colenv;
ImVec4 dgn_minimap_colors;
ImVec4 cp_minimap_colors;
ImVec4 le_minimap_colors;
ImVec4 menu_save_colors;
ImVec4 menu_gameover_colors;
ImVec4 tc_ou_colors;
ImVec4 tc_bu_colors;
ImVec4 menu_equips_colors;
ImVec4 menu_items_colors;
ImVec4 menu_map_colors;
ImVec4 menu_quest_colors;
ImVec4 menu_save_colors;
ImVec4 menu_gameover_colors;
void Table_InitHeader(bool has_header = true) {
    if (has_header) {
        ImGui::TableHeadersRow();
    }
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding(); //This is to adjust Vertical pos of item in a cell to be normlized.
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
}
void Table_NextCol() {
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
}
void Table_NextLine() {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
}
void Draw_Npcs(){
    SohImGui::EnhancementCheckbox("Custom colors for Navi", "gUseNaviCol");
    SohImGui::Tooltip("Enable/Disable custom Navi's colors. \nIf disabled you will have original colors for Navi.\nColors are refreshed when Navi goes back in your pockets.");
    if (CVar_GetS32("gUseNaviCol",0) && ImGui::BeginTable("tableNavi", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Inner colors##Navi", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Navi", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Navi Idle", "gNavi_Idle_Inner_", navi_idle_i_col, ImVec4(255, 255, 255, 255), false);
        SohImGui::Tooltip("Inner color for Navi (idle flying around)");
        Table_NextCol();
        SohImGui::EnhancementColor("Navi Idle", "gNavi_Idle_Outer_", navi_idle_o_col, ImVec4(0, 0, 255, 255), false);
        SohImGui::Tooltip("Outer color for Navi (idle flying around)");
        Table_NextLine();
        SohImGui::EnhancementColor("Navi NPC", "gNavi_NPC_Inner_", navi_npc_i_col, ImVec4(150, 150, 255, 255), false);
        SohImGui::Tooltip("Inner color for Navi (when Navi fly around NPCs)");
        Table_NextCol();
        SohImGui::EnhancementColor("Navi NPC", "gNavi_NPC_Outer_", navi_npc_o_col, ImVec4(150, 150, 255, 255), false);
        SohImGui::Tooltip("Outer color for Navi (when Navi fly around NPCs)");
        Table_NextLine();
        SohImGui::EnhancementColor("Navi Enemy", "gNavi_Enemy_Inner_", navi_enemy_i_col, ImVec4(255, 255, 0, 255), false);
        SohImGui::Tooltip("Inner color for Navi (when Navi fly around Enemies or Bosses)");
        Table_NextCol();
        SohImGui::EnhancementColor("Navi Enemy", "gNavi_Enemy_Outer_", navi_enemy_o_col, ImVec4(220, 155, 0, 255), false);
        SohImGui::Tooltip("Outer color for Navi (when Navi fly around Enemies or Bosses)");
        Table_NextLine();
        SohImGui::EnhancementColor("Navi Prop", "gNavi_Prop_Inner_", navi_prop_i_col, ImVec4(0, 255, 0, 255), false);
        SohImGui::Tooltip("Inner color for Navi (when Navi fly around props (signs etc))");
        Table_NextCol();
        SohImGui::EnhancementColor("Navi Prop", "gNavi_Prop_Outer_", navi_prop_o_col, ImVec4(0, 255, 0, 255), false);
        SohImGui::Tooltip("Outer color for Navi (when Navi fly around props (signs etc))");
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors for Dogs", "gUseDogsCol");
    if (CVar_GetS32("gUseDogsCol",0) && ImGui::BeginTable("tableDogs", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Dog N.1 color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Dog N.2 color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Dog N.1", "gDog1Col", doggo1col, ImVec4(255,255,100,255), true, false, true);
        SohImGui::Tooltip("Color of the white dog");
        Table_NextCol();
        SohImGui::EnhancementColor("Dog N.2", "gDog2Col", doggo2col, ImVec4(150,100,50,255), true, false, true);
        SohImGui::Tooltip("Color of the brown dog");
        ImGui::EndTable();
    }
}
void Draw_ItemsSkills(){
    SohImGui::EnhancementCheckbox("Custom colors on tunics", "gUseTunicsCol");
    SohImGui::Tooltip("Enable/Disable custom Link's tunics colors. \nIf disabled you will have original colors for Link's tunics.");
    if (CVar_GetS32("gUseTunicsCol",0) && ImGui::BeginTable("tableTunics", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Kokiri Tunic", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Goron Tunic", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Zora Tunic", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/3);
        Table_InitHeader();
        SohImGui::EnhancementColor("Kokiri Tunic", "gTunic_Kokiri_", kokiri_col, ImVec4(30, 105, 27, 255), true, false, true);
        Table_NextCol();
        SohImGui::EnhancementColor("Goron Tunic", "gTunic_Goron_", goron_col, ImVec4(100, 20, 0, 255), true, false, true);
        Table_NextCol();
        SohImGui::EnhancementColor("Zora Tunic", "gTunic_Zora_", zora_col, ImVec4(0, 60, 100, 255), true, false, true);
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors on Arrows", "gUseArrowsCol");
    if (CVar_GetS32("gUseArrowsCol",0) && ImGui::BeginTable("tableArrows", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Primary colors##Arrows", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Arrows", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Fire Arrows", "gFireArrowCol", firearrow_col, ImVec4(255,200,0,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Fire Arrows", "gFireArrowColEnv", firearrow_colenv, ImVec4(255,0,0,255));
        Table_NextLine();
        SohImGui::EnhancementColor("Ice Arrows", "gIceArrowCol", icearrow_col, ImVec4(170,255,255,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Ice Arrows", "gIceArrowColEnv", icearrow_colenv, ImVec4(0,0,255,255));
        Table_NextLine();
        SohImGui::EnhancementColor("Light Arrows", "gLightArrowCol", lightarrow_col, ImVec4(255,255,170,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Light Arrows", "gLightArrowColEnv", lightarrow_colenv, ImVec4(255,255,0,255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors on Spells", "gUseSpellsCol");
    if (CVar_GetS32("gUseSpellsCol",0) && ImGui::BeginTable("tableSpells", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Inner colors##Spells", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Spells", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Din's Fire", "gDF_Col", df_col, ImVec4(255,200,0,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Din's Fire", "gDF_Env", df_colenv, ImVec4(255,0,0,255));
        Table_NextLine();
        SohImGui::EnhancementColor("Nayru's Love Diamond", "gNL_Diamond_Col", nl_diam_col, ImVec4(170,255,255,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Nayru's Love Diamond", "gNL_Diamond_Env", nl_diam_colenv, ImVec4(100,255,128,255));
        Table_NextLine();
        SohImGui::EnhancementColor("Nayru's Love Orb", "gNL_Orb_Col", nl_orb_col, ImVec4(170,255,255,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Nayru's Love Orb", "gNL_Orb_Env", nl_orb_colenv, ImVec4(150,255,255,255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom charged swords", "gUseChargedCol");
    if (CVar_GetS32("gUseChargedCol",0) && ImGui::BeginTable("tableChargeAtk", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Primary colors##Charge", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Charge", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Level 1 colors", "gCharged1Col", charged1_col, ImVec4(170,255,255,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Level 1 colors 2", "gCharged1ColEnv", charged1_colenv, ImVec4(0,100,255,255));
        Table_NextLine();
        SohImGui::EnhancementColor("Level 2 colors", "gCharged2Col", charged2_col, ImVec4(255,255,170,255));
        Table_NextCol();
        SohImGui::EnhancementColor("Level 2 colors 2", "gCharged2ColEnv", charged2_colenv, ImVec4(255,100,0,255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom Swords trails", "gUseTrailsCol");
    if (CVar_GetS32("gUseTrailsCol",0) && ImGui::BeginTable("tableChargeAtk", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Custom Swords trails", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("Trail colors", "gTrailCol", trailscol, ImVec4(255,255,255,255));
        SohImGui::EnhancementSliderInt("Trails duration: %dx", "##TrailsMul", "gTrailDurantion", 1, 5, "");
        ImGui::NewLine();
        ImGui::EndTable();
    }
}
void Draw_Menus(){
    if (ImGui::BeginTable("tableFileChoose", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("File Choose color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Bottom text color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("File Choose color", "gCCFileChoosePrim", fileselect_colors, ImVec4(100, 150, 255, 255), true, false, true);
        SohImGui::Tooltip("Affect the File Select menu background.");
        Table_NextCol();
        SohImGui::EnhancementColor("Bottom text color", "gCCFileChooseTextPrim", fileselect_text_colors, ImVec4(0, 100, 255, 255), true, false, true);
        SohImGui::Tooltip("Affect the File Select texts.");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tablePauseMenu", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Pages", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("Equipments", "gCCEquipmentsPrim", menu_equips_colors, ImVec4(0, 100, 255, 255), true, true);
        SohImGui::Tooltip("Affect the Equipments menu background.");
        Table_NextLine();
        SohImGui::EnhancementColor("Items", "gCCItemsPrim", menu_items_colors, ImVec4(0, 100, 255, 255), true, true);
        SohImGui::Tooltip("Affect the Select items menu background.");
        Table_NextLine();
        SohImGui::EnhancementColor("Maps", "gCCMapsPrim", menu_map_colors, ImVec4(0, 100, 255, 255), true, true);
        SohImGui::Tooltip("Affect the Map menu background.");
        Table_NextLine();
        SohImGui::EnhancementColor("Quests", "gCCQuestsPrim", menu_quest_colors, ImVec4(0, 100, 255, 255), true, true);
        SohImGui::Tooltip("Affect the Quests statut menu background.");
        Table_NextLine();
        SohImGui::EnhancementColor("Save", "gCCSavePrim", menu_save_colors, ImVec4(0, 100, 255, 255), true, true);
        SohImGui::Tooltip("Affect the Save menu background.");
        Table_NextLine();
        SohImGui::EnhancementColor("Gameover", "gCCGameoverPrim", menu_gameover_colors, ImVec4(0, 100, 255, 255), true, true);
        SohImGui::Tooltip("Affect the Gameover screen background.");
        ImGui::EndTable();
    }
}
void Draw_Placements(){
    if (ImGui::BeginTable("tableMargins", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("General margins Settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementSliderInt("Top : %dx", "##UIMARGINT", "gHUDMargin_T", -20, 20, "");
        SohImGui::EnhancementSliderInt("Left: %dx", "##UIMARGINL", "gHUDMargin_L", -25, 25, "");
        SohImGui::EnhancementSliderInt("Right: %dx", "##UIMARGINR", "gHUDMargin_R", -25, 25, "");
        SohImGui::EnhancementSliderInt("Bottom: %dx", "##UIMARGINB", "gHUDMargin_B", -20, 20, "");
        ImGui::NewLine();
        ImGui::EndTable();
    }
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::CollapsingHeader("Hearts count position")) {
        SohImGui::EnhancementCheckbox("Hearts count use margins", "gHeartsUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gHeartsCountPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gHeartsCountPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gHeartsCountPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gHeartsCountPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gHeartsCountPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##HeartCountPosY", "gHeartsPosY", -22, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##HeartCountPosX", "gHeartsPosX", -25, ImGui::GetWindowSize().x/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Magic bar position")) {
        SohImGui::EnhancementCheckbox("Magic bar use margins", "gMagicBarUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gMagicBarPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gMagicBarPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gMagicBarPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gMagicBarPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gMagicBarPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##MagicBarPosY", "gMagicBarPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##MagicBarPosX", "gMagicBarPosX", -5, ImGui::GetWindowSize().x/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("B Button position")) {
        SohImGui::EnhancementCheckbox("B Button use margins", "gBBtnUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gBBtnPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gBBtnPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gBBtnPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gBBtnPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gBBtnPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##BBtnPosY", "gBBtnPosY", 0, ImGui::GetWindowSize().y/4+50, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##BBtnPosX", "gBBtnPosX", -1, ImGui::GetWindowSize().x-50, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("A Button position")) {
        SohImGui::EnhancementCheckbox("A Button use margins", "gABtnUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gABtnPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gABtnPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gABtnPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gABtnPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gABtnPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##ABtnPosY", "gABtnPosY", -10, ImGui::GetWindowSize().y/4+50, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##ABtnPosX", "gABtnPosX", -20, ImGui::GetWindowSize().x-50, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Start Button position")) {
        SohImGui::EnhancementCheckbox("Start Button use margins", "gStartBtnUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gStartBtnPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gStartBtnPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gStartBtnPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gStartBtnPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gStartBtnPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##StartBtnPosY", "gStartBtnPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##StartBtnPosX", "gStartBtnPosX", 0, ImGui::GetWindowSize().x/2+70, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("C Button Up position")) {
        SohImGui::EnhancementCheckbox("C Button Up use margins", "gCBtnUUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gCBtnUPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gCBtnUPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gCBtnUPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gCBtnUPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gCBtnUPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnUPosY", "gCBtnUPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnUPosX", "gCBtnUPosX", 0, ImGui::GetWindowSize().x/2+70, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }

    if (ImGui::CollapsingHeader("DPad items position")) {
        SohImGui::EnhancementCheckbox("DPad items use margins", "gDPadUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gDPadPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gDPadPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gDPadPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gDPadPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gDPadPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##DPadPosY", "gDPadPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##DPadPosX", "gDPadPosX", 0, ImGui::GetWindowSize().x/2+70, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }

    if (ImGui::CollapsingHeader("C Button Down position")) {
        SohImGui::EnhancementCheckbox("C Button Down use margins", "gCBtnDUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gCBtnDPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gCBtnDPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gCBtnDPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gCBtnDPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gCBtnDPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnDPosY", "gCBtnDPosY", 0, ImGui::GetWindowSize().y/3, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnDPosX", "gCBtnDPosX", 0, ImGui::GetWindowSize().x/2+70, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("C Button Left position")) {
        SohImGui::EnhancementCheckbox("C Button Left use margins", "gCBtnLUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gCBtnLPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gCBtnLPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gCBtnLPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gCBtnLPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gCBtnLPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnLPosY", "gCBtnLPosY", 0, ImGui::GetWindowSize().y/3, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnLPosX", "gCBtnLPosX", 0, ImGui::GetWindowSize().x/2+70, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("C Button Right position")) {
        SohImGui::EnhancementCheckbox("C Button Right use margins", "gCBtnRUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gCBtnRPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gCBtnRPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gCBtnRPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gCBtnRPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gCBtnRPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnRPosY", "gCBtnRPosY", 0, ImGui::GetWindowSize().y/3, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnRPosX", "gCBtnRPosX", 0, ImGui::GetWindowSize().x/2+70, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Minimap position")) {
        SohImGui::EnhancementCheckbox("Minimap Button use margins", "gMinimapUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gMinimapPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gMinimapPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gMinimapPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        //SohImGui::EnhancementRadioButton("No anchors", "gMinimapPosType", 3); //currently bugged
        //SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gMinimapPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##MinimapPosY", "gMinimapPosY", (ImGui::GetWindowSize().y/3)*-1, ImGui::GetWindowSize().y/3, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##MinimapPosX", "gMinimapPosX", ImGui::GetWindowSize().x*-1, ImGui::GetWindowSize().x/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Small Keys counter position")) {
        SohImGui::EnhancementCheckbox("Small Keys counter use margins", "gSKCUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gSKCPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gSKCPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gSKCPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gSKCPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gSKCPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##SKCPosY", "gSKCPosY", 0, ImGui::GetWindowSize().y/3, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##SKCPosX", "gSKCPosX", -1, ImGui::GetWindowSize().x/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Rupee counter position")) {
        SohImGui::EnhancementCheckbox("Rupee counter use margins", "gRCUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gRCPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gRCPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gRCPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gRCPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gRCPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##RCPosY", "gRCPosY", -2, ImGui::GetWindowSize().y/3, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##RCPosX", "gRCPosX", -3, ImGui::GetWindowSize().x/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Carrots position")) {
        SohImGui::EnhancementCheckbox("Carrots use margins", "gCarrotsUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gCarrotsPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gCarrotsPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gCarrotsPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gCarrotsPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gCarrotsPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CarrotsPosY", "gCarrotsPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CarrotsPosX", "gCarrotsPosX", -50, ImGui::GetWindowSize().x/2+25, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Timers position")) {
        SohImGui::EnhancementCheckbox("Timers use margins", "gTimersUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gTimersPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gTimersPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gTimersPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gTimersPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gTimersPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##TimersPosY", "gTimersPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##TimersPosX", "gTimersPosX", -50, ImGui::GetWindowSize().x/2-50, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Archery Scores position")) {
        SohImGui::EnhancementCheckbox("Archery Scores use margins", "gASUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gASPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gASPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gASPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gASPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gASPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##ASPosY", "gASPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##ASPosX", "gASPosX", -50, ImGui::GetWindowSize().x/2-50, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Title cards (Maps) position")) {
        SohImGui::EnhancementCheckbox("Title cards (Maps) use margins", "gTCMUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gTCMPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gTCMPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gTCMPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gTCMPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gTCMPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##TCMPosY", "gTCMPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##TCMPosX", "gTCMPosX", -50, ImGui::GetWindowSize().x/2+10, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
    if (ImGui::CollapsingHeader("Title cards (Bosses) position")) {
        SohImGui::EnhancementCheckbox("Title cards (Bosses) use margins", "gTCBUseMargins");
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Original position", "gTCBPosType", 0);
        SohImGui::Tooltip("This will use original intended elements position.");
        SohImGui::EnhancementRadioButton("Anchor to the left", "gTCBPosType", 1);
        SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
        SohImGui::EnhancementRadioButton("Anchor to the right", "gTCBPosType", 2);
        SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
        SohImGui::EnhancementRadioButton("No anchors", "gTCBPosType", 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
        SohImGui::EnhancementRadioButton("Hidden", "gTCBPosType", 4);
        SohImGui::Tooltip("This will make your elements hidden");
        SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##TCBPosY", "gTCBPosY", 0, ImGui::GetWindowSize().y/2, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
        SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##TCBPosX", "gTCBPosX", -50, ImGui::GetWindowSize().x/2+10, "");
        SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    }
}
void Draw_HUDButtons(){
    if (ImGui::BeginTable("tableBTN_A", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Button A colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("A Buttons", "gCCABtnPrim", a_btn_colors, ImVec4(90, 90, 255, 255), true, false, true);
        SohImGui::Tooltip("A Buttons colors (Green in original GameCube)\nAffect A buttons colors on interface, in shops, messages boxes, ocarina notes and inventory cursors.");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableBTN_B", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Button B colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("B Buttons", "gCCBBtnPrim", b_btn_colors, ImVec4(0, 150, 0, 255), true, false, true);
        SohImGui::Tooltip("B Button colors (Red in original GameCube)\nAffect B button colors on interface");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableBTN_C", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Button C colors & C Cursor colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("C Buttons", "gCCCBtnPrim", c_btn_colors, ImVec4(255, 160, 0, 255), true, false, true);
        SohImGui::Tooltip("C Buttons colors (Yellowish / Oranges in originals)\nAffect C buttons colors on interface, in inventory and ocarina notes");
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("C Button use separated colors", "gCCparated");
    if (CVar_GetS32("gCCparated",0) && ImGui::BeginTable("tableBTN_CSep", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Button C individual colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("C Buttons Up", "gCCCUBtnPrim", c_btn_u_colors, ImVec4(255,160,0,255));
        SohImGui::Tooltip("C Up Buttons colors (Yellowish / Oranges in originals)\nAffect C Up buttons colors on interface, in inventory and ocarina notes");
        Table_NextLine();
        SohImGui::EnhancementColor("C Buttons Down", "gCCCDBtnPrim", c_btn_d_colors, ImVec4(255,160,0,255));
        SohImGui::Tooltip("C Down Buttons colors (Yellowish / Oranges in originals)\nAffect C Down buttons colors on interface, in inventory and ocarina notes");
        Table_NextLine();
        SohImGui::EnhancementColor("C Buttons Left", "gCCCLBtnPrim", c_btn_l_colors, ImVec4(255,160,0,255));
        SohImGui::Tooltip("C Left Buttons colors (Yellowish / Oranges in originals)\nAffect C Left buttons colors on interface, in inventory and ocarina notes");
        Table_NextLine();
        SohImGui::EnhancementColor("C Buttons Right", "gCCCRBtnPrim", c_btn_r_colors, ImVec4(255,160,0,255));
        SohImGui::Tooltip("C Right Buttons colors (Yellowish / Oranges in originals)\nAffect C Right buttons colors on interface, in inventory and ocarina notes");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableBTN_Start", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Button Start colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("Start Buttons", "gCCStartBtnPrim", start_btn_colors, ImVec4(120, 120, 120, 255), true, false, true);
        SohImGui::Tooltip("Start Button colors (gray in GameCube)\nAffect Start button colors in inventory");
        ImGui::EndTable();
    }
}
void Draw_General(){
    if (ImGui::BeginTable("tableScheme", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("N64 Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        ImGui::TableSetupColumn("Gamecube Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        ImGui::TableSetupColumn("Custom Schemes", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementRadioButton("N64 Colors", "gHudColors", 0);
        SohImGui::Tooltip("Change interface color to N64 style.");
        Table_NextCol();
        SohImGui::EnhancementRadioButton("GCN Colors", "gHudColors", 1);
        SohImGui::Tooltip("Change interface color to Gamecube style.");
        Table_NextCol();
        SohImGui::EnhancementRadioButton("Custom Colors", "gHudColors", 2);
        SohImGui::Tooltip("Change interface color to your own made style.");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableHearts", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("Hearts", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Hearts (DD)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Hearts Outline (DD)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
        Table_InitHeader();
        SohImGui::EnhancementColor("Inner normal", "gCCHeartsPrim", hearts_colors, ImVec4(255,70,50,255));
        SohImGui::Tooltip("Hearts inner color (red in original)\nAffect Normal Hearts only");
        Table_NextCol();
        SohImGui::EnhancementColor("Inner DD", "gCCDDHeartsPrim", hearts_ddi_colors, ImVec4(255,70,50,255));
        SohImGui::Tooltip("Hearts inner color (Double Defense) (red in original)\nAffect Double Defense Hearts red color");
        Table_NextCol();
        SohImGui::EnhancementColor("Outline DD", "gDDCCHeartsPrim", hearts_dd_colors, ImVec4(255,255,255,255));
        SohImGui::Tooltip("Hearts outline color (white in original)\nAffect Double Defense outline only");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableMagicmeter", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("Magic meter", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Magic meter in use", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Borders", "gCCMagicBorderNormPrim", magic_bordern_colors, ImVec4(255,255,255,255), false);
        SohImGui::Tooltip("Affect the border of the magic meter\nWhite in original game, color change only when used one time\nAdvice: Leave it white it is not working properly");
        Table_NextCol();
        SohImGui::EnhancementColor("Borders in use", "gCCMagicBorderPrim", magic_border_colors, ImVec4(255,255,255,255), false);
        SohImGui::Tooltip("Affect the border of the magic meter when being used\nWhite flash in original game");
        Table_NextLine();
        SohImGui::EnhancementColor("Main color", "gCCMagicPrim", magic_remaining_colors, ImVec4(0,200,0,255));
        SohImGui::Tooltip("Affect the magic meter color\nGreen in original game");
        Table_NextCol();
        SohImGui::EnhancementColor("Main color in use", "gCCMagicUsePrim", magic_use_colors, ImVec4(250,250,0,255));
        SohImGui::Tooltip("Affect the magic meter when being used\nYellow in original game");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableMinimapCol", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Minimap colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("Overworlds", "gCCMinimapPrim", minimap_colors, ImVec4(0, 255, 255, 255));
        SohImGui::Tooltip("Affect the Overworld minimaps.");
        Table_NextLine();
        SohImGui::EnhancementColor("Dungeons", "gCCMinimapDGNPrim", dgn_minimap_colors, ImVec4(100, 255, 255, 255));
        SohImGui::Tooltip("Affect the Dungeon minimaps.");
        Table_NextLine();
        SohImGui::EnhancementColor("Current position arrow", "gCCMinimapCPPrim", cp_minimap_colors, ImVec4(200, 255, 0, 255));
        SohImGui::Tooltip("Affect the current position arrow on the minimap (yellow).");
        Table_NextLine();
        SohImGui::EnhancementColor("Last entrance arrow", "gCCMinimapLEPrim", le_minimap_colors, ImVec4(200, 0, 0, 255));
        SohImGui::Tooltip("Affect the last entrance position arrow on the minimap (red).");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableTitleCards", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("Overworld colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Bosses colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        SohImGui::EnhancementColor("Unified colors select", "gCCTC_OW_U_Prim", tc_ou_colors, ImVec4(255, 255, 255, 255), false);
        SohImGui::Tooltip("Affect all the overworld title cards color, white by default.");
        Table_NextCol();
        SohImGui::EnhancementColor("Unified colors select", "gCCTC_B_U_Prim", tc_bu_colors, ImVec4(255, 255, 255, 255), false);
        SohImGui::Tooltip("Affect all the bosses title cards color, white by default.");
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("tableMiscHudCol", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("Misc HUD colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementColor("Rupee icon", "gCCRupeePrim", rupee_colors, ImVec4(200, 255, 100, 255));
        SohImGui::Tooltip("Affect the Rupee icon on interface\nGreen by default.");
        Table_NextLine();
        SohImGui::EnhancementColor("Small Keys icon", "gCCKeysPrim", smolekey_colors, ImVec4(200, 230, 255, 255));
        SohImGui::Tooltip("Affect the Small keys icon on interface\nGray by default.");
        ImGui::EndTable();
    }
}
void DrawCosmeticsEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gCosmeticsEditorEnabled", 0);
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Cosmetics Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("CosmeticsContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("General")) {
            Draw_General();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("HUD Buttons")) {
            Draw_HUDButtons();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Placements")) {
            Draw_Placements();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Items & Skills")) {
            Draw_ItemsSkills();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Npcs")) {
            Draw_Npcs();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Menus")) {
            Draw_Menus();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
void InitCosmeticsEditor() {
    //Draw the bar in the menu.
    SohImGui::AddWindow("Cosmetics", "Cosmetics Editor", DrawCosmeticsEditor);
}
