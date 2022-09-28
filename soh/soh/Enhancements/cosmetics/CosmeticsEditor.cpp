#include "CosmeticsEditor.h"
#include <libultraship/ImGuiImpl.h>

#include <string>
#include <libultraship/Cvar.h>
#include <random>
#include <algorithm>
#include <ultra64/types.h>

#include "../../UIWidgets.hpp"

const char* RainbowColorCvarList[] = {
    //This is the list of possible CVars that has rainbow effect.
    "gTunic_Kokiri", "gTunic_Goron", "gTunic_Zora",
    "gFireArrowCol", "gIceArrowCol",
    "gNormalArrowCol", "gNormalArrowColEnv",
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
    "gSwordTrailTopCol", "gSwordTrailBottomCol", "gBoomTrailStartCol", "gBoomTrailEndCol", "gBombTrailCol",
    "gKSwordTrailTopCol", "gKSwordTrailBottomCol","gMSwordTrailTopCol", "gMSwordTrailBottomCol","gBSwordTrailTopCol", "gBSwordTrailBottomCol",
    "gStickTrailTopCol", "gStickTrailBottomCol","gHammerTrailTopCol", "gHammerTrailBottomCol",
    "gCharged1Col", "gCharged1ColEnv", "gCharged2Col", "gCharged2ColEnv",
    "gCCFileChoosePrim", "gCCFileChooseTextPrim", "gCCEquipmentsPrim", "gCCItemsPrim",
    "gCCMapsPrim", "gCCQuestsPrim", "gCCSavePrim", "gCCGameoverPrim"
};
const char* MarginCvarList[] {
    "gHearts", "gHeartsCount", "gMagicBar", "gVSOA", "gBBtn", "gABtn", "gStartBtn", 
    "gCBtnU", "gCBtnD", "gCBtnL", "gCBtnR", "gDPad", "gMinimap", 
    "gSKC", "gRC", "gCarrots",  "gTimers", "gAS", "gTCM", "gTCB"
};

ImVec4 GetRandomValue(int MaximumPossible){
    ImVec4 NewColor;
    unsigned long range = 255 - 0;
#if !defined(__SWITCH__) && !defined(__WIIU__)
    std::random_device rd;
    std::mt19937 rng(rd());
#else
    size_t seed = std::hash<std::string>{}(std::to_string(rand()));
    std::mt19937_64 rng(seed);
#endif
    std::uniform_int_distribution<int> dist(0, 255 - 1);
    
    NewColor.x = (float)(dist(rng)) / 255;
    NewColor.y = (float)(dist(rng)) / 255;
    NewColor.z = (float)(dist(rng)) / 255;
    return NewColor;
}
void GetRandomColorRGB(CosmeticsColorSection* ColorSection, int SectionSize){
#if defined(__SWITCH__) || defined(__WIIU__)
    srand(time(NULL));
#endif
    for (int i = 0; i < SectionSize; i++){
        CosmeticsColorIndividual* Element = ColorSection[i].Element;
        ImVec4 colors = Element->ModifiedColor;
        Color_RGBA8 NewColors = { 0, 0, 0, 255 };
        std::string cvarName = Element->CvarName;
        std::string Cvar_RBM = cvarName + "RBM";
        colors = RANDOMIZE_32(255);
        NewColors.r = fmin(fmax(colors.x * 255, 0), 255);
        NewColors.g = fmin(fmax(colors.y * 255, 0), 255);
        NewColors.b = fmin(fmax(colors.z * 255, 0), 255);
        Element->ModifiedColor = colors;
        CVar_SetRGBA(cvarName.c_str(), NewColors);
        CVar_SetS32(Cvar_RBM.c_str(), 0);
    }
}
void GetDefaultColorRGB(CosmeticsColorSection* ColorSection, int SectionSize){
    for (int i = 0; i < SectionSize; i++){
        CosmeticsColorIndividual* Element = ColorSection[i].Element;
        ImVec4 colors = Element->ModifiedColor;
        ImVec4 defaultcolors = Element->DefaultColor;
        std::string cvarName = Element->CvarName;
        std::string Cvar_RBM = cvarName + "RBM";
        colors.x = defaultcolors.x;
        colors.y = defaultcolors.y;
        colors.z = defaultcolors.z;
        if (Element->hasAlpha) { colors.w = defaultcolors.w; };
        Element->ModifiedColor = colors;
        Color_RGBA8 colorsRGBA;
        colorsRGBA.r = defaultcolors.x;
        colorsRGBA.g = defaultcolors.y;
        colorsRGBA.b = defaultcolors.z;
        if (Element->hasAlpha) { colorsRGBA.a = defaultcolors.w; };
        CVar_SetRGBA(cvarName.c_str(), colorsRGBA);
        CVar_SetS32(Cvar_RBM.c_str(), 0); //On click disable rainbow mode.

    }
}
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

void ResetTrailLength(const char* variable, int value) {
    if (ImGui::Button("Reset")) {
        CVar_SetS32(variable, value);
        }
    }

void LoadRainbowColor(bool& open) {
    u8 arrayLength = sizeof(RainbowColorCvarList) / sizeof(*RainbowColorCvarList);
    for (u8 s = 0; s < arrayLength; s++) {
        std::string cvarName = RainbowColorCvarList[s];
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
        Color_RGBA8 NewColorRGB = {
            fmin(fmax(NewColor.x, 0), 255),
            fmin(fmax(NewColor.y, 0), 255),
            fmin(fmax(NewColor.z, 0), 255),
            255
        };
        if (CVar_GetS32(Cvar_RBM.c_str(), 0) != 0) {
            CVar_SetRGBA(cvarName.c_str(), NewColorRGB);
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
    UIWidgets::Tooltip(helptext.c_str());
    if (sameline) {
        //I do not use ImGui::SameLine(); because it make some element vanish.
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 22);
    }
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
}
void DrawUseMarginsSlider(const std::string ElementName, const std::string CvarName){
    std::string CvarLabel = CvarName + "UseMargins";
    std::string Label = ElementName + " use margins";
    UIWidgets::EnhancementCheckbox(Label.c_str(), CvarLabel.c_str());
    UIWidgets::Tooltip("Using this allow you move the element with General margins sliders");
}
void DrawPositionsRadioBoxes(const std::string CvarName, bool NoAnchorEnabled = true){
    std::string CvarLabel = CvarName + "PosType";
    UIWidgets::EnhancementRadioButton("Original position", CvarLabel.c_str(), 0);
    UIWidgets::Tooltip("This will use original intended elements position");
    UIWidgets::EnhancementRadioButton("Anchor to the left", CvarLabel.c_str(), 1);
    UIWidgets::Tooltip("This will make your elements follow the left side of your game window");
    UIWidgets::EnhancementRadioButton("Anchor to the right", CvarLabel.c_str(), 2);
    UIWidgets::Tooltip("This will make your elements follow the right side of your game window");
    if (NoAnchorEnabled) {
        UIWidgets::EnhancementRadioButton("No anchors", CvarLabel.c_str(), 3);
        UIWidgets::Tooltip("This will make your elements to not follow any side\nBetter used for center elements");
    }
    UIWidgets::EnhancementRadioButton("Hidden", CvarLabel.c_str(), 4);
    UIWidgets::Tooltip("This will make your elements hidden");
}
void DrawTransitions(const std::string CvarName){
    UIWidgets::EnhancementRadioButton("Really slow fade (white)", CvarName.c_str(), 8);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Really slow fade (black)", CvarName.c_str(), 7);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Slow fade (white)", CvarName.c_str(), 10);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Slow fade (black)", CvarName.c_str(), 9);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Normal fade (white)", CvarName.c_str(), 3);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Normal fade (black)", CvarName.c_str(), 2);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Fast fade (white)", CvarName.c_str(), 5);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Fast fade (black)", CvarName.c_str(), 4);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Fast circle (white)", CvarName.c_str(), 40);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Normal circle (black)", CvarName.c_str(), 32);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Slow circle (white)", CvarName.c_str(), 41);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Slow circle (black)", CvarName.c_str(), 33);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Fast noise circle (white)", CvarName.c_str(), 42);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Fast noise circle (black)", CvarName.c_str(), 34);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Slow noise circle (white)", CvarName.c_str(), 43);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Slow noise circle (black)", CvarName.c_str(), 35);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Normal waves circle (white)", CvarName.c_str(), 44);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Normal waves circle (black)", CvarName.c_str(), 36);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Slow waves circle (white)", CvarName.c_str(), 45);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Slow waves circle (black)", CvarName.c_str(), 37);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Normal close circle (white)", CvarName.c_str(), 46);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Normal close circle (black)", CvarName.c_str(), 38);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Slow close circle (white)", CvarName.c_str(), 47);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Slow close circle (black)", CvarName.c_str(), 39);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Super fast circle (white)", CvarName.c_str(), 56);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Super fast circle (black)", CvarName.c_str(), 58);
    Table_NextLine();
    UIWidgets::EnhancementRadioButton("Super fast noise circle (white)", CvarName.c_str(), 57);
    Table_NextCol();
    UIWidgets::EnhancementRadioButton("Super fast noise circle (black)", CvarName.c_str(), 59);
}
void DrawPositionSlider(const std::string CvarName, int MinY, int MaxY, int MinX, int MaxX){
    std::string PosXCvar = CvarName+"PosX";
    std::string PosYCvar = CvarName+"PosY";
    std::string InvisibleLabelX = "##"+PosXCvar;
    std::string InvisibleLabelY = "##"+PosYCvar;
    UIWidgets::EnhancementSliderInt("Up <-> Down : %d", InvisibleLabelY.c_str(), PosYCvar.c_str(), MinY, MaxY, "", 0, true);
    UIWidgets::Tooltip("This slider is used to move Up and Down your elements.");
    UIWidgets::EnhancementSliderInt("Left <-> Right : %d", InvisibleLabelX.c_str(), PosXCvar.c_str(), MinX, MaxX, "", 0, true);
    UIWidgets::Tooltip("This slider is used to move Left and Right your elements.");
}
void DrawScaleSlider(const std::string CvarName,float DefaultValue){
    std::string InvisibleLabel = "##"+CvarName;
    std::string CvarLabel = CvarName+"Scale";
    //Disabled for now. feature not done and several fixes needed to be merged.
    //UIWidgets::EnhancementSliderFloat("Scale : %dx", InvisibleLabel.c_str(), CvarLabel.c_str(), 0.1f, 3.0f,"",DefaultValue,true,true);
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
        UIWidgets::EnhancementColor(Name.c_str(), Cvar.c_str(), ModifiedColor, DefaultColor, canRainbow, hasAlpha, sameLine);
    }
}
void DrawRandomizeResetButton(const std::string Identifier, CosmeticsColorSection* ColorSection, int SectionSize, bool isAllCosmetics = false){
    std::string TableName = Identifier+"_Table";
    std::string Col1Name = Identifier+"_Col1";
    std::string Col2Name = Identifier+"_Col2";
    std::string Tooltip_RNG = "Affect "+Identifier+" colors";
    std::string RNG_BtnText = "Randomize : "+Identifier;
    std::string Reset_BtnText = "Reset : "+Identifier;
    if (ImGui::BeginTable(TableName.c_str(), 2, FlagsTable)) {
        ImGui::TableSetupColumn(Col1Name.c_str(), FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn(Col2Name.c_str(), FlagsCell, TablesCellsWidth/2);
        Table_InitHeader(false);
    #ifdef __WIIU__
        if(ImGui::Button(RNG_BtnText.c_str(), ImVec2( ImGui::GetContentRegionAvail().x, 20.0f * 2.0f))){
    #else
        if(ImGui::Button(RNG_BtnText.c_str(), ImVec2( ImGui::GetContentRegionAvail().x, 20.0f))){
    #endif
            CVar_SetS32("gHudColors", 2);
            CVar_SetS32("gUseNaviCol", 1);
            CVar_SetS32("gUseKeeseCol", 1);
            CVar_SetS32("gUseDogsCol", 1);
            CVar_SetS32("gUseTunicsCol", 1);
            CVar_SetS32("gUseArrowsCol", 1);
            CVar_SetS32("gUseSpellsCol", 1);
            CVar_SetS32("gUseChargedCol", 1);
            CVar_SetS32("gUseTrailsCol", 1);
            CVar_SetS32("gCCparated", 1);
            GetRandomColorRGB(ColorSection, SectionSize);
        }
        UIWidgets::Tooltip(Tooltip_RNG.c_str());
        Table_NextCol();
    #ifdef __WIIU__
        if(ImGui::Button(Reset_BtnText.c_str(), ImVec2( ImGui::GetContentRegionAvail().x, 20.0f * 2.0f))){
    #else
        if(ImGui::Button(Reset_BtnText.c_str(), ImVec2( ImGui::GetContentRegionAvail().x, 20.0f))){
    #endif
            GetDefaultColorRGB(ColorSection, SectionSize);
        }
        UIWidgets::Tooltip("Enable/Disable custom Link's tunics colors\nIf disabled you will have original colors for Link's tunics.");
        UIWidgets::Tooltip(Tooltip_RNG.c_str());
        ImGui::EndTable();
    }
}

void Draw_Npcs(){
    DrawRandomizeResetButton("all NPCs", NPCs_section, SECTION_SIZE(NPCs_section));
    UIWidgets::EnhancementCheckbox("Custom colors for Navi", "gUseNaviCol");
    UIWidgets::Tooltip("Enable/Disable custom Navi colors\nIf disabled, default colors will be used\nColors go into effect when Navi goes back into your pockets");
    if (CVar_GetS32("gUseNaviCol",0)) { 
        DrawRandomizeResetButton("Navi's", Navi_Section, SECTION_SIZE(Navi_Section)); 
    };
    if (CVar_GetS32("gUseNaviCol",0) && ImGui::BeginTable("tableNavi", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Inner colors##Navi", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Navi", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Navi_Section, SECTION_SIZE(Navi_Section));
        ImGui::EndTable();
    }
    UIWidgets::EnhancementCheckbox("Custom colors for Keese", "gUseKeeseCol");
    UIWidgets::Tooltip("Enable/Disable custom Keese element colors\nIf disabled, default element colors will be used\nColors go into effect when Keese respawn (or when the room is reloaded)");
    if (CVar_GetS32("gUseKeeseCol",0) && ImGui::BeginTable("tableKeese", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Fire colors##Keese", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Ice colors##Keese", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Keese_Section, SECTION_SIZE(Keese_Section));
        ImGui::EndTable();
    }
    UIWidgets::EnhancementCheckbox("Custom colors for Dogs", "gUseDogsCol");
    UIWidgets::Tooltip("Enable/Disable custom colors for the two Dog variants\nIf disabled, default colors will be used");
    if (CVar_GetS32("gUseDogsCol",0) && ImGui::BeginTable("tableDogs", 2, FlagsTable)) {
        ImGui::TableSetupColumn("White Dog color", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Brown Dog color", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Dogs_Section, SECTION_SIZE(Dogs_Section));
        ImGui::EndTable();
    }
}
void Draw_ItemsSkills(){
    DrawRandomizeResetButton("all skills and items", AllItemsSkills_section, SECTION_SIZE(AllItemsSkills_section));
    UIWidgets::EnhancementCheckbox("Custom tunics color", "gUseTunicsCol");
    UIWidgets::Tooltip("Enable/Disable custom Link's tunics colors\nIf disabled you will have original colors for Link's tunics.");
    if (CVar_GetS32("gUseTunicsCol",0)) {
        DrawRandomizeResetButton("Link's tunics", Tunics_Section, SECTION_SIZE(Tunics_Section));
    };
    if (CVar_GetS32("gUseTunicsCol",0) && ImGui::BeginTable("tableTunics", 3, FlagsTable)) {
        ImGui::TableSetupColumn("Kokiri Tunic", FlagsCell, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Goron Tunic", FlagsCell, TablesCellsWidth/3);
        ImGui::TableSetupColumn("Zora Tunic", FlagsCell, TablesCellsWidth/3);
        Table_InitHeader();
        DrawColorSection(Tunics_Section, SECTION_SIZE(Tunics_Section));
        ImGui::EndTable();
    }
    UIWidgets::EnhancementCheckbox("Custom arrows colors", "gUseArrowsCol");
    if (CVar_GetS32("gUseArrowsCol",0)) {
        DrawRandomizeResetButton("elemental arrows", Arrows_section, SECTION_SIZE(Arrows_section));
    }
    if (CVar_GetS32("gUseArrowsCol",0) && ImGui::BeginTable("tableArrows", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Primary colors##Arrows", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Arrows", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Arrows_section, SECTION_SIZE(Arrows_section));
        ImGui::EndTable();
    }
    UIWidgets::EnhancementCheckbox("Custom spells colors", "gUseSpellsCol");
    if (CVar_GetS32("gUseSpellsCol",0)) {
        DrawRandomizeResetButton("spells", Spells_section, SECTION_SIZE(Spells_section));
    }
    if (CVar_GetS32("gUseSpellsCol",0) && ImGui::BeginTable("tableSpells", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Inner colors##Spells", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Outer colors##Spells", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(Spells_section, SECTION_SIZE(Spells_section));
        ImGui::EndTable();
    }
    UIWidgets::EnhancementCheckbox("Custom spin attack colors", "gUseChargedCol");
    if (CVar_GetS32("gUseChargedCol",0)) {
        DrawRandomizeResetButton("spins attack", SpinAtk_section, SECTION_SIZE(SpinAtk_section));
    }
    if (CVar_GetS32("gUseChargedCol",0) && ImGui::BeginTable("tableChargeAtk", 2, FlagsTable)) {
        ImGui::TableSetupColumn("Primary colors##Charge", FlagsCell, TablesCellsWidth/2);
        ImGui::TableSetupColumn("Env colors##Charge", FlagsCell, TablesCellsWidth/2);
        Table_InitHeader();
        DrawColorSection(SpinAtk_section, SECTION_SIZE(SpinAtk_section));
        ImGui::EndTable();
    }
    UIWidgets::EnhancementCheckbox("Custom trails", "gUseTrailsCol");
    if (CVar_GetS32("gUseTrailsCol", 0)) {
        DrawRandomizeResetButton("trails", AllTrail_section, SECTION_SIZE(AllTrail_section));
    }
    if (CVar_GetS32("gUseTrailsCol", 0) && ImGui::BeginTable("tabletrails", 3, FlagsTable)) {
        ImGui::TableSetupColumn("Sword Trails", FlagsCell, TablesCellsWidth);
        ImGui::TableSetupColumn("Boomerang Trails", FlagsCell, TablesCellsWidth);
        ImGui::TableSetupColumn("Bomb Trails", FlagsCell, TablesCellsWidth);
        Table_InitHeader();
        DrawColorSection(Trail_section, SECTION_SIZE(Trail_section));
        ImGui::EndTable();
        UIWidgets::EnhancementSliderInt("Sword Trail Duration: %d", "##TrailsMul", "gTrailDuration", 1, 16, "", 4, true);
        UIWidgets::Tooltip("Determines the duration of Link's sword trails.");
        ResetTrailLength("gTrailDuration", 4);
        UIWidgets::EnhancementCheckbox("Swords use separate colors", "gSeperateSwords");
        if (CVar_GetS32("gSeperateSwords", 0) && ImGui::CollapsingHeader("Individual Sword Colors")) {
            if (ImGui::BeginTable("tabletrailswords", 2, FlagsTable)) {
                ImGui::TableSetupColumn("Kokiri Sword", FlagsCell, TablesCellsWidth / 2);
                ImGui::TableSetupColumn("Master Sword", FlagsCell, TablesCellsWidth / 2);
                ImGui::TableSetupColumn("Biggoron Sword", FlagsCell, TablesCellsWidth / 2);
                ImGui::TableSetupColumn("Deku Stick", FlagsCell, TablesCellsWidth / 2);
                ImGui::TableSetupColumn("Megaton Hammer", FlagsCell, TablesCellsWidth);
                Table_InitHeader();
                DrawColorSection(SwordTrail_section, SECTION_SIZE(SwordTrail_section));
                ImGui::EndTable();
            }
        }
        ImGui::NewLine();
    }
}
void Draw_Menus(){
    if (CVar_GetS32("gHudColors",0) ==2 ){
        if (ImGui::BeginTable("tableFileChoose", 2, FlagsTable)) {
            ImGui::TableSetupColumn("File Choose color", FlagsCell, TablesCellsWidth/2);
            ImGui::TableSetupColumn("Bottom text color", FlagsCell, TablesCellsWidth/2);
            Table_InitHeader();
            DrawColorSection(FileChoose_section, SECTION_SIZE(FileChoose_section));
            ImGui::EndTable();
        }
    } else {
        ImGui::Text("To modify menus colors you need \"Custom Colors\" scheme\nto be selected in \"General\" tab.\nOnce enabled you will be able to modify the following colors:\nFile Choose color\nBottom text color");
    }
}
void Draw_Placements(){
    if (ImGui::BeginTable("tableMargins", 1, FlagsTable)) {
        ImGui::TableSetupColumn("General margins settings", FlagsCell, TablesCellsWidth);
        Table_InitHeader();
        UIWidgets::EnhancementSliderInt("Top : %dx", "##UIMARGINT", "gHUDMargin_T", (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0, true);
        UIWidgets::EnhancementSliderInt("Left: %dx", "##UIMARGINL", "gHUDMargin_L", -25, ImGui::GetWindowViewport()->Size.x, "", 0, true);
        UIWidgets::EnhancementSliderInt("Right: %dx", "##UIMARGINR", "gHUDMargin_R", (ImGui::GetWindowViewport()->Size.x)*-1, 25, "", 0, true);
        UIWidgets::EnhancementSliderInt("Bottom: %dx", "##UIMARGINB", "gHUDMargin_B", (ImGui::GetWindowViewport()->Size.y/2)*-1, 25, "", 0, true);
        SetMarginAll("All margins on",true);
        UIWidgets::Tooltip("Set most of the element to use margin\nSome elements with default position will not be affected\nElements without Archor or Hidden will not be turned on");
        ImGui::SameLine();
        SetMarginAll("All margins off",false);
        UIWidgets::Tooltip("Set all of the element to not use margin");
        ImGui::SameLine();
        ResetPositionAll();
        UIWidgets::Tooltip("Revert every element to use their original position and no margins");
        ImGui::NewLine();
        ImGui::EndTable();
    }
    if (ImGui::CollapsingHeader("Hearts count position")) {
        if (ImGui::BeginTable("tableHeartsCounts", 1, FlagsTable)) {
            ImGui::TableSetupColumn("Hearts counts settings", FlagsCell, TablesCellsWidth);
            Table_InitHeader(false);
            DrawUseMarginsSlider("Hearts counts", "gHearts");
            DrawPositionsRadioBoxes("gHeartsCount");
            DrawPositionSlider("gHeartsCount",-22,ImGui::GetWindowViewport()->Size.y,-125,ImGui::GetWindowViewport()->Size.x);
            DrawScaleSlider("gHeartsCount",0.7f);
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
            DrawScaleSlider("gBBtn",0.95f);
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
            DrawScaleSlider("gABtn",0.95f);
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
            DrawScaleSlider("gStartBtn",0.75f);
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
            DrawScaleSlider("gCBtnU",0.5f);
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
            DrawScaleSlider("gCBtnD",0.87f);
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
            DrawScaleSlider("gCBtnL",0.87f);
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
            DrawScaleSlider("gCBtnR",0.87f);
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
        DrawRandomizeResetButton("every buttons", Buttons_section, SECTION_SIZE(Buttons_section));
        if (ImGui::CollapsingHeader("A Button colors & A Cursors")) {
            if (ImGui::BeginTable("tableBTN_A", 1, FlagsTable)) {
                ImGui::TableSetupColumn("A Button colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                DrawColorSection(A_Btn_section, SECTION_SIZE(A_Btn_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("B Button color")) {
            if (ImGui::BeginTable("tableBTN_B", 1, FlagsTable)) {
                ImGui::TableSetupColumn("B button color", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                DrawColorSection(B_Btn_section, SECTION_SIZE(B_Btn_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("C-Buttons & C Cursor")) {
            if (ImGui::BeginTable("tableBTN_C", 1, FlagsTable)) {
                ImGui::TableSetupColumn("Button C colors & C Cursor colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                DrawColorSection(C_Btn_Unified_section, SECTION_SIZE(C_Btn_Unified_section));
                ImGui::EndTable();
            }
            UIWidgets::EnhancementCheckbox("C-Buttons use separate colors", "gCCparated");
            if (CVar_GetS32("gCCparated",0) && ImGui::CollapsingHeader("C Button individual colors")) {
                if (ImGui::BeginTable("tableBTN_CSep", 1, FlagsTable)) {
                    ImGui::TableSetupColumn("C-Buttons individual colors", FlagsCell, TablesCellsWidth);
                    Table_InitHeader(false);
                    DrawColorSection(C_Btn_Separated_section, SECTION_SIZE(C_Btn_Separated_section));
                    ImGui::EndTable();
                }
            }
        }
        if (ImGui::CollapsingHeader("Start button colors")) {
            if (ImGui::BeginTable("tableBTN_Start", 1, FlagsTable)) {
                ImGui::TableSetupColumn("Start button colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                DrawColorSection(Start_Btn_section, SECTION_SIZE(Start_Btn_section));
                ImGui::EndTable();
            }
        }
        if (CVar_GetS32("gDpadEquips",0) && ImGui::CollapsingHeader("DPad colors")) {
            if (ImGui::BeginTable("tableDpadHud", 1, FlagsTable)) {
                ImGui::TableSetupColumn("DPad color", FlagsCell, TablesCellsWidth);
                Table_InitHeader(false);
                DrawColorSection(DPad_section, SECTION_SIZE(DPad_section));
                ImGui::EndTable();
            }
        }
    } else {
        ImGui::Text("To modify buttons colors you need \"Custom Colors\" scheme\nto be selected in \"General\" tab.\nOnce enabled you will be able to modify the following colors:\nA Button colors,\nB Button colors,\nC Button colors & cursor,\nStart button\nDPad icon");
    }
}
void Draw_General(){
    DrawRandomizeResetButton("all cosmetics", Everything_Section, SECTION_SIZE(Everything_Section), true);
    if (ImGui::BeginTable("tableScheme", 3, FlagsTable | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("N64 Scheme", FlagsCell, TablesCellsWidth);
        ImGui::TableSetupColumn("GameCube Scheme", FlagsCell, TablesCellsWidth);
        ImGui::TableSetupColumn("Custom Schemes", FlagsCell, TablesCellsWidth);
        Table_InitHeader();
        Draw_HelpIcon("Change interface color to N64 style");
        UIWidgets::EnhancementRadioButton("N64 Colors", "gHudColors", 0);
        Table_NextCol();
        Draw_HelpIcon("Change interface color to GameCube style");
        UIWidgets::EnhancementRadioButton("GCN Colors", "gHudColors", 1);
        Table_NextCol();
        Draw_HelpIcon("Lets you change every interface color to your liking");
        UIWidgets::EnhancementRadioButton("Custom Colors", "gHudColors", 2);
        ImGui::EndTable();
    }
    if (CVar_GetS32("gHudColors",0) ==2 ){
        DrawRandomizeResetButton("interface (excluding buttons)", Misc_Interface_section, SECTION_SIZE(Misc_Interface_section));
        if (ImGui::CollapsingHeader("Hearts colors")) {
            UIWidgets::Tooltip("Hearts colors in general\nDD stand for Double Defense");
            if (ImGui::BeginTable("tableHearts", 3, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Hearts (normal)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
                ImGui::TableSetupColumn("Hearts (DD)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
                ImGui::TableSetupColumn("Hearts Outline (DD)", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable, TablesCellsWidth/3);
                Table_InitHeader();
                DrawColorSection(Hearts_section, SECTION_SIZE(Hearts_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Magic Meter colors")) {
            if (ImGui::BeginTable("tableMagicmeter", 2, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Magic meter", FlagsCell, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Magic meter in use", FlagsCell, TablesCellsWidth/2);
                Table_InitHeader();
                DrawColorSection(Magic_Meter_section, SECTION_SIZE(Magic_Meter_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Minimap and compass colors")) {
            if (ImGui::BeginTable("tableMinimapCol", 1, FlagsTable)) {
                ImGui::TableSetupColumn("Minimap color", FlagsCell, TablesCellsWidth);
                Table_InitHeader();
                DrawColorSection(Minimap_section, SECTION_SIZE(Minimap_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Title cards colors")) {
            if (ImGui::BeginTable("tableTitleCards", 2, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Title cards Overworld", FlagsCell, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Title cards Bosses", FlagsCell, TablesCellsWidth/2);
                Table_InitHeader();
                DrawColorSection(TitleCards_section, SECTION_SIZE(TitleCards_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Misc. interface colors")) {
            if (ImGui::BeginTable("tableMiscHudCol", 1, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("Misc HUD colors", FlagsCell, TablesCellsWidth);
                Table_InitHeader();
                DrawColorSection(Misc_section, SECTION_SIZE(Misc_section));
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Scenes transitions")) {
            if (ImGui::BeginTable("tabletransitionotherCol", 2, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("transitionother1", FlagsCell, TablesCellsWidth/2);
                ImGui::TableSetupColumn("transitionother2", FlagsCell, TablesCellsWidth/2);
                Table_InitHeader(false);
                UIWidgets::EnhancementRadioButton("Originals", "gSceneTransitions", 255);
                UIWidgets::Tooltip("This will make the game use original scenes transitions");
                Table_NextCol();
                UIWidgets::EnhancementRadioButton("None", "gSceneTransitions", 11);
                UIWidgets::Tooltip("This will make the game use no any scenes transitions");
                Table_NextLine();
                UIWidgets::EnhancementRadioButton("Desert mode (persistant)", "gSceneTransitions", 14);
                UIWidgets::Tooltip("This will make the game use the sand storm scenes transitions that will persist in map");
                Table_NextCol();
                UIWidgets::EnhancementRadioButton("Desert mode (non persistant)", "gSceneTransitions", 15);
                UIWidgets::Tooltip("This will make the game use the sand storm scenes transitions");
                Table_NextLine();
                UIWidgets::EnhancementRadioButton("Normal fade (green)", "gSceneTransitions", 18);
                UIWidgets::Tooltip("This will make the game use a greenish fade in/out scenes transitions");
                Table_NextCol();
                UIWidgets::EnhancementRadioButton("Normal fade (blue)", "gSceneTransitions", 19);
                UIWidgets::Tooltip("This will make the game use a blue fade in/out scenes transitions");
                Table_NextLine();
                UIWidgets::EnhancementRadioButton("Triforce", "gSceneTransitions", 1);
                ImGui::EndTable();
            }
            if (ImGui::BeginTable("tabletransitionCol", 2, FlagsTable | ImGuiTableFlags_Hideable)) {
                ImGui::TableSetupColumn("White color", FlagsCell, TablesCellsWidth/2);
                ImGui::TableSetupColumn("Black color", FlagsCell, TablesCellsWidth/2);
                Table_InitHeader();
                DrawTransitions("gSceneTransitions");
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

    ImGui::SetNextWindowSize(ImVec2(620, 430), ImGuiCond_FirstUseEver);
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
    SohImGui::AddWindow("Enhancements", "Rainbowfunction", LoadRainbowColor, true, true);
    //Draw the bar in the menu.
    SohImGui::AddWindow("Enhancements", "Cosmetics Editor", DrawCosmeticsEditor);
}
