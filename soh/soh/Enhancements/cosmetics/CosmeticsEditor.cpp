#include "CosmeticsEditor.h"
#include "../../util.h"
#include "../libultraship/ImGuiImpl.h"
#include "GameSettings.h"

#include <array>
#include <bit>
#include <map>
#include <string>
#include <Cvar.h>
#include <PR/ultra64/types.h>

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
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x-60);
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

void DrawUseMarginsSlider(const std::string ElementName, const std::string CvarName){
    std::string CvarLabel = CvarName + "UseMargins";
    std::string Label = ElementName + " use margins";
    SohImGui::EnhancementCheckbox(Label.c_str(), CvarLabel.c_str());
    SohImGui::Tooltip("Using this allow you move the element with General margins sliders");
}
void DrawPositionsRadioBoxes(const std::string CvarName, bool NoAnchorEnabled = true){
    std::string CvarLabel = CvarName + "PosType";
    SohImGui::EnhancementRadioButton("Original position", CvarLabel.c_str(), 0);
    SohImGui::Tooltip("This will use original intended elements position");
    SohImGui::EnhancementRadioButton("Anchor to the left", CvarLabel.c_str(), 1);
    SohImGui::Tooltip("This will make your elements follow the left side of your game window");
    SohImGui::EnhancementRadioButton("Anchor to the right", CvarLabel.c_str(), 2);
    SohImGui::Tooltip("This will make your elements follow the right side of your game window");
    if (NoAnchorEnabled) {
        SohImGui::EnhancementRadioButton("No anchors", CvarLabel.c_str(), 3);
        SohImGui::Tooltip("This will make your elements to not follow any side\nBetter used for center elements");
    }
    SohImGui::EnhancementRadioButton("Hidden", CvarLabel.c_str(), 4);
    SohImGui::Tooltip("This will make your elements hidden");
}
void DrawPositionSlider(const std::string CvarName, int MinY, int MaxY, int MinX, int MaxX){
    std::string PosXCvar = CvarName+"PosX";
    std::string PosYCvar = CvarName+"PosY";
    std::string InvisibleLabelX = "##"+PosXCvar;
    std::string InvisibleLabelY = "##"+PosYCvar;
    SohImGui::EnhancementSliderInt("Up <-> Down : %d", InvisibleLabelY.c_str(), PosYCvar.c_str(), MinY, MaxY, "", 0, true);
    SohImGui::Tooltip("This slider is used to move Up and Down your elements.");
    SohImGui::EnhancementSliderInt("Left <-> Right : %d", InvisibleLabelX.c_str(), PosXCvar.c_str(), MinX, MaxX, "", 0, true);
    SohImGui::Tooltip("This slider is used to move Left and Right your elements.");
}
void DrawScaleSlider(const std::string CvarName,float DefaultValue){
    std::string InvisibleLabel = "##"+CvarName;
    std::string CvarLabel = CvarName+"Scale";
    SohImGui::EnhancementSliderFloat("Scale : %dx", InvisibleLabel.c_str(), CvarLabel.c_str(), 0.1f, 3.0f,"",DefaultValue,true,true);
}
void DrawColorSection(CosmeticsColorSection* ColorSection, int SectionSize) {
    for (s16 i = 0; i < SectionSize; i++) {
        CosmeticsColorIndividual* ThisElement = ColorSection[i].Element;
        const std::string Tooltip = ThisElement->ToolTip;
        const std::string Name = ThisElement->Name;
        const std::string Cvar = ThisElement->CvarName;
        ImVec4 ModifiedColor = ThisElement->ModifiedColor;
        ImVec4 DefaultColor = ThisElement->DefaultColor;
        bool canRainbow = ThisElement->canRainbow;
        bool hasAlpha = ThisElement->hasAlpha;
        bool sameLine = ThisElement->sameLine;
        bool Nextcol = ColorSection[i].Nextcol;
        bool NextLine = ColorSection[i].NextLine;
        if (Nextcol){
            Table_NextCol();
        }
        if (NextLine){
            Table_NextLine();
        }
        Draw_HelpIcon(Tooltip.c_str());
        SohImGui::EnhancementColor(Name.c_str(), Cvar.c_str(), ModifiedColor, DefaultColor, canRainbow, hasAlpha, sameLine);
    }
}
void Draw_Npcs(){
    SohImGui::EnhancementCheckbox("Custom colors for Navi", "gUseNaviCol");
    SohImGui::Tooltip("Enable/Disable custom Navi colors\nIf disabled, default colors will be used\nColors go into effect when Navi goes back into your pockets");
    if (CVar_GetS32("gUseNaviCol",0) && ImGui::BeginTable("tableNavi", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Inner colors##Navi", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Navi", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Navi_Section, SECTION_SIZE(Navi_Section));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors for Keese", "gUseKeeseCol");
    SohImGui::Tooltip("Enable/Disable custom Keese element colors\nIf disabled, default element colors will be used\nColors go into effect when Keese respawn (or when the room is reloaded)");
    if (CVar_GetS32("gUseKeeseCol",0) && ImGui::BeginTable("tableKeese", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Fire colors##Keese", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Ice colors##Keese", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader(false);
        DrawColorSection(Keese_Section, SECTION_SIZE(Keese_Section));
        ImGui::EndTable();
    }
    SohImGui::EnhancementCheckbox("Custom colors for Dogs", "gUseDogsCol");
    SohImGui::Tooltip("Enable/Disable custom colors for the two Dog variants\nIf disabled, default colors will be used");
    if (CVar_GetS32("gUseDogsCol",0) && ImGui::BeginTable("tableDogs", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Dog N.1 color", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Dog N.2 color", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Dogs_Section, SECTION_SIZE(Dogs_Section));
        ImGui::EndTable();
    }
}
void Draw_ItemsSkills(){
    SohImGui::EnhancementCheckbox("Custom tunics color", "gUseTunicsCol");
    SohImGui::Tooltip("Enable/Disable custom Link's tunics colors\nIf disabled you will have original colors for Link's tunics.");
    if (CVar_GetS32("gUseTunicsCol",0) && ImGui::BeginTable("tableTunics", 3, FlagsTable)) {
        ImGui::TableSetupColumn("Kokiri Tunic", FlagsCell, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Goron Tunic", FlagsCell, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Zora Tunic", FlagsCell, TablesCellsWidth/3);
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
    if (CVar_GetS32("gUseArrowsCol",0) && ImGui::BeginTable("tableArrows", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Primary colors##Arrows", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Arrows", FlagsCell, TablesCellsWidth/2);
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
    if (CVar_GetS32("gUseSpellsCol",0) && ImGui::BeginTable("tableSpells", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Inner colors##Spells", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Spells", FlagsCell, TablesCellsWidth/2);
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
    if (CVar_GetS32("gUseChargedCol",0) && ImGui::BeginTable("tableChargeAtk", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Primary colors##Charge", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Charge", FlagsCell, TablesCellsWidth/2);
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
    if (CVar_GetS32("gUseTrailsCol",0) && ImGui::BeginTable("tabletrails", 1, FlagsTable)) {
        ImGui::TableSetupColumn("Custom Trails", FlagsCell, TablesCellsWidth);
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
        if (ImGui::BeginTable("tableFileChoose", 2, FlagsTable)) {
            ImGui::TableSetupColumn("File Choose color", FlagsCell, TablesCellsWidth/2);
            ImGui::TableSetupColumn("Bottom text color", FlagsCell, TablesCellsWidth/2);
            Table_InitHeader();
            Draw_HelpIcon("Affects the File Select menu background.");
            SohImGui::EnhancementColor("File Choose color", "gCCFileChoosePrim", fileselect_colors, ImVec4(100, 150, 255, 255), true, false, true);
            Table_NextCol();
            Draw_HelpIcon("Affects the File Select texts.");
            SohImGui::EnhancementColor("Bottom text color", "gCCFileChooseTextPrim", fileselect_text_colors, ImVec4(100, 255, 255, 255), true, false, true);
            ImGui::EndTable();
        }
        /*
        if (ImGui::BeginTable("tablePauseMenu", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Kaleido pages (Non working atm)", FlagsCell, TablesCellsWidth);
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
    if (ImGui::BeginTable("tableMargins", 1, FlagsTable)) {
        ImGui::TableSetupColumn("General margins settings", FlagsCell, TablesCellsWidth);
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
        if (ImGui::BeginTable("tableHeartsCounts", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Hearts counts settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Hearts counts", "gHearts");
            DrawPositionsRadioBoxes("gHeartsCount");
            DrawPositionSlider("gHeartsCount",-22,ImGui::GetWindowViewport()->Size.y,-25,ImGui::GetWindowViewport()->Size.x);
            DrawScaleSlider("gHeartsCount",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Magic Meter position")) {
        if (ImGui::BeginTable("tablemmpos", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Magic meter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Magic meter", "gMagicBar");
            DrawPositionsRadioBoxes("gMagicBar");
            DrawPositionSlider("gMagicBar", 0, ImGui::GetWindowViewport()->Size.y/2, -5, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gMagicBar",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVar_GetS32("gVisualAgony",0) && ImGui::CollapsingHeader("Visual stone of agony position")) {
        if (ImGui::BeginTable("tabledvisualstoneofagony", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Visual stone of agony settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Visual stone of agony", "gVSOA");
            DrawPositionsRadioBoxes("gVSOA");
            s16 Min_X_VSOA = 0;
            s16 Max_X_VSOA = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gVSOAPosType",0) == 2){
                Max_X_VSOA = 290;
            } else if(CVar_GetS32("gVSOAPosType",0) == 4){
                Min_X_VSOA = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gVSOA", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_VSOA, Max_X_VSOA);
            DrawScaleSlider("gVSOA",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("B Button position")) {
        if (ImGui::BeginTable("tablebbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("B Button settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("B Button", "gBBtn");
            DrawPositionsRadioBoxes("gBBtn");
            DrawPositionSlider("gBBtn", 0, ImGui::GetWindowViewport()->Size.y/4+50, -1, ImGui::GetWindowViewport()->Size.x-50);
            DrawScaleSlider("gBBtn",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("A Button position")) {
        if (ImGui::BeginTable("tableabtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("A Button settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("A Button", "gABtn");
            DrawPositionsRadioBoxes("gABtn");
            DrawPositionSlider("gABtn", -10, ImGui::GetWindowViewport()->Size.y/4+50, -20, ImGui::GetWindowViewport()->Size.x-50);
            DrawScaleSlider("gABtn",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Start Button position")) {
        if (ImGui::BeginTable("tablestartbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Start Button settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Start Button", "gStartBtn");
            DrawPositionsRadioBoxes("gStartBtn");
            DrawPositionSlider("gStartBtn", 0, ImGui::GetWindowViewport()->Size.y/2, 0, ImGui::GetWindowViewport()->Size.x/2+70);
            DrawScaleSlider("gStartBtn",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Up position")) {
        if (ImGui::BeginTable("tablecubtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Up settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Up", "gCBtnU");
            DrawPositionsRadioBoxes("gCBtnU");
            s16 Min_X_CU = 0;
            s16 Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnUPosType",0) == 2){
                Max_X_CU = 294;
            } else if(CVar_GetS32("gCBtnUPosType",0) == 3){
                Max_X_CU = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnUPosType",0) == 4){
                Min_X_CU = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnU", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CU, Max_X_CU);
            DrawScaleSlider("gCBtnU",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Down position")) {
        if (ImGui::BeginTable("tablecdbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Down settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Down", "gCBtnD");
            DrawPositionsRadioBoxes("gCBtnD");
            s16 Min_X_CD = 0;
            s16 Max_X_CD = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnDPosType",0) == 2){
                Max_X_CD = 294;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 3){
                Max_X_CD = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnDPosType",0) == 4){
                Min_X_CD = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnD", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CD, Max_X_CD);
            DrawScaleSlider("gCBtnD",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Left position")) {
        if (ImGui::BeginTable("tableclbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Left settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Left", "gCBtnL");
            DrawPositionsRadioBoxes("gCBtnL");
            s16 Min_X_CL = 0;
            s16 Max_X_CL = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnLPosType",0) == 2){
                Max_X_CL = 294;
            } else if(CVar_GetS32("gCBtnLPosType",0) == 3){
                Max_X_CL = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnLPosType",0) == 4){
                Min_X_CL = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnL", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CL, Max_X_CL);
            DrawScaleSlider("gCBtnL",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("C Button Right position")) {
        if (ImGui::BeginTable("tablecrnbtn", 1, FlagsTable)) {
            ImGui::TableSetupColumn("C Button Right settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("C Button Right", "gCBtnR");
            DrawPositionsRadioBoxes("gCBtnR");
            s16 Min_X_CR = 0;
            s16 Max_X_CR = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gCBtnRPosType",0) == 2){
                Max_X_CR = 294;
            } else if(CVar_GetS32("gCBtnRPosType",0) == 3){
                Max_X_CR = ImGui::GetWindowViewport()->Size.x/2;
            } else if(CVar_GetS32("gCBtnRPosType",0) == 4){
                Min_X_CR = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gCBtnR", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_CR, Max_X_CR);
            DrawScaleSlider("gCBtnR",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (CVar_GetS32("gDpadEquips",0) && ImGui::CollapsingHeader("DPad items position")) {
        if (ImGui::BeginTable("tabledpaditems", 1, FlagsTable)) {
            ImGui::TableSetupColumn("DPad items settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("DPad items", "gDPad");
            DrawPositionsRadioBoxes("gDPad");
            s16 Min_X_Dpad = 0;
            s16 Max_X_Dpad = ImGui::GetWindowViewport()->Size.x/2;
            if(CVar_GetS32("gDPadPosType",0) == 2){
                Max_X_Dpad = 290;
            } else if(CVar_GetS32("gDPadPosType",0) == 4){
                Min_X_Dpad = (ImGui::GetWindowViewport()->Size.x/2)*-1;
            }
            DrawPositionSlider("gDPad", 0, ImGui::GetWindowViewport()->Size.y/2, Min_X_Dpad, Max_X_Dpad);
            DrawScaleSlider("gDPad",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Minimaps position")) {
        if (ImGui::BeginTable("tableminimapspos", 1, FlagsTable)) {
            ImGui::TableSetupColumn("minimaps settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Minimap", "gMinimap");
            DrawPositionsRadioBoxes("gMinimap", false);
            DrawPositionSlider("gMinimap", (ImGui::GetWindowViewport()->Size.y/3)*-1, ImGui::GetWindowViewport()->Size.y/3, ImGui::GetWindowViewport()->Size.x*-1, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gMinimap",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Small Keys counter position")) {
        if (ImGui::BeginTable("tablesmolekeys", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Small Keys counter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Small Keys counter", "gSKC");
            DrawPositionsRadioBoxes("gSKC");
            DrawPositionSlider("gSKC", 0, ImGui::GetWindowViewport()->Size.y/3, -1, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gSKC",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Rupee counter position")) {
        if (ImGui::BeginTable("tablerupeecount", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Rupee counter settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Rupee counter", "gRC");
            DrawPositionsRadioBoxes("gRC");
            DrawPositionSlider("gRC", -2, ImGui::GetWindowViewport()->Size.y/3, -3, ImGui::GetWindowViewport()->Size.x/2);
            DrawScaleSlider("gRC",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Carrots position")) {
        if (ImGui::BeginTable("tableCarrots", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Carrots settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Carrots", "gCarrots");
            DrawPositionsRadioBoxes("gCarrots");
            DrawPositionSlider("gCarrots", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+25);
            DrawScaleSlider("gCarrots",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Timers position")) {
        if (ImGui::BeginTable("tabletimers", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Timers settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Timers", "gTimers");
            DrawPositionsRadioBoxes("gTimers");
            DrawPositionSlider("gTimers", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2-50);
            DrawScaleSlider("gTimers",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Archery Scores position")) {
        if (ImGui::BeginTable("tablearchery", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Archery Scores settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Archery scores", "gAS");
            DrawPositionsRadioBoxes("gAS", false);
            DrawPositionSlider("gAS", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2-50);
            DrawScaleSlider("gAS",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Title cards (Maps) position")) {
        if (ImGui::BeginTable("tabletcmaps", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Titlecard maps settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Title cards (overworld)", "gTCM");
            DrawPositionsRadioBoxes("gTCM");
            DrawPositionSlider("gTCM", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+10);
            DrawScaleSlider("gTCM",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Title cards (Bosses) position")) {
        if (ImGui::BeginTable("tabletcbosses", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Title cards (Bosses) settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Title cards (Bosses)", "gTCB");
            DrawPositionsRadioBoxes("gTCB");
            DrawPositionSlider("gTCB", 0, ImGui::GetWindowViewport()->Size.y/2, -50, ImGui::GetWindowViewport()->Size.x/2+10);
            DrawScaleSlider("gTCB",1.0f);
            ImGui::NewLine();
            ImGui::EndTable();
        }
    }
}
void Draw_HUDButtons(){
    if (CVar_GetS32("gHudColors",0) ==2 ){
        if (ImGui::CollapsingHeader("A Button colors & A Cursors")) {
            if (ImGui::BeginTable("tableBTN_A", 1, FlagsTable)) {
                ImGui::TableSetupColumn("A Button colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the A button colors (and various cursors that use the same theme)", false);
                SohImGui::EnhancementColor("A Buttons", "gCCABtnPrim", a_btn_colors, ImVec4(0, 200, 50, 255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("B Button color")) {
            if (ImGui::BeginTable("tableBTN_B", 1, FlagsTable)) {
                ImGui::TableSetupColumn("B button color", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the B button color", false);
                SohImGui::EnhancementColor("B Button", "gCCBBtnPrim", b_btn_colors, ImVec4(255, 30, 30, 255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("C-Buttons & C Cursor")) {
            if (ImGui::BeginTable("tableBTN_C", 1, FlagsTable)) {
                ImGui::TableSetupColumn("Button C colors & C Cursor colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the C Buttons' color (if not using separate colors)\nAnd various cursor that use C-Buttons colors", false);
                SohImGui::EnhancementColor("C-Buttons", "gCCCBtnPrim", c_btn_colors, ImVec4(255, 160, 0, 255), true, false, true);
                ImGui::EndTable();
            }
            SohImGui::EnhancementCheckbox("C-Buttons use separate colors", "gCCparated");
            if (CVar_GetS32("gCCparated",0) && ImGui::CollapsingHeader("C Button individual colors")) {
                if (ImGui::BeginTable("tableBTN_CSep", 1, FlagsTable)) {
                    ImGui::TableSetupColumn("C-Buttons individual colors", FlagsCell, TablesCellsWidth);
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
            if (ImGui::BeginTable("tableBTN_Start", 1, FlagsTable)) {
                ImGui::TableSetupColumn("Start button colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                Draw_HelpIcon("Affects the Start button color", false);
                SohImGui::EnhancementColor("Start Buttons", "gCCStartBtnPrim", start_btn_colors, ImVec4(200, 0, 0, 255), true, false, true);
                ImGui::EndTable();
            }
        }
        if (CVar_GetS32("gDpadEquips",0) && ImGui::CollapsingHeader("DPad colors")) {
            if (ImGui::BeginTable("tableDpadHud", 1, FlagsTable)) {
                ImGui::TableSetupColumn("DPad color", FlagsCell, TablesCellsWidth);
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
    if (ImGui::BeginTable("tableScheme", 3, FlagsTable | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("N64 Scheme", FlagsCell, TablesCellsWidth);
        ImGui::TableSetupColumn("GameCube Scheme", FlagsCell, TablesCellsWidth);
        ImGui::TableSetupColumn("Custom Schemes", FlagsCell, TablesCellsWidth);
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
            if (ImGui::BeginTable("tableHearts", 3, FlagsTable | ImGuiTableFlags_Hideable)) {
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
            if (ImGui::BeginTable("tableMagicmeter", 2, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Magic meter", FlagsCell, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Magic meter in use", FlagsCell, TablesCellsWidth/2);
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
            if (ImGui::BeginTable("tableMinimapCol", 1, FlagsTable)) {
                ImGui::TableSetupColumn("Minimap color", FlagsCell, TablesCellsWidth);
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
            if (ImGui::BeginTable("tableTitleCards", 2, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Title cards Overworld", FlagsCell, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Title cards Bosses", FlagsCell, TablesCellsWidth/2);
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
            if (ImGui::BeginTable("tableMiscHudCol", 1, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Misc HUD colors", FlagsCell, TablesCellsWidth);
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
    ImGui::SetNextWindowSize(ImVec2(470, 430), ImGuiCond_FirstUseEver);
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
        if (ImGui::BeginTabItem("Placements & Scale")) {
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
    SohImGui::AddWindow("Cosmetics", "Rainbowfunction", LoadRainbowColor, true, true);
    //Draw the bar in the menu.
    SohImGui::AddWindow("Cosmetics", "Cosmetics Editor", DrawCosmeticsEditor);
}