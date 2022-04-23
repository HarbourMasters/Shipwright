#pragma once

struct SoHConfigType {
    // Debug
    struct {
        bool soh = false;
        bool n64mode = false;
        bool menu_bar = false;
        bool soh_sink = true;
    } debug;

    // Audio
    struct {
        float master = 1.0f;
        float music_main = 1.0f;
        float fanfare = 1.0f;
        float sfx = 1.0f;
        float music_sub = 1.0f;
    } audio;

    // Enhancements
    struct {
        int text_speed = 1;
        bool skip_text = false;
        bool disable_lod = false;
        bool animated_pause_menu = false;
        bool dynamic_wallet_icon = false;
        bool minimal_ui = false;
        bool visualagony = false;
        bool mm_bunny_hood = false;

    } enhancements;

    // Controller
    struct {
        float gyro_sensitivity = 1.0f;
        float rumble_strength = 1.0f;
        float input_scale = 1.0f;
        float gyroDriftX = 0.0f;
        float gyroDriftY = 0.0f;
        bool input_enabled = false;
        bool dpad_pause_name = false;
        bool dpad_ocarina_text = false;
        bool dpad_shop = false;
    } controller;

    // Cheats
    struct {
        bool debug_mode = false;
        bool infinite_money = false;
        bool infinite_health = false;
        bool infinite_ammo = false;
        bool infinite_magic = false;
        bool infinite_nayru = false;
        bool no_clip = false;
        bool climb_everything = false;
        bool moon_jump_on_l = false;
        bool super_tunic = false;
        bool ez_isg = false;
        bool no_restrict_item = false;
        bool freeze_time = false;
    } cheats;

    // Graphics
    struct {
        bool show = false;
    } graphics;
    
    // HUD Colors
    struct {
        bool n64_colors = true;
        bool gc_colors = false;
        bool custom_colors = false;
        //hearts colors
        int ccheartsprimr = 255;
        int ccheartsprimg = 70;
        int ccheartsprimb = 50;
        //hearts double defense colors
        int ddccheartsprimr = 255;
        int ddccheartsprimg = 255;
        int ddccheartsprimb = 255;
        //A Button Main colors
        int ccabtnprimr = 90;
        int ccabtnprimg = 90;
        int ccabtnprimb = 255;
        //B Button Main colors
        int ccbbtnprimr = 0;
        int ccbbtnprimg = 150;
        int ccbbtnprimb = 0;
        //C Button Main colors
        int cccbtnprimr = 255;
        int cccbtnprimg = 160;
        int cccbtnprimb = 0;
        //Start Button Main colors
        int ccstartbtnprimr = 200;
        int ccstartbtnprimg = 0;
        int ccstartbtnprimb = 0;
        //Magic Border colors
        int ccmagicborderprimr = 255;
        int ccmagicborderprimg = 255;
        int ccmagicborderprimb = 255;
        //Magic Main colors
        int ccmagicprimr = 10;
        int ccmagicprimg = 255;
        int ccmagicprimb = 10;
        //Magic Being used colors
        int ccmagicuseprimr = 250;
        int ccmagicuseprimg = 250;
        int ccmagicuseprimb = 0;
        //Minimap
        int ccminimapprimr = 10;
        int ccminimapprimg = 10;
        int ccminimapprimb = 180;
        //Rupee icon
        int ccrupeeprimr = 10;
        int ccrupeeprimg = 220;
        int ccrupeeprimb = 10;
    } hudcolors;
};

enum SeqPlayers {
    /* 0 */ SEQ_BGM_MAIN,
    /* 1 */ SEQ_FANFARE,
    /* 2 */ SEQ_SFX,
    /* 3 */ SEQ_BGM_SUB,
    /* 4 */ SEQ_MAX
 };

#define DEBUG_SECTION "DEBUG SETTINGS"
#define AUDIO_SECTION "AUDIO SETTINGS"
#define CONTROLLER_SECTION "CONTROLLER SECTION"
#define ENHANCEMENTS_SECTION "ENHANCEMENT SETTINGS"
#define CHEATS_SECTION "CHEATS SETTINGS"

namespace Game {
    extern SoHConfigType Settings;
    void InitSettings();
    void LoadSettings();
    void SaveSettings();
    void SetSeqPlayerVolume(SeqPlayers playerId, float volume);
}
