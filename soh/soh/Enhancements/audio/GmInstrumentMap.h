#pragma once
#include <cstdint>
#include <cstring>
#include <spdlog/spdlog.h>

// Forward-declared engine runtime state, populated by audio_load.c at startup, so
// GetFontName can return the real per-build font label instead of the compile-time
// kFontNames snapshot. Each entry is a path like "audio/fonts/..." (built-in) or
// "custom/fonts/MyFont" (modded); nullptr before audio_load runs.
extern "C" char** fontMap;
extern "C" size_t fontMapSize;

// ---------------------------------------------------------------------------
// Engine soundfont → General MIDI preset mapping
//
// instOrWave values are DIRECT INDICES into the font's sample bank.
// Bank 1 is the main orchestral bank used by most melodic fonts.
// Bank 0 is the SFX bank — all its instruments are [SKIP].
//
// CONFIDENCE:
//   [GM]    = exact GM equivalent, high confidence
//   [APPROX]= closest available GM program
//   [SKIP]  = SFX / voice / no usable GM equivalent → kUnmapped (silent
//             unless a mod-supplied mapping overrides it)
//
// GM MELODIC PROGRAMS (bank 0):
//    0 Acoustic Grand Piano    19 Church Organ        46 Orchestral Harp
//    1 Bright Acoustic Piano   20 Reed Organ          47 Timpani
//    4 Electric Piano 1        22 Harmonica           48 String Ensemble 1
//    6 Harpsichord             24 Nylon Guitar        52 Choir Aahs
//    9 Glockenspiel            25 Steel Guitar        53 Voice Oohs
//   11 Vibraphone              30 Distortion Guitar   54 Synth Voice
//   12 Marimba                 32 Acoustic Bass       55 Orchestra Hit
//   13 Xylophone               37 Slap Bass 1         56 Trumpet
//   16 Drawbar Organ           40 Violin              57 Trombone
//   17 Percussive Organ        41 Viola               58 Tuba
//   19 Church Organ            42 Cello               60 French Horn
//   21 Accordion               43 Contrabass         68 Oboe
//   23 Tango Accordion         45 Pizzicato Strings  70 Bassoon
//   24 Nylon Guitar            46 Orchestral Harp    71 Clarinet
//   25 Steel Guitar            47 Timpani            73 Flute
//   104 Sitar                  107 Koto              114 Steel Drums
//   105 Banjo                  112 Tinkle Bell        77 Shakuhachi
//
// GM PERCUSSION (bank 128, channel 9):
//   0  Standard Kit    48 Orchestra Kit
//   8  Room Kit        32 Jazz Kit
// ---------------------------------------------------------------------------

namespace SOH {

static constexpr uint8_t kUnmapped = 0xFF;
static constexpr uint8_t kBuiltinFontCount = 38;

struct GmPreset {
    uint8_t bank;     // 0 = GM melodic, 128 = GM percussion
    uint8_t program;  // melodic: GM program; drums: GM drum kit (Standard=0, Orchestra=48, …)
    uint8_t drumNote; // Only meaningful when bank == 128. The GM percussion MIDI note to
                      // fire on channel 9 for this engine instrument. 0 = no specific
                      // mapping; the translator falls back to a heuristic based on the
                      // engine semitone.
};

static constexpr uint16_t kMaxInstPerFont = 128; // bank 1 has 85 entries; leave headroom
static constexpr GmPreset U = { 0, kUnmapped, 0 };

// ---------------------------------------------------------------------------
// Per-font friendly names, indexed by fontId. Used by the bypass UI to label
// discovered (fontId, instOrWave) pairs with a name instead of a raw integer
// pair. Names mirror Audio.xml's <Soundfont Name="..." Index="N"/> entries, with
// the "NN_" prefix stripped and underscores turned into spaces.
//
// There is no per-slot instrument-name table: the engine reads
// sf->instruments[instId] per font, each font having its own instruments array
// in its own order, so there is no shared bank layout to name.
// ---------------------------------------------------------------------------
// clang-format off

static const char* const kFontNames[] = {
    /*  0 */ "Sound Effects 1",
    /*  1 */ "Sound Effects 2",
    /*  2 */ "Ambient Sounds",
    /*  3 */ "Orchestra",
    /*  4 */ "Deku Tree",
    /*  5 */ "Market",
    /*  6 */ "Title Theme",
    /*  7 */ "Jabu Jabu",
    /*  8 */ "Child Kakariko Village",
    /*  9 */ "Fairy Fountain",
    /* 10 */ "Fire Temple",
    /* 11 */ "Dodongos Cavern",
    /* 12 */ "Forest Temple",
    /* 13 */ "Lon Lon Ranch",
    /* 14 */ "Goron City",
    /* 15 */ "Kokiri Forest",
    /* 16 */ "Spirit Temple",
    /* 17 */ "Horse Race",
    /* 18 */ "Warp Songs",
    /* 19 */ "Legends of Hyrule",
    /* 20 */ "Minigames",
    /* 21 */ "Zoras Domain",
    /* 22 */ "Shops",
    /* 23 */ "Ice Cavern",
    /* 24 */ "Shadow Temple",
    /* 25 */ "Water Temple",
    /* 26 */ "Unused",
    /* 27 */ "Gerudo Valley",
    /* 28 */ "Lakeside Laboratory",
    /* 29 */ "Kotake and Koumes Theme",
    /* 30 */ "Ganons Castle Organ",
    /* 31 */ "Ganons Castle",
    /* 32 */ "Ganondorfs Battle",
    /* 33 */ "Ending 1",
    /* 34 */ "Ending 2",
    /* 35 */ "Game Over",
    /* 36 */ "Kaepora Gaeboras Theme",
    /* 37 */ "Unused Deku Tree",
};
static_assert(sizeof(kFontNames) / sizeof(kFontNames[0]) == kBuiltinFontCount,
              "kFontNames must cover every builtin font");

// clang-format on

// ---------------------------------------------------------------------------
// Lookup, called from MidiTranslator::ProcessNote. Returns the unmapped sentinel
// for everything melodic; the JSON layer (defaults + user overrides) drives
// program assignments via mProgramOverride. Only structural cases stay here:
// synthetic waves, SFX-only fonts, and the true-drum trigger stay unmapped so the
// native synth plays them.
// ---------------------------------------------------------------------------
inline GmPreset GetGmPreset(uint8_t fontId, int16_t instOrWave) {
    // Synthetic waveforms (square, triangle...): no SF equivalent.
    if (instOrWave >= 0x80 && instOrWave < 0xC0)
        return U;
    if (instOrWave < 0)
        return U;

    // Fonts 0 & 1: pure SFX bank — never map to GM.
    if (fontId == 0 || fontId == 1)
        return U;

    // Font 2: ambient sounds / loops — pure SFX-flavoured content.
    if (fontId == 2)
        return U;

    // True drum trigger: the engine fetches Audio_GetDrum(fontId, semitone) from a
    // per-font drum bank. Leave unmapped so the native path handles it; a blanket
    // GM percussion fallback clicks on note attack.
    if (instOrWave == 0)
        return U;

    // Everything else: the per-pair JSON drives the mapping. Return U so
    // BypassMode::Auto resolves to native; mProgramOverride is what actually
    // routes a pair through FluidSynth when set.
    return U;
}

// ---------------------------------------------------------------------------
// Friendly label for a fontId. The bypass UI combines this with the raw
// instOrWave index; there is no reliable per-slot instrument name (see the
// kFontNames comment). Returns nullptr for fontIds outside the built-in range
// (typically modded fonts).
// ---------------------------------------------------------------------------
inline const char* GetFontName(uint8_t fontId) {
    // Prefer the runtime fontMap, which tracks the actually loaded assets
    // (including modded fonts). Strip the directory prefix so the UI shows just
    // the final segment ("audio/fonts/Foo" -> "Foo"). Pointers are stable: entries
    // are allocated once during audio_load and never reallocated.
    if (fontMap != nullptr && fontId < fontMapSize && fontMap[fontId] != nullptr) {
        const char* path = fontMap[fontId];
        const char* slash = std::strrchr(path, '/');
        return slash ? slash + 1 : path;
    }
    // Fallback to the compile-time table for very-early-startup calls before
    // audio_load populates fontMap; only built-in fonts exist at that point anyway.
    if (fontId < kBuiltinFontCount)
        return kFontNames[fontId];
    return nullptr;
}

// Convenience returning the same string as GetFontName. The instOrWave argument
// is unused, kept so call sites don't change shape.
inline const char* GetInstrumentName(uint8_t fontId, int16_t /*instOrWave*/) {
    return GetFontName(fontId);
}

// ---------------------------------------------------------------------------
// General MIDI program names — used by the debug UI's GM Prg dropdown so the
// user picks "Acoustic Grand Piano" instead of "0". Indexed by GM program
// number 0..127. Standard GM Level 1 instrument set.
// ---------------------------------------------------------------------------
static const char* const kGmProgramNames[128] = {
    /*   0 */ "Acoustic Grand Piano",
    /*   1 */ "Bright Acoustic Piano",
    /*   2 */ "Electric Grand Piano",
    /*   3 */ "Honky-tonk Piano",
    /*   4 */ "Electric Piano 1",
    /*   5 */ "Electric Piano 2",
    /*   6 */ "Harpsichord",
    /*   7 */ "Clavi",
    /*   8 */ "Celesta",
    /*   9 */ "Glockenspiel",
    /*  10 */ "Music Box",
    /*  11 */ "Vibraphone",
    /*  12 */ "Marimba",
    /*  13 */ "Xylophone",
    /*  14 */ "Tubular Bells",
    /*  15 */ "Dulcimer",
    /*  16 */ "Drawbar Organ",
    /*  17 */ "Percussive Organ",
    /*  18 */ "Rock Organ",
    /*  19 */ "Church Organ",
    /*  20 */ "Reed Organ",
    /*  21 */ "Accordion",
    /*  22 */ "Harmonica",
    /*  23 */ "Tango Accordion",
    /*  24 */ "Acoustic Guitar (nylon)",
    /*  25 */ "Acoustic Guitar (steel)",
    /*  26 */ "Electric Guitar (jazz)",
    /*  27 */ "Electric Guitar (clean)",
    /*  28 */ "Electric Guitar (muted)",
    /*  29 */ "Overdriven Guitar",
    /*  30 */ "Distortion Guitar",
    /*  31 */ "Guitar Harmonics",
    /*  32 */ "Acoustic Bass",
    /*  33 */ "Electric Bass (finger)",
    /*  34 */ "Electric Bass (pick)",
    /*  35 */ "Fretless Bass",
    /*  36 */ "Slap Bass 1",
    /*  37 */ "Slap Bass 2",
    /*  38 */ "Synth Bass 1",
    /*  39 */ "Synth Bass 2",
    /*  40 */ "Violin",
    /*  41 */ "Viola",
    /*  42 */ "Cello",
    /*  43 */ "Contrabass",
    /*  44 */ "Tremolo Strings",
    /*  45 */ "Pizzicato Strings",
    /*  46 */ "Orchestral Harp",
    /*  47 */ "Timpani",
    /*  48 */ "String Ensemble 1",
    /*  49 */ "String Ensemble 2",
    /*  50 */ "Synth Strings 1",
    /*  51 */ "Synth Strings 2",
    /*  52 */ "Choir Aahs",
    /*  53 */ "Voice Oohs",
    /*  54 */ "Synth Voice",
    /*  55 */ "Orchestra Hit",
    /*  56 */ "Trumpet",
    /*  57 */ "Trombone",
    /*  58 */ "Tuba",
    /*  59 */ "Muted Trumpet",
    /*  60 */ "French Horn",
    /*  61 */ "Brass Section",
    /*  62 */ "Synth Brass 1",
    /*  63 */ "Synth Brass 2",
    /*  64 */ "Soprano Sax",
    /*  65 */ "Alto Sax",
    /*  66 */ "Tenor Sax",
    /*  67 */ "Baritone Sax",
    /*  68 */ "Oboe",
    /*  69 */ "English Horn",
    /*  70 */ "Bassoon",
    /*  71 */ "Clarinet",
    /*  72 */ "Piccolo",
    /*  73 */ "Flute",
    /*  74 */ "Recorder",
    /*  75 */ "Pan Flute",
    /*  76 */ "Blown Bottle",
    /*  77 */ "Shakuhachi",
    /*  78 */ "Whistle",
    /*  79 */ "Ocarina",
    /*  80 */ "Lead 1 (square)",
    /*  81 */ "Lead 2 (sawtooth)",
    /*  82 */ "Lead 3 (calliope)",
    /*  83 */ "Lead 4 (chiff)",
    /*  84 */ "Lead 5 (charang)",
    /*  85 */ "Lead 6 (voice)",
    /*  86 */ "Lead 7 (fifths)",
    /*  87 */ "Lead 8 (bass + lead)",
    /*  88 */ "Pad 1 (new age)",
    /*  89 */ "Pad 2 (warm)",
    /*  90 */ "Pad 3 (polysynth)",
    /*  91 */ "Pad 4 (choir)",
    /*  92 */ "Pad 5 (bowed)",
    /*  93 */ "Pad 6 (metallic)",
    /*  94 */ "Pad 7 (halo)",
    /*  95 */ "Pad 8 (sweep)",
    /*  96 */ "FX 1 (rain)",
    /*  97 */ "FX 2 (soundtrack)",
    /*  98 */ "FX 3 (crystal)",
    /*  99 */ "FX 4 (atmosphere)",
    /* 100 */ "FX 5 (brightness)",
    /* 101 */ "FX 6 (goblins)",
    /* 102 */ "FX 7 (echoes)",
    /* 103 */ "FX 8 (sci-fi)",
    /* 104 */ "Sitar",
    /* 105 */ "Banjo",
    /* 106 */ "Shamisen",
    /* 107 */ "Koto",
    /* 108 */ "Kalimba",
    /* 109 */ "Bagpipe",
    /* 110 */ "Fiddle",
    /* 111 */ "Shanai",
    /* 112 */ "Tinkle Bell",
    /* 113 */ "Agogo",
    /* 114 */ "Steel Drums",
    /* 115 */ "Woodblock",
    /* 116 */ "Taiko Drum",
    /* 117 */ "Melodic Tom",
    /* 118 */ "Synth Drum",
    /* 119 */ "Reverse Cymbal",
    /* 120 */ "Guitar Fret Noise",
    /* 121 */ "Breath Noise",
    /* 122 */ "Seashore",
    /* 123 */ "Bird Tweet",
    /* 124 */ "Telephone Ring",
    /* 125 */ "Helicopter",
    /* 126 */ "Applause",
    /* 127 */ "Gunshot",
};

// ---------------------------------------------------------------------------
// General MIDI percussion names — used by the drum-split UI's "Drum Sound"
// combo so a slot picks "Acoustic Snare" instead of "38". GM percussion is
// defined for MIDI notes 35..81. ASCII only (the bundled ImGui font lacks
// extended glyphs).
// ---------------------------------------------------------------------------
static constexpr uint8_t kGmPercussionLo = 35;
static constexpr uint8_t kGmPercussionHi = 81;
static const char* const kGmPercussionNames[] = {
    /* 35 */ "Acoustic Bass Drum",
    /* 36 */ "Bass Drum 1",
    /* 37 */ "Side Stick",
    /* 38 */ "Acoustic Snare",
    /* 39 */ "Hand Clap",
    /* 40 */ "Electric Snare",
    /* 41 */ "Low Floor Tom",
    /* 42 */ "Closed Hi-Hat",
    /* 43 */ "High Floor Tom",
    /* 44 */ "Pedal Hi-Hat",
    /* 45 */ "Low Tom",
    /* 46 */ "Open Hi-Hat",
    /* 47 */ "Low-Mid Tom",
    /* 48 */ "Hi-Mid Tom",
    /* 49 */ "Crash Cymbal 1",
    /* 50 */ "High Tom",
    /* 51 */ "Ride Cymbal 1",
    /* 52 */ "Chinese Cymbal",
    /* 53 */ "Ride Bell",
    /* 54 */ "Tambourine",
    /* 55 */ "Splash Cymbal",
    /* 56 */ "Cowbell",
    /* 57 */ "Crash Cymbal 2",
    /* 58 */ "Vibraslap",
    /* 59 */ "Ride Cymbal 2",
    /* 60 */ "Hi Bongo",
    /* 61 */ "Low Bongo",
    /* 62 */ "Mute Hi Conga",
    /* 63 */ "Open Hi Conga",
    /* 64 */ "Low Conga",
    /* 65 */ "High Timbale",
    /* 66 */ "Low Timbale",
    /* 67 */ "High Agogo",
    /* 68 */ "Low Agogo",
    /* 69 */ "Cabasa",
    /* 70 */ "Maracas",
    /* 71 */ "Short Whistle",
    /* 72 */ "Long Whistle",
    /* 73 */ "Short Guiro",
    /* 74 */ "Long Guiro",
    /* 75 */ "Claves",
    /* 76 */ "Hi Wood Block",
    /* 77 */ "Low Wood Block",
    /* 78 */ "Mute Cuica",
    /* 79 */ "Open Cuica",
    /* 80 */ "Mute Triangle",
    /* 81 */ "Open Triangle",
};

// Name for a GM percussion note, or "" if outside the GM percussion range.
inline const char* GmPercussionName(int note) {
    if (note < kGmPercussionLo || note > kGmPercussionHi)
        return "";
    return kGmPercussionNames[note - kGmPercussionLo];
}

} // namespace SOH
