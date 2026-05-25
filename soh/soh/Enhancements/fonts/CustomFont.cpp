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

extern "C" {
#include "z64.h"
#include "regs.h"
#include "message_data_fmt.h"
#include "soh/cvar_prefixes.h"

extern PlayState* gPlayState;
}

static std::vector<std::string>       sModFontNames;
static std::vector<ImFont*>           sModFontPtrs;
static std::vector<std::vector<char>> sModFontData; // kept alive for the atlas

struct TranslationFile {
    std::string name;
    std::string path;
};

static std::vector<TranslationFile> sTranslationFiles;
static std::unordered_map<uint16_t,
    std::pair<std::vector<std::vector<CustomFont::TextSegment>>, std::vector<uint8_t>>> sActiveTranslation;

// ---------------------------------------------------------------------------
// DEFINE_MESSAGE parser helpers
// ---------------------------------------------------------------------------

static std::string ParseStringLiteral(const std::string& src, size_t& pos) {
    ++pos;
    std::string out;
    while (pos < src.size() && src[pos] != '"') {
        if (src[pos] == '\\' && pos + 1 < src.size()) {
            char esc = src[++pos];
            switch (esc) {
                case 'n':  out += '\n'; break;
                case 't':  out += '\t'; break;
                case '\\': out += '\\'; break;
                case '"':  out += '"';  break;
                case 'x': {
                    if (pos + 2 < src.size()) {
                        auto h = [](char c) -> int {
                            if (c >= '0' && c <= '9') return c - '0';
                            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                            return 0;
                        };
                        out += static_cast<char>(h(src[pos + 1]) << 4 | h(src[pos + 2]));
                        pos += 2;
                    }
                    break;
                }
                default: out += esc; break;
            }
        } else {
            out += src[pos];
        }
        ++pos;
    }
    if (pos < src.size()) ++pos;
    return out;
}

static void SkipWS(const std::string& src, size_t& pos) {
    while (pos < src.size()) {
        if (src[pos] == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
            while (pos < src.size() && src[pos] != '\n') ++pos;
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
    if (name == "DEFAULT")   return def;
    if (name == "RED")       return ImVec4(1.00f, 0.27f, 0.27f, 1.f);
    if (name == "GREEN")     return ImVec4(0.00f, 1.00f, 0.00f, 1.f);
    if (name == "BLUE")      return ImVec4(0.25f, 0.25f, 1.00f, 1.f);
    if (name == "LIGHTBLUE") return ImVec4(0.50f, 0.80f, 1.00f, 1.f);
    if (name == "PURPLE")    return ImVec4(0.75f, 0.25f, 0.75f, 1.f);
    if (name == "YELLOW")    return ImVec4(1.00f, 1.00f, 0.25f, 1.f);
    if (name == "BLACK")     return ImVec4(0.00f, 0.00f, 0.00f, 1.f);
    return def;
}

static const char* BtnIconFromMacro(const std::string& name) {
    if (name == "BUTTON_A")        return dgMsgChar9FButtonATex;
    if (name == "BUTTON_B")        return dgMsgCharA0ButtonBTex;
    if (name == "BUTTON_C")        return dgMsgCharA1ButtonCTex;
    if (name == "BUTTON_L")        return dgMsgCharA2ButtonLTex;
    if (name == "BUTTON_R")        return dgMsgCharA3ButtonRTex;
    if (name == "BUTTON_Z")        return dgMsgCharA4ButtonZTex;
    if (name == "BUTTON_CUP")      return dgMsgCharA5ButtonCUpTex;
    if (name == "BUTTON_CDOWN")    return dgMsgCharA6ButtonCDownTex;
    if (name == "BUTTON_CLEFT")    return dgMsgCharA7ButtonCLeftTex;
    if (name == "BUTTON_CRIGHT")   return dgMsgCharA8ButtonCRightTex;
    if (name == "ZTARGET_SIGN")    return dgMsgCharA9ZTargetSignTex;
    if (name == "CONTROL_STICK")   return dgMsgCharAAControlStickTex;
    if (name == "CONTROL_PAD")     return dgMsgCharABControlPadTex;
    return nullptr;
}

static ImVec4 BtnColorFromMacro(const std::string& name) {
    if (name == "BUTTON_A")                              return ImVec4(0.00f,0.82f,0.20f,1.f);
    if (name == "BUTTON_B")                              return ImVec4(0.78f,0.05f,0.05f,1.f);
    if (name == "BUTTON_C")                              return ImVec4(1.00f,0.65f,0.00f,1.f);
    if (name == "BUTTON_L"||name=="BUTTON_R"||name=="BUTTON_Z") return ImVec4(0.50f,0.80f,1.00f,1.f);
    if (name=="BUTTON_CUP"||name=="BUTTON_CDOWN"||name=="BUTTON_CLEFT"||name=="BUTTON_CRIGHT")
                                                         return ImVec4(1.00f,0.65f,0.00f,1.f);
    if (name == "ZTARGET_SIGN")                          return ImVec4(0.00f,0.82f,0.20f,1.f);
    if (name == "CONTROL_STICK")                         return ImVec4(0.50f,0.80f,1.00f,1.f);
    return ImVec4(1.f,1.f,1.f,1.f);
}

// Replicates Message_DecodeName's charset logic to get the player name as a plain string.
static std::string GetPlayerName() {
    const bool isPAL = (gSaveContext.ship.filenameLanguage == NAME_LANGUAGE_PAL);
    const uint8_t emptyChar = isPAL ? 0x3E : 0xDF;

    int len = 8;
    while (len > 0 && gSaveContext.playerName[len - 1] == emptyChar) len--;

    std::string name;
    for (int i = 0; i < len; i++) {
        uint8_t c = gSaveContext.playerName[i];
        if (isPAL) {
            if      (c == 0x3E) c = ' ';
            else if (c == 0x40) c = '.';
            else if (c == 0x3F) c = '-';
            else if (c <  0x0A) c += '0';
            else if (c <  0x24) c += '7';  // 0x0A + 0x37 = 'A'
            else if (c <  0x3E) c += '=';  // 0x24 + 0x3D = 'a'
        } else {
            if      (c == 0xDF) c = ' ';
            else if (c == 0xEA) c = '.';
            else if (c == 0xE4) c = '-';
            else if (c <  0x0A) c += '0';
            else if (c <  0xC5) c -= 0x6A;
            else if (c <  0xDF) c -= 0x64;
        }
        name += (char)c;
    }
    return name;
}

// Parses a DEFINE_MESSAGE body into per-page segment vectors and a proxy buffer.
// The proxy buffer has one 0x20 per translated character so textDrawPos tracks translation space.
static std::pair<std::vector<std::vector<CustomFont::TextSegment>>, std::vector<uint8_t>>
ParseMessageContent(const std::string& body) {
    std::vector<std::vector<CustomFont::TextSegment>> pages;
    std::vector<CustomFont::TextSegment> currentPage;
    std::vector<uint8_t> proxyBuf;
    const ImVec4 white(1,1,1,1);
    ImVec4 color = white;
    bool   colorIsAdjustable = false;
    std::string acc;
    int8_t choiceIndex = -1;

    auto flush = [&]() {
        if (!acc.empty()) {
            CustomFont::TextSegment seg;
            seg.text         = acc;
            seg.color        = color;
            seg.isAdjustable = colorIsAdjustable;
            seg.choiceIndex  = choiceIndex;
            currentPage.push_back(seg);
            acc.clear();
        }
    };
    auto pushNewline = [&]() {
        flush();
        CustomFont::TextSegment nl;
        nl.newline     = true;
        nl.color       = color;
        nl.choiceIndex = (choiceIndex >= 0) ? choiceIndex : -1;
        currentPage.push_back(nl);
        if (choiceIndex >= 0) choiceIndex++;
    };

    size_t pos = 0;
    while (pos < body.size()) {
        SkipWS(body, pos);
        if (pos >= body.size()) break;

        if (body[pos] == '"') {
            // Bracket sequences like "[A]", "[C-Up]" become icon segments; longer matches first.
            struct BtnSeq { const char* seq; const char* tex; ImVec4 col; };
            static const BtnSeq kBtnSeqs[] = {
                { "[C-Up]",        dgMsgCharA5ButtonCUpTex,    {1.f,0.65f,0.f,1.f} },
                { "[C-Down]",      dgMsgCharA6ButtonCDownTex,  {1.f,0.65f,0.f,1.f} },
                { "[C-Left]",      dgMsgCharA7ButtonCLeftTex,  {1.f,0.65f,0.f,1.f} },
                { "[C-Right]",     dgMsgCharA8ButtonCRightTex, {1.f,0.65f,0.f,1.f} },
                // "Control Pad" = analog stick (0xAA), "D-Pad" = directional cross (0xAB).
                { "[Control-Pad]", dgMsgCharAAControlStickTex, {0.5f,0.8f,1.f,1.f} },
                { "[D-Pad]",       dgMsgCharABControlPadTex,   {1.f,1.f,1.f,1.f}   },
                { "[A]", dgMsgChar9FButtonATex,    {0.f,0.82f,0.2f,1.f}  },
                { "[B]", dgMsgCharA0ButtonBTex,    {0.78f,0.05f,0.05f,1.f} },
                { "[C]", dgMsgCharA1ButtonCTex,    {1.f,0.65f,0.f,1.f}   },
                { "[L]", dgMsgCharA2ButtonLTex,    {0.5f,0.8f,1.f,1.f}   },
                { "[R]", dgMsgCharA3ButtonRTex,    {0.5f,0.8f,1.f,1.f}   },
                { "[Z]", dgMsgCharA4ButtonZTex,    {0.5f,0.8f,1.f,1.f}   },
            };

            std::string lit = ParseStringLiteral(body, pos);
            for (size_t i = 0; i < lit.size(); ) {
                unsigned char c = (unsigned char)lit[i];
                if (c == '\n') {
                    pushNewline();
                    proxyBuf.push_back(MESSAGE_NEWLINE);
                    i++;
                } else if (c == '[') {
                    bool matched = false;
                    for (const auto& btn : kBtnSeqs) {
                        size_t seqLen = strlen(btn.seq);
                        if (i + seqLen <= lit.size() && lit.compare(i, seqLen, btn.seq) == 0) {
                            flush();
                            CustomFont::TextSegment seg;
                            seg.btnIcon     = btn.tex;
                            seg.color       = btn.col;
                            seg.choiceIndex = choiceIndex;
                            currentPage.push_back(seg);
                            proxyBuf.push_back(0x20);
                            i += seqLen;
                            matched = true;
                            break;
                        }
                    }
                    if (!matched) { acc += '['; proxyBuf.push_back(0x20); i++; }
                } else {
                    // Collect one UTF-8 character (1-4 bytes).
                    int seqLen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
                    for (int k = 0; k < seqLen && i < lit.size(); k++, i++)
                        acc += lit[i];
                    proxyBuf.push_back(0x20);
                }
            }
        } else if (std::isalpha((unsigned char)body[pos]) || body[pos] == '_') {
            std::string macro = ReadIdent(body, pos);
            SkipWS(body, pos);

            // Accepts hex/decimal integers ("0x4800", "5") or raw byte strings ("\x48\x00").
            auto parseArgU32 = [](const std::string& arg) -> uint32_t {
                if (arg.empty()) return 0;
                if (std::isdigit((unsigned char)arg[0])) {
                    try { return (uint32_t)std::stoul(arg, nullptr, 0); } catch (...) {}
                }
                uint32_t val = 0;
                for (unsigned char c : arg) val = (val << 8) | c;
                return val;
            };
            auto parseArgByte = [&](const std::string& arg, uint8_t def) -> uint8_t {
                return arg.empty() ? def : (uint8_t)parseArgU32(arg);
            };
            auto parseArgU16 = [&](const std::string& arg) -> uint16_t {
                return (uint16_t)parseArgU32(arg);
            };

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
                if (pos < body.size() && body[pos] == ')') ++pos;

                if (macro == "COLOR") {
                    flush();
                    colorIsAdjustable = (arg == "ADJUSTABLE");
                    color = colorIsAdjustable ? white : ColorFromName(arg, white);
                } else if (macro == "SHIFT") {
                    flush();
                    CustomFont::TextSegment shiftSeg;
                    shiftSeg.shiftX      = arg.empty() ? 0.0f : (float)(unsigned char)arg[0];
                    shiftSeg.color       = color;
                    shiftSeg.choiceIndex = choiceIndex;
                    currentPage.push_back(shiftSeg);
                } else if (macro == "ITEM_ICON") {
                    flush();
                    CustomFont::TextSegment seg;
                    seg.isIcon      = true;
                    seg.itemId      = (uint8_t)(arg.empty() ? 0 : (unsigned char)arg[0]);
                    seg.color       = color;
                    seg.choiceIndex = choiceIndex;
                    currentPage.push_back(seg);
                    proxyBuf.push_back(MESSAGE_ITEM_ICON);
                    proxyBuf.push_back(seg.itemId);
                } else if (macro == "TEXT_SPEED") {
                    proxyBuf.push_back(MESSAGE_TEXT_SPEED);
                    proxyBuf.push_back(parseArgByte(arg, 2));
                } else if (macro == "SFX") {
                    uint16_t sfx = parseArgU16(arg);
                    proxyBuf.push_back(MESSAGE_SFX);
                    proxyBuf.push_back((uint8_t)(sfx >> 8));
                    proxyBuf.push_back((uint8_t)(sfx & 0xFF));
                } else if (macro == "BOX_BREAK_DELAYED") {
                    proxyBuf.push_back(MESSAGE_BOX_BREAK_DELAYED);
                    proxyBuf.push_back(parseArgByte(arg, 0));
                } else if (macro == "FADE") {
                    proxyBuf.push_back(MESSAGE_FADE);
                    proxyBuf.push_back(parseArgByte(arg, 0));
                }
                // HIGHSCORE, BACKGROUND, TEXTID, FADE2 — no proxy bytes.
            } else {
                if (macro == "TWO_CHOICE" || macro == "THREE_CHOICE") {
                    flush();
                    choiceIndex = 0;
                    proxyBuf.push_back(macro == "TWO_CHOICE" ? MESSAGE_TWO_CHOICE : MESSAGE_THREE_CHOICE);
                } else if (macro == "BOX_BREAK") {
                    flush();
                    pages.push_back(std::move(currentPage));
                    currentPage.clear();
                    choiceIndex = -1;
                    color = white;
                    colorIsAdjustable = false;
                    proxyBuf.push_back(MESSAGE_BOX_BREAK);
                } else if (macro == "AWAIT_BUTTON_PRESS") {
                    proxyBuf.push_back(MESSAGE_AWAIT_BUTTON_PRESS);
                } else if (macro == "QUICKTEXT_ENABLE") {
                    proxyBuf.push_back(MESSAGE_QUICKTEXT_ENABLE);
                } else if (macro == "QUICKTEXT_DISABLE") {
                    proxyBuf.push_back(MESSAGE_QUICKTEXT_DISABLE);
                } else if (macro == "PERSISTENT") {
                    proxyBuf.push_back(MESSAGE_PERSISTENT);
                } else if (macro == "UNSKIPPABLE") {
                    proxyBuf.push_back(MESSAGE_UNSKIPPABLE);
                } else if (macro == "EVENT") {
                    proxyBuf.push_back(MESSAGE_EVENT);
                } else if (macro == "NAME") {
                    flush();
                    CustomFont::TextSegment nameSeg;
                    nameSeg.isName       = true;
                    nameSeg.color        = color;
                    nameSeg.isAdjustable = colorIsAdjustable;
                    nameSeg.choiceIndex  = choiceIndex;
                    currentPage.push_back(nameSeg);
                    // One proxy byte per name char so textDrawPos tracks the name's width correctly.
                    const std::string pname = GetPlayerName();
                    for (size_t k = 0; k < pname.size(); k++)
                        proxyBuf.push_back(0x20);
                } else if (macro == "OCARINA") {
                    proxyBuf.push_back(MESSAGE_OCARINA);
                } else {
                    const char* tex = BtnIconFromMacro(macro);
                    if (tex) {
                        flush();
                        CustomFont::TextSegment seg;
                        seg.btnIcon     = tex;
                        seg.color       = BtnColorFromMacro(macro);
                        seg.choiceIndex = choiceIndex;
                        currentPage.push_back(seg);
                        proxyBuf.push_back(0x20);
                    }
                }
            }
        } else {
            ++pos;
        }
    }
    flush();
    pages.push_back(std::move(currentPage));
    proxyBuf.push_back(MESSAGE_END);
    return { pages, proxyBuf };
}

static void ParseTranslationFile(const std::string& text) {
    size_t pos = 0;
    while (pos < text.size()) {
        size_t found = text.find("DEFINE_MESSAGE(", pos);
        if (found == std::string::npos) break;
        pos = found + 15;

        SkipWS(text, pos);

        uint16_t msgId = 0;
        if (pos + 1 < text.size() && text[pos] == '0' &&
            (text[pos+1] == 'x' || text[pos+1] == 'X')) {
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
        for (int commas = 0; commas < 2 && pos < text.size(); ) {
            if (text[pos] == ',') commas++;
            ++pos;
            SkipWS(text, pos);
            ReadIdent(text, pos);
        }
        while (pos < text.size() && text[pos] != ',') ++pos;
        if (pos < text.size()) ++pos;

        // Collect body with paren depth tracking to handle nested macro parens.
        int depth = 1;
        size_t bodyStart = pos;
        while (pos < text.size() && depth > 0) {
            if (text[pos] == '(')       depth++;
            else if (text[pos] == ')')  depth--;
            if (depth > 0) ++pos; else break;
        }
        std::string body = text.substr(bodyStart, pos - bodyStart);
        if (pos < text.size()) ++pos;

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
    if (name == "None" || name.empty()) return;

    auto archiveMgr = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
    for (const auto& tf : sTranslationFiles) {
        if (tf.name != name) continue;
        auto raw = archiveMgr->LoadFile(tf.path);
        if (!raw || !raw->IsLoaded || !raw->Buffer || raw->Buffer->empty()) continue;
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
        if (sModFontNames[i] == name) return sModFontPtrs[i];
    return nullptr;
}

// ---------------------------------------------------------------------------
// InitElement
// ---------------------------------------------------------------------------

static int sCurrentTransPage = 0;

void CustomFont::InitElement() {
    REGISTER_VB_SHOULD(VB_DRAW_MESSAGE_TEXT, {
        if (CVarGetInteger(CVAR_SETTING("AltAssets"), 1) && CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0)) {
            *should = false;
        }
    });
    REGISTER_VB_SHOULD(VB_DRAW_ITEM_ICON, {
        if (CVarGetInteger(CVAR_SETTING("AltAssets"), 1) && CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0)) {
            *should = false;
        }
    });

    // Inject the per-page proxy into msgBufDecoded so textDrawPos tracks translation character space.
    REGISTER_VB_SHOULD(VB_MESSAGE_DECODED, {
        if (!CVarGetInteger(CVAR_SETTING("AltAssets"), 1) || !CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0))
            return;
        PlayState*      play    = va_arg(args, PlayState*);
        int             pageNum = va_arg(args, int);
        MessageContext* msgCtx  = &play->msgCtx;

        sCurrentTransPage = pageNum;

        auto it = sActiveTranslation.find(msgCtx->textId);
        if (it == sActiveTranslation.end()) return;

        // Seek to the start of the requested page in the proxy buffer (pages separated by BOX_BREAK).
        const auto& proxy = it->second.second;
        size_t pageStart = 0;
        for (int p = 0; p < pageNum; p++) {
            while (pageStart < proxy.size() && proxy[pageStart] != MESSAGE_BOX_BREAK)
                pageStart++;
            if (pageStart < proxy.size()) pageStart++; // skip BOX_BREAK
        }

        size_t dst = 0;
        for (size_t src = pageStart;
             src < proxy.size() && dst < sizeof(msgCtx->msgBufDecoded) - 1; src++, dst++) {
            msgCtx->msgBufDecoded[dst] = proxy[src];
            if (proxy[src] == MESSAGE_BOX_BREAK || proxy[src] == MESSAGE_END) {
                dst++;
                break;
            }
        }
        if (dst < sizeof(msgCtx->msgBufDecoded))
            msgCtx->msgBufDecoded[dst] = MESSAGE_END;
        msgCtx->decodedTextLen = (u16)(dst > 0 ? dst - 1 : 0);
    });

    // Built-in fonts in soh.o2r are excluded to avoid duplicates.
    static const char* const kBuiltins[] = {
        "fonts/PressStart2P-Regular.ttf",
        "fonts/Fipps-Regular.otf",
        "fonts/Inconsolata-Regular.ttf",
        "fonts/Montserrat-Regular.ttf",
        "fonts/NotoSansJP-Regular.ttf",
    };

    auto& io = ImGui::GetIO();
    auto archiveMgr = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
    auto modFontFiles = archiveMgr->ListFiles({ "fonts/*.ttf", "fonts/*.otf" }, {});
    for (const auto& path : *modFontFiles) {
        bool isBuiltin = false;
        for (const auto* b : kBuiltins)
            if (path == b) { isBuiltin = true; break; }
        if (isBuiltin) continue;

        auto rawFile = archiveMgr->LoadFile(path);
        if (!rawFile || !rawFile->IsLoaded || !rawFile->Buffer || rawFile->Buffer->empty()) continue;

        sModFontData.push_back(*rawFile->Buffer);

        // Full BMP glyph range — ImGui skips codepoints not present in the TTF.
        static constexpr ImWchar kFullBMP[] = { 0x0020, 0xFFFF, 0 };

        ImFontConfig conf;
        conf.FontDataOwnedByAtlas = false;
        conf.OversampleH          = 1;
        conf.OversampleV          = 1;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(
            sModFontData.back().data(), (int)sModFontData.back().size(), 64.0f, &conf, kFullBMP);
        if (!font) { sModFontData.pop_back(); continue; }

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

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------

void CustomFont::Draw() {
    if (!CVarGetInteger(CVAR_SETTING("AltAssets"), 1) || !CVarGetInteger(CVAR_CUSTOM_FONT_ENABLED, 0)) {
        return;
    }
    if (!gPlayState) {
        return;
    }

    const MessageContext* msgCtx = &gPlayState->msgCtx;

    // Whitelist the modes where Message_DrawText actually runs. Anything outside this set means
    // the buffer is mid-decode or transitioning — rendering then causes flicker or stale content.
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

    // Map N64 320x240 textbox coords to ImGui screen space, preserving 4:3 centering.
    const ImGuiViewport* vp       = ImGui::GetMainViewport();
    const ImVec2         gamePos  = vp->Pos;
    const ImVec2         gameSize = vp->Size;
    const float          scale    = gameSize.y / 240.0f;
    const float          xOffset  = (gameSize.x - 320.0f * scale) / 2.0f;

    const float winX = gamePos.x + xOffset + R_TEXTBOX_X * scale;
    const float winY = gamePos.y + R_TEXTBOX_Y * scale;
    const float winW = R_TEXTBOX_WIDTH  * scale;
    const float winH = R_TEXTBOX_HEIGHT * scale;

    ImGui::SetNextWindowPos(ImVec2(winX, winY));
    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::SetNextWindowBgAlpha(0.0f);

    const ImGuiWindowFlags kFlags =
        ImGuiWindowFlags_NoTitleBar          |
        ImGuiWindowFlags_NoResize            |
        ImGuiWindowFlags_NoMove              |
        ImGuiWindowFlags_NoScrollbar         |
        ImGuiWindowFlags_NoScrollWithMouse   |
        ImGuiWindowFlags_NoInputs            |
        ImGuiWindowFlags_NoBackground        |
        ImGuiWindowFlags_NoBringToFrontOnFocus   |
        ImGuiWindowFlags_NoSavedSettings     |
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

// ---------------------------------------------------------------------------
// DrawElement
// ---------------------------------------------------------------------------

void CustomFont::DrawElement() {
    const MessageContext* msgCtx = &gPlayState->msgCtx;

    static bool sPrevAltAssets    = ResourceMgr_IsAltAssetsEnabled();
    static bool sBtnTexturesLoaded = false;
    static bool sIconLoaded[158]   = {};

    const bool curAltAssets = ResourceMgr_IsAltAssetsEnabled();
    if (curAltAssets != sPrevAltAssets) {
        sBtnTexturesLoaded = false;
        std::fill(std::begin(sIconLoaded), std::end(sIconLoaded), false);
        sPrevAltAssets = curAltAssets;
    }

    // Explicitly check for alt textures — ResourceManager cache may already hold the vanilla version.
    if (!sBtnTexturesLoaded) {
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
        const ImVec4 white(1, 1, 1, 1);

        auto loadBtn = [&](const char* otrPath) {
            static constexpr int kOtrPrefixLen = 7; // strlen("__OTR__")
            if (curAltAssets) {
                const std::string bare    = std::string(otrPath + kOtrPrefixLen);
                const std::string altPath = "alt/" + bare;
                if (ResourceMgr_FileAltExists(bare.c_str())) {
                    gui->LoadGuiTexture(otrPath, altPath, white);
                    return;
                }
            }
            gui->LoadGuiTexture(otrPath, otrPath, white);
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

    // Built-in fonts are identified by their baked atlas size (FontSize), mod fonts by ImFont* pointer.
    static const struct { const char* name; float size; } kBuiltinFonts[] = {
        { "Press Start 2P", 12.0f },
        { "Fipps",          32.0f },
    };

    const std::string fontName = CVarGetString(CVAR_CUSTOM_FONT_NAME, "Default");
    ImFont* selectedFont = nullptr;
    if (fontName != "Default") {
        for (const auto& kf : kBuiltinFonts) {
            if (fontName == kf.name) {
                for (ImFont* f : ImGui::GetIO().Fonts->Fonts) {
                    if (f->FontSize == kf.size) { selectedFont = f; break; }
                }
                break;
            }
        }
        if (!selectedFont) selectedFont = GetModFont(fontName);
    }
    ImGui::PushFont(selectedFont);

    ImFont* activeFont = selectedFont ? selectedFont : ImGui::GetIO().Fonts->Fonts[0];

    const float scaleX  = ImGui::GetWindowWidth()  / (float)R_TEXTBOX_WIDTH;
    const float scaleY  = ImGui::GetWindowHeight() / (float)R_TEXTBOX_HEIGHT;
    const float cursorX = (R_TEXT_INIT_XPOS - R_TEXTBOX_X) * scaleX;
    const float cursorY = (R_TEXT_INIT_YPOS - R_TEXTBOX_Y) * scaleY;

    // Look up the current page's translated segments; null when no translation is active.
    const auto* trans = [&]() -> const std::vector<TextSegment>* {
        auto it = sActiveTranslation.find(msgCtx->textId);
        if (it == sActiveTranslation.end()) return nullptr;
        const auto& pages = it->second.first;
        if (pages.empty()) return nullptr;
        int page = std::min(sCurrentTransPage, (int)pages.size() - 1);
        return &pages[page];
    }();

    const auto origFull  = ParseDecodedBuffer(msgCtx->msgBufDecoded, 0xFFFF);
    const auto origTyped = ParseDecodedBuffer(msgCtx->msgBufDecoded, msgCtx->textDrawPos);

    // Count printable UTF-8 leading bytes (proxy uses one 0x20 per translated char).
    auto countChars = [](const std::vector<TextSegment>& segs) -> size_t {
        size_t n = 0;
        for (const auto& s : segs)
            if (!s.newline && !s.isIcon && s.btnIcon.empty())
                for (unsigned char c : s.text)
                    if ((c & 0xC0) != 0x80) n++;
        return n;
    };

    // Return a copy of segs truncated to the first `limit` printable characters.
    auto limitSegs = [](const std::vector<TextSegment>& segs, size_t limit) {
        std::vector<TextSegment> out;
        size_t count = 0;
        for (const auto& s : segs) {
            if (s.newline || s.isIcon || !s.btnIcon.empty() || s.shiftX != 0.0f) {
                if (count < limit) out.push_back(s);
                continue;
            }
            if (s.isName) {
                if (count < limit) out.push_back(s);
                count += GetPlayerName().size();
                if (count >= limit) break;
                continue;
            }
            TextSegment trimmed = s;
            trimmed.text.clear();
            for (size_t i = 0; i < s.text.size() && count < limit; ) {
                unsigned char c = (unsigned char)s.text[i];
                int seqLen = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
                for (int k = 0; k < seqLen && i < s.text.size(); k++, i++)
                    trimmed.text += s.text[i];
                count++;
            }
            if (!trimmed.text.empty()) out.push_back(trimmed);
            if (count >= limit) break;
        }
        return out;
    };

    const auto& fullSegments = trans ? *trans : origFull;
    const auto  segments     = trans ? limitSegs(*trans, countChars(origTyped)) : origTyped;

    bool    hasItemIcon = false;
    uint8_t itemIconId  = 0;
    for (const auto& seg : fullSegments) {
        if (seg.isIcon) { hasItemIcon = true; itemIconId = seg.itemId; break; }
    }

    bool hasChoices = false;
    for (const auto& seg : fullSegments) {
        if (seg.choiceIndex >= 0) { hasChoices = true; break; }
    }
    const float choiceStartY = hasChoices
        ? (R_TEXT_CHOICE_YPOS(0) - R_TEXTBOX_Y) * scaleY
        : ImGui::GetWindowHeight();

    // Accumulate per-line text and icon counts from fullSegments for width measurement.
    struct LineInfo { std::string text; int btnIconCount = 0; };
    std::vector<LineInfo> lineInfos;
    {
        LineInfo cur;
        for (const auto& seg : fullSegments) {
            if (seg.choiceIndex >= 0) continue;
            if (seg.newline)               { lineInfos.push_back(cur); cur = {}; }
            else if (!seg.btnIcon.empty()) { cur.btnIconCount++; }
            else if (seg.isName)           { cur.text += GetPlayerName(); }
            else if (!seg.isIcon)          { cur.text += seg.text; }
        }
        lineInfos.push_back(cur);
    }

    // Font size matches vanilla: (R_TEXT_CHAR_SCALE / 100) * 16 N64px.
    const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
    const int   numLines    = std::max((int)lineInfos.size(), 1);
    const float desiredSize = (R_TEXT_CHAR_SCALE / 100.0f) * 16.0f * scaleY;

    // Item icon occupies a 24px column; vanilla advances textPosX by 32px to clear it.
    const float iconSize        = hasItemIcon ? (float)R_TEXTBOX_ICON_SIZE * scaleX : 0.0f;
    const float iconColumnWidth = hasItemIcon ? 32.0f * scaleX : 0.0f;

    const float availableWidth = ImGui::GetWindowWidth() - 2.0f * cursorX - iconColumnWidth;

    float maxLineWidth = 0.0f;
    for (const auto& li : lineInfos) {
        float w = li.btnIconCount * desiredSize;
        if (!li.text.empty())
            w += activeFont->CalcTextSizeA(desiredSize, FLT_MAX, 0.0f, li.text.c_str()).x;
        if (w > maxLineWidth) maxLineWidth = w;
    }

    float effectiveSize = desiredSize;
    if (maxLineWidth > availableWidth && maxLineWidth > 0.0f)
        effectiveSize = desiredSize * (availableWidth / maxLineWidth);
    effectiveSize = std::max(effectiveSize, 1.0f);

    const float lineSpacing = effectiveSize + itemSpacing;

    // Lines that start with SHIFT are treated as centered; mid-line SHIFTs are raw pixel advances.
    struct LineLayout { float startX = 0.0f; bool centered = false; };
    std::vector<LineLayout> lineLayouts;
    {
        bool  firstOnLine  = true;
        bool  lineHasShift = false;
        float lineContentW = 0.0f;

        auto finishLine = [&]() {
            LineLayout ll;
            ll.centered = lineHasShift;
            ll.startX   = lineHasShift
                ? std::max(0.0f, (availableWidth - lineContentW) / 2.0f)
                : 0.0f;
            lineLayouts.push_back(ll);
            firstOnLine  = true;
            lineHasShift = false;
            lineContentW = 0.0f;
        };

        for (const auto& s : segments) {
            if (s.choiceIndex >= 0) continue;
            if (s.newline) { finishLine(); continue; }
            if (s.isIcon)  continue;
            if (s.shiftX != 0.0f) {
                if (firstOnLine) lineHasShift = true;
                continue;
            }
            firstOnLine = false;
            if (!s.btnIcon.empty()) lineContentW += effectiveSize;
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
                const std::string bare    = std::string(iconPath + kOtrPrefixLen);
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

    // Resolve ADJUSTABLE color live from game REGs.
    const ImVec4 white(1,1,1,1);
    auto resolveColor = [&](const CustomFont::TextSegment& s) -> ImVec4 {
        return s.isAdjustable ? ColorFromCode(MSGCOL_ADJUSTABLE, white) : s.color;
    };

    ImDrawList* dl     = ImGui::GetWindowDrawList();
    ImVec2      winPos = ImGui::GetWindowPos();
    int         lineNum = 0;
    float       lineX   = lineLayouts.empty() ? 0.0f : lineLayouts[0].startX;

    for (const auto& seg : segments) {
        if (seg.choiceIndex >= 0) continue;

        if (seg.newline) {
            lineNum++;
            lineX = lineNum < (int)lineLayouts.size() ? lineLayouts[lineNum].startX : 0.0f;
            continue;
        }

        if (seg.isIcon) continue;

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
                dl->AddImage(texId, ImVec2(sx, sy), ImVec2(sx + effectiveSize, sy + effectiveSize),
                             ImVec2(0, 0), ImVec2(1, 1), tint);
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

        if (seg.text.empty()) continue;

        const ImU32 col = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
        dl->AddText(activeFont, effectiveSize, ImVec2(sx, sy), col, seg.text.c_str());
        lineX += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, seg.text.c_str()).x;
    }

    // Draw item icon in the left column, vertically centered.
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
            if (seg.choiceIndex > maxChoiceIdx) maxChoiceIdx = seg.choiceIndex;
        const int8_t yposOffset = (maxChoiceIdx == 1) ? 1 : 0;

        float  choiceLineX = 0.0f;
        int8_t prevChoice  = -1;

        for (const auto& seg : fullSegments) {
            if (seg.choiceIndex < 0) continue;

            if (seg.newline) {
                choiceLineX = 0.0f;
                continue;
            }
            if (seg.isIcon) continue;

            if (seg.choiceIndex != prevChoice) {
                choiceLineX = 0.0f;
                prevChoice  = seg.choiceIndex;
            }

            // Vanilla choice text is indented 32 N64px (to clear the selection arrow).
            const float vanillaCharH = (R_TEXT_CHAR_SCALE / 100.0f) * 16.0f * scaleY;
            const float baseX = winPos.x + cursorX + 32.0f * scaleX;
            const float baseY = winPos.y + (R_TEXT_CHOICE_YPOS(seg.choiceIndex + yposOffset) - R_TEXTBOX_Y) * scaleY
                              + (vanillaCharH - effectiveSize) * 0.5f;

            if (!seg.btnIcon.empty()) {
                ImTextureID texId = gui->GetTextureByName(seg.btnIcon);
                if (texId) {
                    const ImU32 tint = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
                    dl->AddImage(texId,
                                 ImVec2(baseX + choiceLineX, baseY),
                                 ImVec2(baseX + choiceLineX + effectiveSize, baseY + effectiveSize),
                                 ImVec2(0, 0), ImVec2(1, 1), tint);
                    choiceLineX += effectiveSize;
                }
                continue;
            }

            if (seg.text.empty()) continue;

            const ImU32 col = ImGui::ColorConvertFloat4ToU32(resolveColor(seg));
            dl->AddText(activeFont, effectiveSize, ImVec2(baseX + choiceLineX, baseY), col, seg.text.c_str());
            choiceLineX += activeFont->CalcTextSizeA(effectiveSize, FLT_MAX, 0.0f, seg.text.c_str()).x;
        }
    }

    ImGui::PopFont();
}

// ---------------------------------------------------------------------------
// ParseDecodedBuffer
// ---------------------------------------------------------------------------

std::vector<CustomFont::TextSegment> CustomFont::ParseDecodedBuffer(const uint8_t* buf, uint16_t drawLen) {
    std::vector<TextSegment> out;
    out.reserve(16);

    const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4       color = white;
    std::string  acc;
    bool         done        = false;
    int8_t       choiceIndex = -1;

    auto flush = [&]() {
        if (!acc.empty()) {
            TextSegment seg;
            seg.text        = acc;
            seg.color       = color;
            seg.choiceIndex = choiceIndex;
            out.push_back(seg);
            acc.clear();
        }
    };

    for (uint16_t i = 0; i < drawLen && !done; i++) {
        const uint8_t c = buf[i];

        switch (c) {
            case MESSAGE_NEWLINE:
                flush();
                {
                    TextSegment nl;
                    nl.newline     = true;
                    nl.color       = color;
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
                    iconSeg.isIcon      = true;
                    iconSeg.itemId      = iconItemId;
                    iconSeg.color       = color;
                    iconSeg.choiceIndex = choiceIndex;
                    out.push_back(iconSeg);
                }
                break;

            case MESSAGE_SHIFT:
                flush();
                if (i + 1 < drawLen) {
                    TextSegment shiftSeg;
                    shiftSeg.shiftX     = (float)(uint8_t)buf[++i];
                    shiftSeg.color      = color;
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
                    if (choiceIndex == -2) choiceIndex = 0;
                    acc += static_cast<char>(c);
                } else if (c >= 0x80 && c <= 0xAF) {
                    // 0x80-0x9E: PAL accented Latin; 0x9F-0xAB: controller button icons.
                    static const char* sLatinMap[] = {
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
                        ImVec4(0.00f, 0.82f, 0.20f, 1.0f), // 0x9F A
                        ImVec4(0.78f, 0.05f, 0.05f, 1.0f), // 0xA0 B
                        ImVec4(1.00f, 0.65f, 0.00f, 1.0f), // 0xA1 C
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xA2 L
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xA3 R
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xA4 Z
                        ImVec4(1.00f, 0.65f, 0.00f, 1.0f), // 0xA5 C-Up
                        ImVec4(1.00f, 0.65f, 0.00f, 1.0f), // 0xA6 C-Down
                        ImVec4(1.00f, 0.65f, 0.00f, 1.0f), // 0xA7 C-Left
                        ImVec4(1.00f, 0.65f, 0.00f, 1.0f), // 0xA8 C-Right
                        ImVec4(0.00f, 0.82f, 0.20f, 1.0f), // 0xA9 Z-target
                        ImVec4(0.50f, 0.80f, 1.00f, 1.0f), // 0xAA stick
                        ImVec4(1.00f, 1.00f, 1.00f, 1.0f), // 0xAB pad
                    };
                    if (c <= 0x9E) {
                        acc += sLatinMap[c - 0x80];
                    } else if (c <= 0xAB) {
                        flush();
                        TextSegment seg;
                        seg.btnIcon     = sBtnTexNames[c - 0x9F];
                        seg.color       = sBtnColors[c - 0x9F];
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

// ---------------------------------------------------------------------------
// ColorFromCode
// ---------------------------------------------------------------------------

ImVec4 CustomFont::ColorFromCode(uint8_t code, const ImVec4& defaultColor) {
    switch (code) {
        case MSGCOL_DEFAULT:    return defaultColor;
        case MSGCOL_RED:        return ImVec4(1.00f, 0.27f, 0.27f, 1.0f);
        case MSGCOL_ADJUSTABLE: return ImVec4(R_TEXT_ADJUST_COLOR_1_R / 255.0f,
                                              R_TEXT_ADJUST_COLOR_1_G / 255.0f,
                                              R_TEXT_ADJUST_COLOR_1_B / 255.0f, 1.0f);
        case MSGCOL_BLUE:       return ImVec4(0.25f, 0.25f, 1.00f, 1.0f);
        case MSGCOL_LIGHTBLUE:  return ImVec4(0.50f, 0.80f, 1.00f, 1.0f);
        case MSGCOL_PURPLE:     return ImVec4(0.75f, 0.25f, 0.75f, 1.0f);
        case MSGCOL_YELLOW:     return ImVec4(1.00f, 1.00f, 0.25f, 1.0f);
        case MSGCOL_BLACK:      return ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
        default:                return defaultColor;
    }
}
