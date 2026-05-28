#include "CustomFont.h"

#include <imgui.h>
#include <set>
#include <unordered_map>
#include <libultraship/bridge.h>
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/archive/ArchiveManager.h>
#include <ship/resource/File.h>

#include "variables.h"
#include "textures/nes_font_static/nes_font_static.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/SaveManager.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/vanilla-behavior/GIVanillaBehavior.h"
#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"
#include <SDL2/SDL_stdinc.h>

extern "C" {
#include "z64.h"
#include "regs.h"
#include "message_data_fmt.h"
#include "soh/cvar_prefixes.h"

extern PlayState* gPlayState;
}

static std::vector<std::string> sModFontNames;
static std::vector<ImFont*> sModFontPtrs;
static std::vector<std::vector<char>> sModFontData; // kept alive for the atlas

struct TranslationFile {
    std::string name;
    std::string path;
};

static std::vector<TranslationFile> sTranslationFiles;
struct TranslationEntry {
    std::vector<std::vector<CustomFont::TextSegment>> pages;
    std::vector<uint8_t> proxyLatin; // u8 for ENG/GER/FRA
    std::vector<uint16_t> proxyJpn;  // u16 SJS for JPN
};
static std::unordered_map<uint16_t, TranslationEntry> sActiveTranslation;

static std::string ParseStringLiteral(const std::string& src, size_t& pos) {
    ++pos;
    std::string out;
    while (pos < src.size() && src[pos] != '"') {
        if (src[pos] == '\\' && pos + 1 < src.size()) {
            char esc = src[++pos];
            switch (esc) {
                case 'n':
                    out += '\n';
                    break;
                case 't':
                    out += '\t';
                    break;
                case '\\':
                    out += '\\';
                    break;
                case '"':
                    out += '"';
                    break;
                case 'x': {
                    if (pos + 2 < src.size()) {
                        auto h = [](char c) -> int {
                            if (c >= '0' && c <= '9')
                                return c - '0';
                            if (c >= 'a' && c <= 'f')
                                return c - 'a' + 10;
                            if (c >= 'A' && c <= 'F')
                                return c - 'A' + 10;
                            return 0;
                        };
                        out += static_cast<char>(h(src[pos + 1]) << 4 | h(src[pos + 2]));
                        pos += 2;
                    }
                    break;
                }
                default:
                    out += esc;
                    break;
            }
        } else {
            out += src[pos];
        }
        ++pos;
    }
    if (pos < src.size())
        ++pos;
    return out;
}

static void SkipWS(const std::string& src, size_t& pos) {
    while (pos < src.size()) {
        if (src[pos] == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
            while (pos < src.size() && src[pos] != '\n')
                ++pos;
        } else if (std::isspace((unsigned char)src[pos])) {
            ++pos;
        } else {
            break;
        }
    }
}

static std::string ReadIdent(const std::string& src, size_t& pos) {
    size_t start = pos;
    while (pos < src.size() && (std::isalnum((unsigned char)src[pos]) || src[pos] == '_'))
        ++pos;
    return src.substr(start, pos - start);
}

static ImVec4 ColorFromName(const std::string& name, const ImVec4& def) {
    if (name == "DEFAULT")
        return def;
    if (name == "RED")
        return ImVec4(1.00f, 0.27f, 0.27f, 1.f);
    if (name == "GREEN")
        return ImVec4(0.00f, 1.00f, 0.00f, 1.f);
    if (name == "BLUE")
        return ImVec4(0.25f, 0.25f, 1.00f, 1.f);
    if (name == "LIGHTBLUE")
        return ImVec4(0.50f, 0.80f, 1.00f, 1.f);
    if (name == "PURPLE")
        return ImVec4(0.75f, 0.25f, 0.75f, 1.f);
    if (name == "YELLOW")
        return ImVec4(1.00f, 1.00f, 0.25f, 1.f);
    if (name == "BLACK")
        return ImVec4(0.00f, 0.00f, 0.00f, 1.f);
    return def;
}

static ImVec4 ABtnColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.AButton.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.AButton.Value"), Color_RGB8{ 90, 90, 255 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    if (CVarGetInteger(CVAR_COSMETIC("DefaultColorScheme"), COLORSCHEME_N64) == COLORSCHEME_GAMECUBE)
        return ImVec4(50 / 255.0f, 1.0f, 130 / 255.0f, 1.0f); // GC A: green
    return ImVec4(90 / 255.0f, 90 / 255.0f, 1.0f, 1.0f);      // N64 A: blue
}

static ImVec4 BBtnColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.BButton.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.BButton.Value"), Color_RGB8{ 0, 150, 0 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    if (CVarGetInteger(CVAR_COSMETIC("DefaultColorScheme"), COLORSCHEME_N64) == COLORSCHEME_GAMECUBE)
        return ImVec4(1.0f, 30 / 255.0f, 30 / 255.0f, 1.0f); // GC B: red
    return ImVec4(0.0f, 150 / 255.0f, 0.0f, 1.0f);           // N64 B: green
}

static ImVec4 CBtnGroupColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CButtons.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), Color_RGB8{ 255, 160, 0 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    return ImVec4(1.0f, 160 / 255.0f, 0.0f, 1.0f);
}

static ImVec4 CUpBtnColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CUpButton.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.CUpButton.Value"), Color_RGB8{ 255, 160, 0 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    return CBtnGroupColor();
}

static ImVec4 CDownBtnColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CDownButton.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.CDownButton.Value"), Color_RGB8{ 255, 160, 0 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    return CBtnGroupColor();
}

static ImVec4 CLeftBtnColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CLeftButton.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.CLeftButton.Value"), Color_RGB8{ 255, 160, 0 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    return CBtnGroupColor();
}

static ImVec4 CRightBtnColor() {
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CRightButton.Changed"), 0)) {
        Color_RGB8 c = CVarGetColor24(CVAR_COSMETIC("HUD.CRightButton.Value"), Color_RGB8{ 255, 160, 0 });
        return ImVec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
    }
    return CBtnGroupColor();
}

// bi: 0=A,1=B,2=C,3=L,4=R,5=Z,6=C-Up,7=C-Down,8=C-Left,9=C-Right,10=Z-target,11=stick,12=pad
static ImVec4 BtnTintColor(int bi) {
    switch (bi) {
        case 0:
            return ABtnColor();
        case 1:
            return BBtnColor();
        case 2:
            return CBtnGroupColor();
        case 6:
            return CUpBtnColor();
        case 7:
            return CDownBtnColor();
        case 8:
            return CLeftBtnColor();
        case 9:
            return CRightBtnColor();
        case 3:
        case 4:
        case 5:
        case 11:
            return ImVec4(0.50f, 0.80f, 1.00f, 1.0f); // L, R, Z, stick
        case 10:
            return ImVec4(0.00f, 0.82f, 0.20f, 1.0f); // Z-target
        default:
            return ImVec4(1.00f, 1.00f, 1.00f, 1.0f); // pad, unknown
    }
}

static const char* BtnIconFromMacro(const std::string& name) {
    if (name == "BUTTON_A")
        return dgMsgChar9FButtonATex;
    if (name == "BUTTON_B")
        return dgMsgCharA0ButtonBTex;
    if (name == "BUTTON_C")
        return dgMsgCharA1ButtonCTex;
    if (name == "BUTTON_L")
        return dgMsgCharA2ButtonLTex;
    if (name == "BUTTON_R")
        return dgMsgCharA3ButtonRTex;
    if (name == "BUTTON_Z")
        return dgMsgCharA4ButtonZTex;
    if (name == "BUTTON_CUP")
        return dgMsgCharA5ButtonCUpTex;
    if (name == "BUTTON_CDOWN")
        return dgMsgCharA6ButtonCDownTex;
    if (name == "BUTTON_CLEFT")
        return dgMsgCharA7ButtonCLeftTex;
    if (name == "BUTTON_CRIGHT")
        return dgMsgCharA8ButtonCRightTex;
    if (name == "ZTARGET_SIGN")
        return dgMsgCharA9ZTargetSignTex;
    if (name == "CONTROL_STICK")
        return dgMsgCharAAControlStickTex;
    if (name == "CONTROL_PAD")
        return dgMsgCharABControlPadTex;
    return nullptr;
}

static ImVec4 BtnColorFromMacro(const std::string& name) {
    if (name == "BUTTON_A")
        return ABtnColor();
    if (name == "BUTTON_B")
        return BBtnColor();
    if (name == "BUTTON_C")
        return CBtnGroupColor();
    if (name == "BUTTON_L" || name == "BUTTON_R" || name == "BUTTON_Z")
        return ImVec4(0.50f, 0.80f, 1.00f, 1.f);
    if (name == "BUTTON_CUP")
        return CUpBtnColor();
    if (name == "BUTTON_CDOWN")
        return CDownBtnColor();
    if (name == "BUTTON_CLEFT")
        return CLeftBtnColor();
    if (name == "BUTTON_CRIGHT")
        return CRightBtnColor();
    if (name == "ZTARGET_SIGN")
        return ImVec4(0.00f, 0.82f, 0.20f, 1.f);
    if (name == "CONTROL_STICK")
        return ImVec4(0.50f, 0.80f, 1.00f, 1.f);
    return ImVec4(1.f, 1.f, 1.f, 1.f);
}

// Replicates Message_DecodeName's charset logic to get the player name as a plain UTF-8 string.
// JPN names use font-buffer indices (from gKeyboardCharactersHiragana / gKeyboardCharactersKatakana
// / gKeyboardCharactersAlphanumeric) which are looked up to their Unicode codepoints here.
static std::string GetPlayerName() {
    const bool isPAL = (gSaveContext.ship.filenameLanguage == NAME_LANGUAGE_PAL);
    const bool isJpnNm = (gSaveContext.ship.filenameLanguage == NAME_LANGUAGE_NTSC_JPN);
    const uint8_t emptyChar = isPAL ? 0x3E : 0xDF;

    int len = 8;
    while (len > 0 && gSaveContext.playerName[len - 1] == emptyChar)
        len--;

    std::string name;

    if (isJpnNm) {
        // Emit UTF-8 for a Unicode codepoint (BMP-only is sufficient here).
        auto u8 = [&](uint32_t cp) {
            if (cp < 0x80) {
                name += (char)cp;
            } else if (cp < 0x800) {
                name += (char)(0xC0 | (cp >> 6));
                name += (char)(0x80 | (cp & 0x3F));
            } else {
                name += (char)(0xE0 | (cp >> 12));
                name += (char)(0x80 | ((cp >> 6) & 0x3F));
                name += (char)(0x80 | (cp & 0x3F));
            }
        };
        for (int i = 0; i < len; i++) {
            const uint8_t c = gSaveContext.playerName[i];
            // Latin from alphanumeric keyboard (gKeyboardCharactersAlphanumeric)
            if (c >= 0xAB && c < 0xC5) {
                u8('A' + c - 0xAB);
                continue;
            } // A–Z
            if (c >= 0xC5 && c < 0xDF) {
                u8('a' + c - 0xC5);
                continue;
            } // a–z
            if (c < 0x0A) {
                u8('0' + c);
                continue;
            } // 0–9
            // clang-format off
            switch (c) {
                case 0xDF: u8(' ');      break;
                case 0xEA: u8('.');      break;
                case 0xE4: u8(0x30FC);  break; // ー (long vowel / dash glyph)
                case 0xE7: u8(0x309B);  break; // ゛
                case 0xE8: u8(0x309C);  break; // ゜
                // Hiragana (gKeyboardCharactersHiragana)
                case 0x0A: u8(0x3042); break; case 0x0B: u8(0x3044); break; // あ い
                case 0x0C: u8(0x3046); break; case 0x0D: u8(0x3048); break; // う え
                case 0x0E: u8(0x304A); break; case 0x0F: u8(0x304B); break; // お か
                case 0x10: u8(0x304D); break; case 0x11: u8(0x304F); break; // き く
                case 0x12: u8(0x3051); break; case 0x13: u8(0x3053); break; // け こ
                case 0x14: u8(0x3055); break; case 0x15: u8(0x3057); break; // さ し
                case 0x16: u8(0x3059); break; case 0x17: u8(0x305B); break; // す せ
                case 0x18: u8(0x305D); break; case 0x19: u8(0x305F); break; // そ た
                case 0x1A: u8(0x3061); break; case 0x1B: u8(0x3064); break; // ち つ
                case 0x1C: u8(0x3066); break; case 0x1D: u8(0x3068); break; // て と
                case 0x1E: u8(0x306A); break; case 0x1F: u8(0x306B); break; // な に
                case 0x20: u8(0x306C); break; case 0x21: u8(0x306D); break; // ぬ ね
                case 0x22: u8(0x306E); break; case 0x23: u8(0x306F); break; // の は
                case 0x24: u8(0x3072); break; case 0x25: u8(0x3075); break; // ひ ふ
                case 0x26: u8(0x3078); break; case 0x27: u8(0x307B); break; // へ ほ
                case 0x28: u8(0x307E); break; case 0x29: u8(0x307F); break; // ま み
                case 0x2A: u8(0x3080); break; case 0x2B: u8(0x3081); break; // む め
                case 0x2C: u8(0x3082); break; case 0x2D: u8(0x3084); break; // も や
                case 0x2E: u8(0x3086); break; case 0x2F: u8(0x3088); break; // ゆ よ
                case 0x30: u8(0x3089); break; case 0x31: u8(0x308A); break; // ら り
                case 0x32: u8(0x308B); break; case 0x33: u8(0x308C); break; // る れ
                case 0x34: u8(0x308D); break; case 0x35: u8(0x308F); break; // ろ わ
                case 0x36: u8(0x3092); break; case 0x37: u8(0x3093); break; // を ん
                case 0x38: u8(0x3041); break; case 0x39: u8(0x3043); break; // ぁ ぃ
                case 0x3A: u8(0x3045); break; case 0x3B: u8(0x3047); break; // ぅ ぇ
                case 0x3C: u8(0x3049); break; case 0x3D: u8(0x3063); break; // ぉ っ
                case 0x3E: u8(0x3083); break; case 0x3F: u8(0x3085); break; // ゃ ゅ
                case 0x40: u8(0x3087); break;                                // ょ
                // Katakana (gKeyboardCharactersKatakana)
                case 0x5A: u8(0x30A2); break; case 0x5B: u8(0x30A4); break; // ア イ
                case 0x5C: u8(0x30A6); break; case 0x5D: u8(0x30A8); break; // ウ エ
                case 0x5E: u8(0x30AA); break; case 0x5F: u8(0x30AB); break; // オ カ
                case 0x60: u8(0x30AD); break; case 0x61: u8(0x30AF); break; // キ ク
                case 0x62: u8(0x30B1); break; case 0x63: u8(0x30B3); break; // ケ コ
                case 0x64: u8(0x30B5); break; case 0x65: u8(0x30B7); break; // サ シ
                case 0x66: u8(0x30B9); break; case 0x67: u8(0x30BB); break; // ス セ
                case 0x68: u8(0x30BD); break; case 0x69: u8(0x30BF); break; // ソ タ
                case 0x6A: u8(0x30C1); break; case 0x6B: u8(0x30C4); break; // チ ツ
                case 0x6C: u8(0x30C6); break; case 0x6D: u8(0x30C8); break; // テ ト
                case 0x6E: u8(0x30CA); break; case 0x6F: u8(0x30CB); break; // ナ ニ
                case 0x70: u8(0x30CC); break; case 0x71: u8(0x30CD); break; // ヌ ネ
                case 0x72: u8(0x30CE); break; case 0x73: u8(0x30CF); break; // ノ ハ
                case 0x74: u8(0x30D2); break; case 0x75: u8(0x30D5); break; // ヒ フ
                case 0x76: u8(0x30D8); break; case 0x77: u8(0x30DB); break; // ヘ ホ
                case 0x78: u8(0x30DE); break; case 0x79: u8(0x30DF); break; // マ ミ
                case 0x7A: u8(0x30E0); break; case 0x7B: u8(0x30E1); break; // ム メ
                case 0x7C: u8(0x30E2); break; case 0x7D: u8(0x30E4); break; // モ ヤ
                case 0x7E: u8(0x30E6); break; case 0x7F: u8(0x30E8); break; // ユ ヨ
                case 0x80: u8(0x30E9); break; case 0x81: u8(0x30EA); break; // ラ リ
                case 0x82: u8(0x30EB); break; case 0x83: u8(0x30EC); break; // ル レ
                case 0x84: u8(0x30ED); break; case 0x85: u8(0x30EF); break; // ロ ワ
                case 0x86: u8(0x30F2); break; case 0x87: u8(0x30F3); break; // ヲ ン
                case 0x88: u8(0x30A1); break; case 0x89: u8(0x30A3); break; // ァ ィ
                case 0x8A: u8(0x30A5); break; case 0x8B: u8(0x30A7); break; // ゥ ェ
                case 0x8C: u8(0x30A9); break; case 0x8D: u8(0x30C3); break; // ォ ッ
                case 0x8E: u8(0x30E3); break; case 0x8F: u8(0x30E5); break; // ャ ュ
                case 0x90: u8(0x30E7); break;                                // ョ
                default: break; // unknown — skip
            }
            // clang-format on
        }
        return name;
    }

    for (int i = 0; i < len; i++) {
        uint8_t c = gSaveContext.playerName[i];
        if (isPAL) {
            if (c == 0x3E)
                c = ' ';
            else if (c == 0x40)
                c = '.';
            else if (c == 0x3F)
                c = '-';
            else if (c < 0x0A)
                c += '0';
            else if (c < 0x24)
                c += '7'; // 0x0A + 0x37 = 'A'
            else if (c < 0x3E)
                c += '='; // 0x24 + 0x3D = 'a'
        } else {
            if (c == 0xDF)
                c = ' ';
            else if (c == 0xEA)
                c = '.';
            else if (c == 0xE4)
                c = '-';
            else if (c < 0x0A)
                c += '0';
            else if (c < 0xC5)
                c -= 0x6A;
            else if (c < 0xDF)
                c -= 0x64;
        }
        name += (char)c;
    }
    return name;
}

// Parses a DEFINE_MESSAGE body into a TranslationEntry.
// Both proxy streams are built in parallel so the VB hook can copy either one directly
// without any conversion at message-display time.
static TranslationEntry ParseMessageContent(const std::string& body) {
    std::vector<std::vector<CustomFont::TextSegment>> pages;
    std::vector<CustomFont::TextSegment> currentPage;
    std::vector<uint8_t> proxyLatin;
    std::vector<uint16_t> proxyJpn;
    const ImVec4 white(1, 1, 1, 1);
    ImVec4 color = white;
    bool colorIsAdjustable = false;
    std::string acc;
    int8_t choiceIndex = -1;

    auto pushChar = [&]() {
        proxyLatin.push_back(0x20);
        proxyJpn.push_back(MESSAGE_SPACE_JPN);
    };

    auto flush = [&]() {
        if (!acc.empty()) {
            CustomFont::TextSegment seg;
            seg.text = acc;
            seg.color = color;
            seg.isAdjustable = colorIsAdjustable;
            seg.choiceIndex = choiceIndex;
            currentPage.push_back(seg);
            acc.clear();
        }
    };
    auto pushNewline = [&]() {
        flush();
        CustomFont::TextSegment nl;
        nl.newline = true;
        nl.color = color;
        nl.choiceIndex = (choiceIndex >= 0) ? choiceIndex : -1;
        currentPage.push_back(nl);
        if (choiceIndex >= 0)
            choiceIndex++;
        proxyLatin.push_back(MESSAGE_NEWLINE);
        proxyJpn.push_back(MESSAGE_NEWLINE_JPN);
    };

    size_t pos = 0;
    while (pos < body.size()) {
        SkipWS(body, pos);
        if (pos >= body.size())
            break;

        if (body[pos] == '"') {
            struct BtnSeq {
                const char* seq;
                const char* tex;
                int bi; // button index for BtnTintColor
            };
            static const BtnSeq kBtnSeqs[] = {
                { "[C-Up]", dgMsgCharA5ButtonCUpTex, 6 },
                { "[C-Down]", dgMsgCharA6ButtonCDownTex, 7 },
                { "[C-Left]", dgMsgCharA7ButtonCLeftTex, 8 },
                { "[C-Right]", dgMsgCharA8ButtonCRightTex, 9 },
                // "Control Pad" = analog stick (0xAA), "D-Pad" = directional cross (0xAB).
                { "[Control-Pad]", dgMsgCharAAControlStickTex, 11 },
                { "[D-Pad]", dgMsgCharABControlPadTex, 12 },
                { "[A]", dgMsgChar9FButtonATex, 0 },
                { "[B]", dgMsgCharA0ButtonBTex, 1 },
                { "[C]", dgMsgCharA1ButtonCTex, 2 },
                { "[L]", dgMsgCharA2ButtonLTex, 3 },
                { "[R]", dgMsgCharA3ButtonRTex, 4 },
                { "[Z]", dgMsgCharA4ButtonZTex, 5 },
            };

            std::string lit = ParseStringLiteral(body, pos);
            for (size_t i = 0; i < lit.size();) {
                unsigned char c = (unsigned char)lit[i];
                if (c == '\n') {
                    pushNewline();
                    i++;
                } else if (c == '[') {
                    bool matched = false;
                    for (const auto& btn : kBtnSeqs) {
                        size_t seqLen = strlen(btn.seq);
                        if (i + seqLen <= lit.size() && lit.compare(i, seqLen, btn.seq) == 0) {
                            flush();
                            CustomFont::TextSegment seg;
                            seg.btnIcon = btn.tex;
                            seg.color = BtnTintColor(btn.bi);
                            seg.choiceIndex = choiceIndex;
                            currentPage.push_back(seg);
                            pushChar();
                            i += seqLen;
                            matched = true;
                            break;
                        }
                    }
                    if (!matched) {
                        acc += '[';
                        pushChar();
                        i++;
                    }
                } else {
                    // Collect one UTF-8 character (1-4 bytes).
                    int seqLen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
                    for (int k = 0; k < seqLen && i < lit.size(); k++, i++)
                        acc += lit[i];
                    pushChar();
                }
            }
        } else if (std::isalpha((unsigned char)body[pos]) || body[pos] == '_') {
            std::string macro = ReadIdent(body, pos);
            SkipWS(body, pos);

            // Accepts hex/decimal integers ("0x4800", "5") or raw byte strings ("\x48\x00").
            auto parseArgU32 = [](const std::string& arg) -> uint32_t {
                if (arg.empty())
                    return 0;
                if (std::isdigit((unsigned char)arg[0])) {
                    try {
                        return (uint32_t)std::stoul(arg, nullptr, 0);
                    } catch (...) {}
                }
                uint32_t val = 0;
                for (unsigned char c : arg)
                    val = (val << 8) | c;
                return val;
            };
            auto parseArgByte = [&](const std::string& arg, uint8_t def) -> uint8_t {
                return arg.empty() ? def : (uint8_t)parseArgU32(arg);
            };
            auto parseArgU16 = [&](const std::string& arg) -> uint16_t { return (uint16_t)parseArgU32(arg); };

            if (pos < body.size() && body[pos] == '(') {
                ++pos;
                SkipWS(body, pos);
                std::string arg;
                if (pos < body.size() && body[pos] == '"') {
                    arg = ParseStringLiteral(body, pos);
                } else {
                    size_t argStart = pos;
                    while (pos < body.size() &&
                           (std::isalnum((unsigned char)body[pos]) || body[pos] == '_' || body[pos] == 'x'))
                        ++pos;
                    arg = body.substr(argStart, pos - argStart);
                }
                SkipWS(body, pos);
                if (pos < body.size() && body[pos] == ')')
                    ++pos;

                if (macro == "COLOR") {
                    flush();
                    colorIsAdjustable = (arg == "ADJUSTABLE");
                    color = colorIsAdjustable ? white : ColorFromName(arg, white);
                } else if (macro == "SHIFT") {
                    flush();
                    CustomFont::TextSegment shiftSeg;
                    shiftSeg.shiftX = arg.empty() ? 0.0f : (float)(unsigned char)arg[0];
                    shiftSeg.color = color;
                    shiftSeg.choiceIndex = choiceIndex;
                    currentPage.push_back(shiftSeg);
                } else if (macro == "ITEM_ICON") {
                    flush();
                    CustomFont::TextSegment seg;
                    seg.isIcon = true;
                    seg.itemId = (uint8_t)(arg.empty() ? 0 : (unsigned char)arg[0]);
                    seg.color = color;
                    seg.choiceIndex = choiceIndex;
                    currentPage.push_back(seg);
                    proxyLatin.push_back(MESSAGE_ITEM_ICON);
                    proxyLatin.push_back(seg.itemId);
                    proxyJpn.push_back(MESSAGE_ITEM_ICON_JPN);
                    proxyJpn.push_back((uint16_t)seg.itemId);
                } else if (macro == "TEXT_SPEED") {
                    const uint8_t spd = parseArgByte(arg, 2);
                    proxyLatin.push_back(MESSAGE_TEXT_SPEED);
                    proxyLatin.push_back(spd);
                    proxyJpn.push_back(MESSAGE_TEXT_SPEED_JPN);
                    proxyJpn.push_back((uint16_t)spd);
                } else if (macro == "SFX") {
                    const uint16_t sfx = parseArgU16(arg);
                    proxyLatin.push_back(MESSAGE_SFX);
                    proxyLatin.push_back((uint8_t)(sfx >> 8));
                    proxyLatin.push_back((uint8_t)(sfx & 0xFF));
                    proxyJpn.push_back(MESSAGE_SFX_JPN);
                    proxyJpn.push_back(sfx);
                } else if (macro == "BOX_BREAK_DELAYED") {
                    const uint8_t delay = parseArgByte(arg, 0);
                    proxyLatin.push_back(MESSAGE_BOX_BREAK_DELAYED);
                    proxyLatin.push_back(delay);
                    proxyJpn.push_back(MESSAGE_BOX_BREAK_DELAYED_JPN);
                    proxyJpn.push_back((uint16_t)delay);
                } else if (macro == "FADE") {
                    const uint8_t frames = parseArgByte(arg, 0);
                    proxyLatin.push_back(MESSAGE_FADE);
                    proxyLatin.push_back(frames);
                    proxyJpn.push_back(MESSAGE_FADE_JPN);
                    proxyJpn.push_back((uint16_t)frames);
                }
            } else {
                if (macro == "TWO_CHOICE" || macro == "THREE_CHOICE") {
                    flush();
                    choiceIndex = 0;
                    const bool isTwo = (macro == "TWO_CHOICE");
                    proxyLatin.push_back(isTwo ? MESSAGE_TWO_CHOICE : MESSAGE_THREE_CHOICE);
                    proxyJpn.push_back(isTwo ? MESSAGE_TWO_CHOICE_JPN : MESSAGE_THREE_CHOICE_JPN);
                } else if (macro == "BOX_BREAK") {
                    flush();
                    pages.push_back(std::move(currentPage));
                    currentPage.clear();
                    choiceIndex = -1;
                    color = white;
                    colorIsAdjustable = false;
                    proxyLatin.push_back(MESSAGE_BOX_BREAK);
                    proxyJpn.push_back(MESSAGE_BOX_BREAK_JPN);
                } else if (macro == "AWAIT_BUTTON_PRESS") {
                    proxyLatin.push_back(MESSAGE_AWAIT_BUTTON_PRESS);
                    proxyJpn.push_back(MESSAGE_AWAIT_BUTTON_PRESS_JPN);
                } else if (macro == "QUICKTEXT_ENABLE") {
                    proxyLatin.push_back(MESSAGE_QUICKTEXT_ENABLE);
                    proxyJpn.push_back(MESSAGE_QUICKTEXT_ENABLE_JPN);
                } else if (macro == "QUICKTEXT_DISABLE") {
                    proxyLatin.push_back(MESSAGE_QUICKTEXT_DISABLE);
                    proxyJpn.push_back(MESSAGE_QUICKTEXT_DISABLE_JPN);
                } else if (macro == "PERSISTENT") {
                    proxyLatin.push_back(MESSAGE_PERSISTENT);
                    proxyJpn.push_back(MESSAGE_PERSISTENT_JPN);
                } else if (macro == "UNSKIPPABLE") {
                    proxyLatin.push_back(MESSAGE_UNSKIPPABLE);
                    proxyJpn.push_back(MESSAGE_UNSKIPPABLE_JPN);
                } else if (macro == "EVENT") {
                    proxyLatin.push_back(MESSAGE_EVENT);
                    proxyJpn.push_back(MESSAGE_EVENT_JPN);
                } else if (macro == "NAME") {
                    flush();
                    CustomFont::TextSegment nameSeg;
                    nameSeg.isName = true;
                    nameSeg.color = color;
                    nameSeg.isAdjustable = colorIsAdjustable;
                    nameSeg.choiceIndex = choiceIndex;
                    currentPage.push_back(nameSeg);
                    const std::string pname = GetPlayerName();
                    for (unsigned char b : pname)
                        if ((b & 0xC0) != 0x80)
                            pushChar();
                } else if (macro == "OCARINA") {
                    proxyLatin.push_back(MESSAGE_OCARINA);
                    proxyJpn.push_back(MESSAGE_OCARINA_JPN);
                } else {
                    const char* tex = BtnIconFromMacro(macro);
                    if (tex) {
                        flush();
                        CustomFont::TextSegment seg;
                        seg.btnIcon = tex;
                        seg.color = BtnColorFromMacro(macro);
                        seg.choiceIndex = choiceIndex;
                        currentPage.push_back(seg);
                        pushChar();
                    }
                }
            }
        } else {
            ++pos;
        }
    }
    flush();
    pages.push_back(std::move(currentPage));
    proxyLatin.push_back(MESSAGE_END);
    proxyJpn.push_back(MESSAGE_END_JPN);
    return { pages, proxyLatin, proxyJpn };
}

static void ParseTranslationFile(const std::string& text) {
    size_t pos = 0;
    while (pos < text.size()) {
        size_t found = text.find("DEFINE_MESSAGE(", pos);
        if (found == std::string::npos)
            break;
        pos = found + 15;

        SkipWS(text, pos);

        uint16_t msgId = 0;
        if (pos + 1 < text.size() && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X')) {
            pos += 2;
            while (pos < text.size() && std::isxdigit((unsigned char)text[pos])) {
                msgId = (uint16_t)(msgId * 16 + (std::isdigit((unsigned char)text[pos])
                                                     ? text[pos] - '0'
                                                     : std::tolower((unsigned char)text[pos]) - 'a' + 10));
                ++pos;
            }
        } else {
            while (pos < text.size() && std::isdigit((unsigned char)text[pos]))
                msgId = (uint16_t)(msgId * 10 + (text[pos++] - '0'));
        }

        // Skip textboxType and textboxPos args.
        for (int commas = 0; commas < 2 && pos < text.size();) {
            if (text[pos] == ',')
                commas++;
            ++pos;
            SkipWS(text, pos);
            ReadIdent(text, pos);
        }
        while (pos < text.size() && text[pos] != ',')
            ++pos;
        if (pos < text.size())
            ++pos;

        // Collect body with paren depth tracking to handle nested macro parens.
        int depth = 1;
        size_t bodyStart = pos;
        while (pos < text.size() && depth > 0) {
            if (text[pos] == '(')
                depth++;
            else if (text[pos] == ')')
                depth--;
            if (depth > 0)
                ++pos;
            else
                break;
        }
        std::string body = text.substr(bodyStart, pos - bodyStart);
        if (pos < text.size())
            ++pos;

        sActiveTranslation[msgId] = ParseMessageContent(body);
    }
}

const std::vector<std::string>& CustomFont::GetTranslationNames() {
    static std::vector<std::string> names;
    names.clear();
    names.push_back("None");
    for (const auto& f : sTranslationFiles)
        names.push_back(f.name);
    return names;
}

void CustomFont::LoadTranslation(const std::string& name) {
    sActiveTranslation.clear();
    if (name == "None" || name.empty())
        return;

    auto archiveMgr = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
    for (const auto& tf : sTranslationFiles) {
        if (tf.name != name)
            continue;
        auto raw = archiveMgr->LoadFile(tf.path);
        if (!raw || !raw->IsLoaded || !raw->Buffer || raw->Buffer->empty())
            continue;
        std::string text(raw->Buffer->begin(), raw->Buffer->end());
        ParseTranslationFile(text);
    }
}

void CustomFont::RegisterModFont(const std::string& name, ImFont* font) {
    sModFontNames.push_back(name);
    sModFontPtrs.push_back(font);
}

const std::vector<std::string>& CustomFont::GetModFontNames() {
    return sModFontNames;
}

ImFont* CustomFont::GetModFont(const std::string& name) {
    for (size_t i = 0; i < sModFontNames.size(); i++)
        if (sModFontNames[i] == name)
            return sModFontPtrs[i];
    return nullptr;
}

static int sCurrentTransPage = 0;
static bool sLastDecodeWasJpn = false; // true when Message_DecodeJPN ran, false for Latin decode path

void CustomFont::InitElement() {
    REGISTER_VB_SHOULD(VB_DRAW_MESSAGE_TEXT, {
        if (CVarGetInteger(CVAR_SETTING("AltAssets"), 1) && CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0))
            *should = false;
    });
    REGISTER_VB_SHOULD(VB_DRAW_ITEM_ICON, {
        if (CVarGetInteger(CVAR_SETTING("AltAssets"), 1) && CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0))
            *should = false;
    });

    REGISTER_VB_SHOULD(VB_MESSAGE_DECODED, {
        if (!CVarGetInteger(CVAR_SETTING("AltAssets"), 1) || !CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0))
            return;
        PlayState* play = va_arg(args, PlayState*);
        int pageNum = va_arg(args, int);
        MessageContext* msgCtx = &play->msgCtx;

        sCurrentTransPage = pageNum;

        {
            // Message_DecodeJPN writes a JPN terminator at decodedTextLen; Message_Decode never touches
            // msgBufDecodedWide.
            const u16 term = msgCtx->msgBufDecodedWide[msgCtx->decodedTextLen];
            sLastDecodeWasJpn =
                (term == MESSAGE_END_JPN || term == MESSAGE_BOX_BREAK_JPN || term == MESSAGE_BOX_BREAK_DELAYED_JPN);
        }

        auto it = sActiveTranslation.find(msgCtx->textId);
        if (it == sActiveTranslation.end())
            return;

        const auto& proxyLatin = it->second.proxyLatin;
        size_t pageStart = 0;
        for (int p = 0; p < pageNum; p++) {
            while (pageStart < proxyLatin.size() && proxyLatin[pageStart] != MESSAGE_BOX_BREAK)
                pageStart++;
            if (pageStart < proxyLatin.size())
                pageStart++; // skip BOX_BREAK
        }

        if (gSaveContext.language == LANGUAGE_JPN && sLastDecodeWasJpn) {
            const auto& jpn = it->second.proxyJpn;
            size_t jpnStart = 0;
            for (int p = 0; p < pageNum; p++) {
                while (jpnStart < jpn.size() && jpn[jpnStart] != MESSAGE_BOX_BREAK_JPN)
                    jpnStart++;
                if (jpnStart < jpn.size())
                    jpnStart++; // skip BOX_BREAK_JPN itself
            }
            size_t dst = 0;
            for (size_t src = jpnStart; src < jpn.size() && dst < 99; src++, dst++) {
                msgCtx->msgBufDecodedWide[dst] = jpn[src];
                if (jpn[src] == MESSAGE_BOX_BREAK_JPN || jpn[src] == MESSAGE_END_JPN) {
                    dst++;
                    break;
                }
            }
            if (dst < 100)
                msgCtx->msgBufDecodedWide[dst] = MESSAGE_END_JPN;
            msgCtx->decodedTextLen = (u16)(dst > 0 ? dst - 1 : 0);
            return;
        }

        size_t dst = 0;
        for (size_t src = pageStart; src < proxyLatin.size() && dst < sizeof(msgCtx->msgBufDecoded) - 1; src++, dst++) {
            msgCtx->msgBufDecoded[dst] = proxyLatin[src];
            if (proxyLatin[src] == MESSAGE_BOX_BREAK || proxyLatin[src] == MESSAGE_END) {
                dst++;
                break;
            }
        }
        if (dst < sizeof(msgCtx->msgBufDecoded))
            msgCtx->msgBufDecoded[dst] = MESSAGE_END;
        msgCtx->decodedTextLen = (u16)(dst > 0 ? dst - 1 : 0);
    });

    static const char* const kBuiltins[] = {
        "fonts/PressStart2P-Regular.ttf", "fonts/Fipps-Regular.otf",      "fonts/Inconsolata-Regular.ttf",
        "fonts/Montserrat-Regular.ttf",   "fonts/NotoSansJP-Regular.ttf",
    };

    auto& io = ImGui::GetIO();
    auto archiveMgr = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
    auto modFontFiles = archiveMgr->ListFiles({ "fonts/*.ttf", "fonts/*.otf" }, {});
    for (const auto& path : *modFontFiles) {
        bool isBuiltin = false;
        for (const auto* b : kBuiltins)
            if (path == b) {
                isBuiltin = true;
                break;
            }
        if (isBuiltin)
            continue;

        auto rawFile = archiveMgr->LoadFile(path);
        if (!rawFile || !rawFile->IsLoaded || !rawFile->Buffer || rawFile->Buffer->empty())
            continue;

        sModFontData.push_back(*rawFile->Buffer);

        // Full BMP glyph range — ImGui skips codepoints not present in the TTF.
        static constexpr ImWchar kFullBMP[] = { 0x0020, 0xFFFF, 0 };

        ImFontConfig conf;
        conf.FontDataOwnedByAtlas = false;
        conf.OversampleH = 1;
        conf.OversampleV = 1;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(sModFontData.back().data(), (int)sModFontData.back().size(),
                                                      64.0f, &conf, kFullBMP);
        if (!font) {
            sModFontData.pop_back();
            continue;
        }

        std::string name = path.substr(path.rfind('/') + 1);
        name = name.substr(0, name.rfind('.'));
        RegisterModFont(name, font);
    }

    // Force atlas re-upload if it was already built before our fonts were added.
    if (!sModFontNames.empty() && io.Fonts->IsBuilt()) {
        io.Fonts->TexID = nullptr;
    }

    for (const auto* glob : { "translations/*.txt", "translations/*.json" }) {
        auto files = archiveMgr->ListFiles(glob);
        for (const auto& path : *files) {
            std::string stem = path.substr(path.rfind('/') + 1);
            stem = stem.substr(0, stem.rfind('.'));
            sTranslationFiles.push_back(TranslationFile{ stem, path });
        }
    }

    // Deduplicate by name, keeping the first occurrence (highest-priority archive).
    {
        std::vector<TranslationFile> deduped;
        std::set<std::string> seen;
        for (auto& tf : sTranslationFiles) {
            if (seen.insert(tf.name).second)
                deduped.push_back(tf);
        }
        sTranslationFiles = std::move(deduped);
    }

    const std::string selected = CVarGetString(CVAR_CUSTOM_FONT_TRANSLATION, "None");
    LoadTranslation(selected);
}

void CustomFont::Draw() {
    if (!CVarGetInteger(CVAR_SETTING("AltAssets"), 1) || !CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0)) {
        return;
    }
    if (!gPlayState) {
        return;
    }

    const MessageContext* msgCtx = &gPlayState->msgCtx;

    switch (msgCtx->msgMode) {
        case MSGMODE_TEXT_DISPLAYING:
        case MSGMODE_TEXT_DELAYED_BREAK:
        case MSGMODE_TEXT_AWAIT_INPUT:
        case MSGMODE_TEXT_AWAIT_NEXT:
        case MSGMODE_TEXT_DONE:
            break;
        default:
            return;
    }
    if (R_TEXTBOX_WIDTH == 0 || R_TEXTBOX_HEIGHT == 0) {
        return;
    }

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 gamePos = vp->Pos;
    const ImVec2 gameSize = vp->Size;
    const float scale = gameSize.y / 240.0f;
    const float xOffset = (gameSize.x - 320.0f * scale) / 2.0f;

    const float winX = gamePos.x + xOffset + R_TEXTBOX_X * scale;
    const float winY = gamePos.y + R_TEXTBOX_Y * scale;
    const float winW = R_TEXTBOX_WIDTH * scale;
    const float winH = R_TEXTBOX_HEIGHT * scale;

    ImGui::SetNextWindowPos(ImVec2(winX, winY));
    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::SetNextWindowBgAlpha(0.0f);

    const ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                                    ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings |
                                    ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("##CustomFontOverlay", nullptr, kFlags)) {
        ImGui::PopStyleVar();
        DrawElement();
    } else {
        ImGui::PopStyleVar();
    }

    ImGui::End();
}

static std::string Utf8FromCodepoint(uint32_t cp) {
    std::string s;
    if (cp < 0x800) {
        s += (char)(0xC0 | (cp >> 6));
        s += (char)(0x80 | (cp & 0x3F));
    } else {
        s += (char)(0xE0 | (cp >> 12));
        s += (char)(0x80 | ((cp >> 6) & 0x3F));
        s += (char)(0x80 | (cp & 0x3F));
    }
    return s;
}

static std::string SjisToUtf8(uint16_t c) {
    if (c >= 0x829F && c <= 0x82F1)
        return Utf8FromCodepoint(0x3041 + (c - 0x829F)); // hiragana
    if (c >= 0x8340 && c <= 0x837E)
        return Utf8FromCodepoint(0x30A1 + (c - 0x8340)); // katakana
    if (c >= 0x8380 && c <= 0x8396)
        return Utf8FromCodepoint(0x30E0 + (c - 0x8380)); // katakana cont.
    if (c >= 0x824F && c <= 0x8258)
        return Utf8FromCodepoint(0xFF10 + (c - 0x824F)); // full-width 0-9
    if (c >= 0x8260 && c <= 0x8279)
        return Utf8FromCodepoint(0xFF21 + (c - 0x8260)); // full-width A-Z
    if (c >= 0x8281 && c <= 0x829A)
        return Utf8FromCodepoint(0xFF41 + (c - 0x8281)); // full-width a-z
    static constexpr struct {
        uint16_t sjis;
        uint32_t cp;
    } kPunct[] = {
        { 0x8140, 0x3000 }, // 　 ideographic space
        { 0x8141, 0x3001 }, // 、
        { 0x8142, 0x3002 }, // 。
        { 0x8145, 0x30FB }, // ・
        { 0x8148, 0xFF1F }, // ？
        { 0x8149, 0xFF01 }, // ！
        { 0x8156, 0x2015 }, // ―
        { 0x8158, 0x2026 }, // …
        { 0x815B, 0x30FC }, // ー (long vowel mark)
        { 0x8162, 0x300C }, // 「
        { 0x8163, 0x300D }, // 」
        { 0x8164, 0x300E }, // 『
        { 0x8165, 0x300F }, // 』
        { 0x8168, 0xFF08 }, // （
        { 0x8169, 0xFF09 }, // ）
    };
    for (const auto& p : kPunct)
        if (p.sjis == c)
            return Utf8FromCodepoint(p.cp);

    // Kanji and other unmapped double-byte SJS: convert via SDL's iconv wrapper.
    // Result is cached — SDL_iconv_string is called at most once per unique code point.
    const uint8_t b1 = (c >> 8) & 0xFF;
    const uint8_t b2 = c & 0xFF;
    if (b1 < 0x81 || b2 < 0x40)
        return {};

    static std::unordered_map<uint16_t, std::string> sCache;
    auto cit = sCache.find(c);
    if (cit != sCache.end())
        return cit->second;

    char sjis[2] = { (char)b1, (char)b2 };
    char* utf8 = SDL_iconv_string("UTF-8", "SHIFT_JIS", sjis, 2);
    std::string result = utf8 ? utf8 : "";
    SDL_free(utf8);
    sCache[c] = result;
    return result;
}

// Parses msgBufDecodedWide (Shift-JIS u16 stream) up to drawLen entries into
// renderable TextSegments.  Used for untranslated JPN messages so that hiragana
// and katakana reach the ImGui renderer instead of the u8 alias garbage.
static std::vector<CustomFont::TextSegment> ParseJpnBuffer(const uint16_t* buf, uint16_t drawLen) {
    std::vector<CustomFont::TextSegment> out;
    const ImVec4 white(1, 1, 1, 1);
    ImVec4 color = white;
    std::string acc;
    bool done = false;
    int8_t choiceIndex = -1;

    auto flush = [&]() {
        if (!acc.empty()) {
            CustomFont::TextSegment seg;
            seg.text = acc;
            seg.color = color;
            seg.choiceIndex = choiceIndex;
            out.push_back(seg);
            acc.clear();
        }
    };

    const uint16_t safeLen = std::min(drawLen, (uint16_t)100);
    for (uint16_t i = 0; i < safeLen && !done; i++) {
        const uint16_t c = buf[i];
        switch (c) {
            case MESSAGE_NEWLINE_JPN:
                flush();
                {
                    CustomFont::TextSegment nl;
                    nl.newline = true;
                    nl.color = color;
                    nl.choiceIndex = (choiceIndex >= 0) ? choiceIndex : -1;
                    out.push_back(nl);
                    if (choiceIndex == -2)
                        choiceIndex = 0;
                    else if (choiceIndex >= 0)
                        choiceIndex++;
                }
                break;
            case MESSAGE_END_JPN:
            case MESSAGE_BOX_BREAK_JPN:
            case MESSAGE_PERSISTENT_JPN:
            case MESSAGE_EVENT_JPN:
            case MESSAGE_AWAIT_BUTTON_PRESS_JPN:
            case MESSAGE_OCARINA_JPN:
                flush();
                done = true;
                break;
            case MESSAGE_BOX_BREAK_DELAYED_JPN:
            case MESSAGE_FADE_JPN:
                flush();
                i++;
                done = true;
                break;
            case MESSAGE_COLOR_JPN:
                flush();
                if (i + 1 < safeLen)
                    color = CustomFont::ColorFromCode((uint8_t)buf[++i] & 0x0F, white);
                break;
            case MESSAGE_ITEM_ICON_JPN:
                flush();
                if (i + 1 < safeLen) {
                    CustomFont::TextSegment iconSeg;
                    iconSeg.isIcon = true;
                    iconSeg.itemId = (uint8_t)buf[++i];
                    iconSeg.color = color;
                    iconSeg.choiceIndex = choiceIndex;
                    out.push_back(iconSeg);
                }
                break;
            case MESSAGE_SHIFT_JPN:
                flush();
                if (i + 1 < safeLen) {
                    CustomFont::TextSegment shiftSeg;
                    shiftSeg.shiftX = (float)(uint16_t)buf[++i];
                    shiftSeg.color = color;
                    shiftSeg.choiceIndex = choiceIndex;
                    out.push_back(shiftSeg);
                }
                break;
            case MESSAGE_TEXT_SPEED_JPN:
            case MESSAGE_HIGHSCORE_JPN:
                i++; // skip arg
                break;
            case MESSAGE_SFX_JPN:
                i++; // skip sfx code (already consumed as one u16 entry)
                break;
            case MESSAGE_NAME_JPN:
                flush();
                {
                    CustomFont::TextSegment nameSeg;
                    nameSeg.isName = true;
                    nameSeg.color = color;
                    nameSeg.choiceIndex = choiceIndex;
                    out.push_back(nameSeg);
                }
                break;
            case MESSAGE_SPACE_JPN:
                acc += ' ';
                break;
            case MESSAGE_TWO_CHOICE_JPN:
            case MESSAGE_THREE_CHOICE_JPN:
                flush();
                choiceIndex = -2;
                break;
            case MESSAGE_QUICKTEXT_ENABLE_JPN:
            case MESSAGE_QUICKTEXT_DISABLE_JPN:
            case MESSAGE_UNSKIPPABLE_JPN:
                break;
            default: {
                if (c >= 0x839F && c <= 0x83AA) { // JPN button icons
                    static const char* sJpnBtnTexNames[] = {
                        dgMsgChar9FButtonATex,      // 0x839F A
                        dgMsgCharA0ButtonBTex,      // 0x83A0 B
                        dgMsgCharA1ButtonCTex,      // 0x83A1 C
                        dgMsgCharA2ButtonLTex,      // 0x83A2 L
                        dgMsgCharA3ButtonRTex,      // 0x83A3 R
                        dgMsgCharA4ButtonZTex,      // 0x83A4 Z
                        dgMsgCharA5ButtonCUpTex,    // 0x83A5 C-Up
                        dgMsgCharA6ButtonCDownTex,  // 0x83A6 C-Down
                        dgMsgCharA7ButtonCLeftTex,  // 0x83A7 C-Left
                        dgMsgCharA8ButtonCRightTex, // 0x83A8 C-Right
                        dgMsgCharA9ZTargetSignTex,  // 0x83A9 Z-target
                        dgMsgCharAAControlStickTex, // 0x83AA Control Stick
                    };
                    static const ImVec4 sJpnBtnColors[] = {
                        {},                                // A — via ABtnColor()
                        {},                                // B — via BBtnColor()
                        {},                                // C — via CBtnGroupColor()
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // L
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // R
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // Z
                        {},                                // C-Up — via CUpBtnColor()
                        {},                                // C-Down — via CDownBtnColor()
                        {},                                // C-Left — via CLeftBtnColor()
                        {},                                // C-Right — via CRightBtnColor()
                        ImVec4(0.00f, 0.82f, 0.20f, 1.0f), // Z-target
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // Control Stick
                    };
                    const int bi = c - 0x839F;
                    flush();
                    if (choiceIndex == -2)
                        choiceIndex = 0;
                    CustomFont::TextSegment seg;
                    seg.btnIcon = sJpnBtnTexNames[bi];
                    seg.color = (bi == 0)   ? ABtnColor()
                                : (bi == 1) ? BBtnColor()
                                : (bi == 2) ? CBtnGroupColor()
                                : (bi == 6) ? CUpBtnColor()
                                : (bi == 7) ? CDownBtnColor()
                                : (bi == 8) ? CLeftBtnColor()
                                : (bi == 9) ? CRightBtnColor()
                                            : sJpnBtnColors[bi];
                    seg.choiceIndex = choiceIndex;
                    out.push_back(std::move(seg));
                    break;
                }
                const std::string utf8 = SjisToUtf8(c);
                if (!utf8.empty()) {
                    if (choiceIndex == -2)
                        choiceIndex = 0;
                    acc += utf8;
                }
                break;
            }
        }
    }
    flush();
    return out;
}

void CustomFont::DrawElement() {
    const MessageContext* msgCtx = &gPlayState->msgCtx;

    static bool sPrevAltAssets = ResourceMgr_IsAltAssetsEnabled();
    static bool sBtnTexturesLoaded = false;
    static bool sIconLoaded[158] = {};

    const bool curAltAssets = ResourceMgr_IsAltAssetsEnabled();
    if (curAltAssets != sPrevAltAssets) {
        sBtnTexturesLoaded = false;
        std::fill(std::begin(sIconLoaded), std::end(sIconLoaded), false);
        sPrevAltAssets = curAltAssets;
    }

    if (!sBtnTexturesLoaded) {
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
        const ImVec4 white(1, 1, 1, 1);

        auto loadBtn = [&](const char* otrPath) {
            static constexpr int kOtrPrefixLen = 7;
            const std::string bare = std::string(otrPath + kOtrPrefixLen);
            if (curAltAssets) {
                const std::string altPath = "alt/" + bare;
                if (ResourceMgr_FileAltExists(bare.c_str())) {
                    gui->LoadGuiTexture(otrPath, altPath, white);
                    return;
                }
            }
            if (ResourceMgr_FileExists(bare.c_str())) {
                gui->LoadGuiTexture(otrPath, otrPath, white);
            }
        };

        loadBtn(dgMsgChar9FButtonATex);
        loadBtn(dgMsgCharA0ButtonBTex);
        loadBtn(dgMsgCharA1ButtonCTex);
        loadBtn(dgMsgCharA2ButtonLTex);
        loadBtn(dgMsgCharA3ButtonRTex);
        loadBtn(dgMsgCharA4ButtonZTex);
        loadBtn(dgMsgCharA5ButtonCUpTex);
        loadBtn(dgMsgCharA6ButtonCDownTex);
        loadBtn(dgMsgCharA7ButtonCLeftTex);
        loadBtn(dgMsgCharA8ButtonCRightTex);
        loadBtn(dgMsgCharA9ZTargetSignTex);
        loadBtn(dgMsgCharAAControlStickTex);
        loadBtn(dgMsgCharABControlPadTex);
        sBtnTexturesLoaded = true;
    }

    static const struct {
        const char* name;
        float size;
    } kBuiltinFonts[] = {
        { "Press Start 2P", 12.0f },
        { "Fipps", 32.0f },
    };

    const std::string fontName = CVarGetString(CVAR_CUSTOM_FONT_NAME, "Default");
    ImFont* selectedFont = nullptr;
    if (fontName != "Default") {
        for (const auto& kf : kBuiltinFonts) {
            if (fontName == kf.name) {
                for (ImFont* f : ImGui::GetIO().Fonts->Fonts) {
                    if (f->FontSize == kf.size) {
                        selectedFont = f;
                        break;
                    }
                }
                break;
            }
        }
        if (!selectedFont)
            selectedFont = GetModFont(fontName);
    }
    ImGui::PushFont(selectedFont);

    ImFont* activeFont = selectedFont ? selectedFont : ImGui::GetIO().Fonts->Fonts[0];

    const float scaleX = ImGui::GetWindowWidth() / (float)R_TEXTBOX_WIDTH;
    const float scaleY = ImGui::GetWindowHeight() / (float)R_TEXTBOX_HEIGHT;
    const float cursorX = (R_TEXT_INIT_XPOS - R_TEXTBOX_X) * scaleX;
    const float cursorY = (R_TEXT_INIT_YPOS - R_TEXTBOX_Y) * scaleY;

    const auto* trans = [&]() -> const std::vector<TextSegment>* {
        auto it = sActiveTranslation.find(msgCtx->textId);
        if (it == sActiveTranslation.end())
            return nullptr;
        const auto& pages = it->second.pages;
        if (pages.empty())
            return nullptr;
        int page = std::min(sCurrentTransPage, (int)pages.size() - 1);
        return &pages[page];
    }();

    const bool isJpnTrans = (gSaveContext.language == LANGUAGE_JPN && sLastDecodeWasJpn && trans != nullptr);
    const bool isUntranslatedJpn = (gSaveContext.language == LANGUAGE_JPN && sLastDecodeWasJpn && !isJpnTrans);
    const auto origFull = isJpnTrans          ? std::vector<TextSegment>{}
                          : isUntranslatedJpn ? ParseJpnBuffer(msgCtx->msgBufDecodedWide, 100)
                                              : ParseDecodedBuffer(msgCtx->msgBufDecoded, 0xFFFF);
    const auto origTyped = isJpnTrans          ? std::vector<TextSegment>{}
                           : isUntranslatedJpn ? ParseJpnBuffer(msgCtx->msgBufDecodedWide, msgCtx->textDrawPos)
                                               : ParseDecodedBuffer(msgCtx->msgBufDecoded, msgCtx->textDrawPos);

    auto countChars = [](const std::vector<TextSegment>& segs) -> size_t {
        size_t n = 0;
        for (const auto& s : segs)
            if (!s.newline && !s.isIcon && s.btnIcon.empty())
                for (unsigned char c : s.text)
                    if ((c & 0xC0) != 0x80)
                        n++;
        return n;
    };

    auto limitSegs = [](const std::vector<TextSegment>& segs, size_t limit) {
        std::vector<TextSegment> out;
        size_t count = 0;
        for (const auto& s : segs) {
            if (s.newline || s.isIcon || !s.btnIcon.empty() || s.shiftX != 0.0f) {
                if (count < limit)
                    out.push_back(s);
                continue;
            }
            if (s.isName) {
                if (count < limit)
                    out.push_back(s);
                for (unsigned char b : GetPlayerName())
                    if ((b & 0xC0) != 0x80)
                        count++;
                if (count >= limit)
                    break;
                continue;
            }
            TextSegment trimmed = s;
            trimmed.text.clear();
            for (size_t i = 0; i < s.text.size() && count < limit;) {
                unsigned char c = (unsigned char)s.text[i];
                int seqLen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
                for (int k = 0; k < seqLen && i < s.text.size(); k++, i++)
                    trimmed.text += s.text[i];
                count++;
            }
            if (!trimmed.text.empty())
                out.push_back(trimmed);
            if (count >= limit)
                break;
        }
        return out;
    };

    const auto& fullSegments = trans ? *trans : origFull;
    const auto segments =
        trans ? limitSegs(*trans, isJpnTrans ? (size_t)msgCtx->textDrawPos : countChars(origTyped)) : origTyped;

    bool hasItemIcon = false;
    uint8_t itemIconId = 0;
    for (const auto& seg : fullSegments) {
        if (seg.isIcon) {
            hasItemIcon = true;
            itemIconId = seg.itemId;
            break;
        }
    }

    bool hasChoices = false;
    for (const auto& seg : fullSegments) {
        if (seg.choiceIndex >= 0) {
            hasChoices = true;
            break;
        }
    }
    const float choiceStartY = hasChoices ? (R_TEXT_CHOICE_YPOS(0) - R_TEXTBOX_Y) * scaleY : ImGui::GetWindowHeight();

    struct LineInfo {
        std::string text;
        int btnIconCount = 0;
    };
    std::vector<LineInfo> lineInfos;
    {
        LineInfo cur;
        for (const auto& seg : fullSegments) {
            if (seg.choiceIndex >= 0)
                continue;
            if (seg.newline) {
                lineInfos.push_back(cur);
                cur = {};
            } else if (!seg.btnIcon.empty()) {
                cur.btnIconCount++;
            } else if (seg.isName) {
                cur.text += GetPlayerName();
            } else if (!seg.isIcon) {
                cur.text += seg.text;
            }
        }
        lineInfos.push_back(cur);
    }

    const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
    const int numLines = std::max((int)lineInfos.size(), 1);
    const float desiredSize = (R_TEXT_CHAR_SCALE / 100.0f) * 16.0f * scaleY;

    const float iconSize = hasItemIcon ? (float)R_TEXTBOX_ICON_SIZE * scaleX : 0.0f;
    const float iconColumnWidth = hasItemIcon ? 32.0f * scaleX : 0.0f;

    const float availableWidth = ImGui::GetWindowWidth() - 2.0f * cursorX - iconColumnWidth;

    float maxLineWidth = 0.0f;
    for (const auto& li : lineInfos) {
        float w = li.btnIconCount * desiredSize;
        if (!li.text.empty())
            w += activeFont->CalcTextSizeA(desiredSize, FLT_MAX, 0.0f, li.text.c_str()).x;
        if (w > maxLineWidth)
            maxLineWidth = w;
    }

    float effectiveSize = desiredSize;
    if (gSaveContext.language != LANGUAGE_JPN && maxLineWidth > availableWidth && maxLineWidth > 0.0f)
        effectiveSize = desiredSize * (availableWidth / maxLineWidth);
    effectiveSize = std::max(effectiveSize, 1.0f);

    const float lineSpacing = effectiveSize + itemSpacing;

    struct LineLayout {
        float startX = 0.0f;
        bool centered = false;
    };
    std::vector<LineLayout> lineLayouts;
    {
        bool firstOnLine = true;
        bool lineHasShift = false;
        float lineContentW = 0.0f;

        auto finishLine = [&]() {
            LineLayout ll;
            ll.centered = lineHasShift;
            ll.startX = lineHasShift ? std::max(0.0f, (availableWidth - lineContentW) / 2.0f) : 0.0f;
            lineLayouts.push_back(ll);
            firstOnLine = true;
            lineHasShift = false;
            lineContentW = 0.0f;
        };

        for (const auto& s : segments) {
            if (s.choiceIndex >= 0)
                continue;
            if (s.newline) {
                finishLine();
                continue;
            }
            if (s.isIcon)
                continue;
            if (s.shiftX != 0.0f) {
                if (firstOnLine)
                    lineHasShift = true;
                continue;
            }
            firstOnLine = false;
            if (!s.btnIcon.empty())
                lineContentW += effectiveSize;
            else if (s.isName) {
                const std::string pname = GetPlayerName();
                lineContentW += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, pname.c_str()).x;
            } else if (!s.text.empty())
                lineContentW += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, s.text.c_str()).x;
        }
        finishLine();
    }

    const float totalTextHeight = numLines * lineSpacing;
    const float vOffset = std::max(0.0f, (choiceStartY - totalTextHeight) * 0.5f - cursorY);

    auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();

    if (hasItemIcon && itemIconId < 158) {
        const char* iconPath = static_cast<char*>(gItemIcons[itemIconId]);
        if (iconPath && !sIconLoaded[itemIconId]) {
            const ImVec4 white(1, 1, 1, 1);
            static constexpr int kOtrPrefixLen = 7;
            if (curAltAssets) {
                const std::string bare = std::string(iconPath + kOtrPrefixLen);
                const std::string altPath = "alt/" + bare;
                if (ResourceMgr_FileAltExists(bare.c_str())) {
                    gui->LoadGuiTexture(iconPath, altPath, white);
                    sIconLoaded[itemIconId] = true;
                }
            }
            if (!sIconLoaded[itemIconId]) {
                gui->LoadGuiTexture(iconPath, iconPath, white);
                sIconLoaded[itemIconId] = true;
            }
        }
    }

    const ImVec4 white(1, 1, 1, 1);
    auto resolveColor = [&](const CustomFont::TextSegment& s) -> ImVec4 {
        return s.isAdjustable ? ColorFromCode(MSGCOL_ADJUSTABLE, white) : s.color;
    };

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();
    int lineNum = 0;
    float lineX = lineLayouts.empty() ? 0.0f : lineLayouts[0].startX;

    for (const auto& seg : segments) {
        if (seg.choiceIndex >= 0)
            continue;

        if (seg.newline) {
            lineNum++;
            lineX = lineNum < (int)lineLayouts.size() ? lineLayouts[lineNum].startX : 0.0f;
            continue;
        }

        if (seg.isIcon)
            continue;

        if (seg.shiftX != 0.0f) {
            // Line-start SHIFT on a centered line is already baked into lineLayouts.startX.
            if (lineNum < (int)lineLayouts.size() && lineLayouts[lineNum].centered)
                continue;
            lineX += seg.shiftX * scaleX;
            continue;
        }

        const float sx = winPos.x + cursorX + iconColumnWidth + lineX;
        const float sy = winPos.y + cursorY + vOffset + lineNum * lineSpacing;

        if (!seg.btnIcon.empty()) {
            ImTextureID texId = gui->GetTextureByName(seg.btnIcon);
            if (texId) {
                const ImU32 tint = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
                dl->AddImage(texId, ImVec2(sx, sy), ImVec2(sx + effectiveSize, sy + effectiveSize), ImVec2(0, 0),
                             ImVec2(1, 1), tint);
                lineX += effectiveSize;
            }
            continue;
        }

        if (seg.isName) {
            const std::string pname = GetPlayerName();
            if (!pname.empty()) {
                const ImU32 col = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
                dl->AddText(activeFont, effectiveSize, ImVec2(sx, sy), col, pname.c_str());
                lineX += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, pname.c_str()).x;
            }
            continue;
        }

        if (seg.text.empty())
            continue;

        const ImU32 col = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
        dl->AddText(activeFont, effectiveSize, ImVec2(sx, sy), col, seg.text.c_str());
        lineX += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, seg.text.c_str()).x;
    }

    if (hasItemIcon && itemIconId < 158) {
        const char* iconPath = static_cast<char*>(gItemIcons[itemIconId]);
        ImTextureID texId = iconPath ? gui->GetTextureByName(iconPath) : nullptr;
        if (texId) {
            const float iconX = winPos.x + (R_TEXT_INIT_XPOS + R_TEXTBOX_ICON_XPOS - R_TEXTBOX_X) * scaleX;
            const float iconY = winPos.y + (ImGui::GetWindowHeight() - iconSize) * 0.5f;
            dl->AddImage(texId, ImVec2(iconX, iconY), ImVec2(iconX + iconSize, iconY + iconSize));
        }
    }

    if (hasChoices) {
        // TWO_CHOICE uses YPOS(index+1); THREE_CHOICE uses YPOS(index) directly.
        // Detect by max choiceIndex: max==1 → TWO_CHOICE (+1 offset).
        int8_t maxChoiceIdx = 0;
        for (const auto& seg : fullSegments)
            if (seg.choiceIndex > maxChoiceIdx)
                maxChoiceIdx = seg.choiceIndex;
        const int8_t yposOffset = (maxChoiceIdx == 1) ? 1 : 0;

        float choiceLineX = 0.0f;
        int8_t prevChoice = -1;

        for (const auto& seg : fullSegments) {
            if (seg.choiceIndex < 0)
                continue;

            if (seg.newline) {
                choiceLineX = 0.0f;
                continue;
            }
            if (seg.isIcon)
                continue;

            if (seg.choiceIndex != prevChoice) {
                choiceLineX = 0.0f;
                prevChoice = seg.choiceIndex;
            }

            // Vanilla choice text is indented 32 N64px (to clear the selection arrow).
            const float vanillaCharH = (R_TEXT_CHAR_SCALE / 100.0f) * 16.0f * scaleY;
            const float baseX = winPos.x + cursorX + 32.0f * scaleX;
            const float baseY = winPos.y + (R_TEXT_CHOICE_YPOS(seg.choiceIndex + yposOffset) - R_TEXTBOX_Y) * scaleY +
                                (vanillaCharH - effectiveSize) * 0.5f;

            if (!seg.btnIcon.empty()) {
                ImTextureID texId = gui->GetTextureByName(seg.btnIcon);
                if (texId) {
                    const ImU32 tint = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
                    dl->AddImage(texId, ImVec2(baseX + choiceLineX, baseY),
                                 ImVec2(baseX + choiceLineX + effectiveSize, baseY + effectiveSize), ImVec2(0, 0),
                                 ImVec2(1, 1), tint);
                    choiceLineX += effectiveSize;
                }
                continue;
            }

            if (seg.text.empty())
                continue;

            const ImU32 col = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
            dl->AddText(activeFont, effectiveSize, ImVec2(baseX + choiceLineX, baseY), col, seg.text.c_str());
            choiceLineX += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, seg.text.c_str()).x;
        }
    }

    ImGui::PopFont();
}

std::vector<CustomFont::TextSegment> CustomFont::ParseDecodedBuffer(const uint8_t* buf, uint16_t drawLen) {
    std::vector<TextSegment> out;
    out.reserve(16);

    const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 color = white;
    std::string acc;
    bool done = false;
    int8_t choiceIndex = -1;

    auto flush = [&]() {
        if (!acc.empty()) {
            TextSegment seg;
            seg.text = acc;
            seg.color = color;
            seg.choiceIndex = choiceIndex;
            out.push_back(seg);
            acc.clear();
        }
    };

    const uint16_t safeLen = std::min(drawLen, (uint16_t)200);
    for (uint16_t i = 0; i < safeLen && !done; i++) {
        const uint8_t c = buf[i];

        switch (c) {
            case MESSAGE_NEWLINE:
                flush();
                {
                    TextSegment nl;
                    nl.newline = true;
                    nl.color = color;
                    nl.choiceIndex = (choiceIndex >= 0) ? choiceIndex : -1;
                    out.push_back(nl);
                    if (choiceIndex == -2) {
                        // First newline after TWO/THREE_CHOICE positions the cursor; don't count as separator.
                        choiceIndex = 0;
                    } else if (choiceIndex >= 0) {
                        choiceIndex++;
                    }
                }
                break;

            case MESSAGE_END:
            case MESSAGE_BOX_BREAK:
            case MESSAGE_PERSISTENT:
            case MESSAGE_EVENT:
            case MESSAGE_TEXTID:
            case MESSAGE_AWAIT_BUTTON_PRESS:
            case MESSAGE_OCARINA:
                flush();
                done = true;
                break;

            case MESSAGE_BOX_BREAK_DELAYED:
            case MESSAGE_FADE:
                flush();
                i++;
                done = true;
                break;

            case MESSAGE_FADE2:
                flush();
                i += 2;
                done = true;
                break;

            case MESSAGE_COLOR:
                flush();
                if (i + 1 < drawLen) {
                    color = ColorFromCode(buf[++i] & 0x0F, white);
                }
                break;

            case MESSAGE_ITEM_ICON:
                flush();
                if (i + 1 < drawLen) {
                    uint8_t iconItemId = buf[++i];
                    TextSegment iconSeg;
                    iconSeg.isIcon = true;
                    iconSeg.itemId = iconItemId;
                    iconSeg.color = color;
                    iconSeg.choiceIndex = choiceIndex;
                    out.push_back(iconSeg);
                }
                break;

            case MESSAGE_SHIFT:
                flush();
                if (i + 1 < drawLen) {
                    TextSegment shiftSeg;
                    shiftSeg.shiftX = (float)(uint8_t)buf[++i];
                    shiftSeg.color = color;
                    shiftSeg.choiceIndex = choiceIndex;
                    out.push_back(shiftSeg);
                }
                break;

            case MESSAGE_TEXT_SPEED:
            case MESSAGE_HIGHSCORE:
                i++;
                break;

            case MESSAGE_SFX:
                i += 2;
                break;

            case MESSAGE_BACKGROUND:
                i += 3;
                break;

            case MESSAGE_TWO_CHOICE:
            case MESSAGE_THREE_CHOICE:
                flush();
                choiceIndex = -2; // -2: next newline is a cursor-positioning newline, not a separator
                break;

            case MESSAGE_QUICKTEXT_ENABLE:
            case MESSAGE_QUICKTEXT_DISABLE:
            case MESSAGE_UNSKIPPABLE:
            case MESSAGE_NAME:
                break;

            default:
                if (c >= 0x20 && c < 0x80) {
                    if (choiceIndex == -2)
                        choiceIndex = 0;
                    acc += static_cast<char>(c);
                } else if (c >= 0x80 && c <= 0xAF) {
                    static const char* sLatinMap[] = {
                        // 0x80-0x9E PAL accented Latin
                        "\xC3\x80", // 0x80 À
                        "\xC3\xAE", // 0x81 î
                        "\xC3\x82", // 0x82 Â
                        "\xC3\x84", // 0x83 Ä
                        "\xC3\x87", // 0x84 Ç
                        "\xC3\x88", // 0x85 È
                        "\xC3\x89", // 0x86 É
                        "\xC3\x8A", // 0x87 Ê
                        "\xC3\x8B", // 0x88 Ë
                        "\xC3\x8F", // 0x89 Ï
                        "\xC3\x94", // 0x8A Ô
                        "\xC3\x96", // 0x8B Ö
                        "\xC3\x99", // 0x8C Ù
                        "\xC3\x9B", // 0x8D Û
                        "\xC3\x9C", // 0x8E Ü
                        "\xC3\x9F", // 0x8F ß
                        "\xC3\xA0", // 0x90 à
                        "\xC3\xA1", // 0x91 á
                        "\xC3\xA2", // 0x92 â
                        "\xC3\xA4", // 0x93 ä
                        "\xC3\xA7", // 0x94 ç
                        "\xC3\xA8", // 0x95 è
                        "\xC3\xA9", // 0x96 é
                        "\xC3\xAA", // 0x97 ê
                        "\xC3\xAB", // 0x98 ë
                        "\xC3\xAF", // 0x99 ï
                        "\xC3\xB4", // 0x9A ô
                        "\xC3\xB6", // 0x9B ö
                        "\xC3\xB9", // 0x9C ù
                        "\xC3\xBB", // 0x9D û
                        "\xC3\xBC", // 0x9E ü
                    };
                    static const char* sBtnTexNames[] = {
                        dgMsgChar9FButtonATex,      // 0x9F
                        dgMsgCharA0ButtonBTex,      // 0xA0
                        dgMsgCharA1ButtonCTex,      // 0xA1
                        dgMsgCharA2ButtonLTex,      // 0xA2
                        dgMsgCharA3ButtonRTex,      // 0xA3
                        dgMsgCharA4ButtonZTex,      // 0xA4
                        dgMsgCharA5ButtonCUpTex,    // 0xA5
                        dgMsgCharA6ButtonCDownTex,  // 0xA6
                        dgMsgCharA7ButtonCLeftTex,  // 0xA7
                        dgMsgCharA8ButtonCRightTex, // 0xA8
                        dgMsgCharA9ZTargetSignTex,  // 0xA9
                        dgMsgCharAAControlStickTex, // 0xAA
                        dgMsgCharABControlPadTex,   // 0xAB
                    };
                    static const ImVec4 sBtnColors[] = {
                        {},                                // 0x9F A — via ABtnColor()
                        {},                                // 0xA0 B — via BBtnColor()
                        {},                                // 0xA1 C — via CBtnGroupColor()
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xA2 L
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xA3 R
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xA4 Z
                        {},                                // 0xA5 C-Up — via CUpBtnColor()
                        {},                                // 0xA6 C-Down — via CDownBtnColor()
                        {},                                // 0xA7 C-Left — via CLeftBtnColor()
                        {},                                // 0xA8 C-Right — via CRightBtnColor()
                        ImVec4(0.00f, 0.82f, 0.20f, 1.0f), // 0xA9 Z-target
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xAA stick
                        ImVec4(1.00f, 1.00f, 1.00f, 1.0f), // 0xAB pad
                    };
                    if (c <= 0x9E) {
                        acc += sLatinMap[c - 0x80];
                    } else if (c <= 0xAB) {
                        flush();
                        const int bi = c - 0x9F;
                        TextSegment seg;
                        seg.btnIcon = sBtnTexNames[bi];
                        seg.color = (bi == 0)   ? ABtnColor()
                                    : (bi == 1) ? BBtnColor()
                                    : (bi == 2) ? CBtnGroupColor()
                                    : (bi == 6) ? CUpBtnColor()
                                    : (bi == 7) ? CDownBtnColor()
                                    : (bi == 8) ? CLeftBtnColor()
                                    : (bi == 9) ? CRightBtnColor()
                                                : sBtnColors[bi];
                        seg.choiceIndex = choiceIndex;
                        out.push_back(std::move(seg));
                    }
                }
                break;
        }
    }

    flush();
    return out;
}

ImVec4 CustomFont::ColorFromCode(uint8_t code, const ImVec4& defaultColor) {
    switch (code) {
        case MSGCOL_DEFAULT:
            return defaultColor;
        case MSGCOL_RED:
            return ImVec4(1.00f, 0.27f, 0.27f, 1.0f);
        case MSGCOL_ADJUSTABLE:
            return ImVec4(R_TEXT_ADJUST_COLOR_1_R / 255.0f, R_TEXT_ADJUST_COLOR_1_G / 255.0f,
                          R_TEXT_ADJUST_COLOR_1_B / 255.0f, 1.0f);
        case MSGCOL_BLUE:
            return ImVec4(0.25f, 0.25f, 1.00f, 1.0f);
        case MSGCOL_LIGHTBLUE:
            return ImVec4(0.50f, 0.80f, 1.00f, 1.0f);
        case MSGCOL_PURPLE:
            return ImVec4(0.75f, 0.25f, 0.75f, 1.0f);
        case MSGCOL_YELLOW:
            return ImVec4(1.00f, 1.00f, 0.25f, 1.0f);
        case MSGCOL_BLACK:
            return ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
        default:
            return defaultColor;
    }
}
