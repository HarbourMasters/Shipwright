#pragma once

void InitCosmeticsEditor();//Init the menu itself
void LoadRainbowColor();
void NewSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0);