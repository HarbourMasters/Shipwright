#include "CosmeticsEditor.h"
#include "../libultraship/ImGuiImpl.h"

#include <string>
#include <Cvar.h>
#include <PR/ultra64/types.h>

/**
 * Colors variables
 */
float TablesCellsWidth = 300.0f; //1 Col
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
ImVec4 Keese1_primcol;
ImVec4 Keese2_primcol;
ImVec4 Keese1_envcol;
ImVec4 Keese2_envcol;
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
ImVec4 tc_ou_colors;
ImVec4 tc_bu_colors;
ImVec4 dpad_colors;
ImVec4 visualagony_colors;
/*ImVec4 menu_equips_colors;
ImVec4 menu_items_colors;
ImVec4 menu_map_colors;
ImVec4 menu_quest_colors;
ImVec4 menu_save_colors;
ImVec4 menu_gameover_colors;*/
const char* RainbowColorCvarList[] = {
    //This is the list of possible CVars that has rainbow effect.
    "gTunic_Kokiri_", "gTunic_Goron_", "gTunic_Zora_",
    "gFireArrowCol", "gIceArrowCol", "gTunic_Zora_",
    "gFireArrowColEnv", "gIceArrowColEnv", "gLightArrowColEnv",
    "gCCHeartsPrim", "gDDCCHeartsPrim", "gLightArrowCol", "gCCDDHeartsPrim",
    "gCCABtnPrim", "gCCBBtnPrim", "gCCCBtnPrim", "gCCStartBtnPrim",
    "gCCCUBtnPrim", "gCCCLBtnPrim", "gCCCRBtnPrim", "gCCCDBtnPrim", "gCCDpadPrim",
    "gCCMagicBorderNormPrim", "gCCMagicBorderPrim", "gCCMagicPrim", "gCCMagicUsePrim",
    "gCCMinimapPrim", "gCCMinimapDGNPrim", "gCCMinimapCPPrim", "gCCMinimapLEPrim",
    "gCCRupeePrim", "gCCKeysPrim", "gDog1Col", "gDog2Col", "gCCVSOAPrim",
    "gKeese1_Ef_Prim","gKeese2_Ef_Prim","gKeese1_Ef_Env","gKeese2_Ef_Env",
    "gDF_Col", "gDF_Env",
    "gNL_Diamond_Col", "gNL_Diamond_Env", "gNL_Orb_Col", "gNL_Orb_Env",
    "gTrailCol", "gCharged1Col", "gCharged1ColEnv", "gCharged2Col", "gCharged2ColEnv",
    "gCCFileChoosePrim", "gCCFileChooseTextPrim", "gCCEquipmentsPrim", "gCCItemsPrim",
    "gCCMapsPrim", "gCCQuestsPrim", "gCCSavePrim", "gCCGameoverPrim",
};
const char* MarginCvarList[] {
    "gHearts", "gMagicBar", "gVSOA", "gBBtn", "gABtn", "gStartBtn", 
    "gCBtnU", "gCBtnD", "gCBtnL", "gCBtnR", "gDPad", "gMinimap", 
    "gSKC", "gRC", "gCarrots",  "gTimers", "gAS", "gTCM", "gTCB"
};

void SetMarginAll(const char* ButtonName, bool SetActivated) {
    if (ImGui::Button(ButtonName)) {
        u8 arrayLength = sizeof(MarginCvarList) / sizeof(*MarginCvarList);
        //MarginCvarNonAnchor is an array that list every element that has No anchor by default, because if that the case this function will not touch it with pose type 0.
        const char* MarginCvarNonAnchor[] { "gCarrots", "gTimers", "gAS", "gTCM","gTCB" };
        u8 arrayLengthNonMargin = sizeof(MarginCvarNonAnchor) / sizeof(*MarginCvarNonAnchor);
        for (u8 s = 0; s < arrayLength; s++) {
            std::string cvarName = MarginCvarList[s];
            std::string cvarPosType = cvarName+"PosType";
            std::string cvarNameMargins = cvarName+"UseMargins";
            if (CVar_GetS32(cvarPosType.c_str(),0) <= 2 && SetActivated) { //Our element is not Hidden or Non anchor
                for(int i = 0; i < arrayLengthNonMargin; i++){
                    if(MarginCvarNonAnchor[i] == cvarName && CVar_GetS32(cvarPosType.c_str(),0) == 0){ //Our element is both in original position and do not have anchor by default so we skip it.
                        CVar_SetS32(cvarNameMargins.c_str(), false); //force set off
                    } else if(MarginCvarNonAnchor[i] == cvarName && CVar_GetS32(cvarPosType.c_str(),0) != 0){ //Our element is not in original position regarless it has no anchor by default since player made it anchored we can toggle margins
                        CVar_SetS32(cvarNameMargins.c_str(), SetActivated);
                    } else if(MarginCvarNonAnchor[i] != cvarName){ //Our elements has an anchor by default so regarless of it's position right now that okay to toggle margins.
                        CVar_SetS32(cvarNameMargins.c_str(), SetActivated);
                    }
                }
            } else { //Since the user requested to turn all margin off no need to do any check there.
                CVar_SetS32(cvarNameMargins.c_str(), SetActivated);
            }
        }
    }
}
void ResetPositionAll() {
    if (ImGui::Button("Reset all positions")) {
        u8 arrayLength = sizeof(MarginCvarList) / sizeof(*MarginCvarList);
        for (u8 s = 0; s < arrayLength; s++) {
            std::string cvarName = MarginCvarList[s];
            std::string cvarPosType = cvarName+"PosType";
            std::string cvarNameMargins = cvarName+"UseMargins";
            CVar_SetS32(cvarPosType.c_str(), 0);
            CVar_SetS32(cvarNameMargins.c_str(), false); //Turn margin off to everythings as that original position.
        }
    }
}
void LoadRainbowColor(bool& open) {
    u8 arrayLength = sizeof(RainbowColorCvarList) / sizeof(*RainbowColorCvarList);
    for (u8 s = 0; s < arrayLength; s++) {
        std::string cvarName = RainbowColorCvarList[s];
        std::string Cvar_Red = cvarName;
        Cvar_Red += "R";
        std::string Cvar_Green = cvarName;
        Cvar_Green += "G";
        std::string Cvar_Blue = cvarName;
        Cvar_Blue += "B";
        std::string Cvar_RBM = cvarName;
        Cvar_RBM += "RBM";
        std::string RBM_HUE = cvarName;
        RBM_HUE += "Hue";
        f32 Canon = 10.f * s;
        ImVec4 NewColor;
        const f32 deltaTime = 1.0f / ImGui::GetIO().Framerate;
        f32 hue = CVar_GetFloat(RBM_HUE.c_str(), 0.0f);
        f32 newHue = hue + CVar_GetS32("gColorRainbowSpeed", 1) * 36.0f * deltaTime;
        if (newHue >= 360)
            newHue = 0;
        CVar_SetFloat(RBM_HUE.c_str(), newHue);
        f32 current_hue = CVar_GetFloat(RBM_HUE.c_str(), 0);
        u8 i = current_hue / 60 + 1;
        u8 a = (-current_hue / 60.0f + i) * 255;
        u8 b = (current_hue / 60.0f + (1 - i)) * 255;

        switch (i) {
        case 1: NewColor.x = 255; NewColor.y = b; NewColor.z = 0; break;
        case 2: NewColor.x = a; NewColor.y = 255; NewColor.z = 0; break;
        case 3: NewColor.x = 0; NewColor.y = 255; NewColor.z = b; break;
        case 4: NewColor.x = 0; NewColor.y = a; NewColor.z = 255; break;
        case 5: NewColor.x = b; NewColor.y = 0; NewColor.z = 255; break;
        case 6: NewColor.x = 255; NewColor.y = 0; NewColor.z = a; break;
        }

        if (CVar_GetS32(Cvar_RBM.c_str(), 0) != 0) {
            CVar_SetS32(Cvar_Red.c_str(), SohImGui::ClampFloatToInt(NewColor.x, 0, 255));
            CVar_SetS32(Cvar_Green.c_str(), SohImGui::ClampFloatToInt(NewColor.y, 0, 255));
            CVar_SetS32(Cvar_Blue.c_str(), SohImGui::ClampFloatToInt(NewColor.z, 0, 255));
        }
    }
}

void Table_InitHeader(bool has_header = true) {
    if (has_header) {
        ImGui::TableHeadersRow();
    }
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding(); //This is to adjust Vertical pos of item in a cell to be normlized.
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2);
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

void Draw_HelpIcon(const std::string& helptext, bool sameline = true, int Pos = 0) {
    // place the ? button to the most of the right side of the cell it is using.
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x-60);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 15);
    ImGui::SmallButton("?");
    SohImGui::Tooltip(helptext.c_str());
    if (sameline) {
        //I do not use ImGui::SameLine(); because it make some element vanish.
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 22);
    }
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
}
void Draw_Npcs(){
    SohImGui::EnhancementCheckbox("Custom colors for Navi", "gUseNaviCol");
    SohImGui::Tooltip("Enable/Disable custom Navi colors\nIf disabled, default colors will be used\nColors go into effect when Navi goes back into your pockets");
    if (CVar_GetS32("gUseNaviCol",0) && ImGui::BeginTable("tableNavi", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Inner colors##Navi", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Navi", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        Draw_HelpIcon("Inner color for Navi (idle flying around)");
        SohImGui::EnhancementColor("Navi Idle (Primary)", "gNavi_Idle_Inner", navi_idle_i_col, ImVec4(255, 255, 255, 255), false);
        Table_NextCol();
        Draw_HelpIcon("Outer color for Navi (idle flying around)");
        SohImGui::EnhancementColor("Navi Idle (Secondary)", "gNavi_Idle_Outer", navi_idle_o_col, ImVec4(0, 0, 255, 255), false);
        Table_NextLine();
        Draw_HelpIcon("Inner color for Navi (when Navi fly around NPCs)");
        SohImGui::EnhancementColor("Navi NPC (Primary)", "gNavi_NPC_Inner", navi_npc_i_col, ImVec4(150, 150, 255, 255), false);
        Table_NextCol();
        Draw_HelpIcon("Outer color for Navi (when Navi fly around NPCs)");
        SohImGui::EnhancementColor("Navi NPC (Secondary)", "gNavi_NPC_Outer", navi_npc_o_col, ImVec4(150, 150, 255, 255), false);
        Table_NextLine();
        Draw_HelpIcon("Inner color for Navi (when Navi fly around Enemies or Bosses)");
        SohImGui::EnhancementColor("Navi Enemy", "gNavi_Enemy_Inner", navi_enemy_i_col, ImVec4(255, 255, 0, 255), false);
        Table_NextCol();
        Draw_HelpIcon("Outer color for Navi (when Navi fly around Enemies or Bosses)");
        SohImGui::EnhancementColor("Navi Enemy (Secondary)", "gNavi_Enemy_Outer", navi_enemy_o_col, ImVec4(220, 155, 0, 255), false);
        Table_NextLine();
        Draw_HelpIcon("Inner color for Navi (when Navi fly around props (signs etc))");
        SohImGui::EnhancementColor("Navi Prop (Primary)", "gNavi_Prop_Inner", navi_prop_i_col, ImVec4(0, 255, 0, 255), false);
        Table_NextCol();
        Draw_HelpIcon("Outer color for Navi (when Navi fly around props (signs etc))");
        SohImGui::EnhancementColor("Navi Prop (Secondary)", "gNavi_Prop_Outer", navi_prop_o_col, ImVec4(0, 255, 0, 255), false);
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors for Keese", "gUseKeeseCol");
    SohImGui::Tooltip("Enable/Disable custom Keese element colors\nIf disabled, default element colors will be used\nColors go into effect when Keese respawn (or when the room is reloaded)");
    if (CVar_GetS32("gUseKeeseCol",0) && ImGui::BeginTable("tableKeese", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Fire colors##Keese", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Ice colors##Keese", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader(false);
        Draw_HelpIcon("Affects the primary color of the Fire itself of the Keese");
        SohImGui::EnhancementColor("Fire Primary color", "gKeese1_Ef_Prim", Keese1_primcol, ImVec4(255, 255, 100, 255));
        Table_NextCol();
        Draw_HelpIcon("Affects the primary color of the Ice itself of the Keese");
        SohImGui::EnhancementColor("Ice Primary color", "gKeese2_Ef_Prim", Keese2_primcol, ImVec4(100, 200, 255, 255));
        Table_NextLine();
        Draw_HelpIcon("Affects the secondary color of the Fire itself of the Keese");
        SohImGui::EnhancementColor("Fire Secondary color", "gKeese1_Ef_Env", Keese1_envcol, ImVec4(255, 50, 0, 255));
        Table_NextCol();
        Draw_HelpIcon("Affects the secondary color of the Ice itself of the Keese");
        SohImGui::EnhancementColor("Ice Secondary color", "gKeese2_Ef_Env", Keese2_envcol, ImVec4(0, 0, 255, 255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors for Dogs", "gUseDogsCol");
    SohImGui::Tooltip("Enable/Disable custom colors for the two Dog variants\nIf disabled, default colors will be used");
    if (CVar_GetS32("gUseDogsCol",0) && ImGui::BeginTable("tableDogs", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Dog N.1 color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Dog N.2 color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        Draw_HelpIcon("Affects the colors of the white dog");
        SohImGui::EnhancementColor("Dog white", "gDog1Col", doggo1col, ImVec4(255,255,200,255), true, false, true);
        Table_NextCol();
        Draw_HelpIcon("Affects the colors of the brown dog");
        SohImGui::EnhancementColor("Dog brown", "gDog2Col", doggo2col, ImVec4(150,100,50,255), true, false, true);
        ImGui::EndTable();
    }
}
void Draw_ItemsSkills(){
    SohImGui::EnhancementCheckbox("Custom tunics color", "gUseTunicsCol");
    SohImGui::Tooltip("Enable/Disable custom Link's tunics colors\nIf disabled you will have original colors for Link's tunics.");
    if (CVar_GetS32("gUseTunicsCol",0) && ImGui::BeginTable("tableTunics", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Kokiri Tunic", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Goron Tunic", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Zora Tunic", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/3);
        Table_InitHeader();
        Draw_HelpIcon("Affects Kokiri Tunic color", false);
        SohImGui::EnhancementColor("Kokiri Tunic", "gTunic_Kokiri_", kokiri_col, ImVec4(30, 105, 27, 255), true, false, true);
        Table_NextCol();
        Draw_HelpIcon("Affects Goron Tunic color", false);
        SohImGui::EnhancementColor("Goron Tunic", "gTunic_Goron_", goron_col, ImVec4(100, 20, 0, 255), true, false, true);
        Table_NextCol();
        Draw_HelpIcon("Affects Zora Tunic color", false);
        SohImGui::EnhancementColor("Zora Tunic", "gTunic_Zora_", zora_col, ImVec4(0, 60, 100, 255), true, false, true);
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom arrows colors", "gUseArrowsCol");
    if (CVar_GetS32("gUseArrowsCol",0) && ImGui::BeginTable("tableArrows", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Primary colors##Arrows", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Arrows", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Fire Arrows (primary)", "gFireArrowCol", firearrow_col, ImVec4(255,200,0,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Fire Arrows", "gFireArrowColEnv", firearrow_colenv, ImVec4(255,0,0,255));
        Table_NextLine();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Ice Arrows (primary)", "gIceArrowCol", icearrow_col, ImVec4(170,255,255,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Ice Arrows", "gIceArrowColEnv", icearrow_colenv, ImVec4(0,0,255,255));
        Table_NextLine();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Light Arrows (primary)", "gLightArrowCol", lightarrow_col, ImVec4(255,255,170,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Light Arrows", "gLightArrowColEnv", lightarrow_colenv, ImVec4(255,255,0,255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom spells colors", "gUseSpellsCol");
    if (CVar_GetS32("gUseSpellsCol",0) && ImGui::BeginTable("tableSpells", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Inner colors##Spells", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Spells", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Din's Fire (primary)", "gDF_Col", df_col, ImVec4(255,200,0,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Din's Fire", "gDF_Env", df_colenv, ImVec4(255,0,0,255));
        Table_NextLine();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Nayru's Love Diamond (primary)", "gNL_Diamond_Col", nl_diam_col, ImVec4(170,255,255,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Nayru's Love Diamond", "gNL_Diamond_Env", nl_diam_colenv, ImVec4(100,255,128,255));
        Table_NextLine();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Nayru's Love Orb (primary)", "gNL_Orb_Col", nl_orb_col, ImVec4(170,255,255,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Nayru's Love Orb", "gNL_Orb_Env", nl_orb_colenv, ImVec4(150,255,255,255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom spin attack colors", "gUseChargedCol");
    if (CVar_GetS32("gUseChargedCol",0) && ImGui::BeginTable("tableChargeAtk", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Primary colors##Charge", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Charge", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
        Table_InitHeader();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Level 1 color (primary)", "gCharged1Col", charged1_col, ImVec4(170,255,255,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Level 1 color", "gCharged1ColEnv", charged1_colenv, ImVec4(0,100,255,255));
        Table_NextLine();
        Draw_HelpIcon("Affects Primary color");
        SohImGui::EnhancementColor("Level 2 color (primary)", "gCharged2Col", charged2_col, ImVec4(255,255,170,255));
        Table_NextCol();
        Draw_HelpIcon("Affects Secondary color");
        SohImGui::EnhancementColor("Level 2 color", "gCharged2ColEnv", charged2_colenv, ImVec4(255,100,0,255));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom trails color", "gUseTrailsCol");
    if (CVar_GetS32("gUseTrailsCol",0) && ImGui::BeginTable("tabletrails", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Custom Trails", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        Draw_HelpIcon("Affects Swords slash, boomerang and Bombchu trails color");
        SohImGui::EnhancementColor("Trails color", "gTrailCol", trailscol, ImVec4(255,255,255,255));
        SohImGui::EnhancementSliderInt("Trails duration: %dx", "##TrailsMul", "gTrailDurantion", 1, 5, "");
        SohImGui::Tooltip("The longer the trails the weirder it become");
        ImGui::NewLine();
        ImGui::EndTable();
    }
}
void Draw_Menus(){
    if (CVar_GetS32("gHudColors",0) ==2 ){
        if (ImGui::BeginTable("tableFileChoose", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("File Choose color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
            ImGui::TableSetupColumn("Bottom text color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
            Table_InitHeader();
            Draw_HelpIcon("Affects the File Select menu background.");
            SohImGui::EnhancementColor("File Choose color", "gCCFileChoosePrim", fileselect_colors, ImVec4(100, 150, 255, 255), true, false, true);
            Table_NextCol();
            Draw_HelpIcon("Affects the File Select texts.");
            SohImGui::EnhancementColor("Bottom text color", "gCCFileChooseTextPrim", fileselect_text_colors, ImVec4(100, 255, 255, 255), true, false, true);
            ImGui::EndTable();
        }
        /*
        if (ImGui::BeginTable("tablePauseMenu", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Kaleido pages (Non working atm)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader();
            Draw_HelpIcon("Affect the Equipments menu background.");
            SohImGui::EnhancementColor("Equipments", "gCCEquipmentsPrim", menu_equips_colors, ImVec4(0, 100, 255, 255), true, true);
            Table_NextLine();
            Draw_HelpIcon("Affect the Select items menu background.");
            SohImGui::EnhancementColor("Items", "gCCItemsPrim", menu_items_colors, ImVec4(0, 100, 255, 255), true, true);
            Table_NextLine();
            Draw_HelpIcon("Affect the Map menu background.");
            SohImGui::EnhancementColor("Maps", "gCCMapsPrim", menu_map_colors, ImVec4(0, 100, 255, 255), true, true);
            Table_NextLine();
            Draw_HelpIcon("Affect the Quests statut menu background.");
            SohImGui::EnhancementColor("Quests", "gCCQuestsPrim", menu_quest_colors, ImVec4(0, 100, 255, 255), true, true);
            Table_NextLine();
            Draw_HelpIcon("Affect the Save menu background.");
            SohImGui::EnhancementColor("Save", "gCCSavePrim", menu_save_colors, ImVec4(0, 100, 255, 255), true, true);
            Table_NextLine();
            Draw_HelpIcon("Affect the Gameover screen background.");
            SohImGui::EnhancementColor("Gameover", "gCCGameoverPrim", menu_gameover_colors, ImVec4(0, 100, 255, 255), true, true);
            ImGui::EndTable();
        }
        */
    } else {
        ImGui::Text("To modify menus colors you need \"Custom Colors\" scheme\nto be selected in \"General\" tab.\nOnce enabled you will be able to modify the following colors:\nFile Choose color\nBottom text color");
    }
}
void Draw_Placements(){
    if (ImGui::BeginTable("tableMargins", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("General margins settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        SohImGui::EnhancementSliderInt("Top : %dx", "##UIMARGINT", "gHUDMargin_T", (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0, true);
        SohImGui::EnhancementSliderInt("Left: %dx", "##UIMARGINL", "gHUDMargin_L", -25, ImGui::GetWindowViewport()->Size.x, "", 0, true);
        SohImGui::EnhancementSliderInt("Right: %dx", "##UIMARGINR", "gHUDMargin_R", (ImGui::GetWindowViewport()->Size.x)*-1, 25, "", 0, true);
        SohImGui::EnhancementSliderInt("Bottom: %dx", "##UIMARGINB", "gHUDMargin_B", (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0, true);
        SetMarginAll("All margins on",true);
        SohImGui::Tooltip("Set most of the element to use margin\nSome elements with default position will not be affected\nElements without Archor or Hidden will not be turned on");
        ImGui::SameLine();
        SetMarginAll("All margins off",false);
        SohImGui::Tooltip("Set all of the element to not use margin");
        ImGui::SameLine();
        ResetPositionAll();
        SohImGui::Tooltip("Revert every element to use their original position and no margins");
        ImGui::NewLine();
        ImGui::EndTable();
    }
    if (ImGui::CollapsingHeader("Hearts count position")) {
        if (ImGui::BeginTable("tableHeartsCounts", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Hearts counts settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##HeartCountPosY", "gHeartsPosY", -22, ImGui::GetWindowViewport()->Size.y, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##HeartCountPosX", "gHeartsPosX", -25, ImGui::GetWindowViewport()->Size.x, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Magic Meter position")) {
        if (ImGui::BeginTable("tablemmpos", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Magic meter settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
            SohImGui::EnhancementCheckbox("Magic meter use margins", "gMagicBarUseMargins");
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##MagicBarPosY", "gMagicBarPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##MagicBarPosX", "gMagicBarPosX", -5, ImGui::GetWindowViewport()->Size.x/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVar_GetS32("gVisualAgony",0) && ImGui::CollapsingHeader("Visual stone of agony position")) {
        if (ImGui::BeginTable("tabledvisualstoneofagony", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Visual stone of agony settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
            SohImGui::EnhancementCheckbox("Visual stone of agony use margins", "gVSOAUseMargins");
            SohImGui::Tooltip("This will use original intended elements position.");
            SohImGui::EnhancementRadioButton("Original position", "gVSOAPosType", 0);
            SohImGui::Tooltip("This will use original intended elements position.");
            SohImGui::EnhancementRadioButton("Anchor to the left", "gVSOAPosType", 1);
            SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
            SohImGui::EnhancementRadioButton("Anchor to the right", "gVSOAPosType", 2);
            SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
            SohImGui::EnhancementRadioButton("No anchors", "gVSOAPosType", 3);
            SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
            SohImGui::EnhancementRadioButton("Hidden", "gVSOAPosType", 4); //in case you want only SFX
            SohImGui::Tooltip("This will make your elements hidden");
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##VSOAPosY", "gVSOAPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            s16 Min_X_Dpad = 0;
            s16 Max_X_Dpad = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gVSOAPosType",0) == 2){
                Max_X_Dpad = 290;
            } else if(CVar_GetS32("gVSOAPosType",0) == 4){
                Min_X_Dpad = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##VSOAPosX", "gVSOAPosX", Min_X_Dpad, Max_X_Dpad, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("B Button position")) {
        if (ImGui::BeginTable("tablebbtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("B Button settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##BBtnPosY", "gBBtnPosY", 0, ImGui::GetWindowViewport()->Size.y/4+50, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##BBtnPosX", "gBBtnPosX", -1, ImGui::GetWindowViewport()->Size.x-50, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("A Button position")) {
        if (ImGui::BeginTable("tableabtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("A Button settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##ABtnPosY", "gABtnPosY", -10, ImGui::GetWindowViewport()->Size.y/4+50, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##ABtnPosX", "gABtnPosX", -20, ImGui::GetWindowViewport()->Size.x-50, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Start Button position")) {
        if (ImGui::BeginTable("tablestartbtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Start Button settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##StartBtnPosY", "gStartBtnPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##StartBtnPosX", "gStartBtnPosX", 0, ImGui::GetWindowViewport()->Size.x/2+70, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Up position")) {
        if (ImGui::BeginTable("tablecubtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("C Button Up settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnUPosY", "gCBtnUPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            s16 Min_X_CU = 0;
            s16 Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnUPosType",0) == 2){
                Max_X_CU = 294;
            } else if(CVar_GetS32("gCBtnUPosType",0) == 3){
                Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnUPosType",0) == 4){
                Min_X_CU = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnUPosX", "gCBtnUPosX", Min_X_CU, Max_X_CU, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Down position")) {
        if (ImGui::BeginTable("tablecdbtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("C Button Down settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnDPosY", "gCBtnDPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            s16 Min_X_CD = 0;
            s16 Max_X_CD = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnDPosType",0) == 2){
                Max_X_CD = 294;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 3){
                Max_X_CD = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 4){
                Min_X_CD = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnDPosX", "gCBtnDPosX", Min_X_CD, Max_X_CD, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Left position")) {
        if (ImGui::BeginTable("tableclbtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("C Button Left settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnLPosY", "gCBtnLPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            s16 Min_X_CL = 0;
            s16 Max_X_CL = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnDPosType",0) == 2){
                Max_X_CL = 294;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 3){
                Max_X_CL = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 4){
                Min_X_CL = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnLPosX", "gCBtnLPosX", Min_X_CL, Max_X_CL, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Right position")) {
        if (ImGui::BeginTable("tablecrnbtn", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("C Button Right settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CBtnRPosY", "gCBtnRPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            s16 Min_X_CR = 0;
            s16 Max_X_CR = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnRPosType",0) == 2){
                Max_X_CR = 294;
            } else if(CVar_GetS32("gCBtnRPosType",0) == 3){
                Max_X_CR = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnRPosType",0) == 4){
                Min_X_CR = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CBtnRPosX", "gCBtnRPosX", Min_X_CR, Max_X_CR, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVar_GetS32("gDpadEquips",0) && ImGui::CollapsingHeader("DPad items position")) {
        if (ImGui::BeginTable("tabledpaditems", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("DPad items settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##DPadPosY", "gDPadPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            s16 Min_X_Dpad = 0;
            s16 Max_X_Dpad = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gDPadPosType",0) == 2){
                Max_X_Dpad = 290;
            } else if(CVar_GetS32("gDPadPosType",0) == 4){
                Min_X_Dpad = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##DPadPosX", "gDPadPosX", Min_X_Dpad, Max_X_Dpad, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Minimaps position")) {
        if (ImGui::BeginTable("tableminimapspos", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("minimaps settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##MinimapPosY", "gMinimapPosY", (ImGui::GetWindowViewport()->Size.y/3)*-1, ImGui::GetWindowViewport()->Size.y/3, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##MinimapPosX", "gMinimapPosX", ImGui::GetWindowViewport()->Size.x*-1, ImGui::GetWindowViewport()->Size.x/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Small Keys counter position")) {
        if (ImGui::BeginTable("tablesmolekeys", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Small Keys counter settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##SKCPosY", "gSKCPosY", 0, ImGui::GetWindowViewport()->Size.y/3, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##SKCPosX", "gSKCPosX", -1, ImGui::GetWindowViewport()->Size.x/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Rupee counter position")) {
        if (ImGui::BeginTable("tablerupeecount", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Rupee counter settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##RCPosY", "gRCPosY", -2, ImGui::GetWindowViewport()->Size.y/3, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##RCPosX", "gRCPosX", -3, ImGui::GetWindowViewport()->Size.x/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Carrots position")) {
        if (ImGui::BeginTable("tableCarrots", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Carrots settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##CarrotsPosY", "gCarrotsPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##CarrotsPosX", "gCarrotsPosX", -50, ImGui::GetWindowViewport()->Size.x/2+25, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Timers position")) {
        if (ImGui::BeginTable("tabletimers", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Timers settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##TimersPosY", "gTimersPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##TimersPosX", "gTimersPosX", -50, ImGui::GetWindowViewport()->Size.x/2-50, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Archery Scores position")) {
        if (ImGui::BeginTable("tablearchery", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Archery Scores settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
            SohImGui::EnhancementCheckbox("Archery Scores use margins", "gASUseMargins");
            SohImGui::Tooltip("This will use original intended elements position.");
            SohImGui::EnhancementRadioButton("Original position", "gASPosType", 0);
            SohImGui::Tooltip("This will use original intended elements position.");
            SohImGui::EnhancementRadioButton("Anchor to the left", "gASPosType", 1);
            SohImGui::Tooltip("This will make your elements follow the left side of your game window.");
            SohImGui::EnhancementRadioButton("Anchor to the right", "gASPosType", 2);
            SohImGui::Tooltip("This will make your elements follow the right side of your game window.");
            //SohImGui::EnhancementRadioButton("No anchors", "gASPosType", 3); //currently bugged
            //SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements.");
            SohImGui::EnhancementRadioButton("Hidden", "gASPosType", 4);
            SohImGui::Tooltip("This will make your elements hidden");
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##ASPosY", "gASPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##ASPosX", "gASPosX", -50, ImGui::GetWindowViewport()->Size.x/2-50, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Title cards (Maps) position")) {
        if (ImGui::BeginTable("tabletcmaps", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Titlecard maps settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##TCMPosY", "gTCMPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##TCMPosX", "gTCMPosX", -50, ImGui::GetWindowViewport()->Size.x/2+10, ""), true;
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Title cards (Bosses) position")) {
        if (ImGui::BeginTable("tabletcbosses", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
            ImGui::TableSetupColumn("Title cards (Bosses) settings", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
            Table_InitHeader(false);
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
            SohImGui::EnhancementSliderInt("Up <-> Down : %d", "##TCBPosY", "gTCBPosY", 0, ImGui::GetWindowViewport()->Size.y/2, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
            SohImGui::EnhancementSliderInt("Left <-> Right : %d", "##TCBPosX", "gTCBPosX", -50, ImGui::GetWindowViewport()->Size.x/2+10, "", 0, true);
            SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
}
void Draw_HUDButtons(){
    if (CVar_GetS32("gHudColors",0) ==2 ){
        if (ImGui::CollapsingHeader("A Button colors & A Cursors")) {
            if (ImGui::BeginTable("tableBTN_A", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("A Button colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the A button colors (and various cursors that use the same theme)", false);
                SohImGui::EnhancementColor("A Buttons", "gCCABtnPrim", a_btn_colors, ImVec4(0, 200, 50, 255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("B Button color")) {
            if (ImGui::BeginTable("tableBTN_B", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("B button color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the B button color", false);
                SohImGui::EnhancementColor("B Button", "gCCBBtnPrim", b_btn_colors, ImVec4(255, 30, 30, 255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("C-Buttons & C Cursor")) {
            if (ImGui::BeginTable("tableBTN_C", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Button C colors & C Cursor colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the C Buttons' color (if not using separate colors)\nAnd various cursor that use C-Buttons colors", false);
                SohImGui::EnhancementColor("C-Buttons", "gCCCBtnPrim", c_btn_colors, ImVec4(255, 160, 0, 255), true, false, true);
                ImGui::EndTable();
            }
            SohImGui::EnhancementCheckbox("C-Buttons use separate colors", "gCCparated");
            if (CVar_GetS32("gCCparated",0) && ImGui::CollapsingHeader("C Button individual colors")) {
                if (ImGui::BeginTable("tableBTN_CSep", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("C-Buttons individual colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                    Table_InitHeader(false);
                    Draw_HelpIcon("Affects C-Buttons Up colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top");
                    SohImGui::EnhancementColor("C Buttons Up", "gCCCUBtnPrim", c_btn_u_colors, ImVec4(255,160,0,255));
                    Table_NextLine();
                    Draw_HelpIcon("Affects C-Buttons Down colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top");
                    SohImGui::EnhancementColor("C Buttons Down", "gCCCDBtnPrim", c_btn_d_colors, ImVec4(255,160,0,255));
                    Table_NextLine();
                    Draw_HelpIcon("Affects C-Buttons Left colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top");
                    SohImGui::EnhancementColor("C Buttons Left", "gCCCLBtnPrim", c_btn_l_colors, ImVec4(255,160,0,255));
                    Table_NextLine();
                    Draw_HelpIcon("Affects C-Buttons Right colors, but not C cursor colors\nTo edit C Cursor check C-Buttons color on top");
                    SohImGui::EnhancementColor("C Buttons Right", "gCCCRBtnPrim", c_btn_r_colors, ImVec4(255,160,0,255));
                    ImGui::EndTable();
                }
            }
        }
        if (ImGui::CollapsingHeader("Start button colors")) {
            if (ImGui::BeginTable("tableBTN_Start", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Start button colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the Start button color", false);
                SohImGui::EnhancementColor("Start Buttons", "gCCStartBtnPrim", start_btn_colors, ImVec4(200, 0, 0, 255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (CVar_GetS32("gDpadEquips",0) && ImGui::CollapsingHeader("DPad colors")) {
            if (ImGui::BeginTable("tableDpadHud", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("DPad color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("DPad background color, White is the default value");
                SohImGui::EnhancementColor("DPad background color", "gCCDpadPrim", dpad_colors, ImVec4(255, 255, 255, 255));
                ImGui::EndTable();
            }
        }
    } else {
        ImGui::Text("To modify buttons colors you need \"Custom Colors\" scheme\nto be selected in \"General\" tab.\nOnce enabled you will be able to modify the following colors:\nA Button colors,\nB Button colors,\nC Button colors & cursor,\nStart button\nDPad icon");
    }
}
void Draw_General(){
    if (ImGui::BeginTable("tableScheme", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("N64 Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        ImGui::TableSetupColumn("GameCube Scheme", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        ImGui::TableSetupColumn("Custom Schemes", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
        Table_InitHeader();
        Draw_HelpIcon("Change interface color to N64 style");
        SohImGui::EnhancementRadioButton("N64 Colors", "gHudColors", 0);
        Table_NextCol();
        Draw_HelpIcon("Change interface color to GameCube style");
        SohImGui::EnhancementRadioButton("GCN Colors", "gHudColors", 1);
        Table_NextCol();
        Draw_HelpIcon("Lets you change every interface color to your liking");
        SohImGui::EnhancementRadioButton("Custom Colors", "gHudColors", 2);
        ImGui::EndTable();
    }
    if (CVar_GetS32("gHudColors",0) ==2 ){
        if (ImGui::CollapsingHeader("Hearts colors")) {
            SohImGui::Tooltip("Hearts colors in general\nDD stand for Double Defense");
            if (ImGui::BeginTable("tableHearts", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Hearts (normal)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
                ImGui::TableSetupColumn("Hearts (DD)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
                ImGui::TableSetupColumn("Hearts Outline (DD)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
                Table_InitHeader();
                Draw_HelpIcon("Affects the inner color", false);
                SohImGui::EnhancementColor("Inner normal", "gCCHeartsPrim", hearts_colors, ImVec4(255,70,50,255), true, false, true);
                Table_NextCol();
                Draw_HelpIcon("Affects the inner color", false);
                SohImGui::EnhancementColor("Inner DD", "gCCDDHeartsPrim", hearts_ddi_colors, ImVec4(255,70,50,255), true, false, true);
                Table_NextCol();
                Draw_HelpIcon("Affects the outline color of hearts when you have Double Defense\nWhite is the default value", false);
                SohImGui::EnhancementColor("Outline DD", "gDDCCHeartsPrim", hearts_dd_colors, ImVec4(255,255,255,255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Magic Meter colors")) {
            if (ImGui::BeginTable("tableMagicmeter", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Magic meter", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Magic meter in use", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
                Table_InitHeader();
                Draw_HelpIcon("Affects the border of the magic meter\nWhite is the default value, color change only when used one time");
                SohImGui::EnhancementColor("Borders", "gCCMagicBorderNormPrim", magic_bordern_colors, ImVec4(255,255,255,255), false);
                Table_NextCol();
                Draw_HelpIcon("Affects the border of the magic meter when being used\nWhite is the default value");
                SohImGui::EnhancementColor("Borders in use", "gCCMagicBorderPrim", magic_border_colors, ImVec4(255,255,255,255), false);
                Table_NextLine();
                Draw_HelpIcon("Affects the magic meter color\nGreen is the default value");
                SohImGui::EnhancementColor("Main color", "gCCMagicPrim", magic_remaining_colors, ImVec4(0,200,0,255));
                Table_NextCol();
                Draw_HelpIcon("Affects the magic meter when being used\nYellow is the default value");
                SohImGui::EnhancementColor("Main color in use", "gCCMagicUsePrim", magic_use_colors, ImVec4(250,250,0,255));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Minimap and compass colors")) {
            if (ImGui::BeginTable("tableMinimapCol", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Minimap color", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader();
                Draw_HelpIcon("Affects the Overworld minimaps");
                SohImGui::EnhancementColor("Overworlds", "gCCMinimapPrim", minimap_colors, ImVec4(0, 255, 255, 255));
                Table_NextLine();
                Draw_HelpIcon("Affects the Dungeon minimaps");
                SohImGui::EnhancementColor("Dungeons", "gCCMinimapDGNPrim", dgn_minimap_colors, ImVec4(100, 255, 255, 255));
                Table_NextLine();
                Draw_HelpIcon("Affects the current position arrow on the minimap\nYellow is the default value");
                SohImGui::EnhancementColor("Current position arrow", "gCCMinimapCPPrim", cp_minimap_colors, ImVec4(200, 255, 0, 255));
                Table_NextLine();
                Draw_HelpIcon("Affects the last entrance position arrow on the minimap\nRed is the default value");
                SohImGui::EnhancementColor("Last entrance arrow", "gCCMinimapLEPrim", le_minimap_colors, ImVec4(200, 0, 0, 255));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Title cards colors")) {
            if (ImGui::BeginTable("tableTitleCards", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Title cards Overworld", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Title cards Bosses", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth/2);
                Table_InitHeader();
                Draw_HelpIcon("Affects all the overworld title cards color, white is the default value");
                SohImGui::EnhancementColor("Main color", "gCCTC_OW_U_Prim", tc_ou_colors, ImVec4(255, 255, 255, 255), false);
                Table_NextCol();
                Draw_HelpIcon("Affects all the bosses title cards color, white is the default value");
                SohImGui::EnhancementColor("Main color", "gCCTC_B_U_Prim", tc_bu_colors, ImVec4(255, 255, 255, 255), false);
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Misc. interface colors")) {
            if (ImGui::BeginTable("tableMiscHudCol", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Misc HUD colors", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, TablesCellsWidth);
                Table_InitHeader();
                Draw_HelpIcon("Affects the Rupee icon on interface\nGreen is the default value");
                SohImGui::EnhancementColor("Rupee icon", "gCCRupeePrim", rupee_colors, ImVec4(200, 255, 100, 255));
                Table_NextLine();
                Draw_HelpIcon("Affects the Small keys icon on interface\nGray is the default value");
                SohImGui::EnhancementColor("Small Keys icon", "gCCKeysPrim", smolekey_colors, ImVec4(200, 230, 255, 255));
                Table_NextLine();
                Draw_HelpIcon("Affects the Stone of Agony icon on interface\nWhite is the default value");
                SohImGui::EnhancementColor("Stone of agony icon", "gCCVSOAPrim", visualagony_colors, ImVec4(255, 255, 255, 255));
                ImGui::EndTable();
            }
        }
    } else {
        ImGui::Text("To modify some generic colors you need \"Custom Colors\" scheme\nto be selected in \"General\" tab.\nOnce enabled you will be able to modify the following colors :\nHearts,\nMagic Meter,\nMinimap & compass,\nTitle card\nKeys & Rupee icons\nStone of Agony (Visual)");
    }
}
void DrawCosmeticsEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gCosmeticsEditorEnabled", 0);
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(465, 430), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Cosmetics Editor", &open)) {
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
        if (ImGui::BeginTabItem("Items & Skills")) {
            Draw_ItemsSkills();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("NPCs")) {
            Draw_Npcs();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Menus")) {
            Draw_Menus();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Placements")) {
            Draw_Placements();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
void InitCosmeticsEditor() {
    //This allow to hide a window without disturbing the player nor adding things in menu
    //LoadRainbowColor() will this way run in background once it's window is activated
    //ImGui::SetNextItemWidth(0.0f);
    SohImGui::AddWindow("Enhancements", "Rainbowfunction", LoadRainbowColor, true, true);
    //Draw the bar in the menu.
    SohImGui::AddWindow("Enhancements", "Cosmetics Editor", DrawCosmeticsEditor);
}
