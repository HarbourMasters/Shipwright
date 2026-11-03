#include <functional>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

#include <ship/Context.h>

#include "AudioEditor.h"
#include "sequence.h"
#include <functions.h>
#include "soh/ShipUtils.h"
#include "soh/OTRGlobals.h"

extern "C" int AudioPlayer_GetOutputRate(void);
#include "soh/cvar_prefixes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"
#include "AudioCollection.h"
#include "OotrsArchive.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/SohGui/UIWidgets.hpp"

#if ENABLE_FLUIDSYNTH
#include "soh/Enhancements/audio/MidiSynthManager.h"
#include "soh/Enhancements/audio/FluidSynth.h"
#include "MidiTranslator.h"
#include "soh/Extractor/portable-file-dialogs.h"
#include "GmInstrumentMap.h"
#include "InstrumentNames.h"
#include <ship/resource/archive/ArchiveManager.h>
#include <ship/resource/archive/O2rArchive.h>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iterator>
#include <optional>
#include <fstream>
#include <system_error>
#include <utility>
#include <vector>
extern "C" void SOH_MidiTranslator_Reset();

namespace {
// A mod ships each soundfont as audio/synth/<pack>/soundfont.sf[23] with its
// mapping.json beside it. Mods are read one archive at a time, in mount order,
// so two mods using the same pack name never shadow each other.
constexpr const char* kSynthPackRoot = "audio/synth";
constexpr const char* kSynthPackSfGlob = "soundfont.sf[23]";
constexpr const char* kSynthPackJsonName = "mapping.json";

struct SynthPackEntry {
    std::string mod;  // the archive's file stem, as the mods menu lists it
    std::string name; // the soundfont folder inside the mod
    std::shared_ptr<Ship::Archive> archive;
    std::string sfPath;
    std::string mappingPath;
};

// One row per (sfontId, bank, program) across loaded SFs; filled after the load
// loop, consumed by the bypass UI. Names come from the SF phdr (the author's).
struct LoadedPresetEntry {
    int sfontId;
    std::string modName;
    std::string packName;
    int bank;
    int program;
    std::string name;
};
static std::vector<LoadedPresetEntry> sLoadedPresets;

// Derived from sLoadedPresets — unique (sfontId, bank) tuples in load
// order. Drives the Bank/Pack combo. Kept in step with sLoadedPresets.
struct BankSelectorEntry {
    int sfontId;
    std::string packName;
    int bank;
};
static std::vector<BankSelectorEntry> sBankSelectors;

// Edits live in memory until Save writes them into the mod being edited.
static bool sEditedModDirty = false;
void MarkModEdited() {
    if (!SOH::MidiTranslator::Instance().GetEditMod().empty())
        sEditedModDirty = true;
}

// Session-only Override-column state, never persisted.
// sSoloedPairs: pairs the user has soloed; when non-empty, every non-soloed
//   discovered pair is force-muted. Multiple rows can be soloed at once.
// sExplicitMutedPairs: per-row mutes from the Mute button on Native rows. (Synth
//   rows mute via the temp-volume 0.0 stop instead.) Stacks with solo.
static std::set<std::pair<uint8_t, int16_t>> sSoloedPairs;
static std::set<std::pair<uint8_t, int16_t>> sExplicitMutedPairs;

// Per-drum-slot analogues, keyed by (fontId, instOrWave, slot/noteLow), so an
// individual drum sound can be soloed/muted to isolate it. A soloed slot
// counts toward the global solo state (it mutes other pairs and other slots);
// see the effective-mute apply pass.
static std::set<std::tuple<uint8_t, int16_t, uint8_t>> sSoloedSlots;
static std::set<std::tuple<uint8_t, int16_t, uint8_t>> sExplicitMutedSlots;

std::string ModNameOf(Ship::Archive& archive) {
    return std::filesystem::path(archive.GetPath()).stem().string();
}

// Every soundfont of every mounted mod, in mount order (the last one wins a
// tie), and alphabetically within a mod.
std::vector<SynthPackEntry> EnumerateSynthPacks() {
    std::vector<SynthPackEntry> result;
    auto archives = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetArchives();
    if (!archives)
        return result;
    const std::string root = std::string(kSynthPackRoot) + "/";
    for (const auto& archive : *archives) {
        auto files = archive->ListFiles(root + "*/" + kSynthPackSfGlob);
        if (!files || files->empty())
            continue;
        const std::string mod = ModNameOf(*archive);
        std::vector<SynthPackEntry> inMod;
        for (const auto& [hash, path] : *files) {
            const std::string name = std::filesystem::path(path).parent_path().filename().string();
            if (path != root + name + "/" + std::filesystem::path(path).filename().string())
                continue; // deeper than one folder
            inMod.push_back({ mod, name, archive, path, root + name + "/" + kSynthPackJsonName });
        }
        std::sort(inMod.begin(), inMod.end(),
                  [](const SynthPackEntry& a, const SynthPackEntry& b) { return a.name < b.name; });
        for (auto& e : inMod)
            result.push_back(std::move(e));
    }
    return result;
}

std::vector<uint8_t> ReadArchiveFile(Ship::Archive& archive, const std::string& path) {
    std::vector<uint8_t> bytes;
    auto file = archive.LoadFile(path);
    if (!file || !file->Buffer || file->Buffer->size() <= file->BufferOffset)
        return bytes;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(file->Buffer->data()) + file->BufferOffset;
    bytes.assign(data, data + (file->Buffer->size() - file->BufferOffset));
    return bytes;
}

// Push the SF stack + mod order into the translator and recompute each entry's
// sfontId. Called after any change to entry resolution (pack load, user pick).
void RefreshEntryResolution(const std::vector<SynthPackEntry>& packs) {
    auto& tr = SOH::MidiTranslator::Instance();
    std::vector<std::string> order;
    for (const auto& p : packs)
        if (order.empty() || order.back() != p.mod)
            order.push_back(p.mod);
    tr.SetModOrder(order);

    std::vector<SOH::MidiTranslator::LoadedPresetRef> refs;
    refs.reserve(sLoadedPresets.size());
    for (const auto& lp : sLoadedPresets) {
        refs.push_back({ lp.sfontId, lp.modName, lp.packName, lp.bank, lp.program });
    }
    tr.RefreshEntrySfontIds(refs);
    tr.RecomputeAllActive();
}

// Rebuild the translator from what the mods hold, dropping unsaved edits.
void LoadModMappings(const std::vector<SynthPackEntry>& packs) {
    SOH::MidiTranslator::Instance().ResetAllOverrides();
    for (const auto& pack : packs) {
        auto bytes = ReadArchiveFile(*pack.archive, pack.mappingPath);
        if (bytes.empty())
            continue; // a soundfont without a mapping yet is valid
        std::string json(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        SOH::MidiTranslator::Instance().ApplyModMapping(json, pack.mod, pack.name);
    }
    sEditedModDirty = false;
}

void ReloadModMappings() {
    const auto packs = EnumerateSynthPacks();
    LoadModMappings(packs);
    RefreshEntryResolution(packs);
}

// Write the edited mod's mapping.json files back into its archive. Each write
// rewrites the archive, so only soundfonts whose mapping changed are written.
bool SaveEditedMod(std::string& outMsg) {
    const std::string& mod = SOH::MidiTranslator::Instance().GetEditMod();
    std::vector<SynthPackEntry> packs;
    for (auto& p : EnumerateSynthPacks())
        if (p.mod == mod)
            packs.push_back(std::move(p));
    if (packs.empty()) {
        outMsg = "No soundfont found in " + mod + ".";
        return false;
    }
    int rows = 0;
    int written = 0;
    for (size_t i = 0; i < packs.size(); i++) {
        int n = 0;
        const std::string json = SOH::MidiTranslator::Instance().BuildPackMappingJson(mod, packs[i].name, i == 0, n);
        rows += n;
        auto current = ReadArchiveFile(*packs[i].archive, packs[i].mappingPath);
        if (std::string(current.begin(), current.end()) == json)
            continue;
        if (!packs[i].archive->WriteFile(packs[i].mappingPath, std::vector<uint8_t>(json.begin(), json.end()))) {
            outMsg = "Could not write " + packs[i].mappingPath + " into " + mod + " (see log).";
            return false;
        }
        written++;
    }
    sEditedModDirty = false;
    outMsg = "Saved " + std::to_string(rows) + " rows to " + mod + " (" + std::to_string(written) + " of " +
             std::to_string(packs.size()) + " mappings changed).";
    return true;
}

std::optional<std::vector<uint8_t>> ReadSoundfontFile(const std::string& path, std::string& outExt,
                                                      std::string& outMsg) {
    outExt = std::filesystem::path(path).extension().string();
    std::transform(outExt.begin(), outExt.end(), outExt.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (outExt != ".sf2" && outExt != ".sf3") {
        outMsg = "Not a soundfont: " + path;
        return std::nullopt;
    }
    std::ifstream in(path, std::ios::binary);
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (bytes.empty()) {
        outMsg = "Could not read " + path;
        return std::nullopt;
    }
    return bytes;
}

// Put a soundfont from disk into a mod: a new mods/<name>.o2r, or the mounted
// mod being edited. Returns the mod's name.
std::optional<std::string> AddSoundfontToMod(const std::string& sfPath, bool newMod, std::string& outMsg) {
    std::string ext;
    auto bytes = ReadSoundfontFile(sfPath, ext, outMsg);
    if (!bytes)
        return std::nullopt;
    const std::string pack = std::filesystem::path(sfPath).stem().string();
    const std::string base = std::string(kSynthPackRoot) + "/" + pack + "/";
    const std::string emptyMapping = "{\n  \"version\": 2,\n  \"entries\": []\n}";

    auto archiveManager = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();
    std::shared_ptr<Ship::Archive> archive;
    if (newMod) {
        const std::string modsDir = Ship::Context::GetPathRelativeToAppDirectory("mods", appShortName);
        const std::string path = (std::filesystem::path(modsDir) / (pack + ".o2r")).generic_string();
        if (std::filesystem::exists(path)) {
            outMsg = path + " already exists. Edit that mod, or rename the soundfont.";
            return std::nullopt;
        }
        auto created = std::make_shared<Ship::O2rArchive>(path);
        const bool ok = created->Open() && created->WriteFile(base + "soundfont" + ext, *bytes) &&
                        created->WriteFile(base + kSynthPackJsonName,
                                           std::vector<uint8_t>(emptyMapping.begin(), emptyMapping.end()));
        created->Close();
        if (!ok) {
            outMsg = "Could not write " + path + " (see log).";
            return std::nullopt;
        }
        // Mounted last, where the mods menu also puts a new file next launch.
        archive = archiveManager->AddArchive(path);
        if (!archive) {
            outMsg = "Created " + path + " but could not mount it; restart to load it.";
            return std::nullopt;
        }
    } else {
        const std::string& mod = SOH::MidiTranslator::Instance().GetEditMod();
        for (const auto& a : *archiveManager->GetArchives())
            if (ModNameOf(*a) == mod)
                archive = a;
        if (!archive) {
            outMsg = "Choose the mod to add the soundfont to first.";
            return std::nullopt;
        }
        if (archive->HasFile(base + "soundfont.sf2") || archive->HasFile(base + "soundfont.sf3")) {
            outMsg = mod + " already has a soundfont named " + pack + ".";
            return std::nullopt;
        }
        if (!archive->WriteFile(base + "soundfont" + ext, *bytes) ||
            !archive->WriteFile(base + kSynthPackJsonName,
                                std::vector<uint8_t>(emptyMapping.begin(), emptyMapping.end()))) {
            outMsg = "Could not write into " + mod + " (see log).";
            return std::nullopt;
        }
    }
    const std::string mod = ModNameOf(*archive);
    outMsg = "Added " + pack + " to " + mod + ".";
    return mod;
}

// Status line surfaced by the FluidSynth tab so the user sees what's
// active right now and what (if anything) failed. Written by the apply /
// reconcile paths, read by the tab UI. Plain-string status keeps the
// state model trivial; isError just toggles the colour.
struct PipelineStatus {
    std::string message;
    bool isError = false;
    // Per-pack skip reasons ("<name>: <reason>"). A pack that fails to load
    // for any reason is skipped (the others still load); each skip is recorded
    // here and rendered amber under the main status line so a partial failure
    // is visible on screen, not just in spdlog.
    std::vector<std::string> warnings;
};
static PipelineStatus sLastStatus;

// Setting a fresh status clears any prior skip list — stale warnings from a
// previous apply must not linger once the pipeline is reconciled again.
void SetStatus(std::string msg, bool isError = false) {
    sLastStatus.message = std::move(msg);
    sLastStatus.isError = isError;
    sLastStatus.warnings.clear();
}

// Attach the skipped-pack list to the current status. Call AFTER SetStatus
// (which clears it). Empty list = no warnings shown.
void SetStatusWarnings(std::vector<std::string> warnings) {
    sLastStatus.warnings = std::move(warnings);
}

// Effective per-mode global synth gain handed to the translator. The on-screen
// slider is a relative level (1.0 = 100% = matched to native); each mode's real
// calibration to native loudness is hidden here so the UI never shows the raw
// values. Authentic is the louder mode (reverb), so it needs the deeper trim.
static float ComputeSynthGlobalGain(SOH::SynthMode mode) {
    // Per-mode constants that match synth loudness to the native engine. The
    // exponent differs because loudness rides a different FluidSynth curve per mode:
    //   Authentic: loudness via CC7 and key pressure through the halved (480 cB)
    //     modulators, so amplitude is linear in each control.
    //   Enhanced: loudness via CC7 and NoteOn velocity through the stock concave
    //     (960 cB) modulators, so amplitude goes as the square of each.
    // See MidiTranslator::ProcessNote and FluidSynth::InstallLinearVelocityModulators.
    constexpr float kGainCalAuthentic = 0.55f;
    constexpr float kGainCalEnhanced = 1.107f;
    const bool enhanced = (mode == SOH::SynthMode::Enhanced);
    const float rel =
        CVarGetFloat(enhanced ? CVAR_AUDIO("FluidSynthGainEnhanced") : CVAR_AUDIO("FluidSynthGainAuthentic"), 1.0f);
    return rel * (enhanced ? kGainCalEnhanced : kGainCalAuthentic);
}

// FluidSynth's master output gain tracks the Master Volume slider so the synth
// scales exactly as the native engine does (Master is applied per native note in
// audio_playback.c; the synth renders its own PCM downstream of that). Master is
// mode-neutral, like synth.gain, so it lives here and NOT in the per-mode trim
// (ComputeSynthGlobalGain). Default 40 mirrors the slider's default.
static float SynthMasterGainFromCVar() {
    return CVarGetInteger(CVAR_SETTING("Volume.Master"), 40) / 100.0f;
}

// Apply the Modern pipeline + synth config: install FluidSynth and stack every
// enabled pack's SF (with none enabled, native plays unchanged). Returns false
// -- surfaced as the checkbox flipping back off -- when it can't apply.
bool ApplyFluidSynthFromCVars() {
    auto audioPlayer = Ship::Context::GetRawInstance()->GetAudio()->GetAudioPlayer();
    if (!audioPlayer) {
        SPDLOG_INFO("[AudioEditor] Float audio: audio player not ready, skipping apply");
        SetStatus("Audio player not ready.", true);
        return false;
    }
    // No float-mode toggle here: the device runs at the chosen rate and the audio
    // thread always resamples up to it, so installing a synth is the whole job.
    auto packs = EnumerateSynthPacks();

    // Drop the prior synth first so the audio thread sees silence, not two synths
    // overlapping; the old shared_ptr frees once the manager releases it.
    SOH::MidiSynthManager::Instance().SetSynth(nullptr);

    if (packs.empty()) {
        // Float-only mode: native engine through the float path, no SF layer.
        SOH_MidiTranslator_Reset();
        sLoadedPresets.clear();
        sBankSelectors.clear();
        SPDLOG_INFO("[AudioEditor] Float audio: pipeline active (no synth packs enabled)");
        SetStatus("Modern audio pipeline active. No synth packs enabled.");
        return true;
    }

    // Log the intended load order. The last entry has highest priority in
    // FluidSynth's reverse-load-order preset lookup.
    {
        std::string orderLog;
        for (const auto& p : packs) {
            if (!orderLog.empty())
                orderLog += " -> ";
            orderLog += p.mod + "/" + p.name;
        }
        SPDLOG_INFO("[AudioEditor] FluidSynth: pack load order (last wins): {}", orderLog);
    }

    // The rate setting only applies on restart, so the CVar can name a rate the device is not running at.
    double sampleRate = static_cast<double>(AudioPlayer_GetOutputRate());
    SOH::FluidSynthConfig synthConfig;
    synthConfig.sampleRate = sampleRate;

    // Mode-driven configuration. Authentic = richer modulators + console-era reverb.
    // Enhanced = stock SF modulators + a subtle reverb that lets the SF's musical
    // interpretation breathe through. The translator branches its NoteOn / CC11
    // routing on the same mode.
    auto mode = static_cast<SOH::SynthMode>(CVarGetInteger(CVAR_AUDIO("FluidSynthMode"), 0));
    synthConfig.linearVelocity = mode == SOH::SynthMode::Authentic;
    synthConfig.hostReverb = mode == SOH::SynthMode::Authentic;

    // FluidSynth defaults to 256 voices. The game's native scores stay well under
    // that, but custom/modded songs can approach it, so bump to 512 for headroom.
    // If it is ever hit, the log says so and you may hear dropped or stuck notes.
    synthConfig.polyphony = 512;

    // FluidSynth's master output gain tracks the Master Volume slider, so the synth
    // scales exactly as the native engine does (Master is applied per native note in
    // audio_playback.c; the synth renders downstream of that). Loudness matching is a
    // separate per-mode trim (ComputeSynthGlobalGain -> SetGlobalGain). The live
    // slider stays in sync without a rebuild via AudioEditor_ApplySynthMasterVolume.
    synthConfig.gain = SynthMasterGainFromCVar();
    auto synth = std::make_shared<SOH::FluidSynth>(synthConfig);

    // Stack every enabled pack's SF in discovery order. FluidSynth walks
    // loaded sfonts in reverse on preset lookup, so the LAST loaded pack
    // wins on (bank, program) collisions — matches the mod stack precedence.
    size_t totalBytes = 0;
    size_t loadedPacks = 0;
    // One "<name>: <reason>" entry per pack we couldn't load. Loading is
    // best-effort: any pack that fails is skipped and the rest still load.
    std::vector<std::string> skipped;
    std::unordered_map<int, const SynthPackEntry*> idToPack;
    for (const auto& pack : packs) {
        auto bytes = ReadArchiveFile(*pack.archive, pack.sfPath);
        if (bytes.empty()) {
            const char* reason = "soundfont file missing or unreadable";
            SPDLOG_ERROR("[AudioEditor] FluidSynth: skipping pack '{}/{}' -- {}", pack.mod, pack.name, reason);
            skipped.push_back(pack.mod + "/" + pack.name + ": " + reason);
            continue;
        }
        int id = synth->AddSoundFontFromMemory(bytes.data(), bytes.size());
        if (id < 0) {
            // -1 is FluidSynth's generic loader failure: corrupt/unsupported
            // soundfont, or an .sf3 when the linked FluidSynth lacks libsndfile
            // + Vorbis. We can't tell which apart, so name both possibilities.
            const char* reason = "rejected by FluidSynth (corrupt soundfont, or "
                                 ".sf3 without libsndfile/Vorbis support)";
            SPDLOG_ERROR("[AudioEditor] FluidSynth: skipping pack '{}/{}' -- {}", pack.mod, pack.name, reason);
            skipped.push_back(pack.mod + "/" + pack.name + ": " + reason);
            continue;
        }
        idToPack[id] = &pack;
        totalBytes += bytes.size();
        loadedPacks++;
        SPDLOG_INFO("[AudioEditor] FluidSynth: loaded soundfont from pack '{}' ({} bytes, id={})", pack.name,
                    bytes.size(), id);
    }

    if (loadedPacks == 0) {
        SetStatus("No synth pack could be loaded.", true);
        SetStatusWarnings(skipped);
        return false;
    }

    // Refresh the bypass-UI preset cache off the freshly-loaded sfonts.
    // FluidSynth's iteration order is the SF's phdr order grouped by
    // sfont; we keep that ordering for the cache so the UI rows track
    // the load sequence (and therefore the priority order FluidSynth
    // applies on preset lookup).
    {
        auto raw = synth->EnumerateLoadedPresets();
        sLoadedPresets.clear();
        sLoadedPresets.reserve(raw.size());
        for (auto& r : raw) {
            LoadedPresetEntry e;
            e.sfontId = r.sfontId;
            auto it = idToPack.find(r.sfontId);
            e.modName = (it != idToPack.end()) ? it->second->mod : "(unknown)";
            e.packName = (it != idToPack.end()) ? it->second->name : "(unknown)";
            e.bank = r.bank;
            e.program = r.program;
            e.name = std::move(r.name);
            sLoadedPresets.push_back(std::move(e));
        }
        // Unique (sfontId, bank) tuples preserving the load-order of sfonts.
        // Inside one sfont, banks come out in iteration order — usually
        // numerically ascending but we don't rely on that.
        sBankSelectors.clear();
        for (const auto& p : sLoadedPresets) {
            bool exists = false;
            for (const auto& b : sBankSelectors) {
                if (b.sfontId == p.sfontId && b.bank == p.bank) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                sBankSelectors.push_back({ p.sfontId, p.packName, p.bank });
            }
        }
        SPDLOG_INFO("[AudioEditor] FluidSynth: {} presets across {} (sfont, bank) groups", sLoadedPresets.size(),
                    sBankSelectors.size());
    }

    // Refresh entry resolution against the new SF stack. Both inputs
    // (entry sfontIds and the pack load order) changed here.
    RefreshEntryResolution(packs);

    SetStatus(std::to_string(loadedPacks) + " pack" + (loadedPacks == 1 ? "" : "s") + " loaded (" +
              std::to_string(totalBytes / (1024 * 1024)) + " MiB total, " + std::to_string(sLoadedPresets.size()) +
              " presets).");
    // Some packs loaded, but not all -- surface the skipped ones too so a
    // partial failure isn't silent on screen.
    SetStatusWarnings(skipped);

    if (mode == SOH::SynthMode::Enhanced) {
        synth->SetReverbParams(0.20, 0.5, 0.5, 0.30);
    }
    // Install the synth last. The audio thread (OTRAudio_Thread) picks it up via
    // MidiSynthManager::GetActiveSynth() and mixes its Render() output into the
    // resampled native bus -- no per-player mix source to register, so the synth
    // survives backend switches (the manager owns it, not the AudioPlayer).
    SOH::MidiSynthManager::Instance().SetSynth(synth);
    SOH::MidiTranslator::Instance().SetSynthMode(mode);
    SOH::MidiTranslator::Instance().SetGlobalGain(ComputeSynthGlobalGain(mode));
    SOH_MidiTranslator_Reset();
    return true;
}
} // namespace
#endif

namespace {
// Enable/disable the Modern pipeline. The device already runs at the chosen rate
// with the audio thread resampling up, so the toggle only manages the synth.
void DisableModernAudioPipeline() {
#if ENABLE_FLUIDSYNTH
    SOH::MidiSynthManager::Instance().SetSynth(nullptr);
    SOH_MidiTranslator_Reset();
    SetStatus("Modern audio pipeline disabled.");
#endif
}

bool EnableModernAudioPipeline() {
#if ENABLE_FLUIDSYNTH
    // Full apply path: (optional) synth pack + status line.
    return ApplyFluidSynthFromCVars();
#else
    return true;
#endif
}

// Reconcile the CVar against live state once per draw, so a toggle on any tab (or
// the menu search) applies even when the original handler's tab isn't drawn.
void ReconcileModernAudioPipelineIfChanged() {
    int now = CVarGetInteger(CVAR_AUDIO("ModernAudioPipeline"), 0);
    static int sLast = now;
    if (sLast == now) {
        return;
    }
    if (now) {
        if (!EnableModernAudioPipeline()) {
            CVarSetInteger(CVAR_AUDIO("ModernAudioPipeline"), 0);
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            now = 0;
        }
    } else {
        DisableModernAudioPipeline();
    }
    sLast = now;
}
} // namespace

extern "C" {
#include "sequence.h"
#include <functions.h>
#include "z64save.h"
extern SaveContext gSaveContext;
extern ActiveSequence gActiveSeqs[4];
}

Vec3f pos = { 0.0f, 0.0f, 0.0f };
f32 freqScale = 1.0f;
s8 reverbAdd = 0;

using namespace UIWidgets;

static WidgetInfo lowHpAlarm;
static WidgetInfo naviCall;
static WidgetInfo enemyProx;
static WidgetInfo leeverProx;
static WidgetInfo leadingMusic;
static WidgetInfo displaySeqName;
static WidgetInfo ovlDuration;
static WidgetInfo voicePitch;
static WidgetInfo randomAudioGenModes;
static WidgetInfo lowerOctaves;

#if ENABLE_FLUIDSYNTH
static WidgetInfo fluidSynthEnabled;
// Global synth gain is per-mode: Authentic and Enhanced differ in loudness
// (reverb level + velocity curve), so each carries its own trim calibrated to
// the native engine. The active mode's widget is drawn in the FluidSynth tab.
static WidgetInfo fluidSynthGainAuthentic;
static WidgetInfo fluidSynthGainEnhanced;
#endif

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

// Authentic sequence counts
// used to ensure we have enough to shuffle
#define SEQ_COUNT_BGM_WORLD 30
#define SEQ_COUNT_BGM_BATTLE 6
#define SEQ_COUNT_FANFARE 15
#define SEQ_COUNT_OCARINA 12
#define SEQ_COUNT_NOSHUFFLE 6
#define SEQ_COUNT_BGM_EVENT 17
#define SEQ_COUNT_INSTRUMENT 6
#define SEQ_COUNT_SFX 57
#define SEQ_COUNT_VOICE 108
#define SEQ_COUNT_ENDING 5

size_t AuthenticCountBySequenceType(SeqType type) {
    switch (type) {
        case SEQ_NOSHUFFLE:
            return SEQ_COUNT_NOSHUFFLE;
        case SEQ_BGM_WORLD:
            return SEQ_COUNT_BGM_WORLD;
        case SEQ_BGM_EVENT:
            return SEQ_COUNT_BGM_EVENT;
        case SEQ_BGM_BATTLE:
            return SEQ_COUNT_BGM_BATTLE;
        case SEQ_OCARINA:
            return SEQ_COUNT_OCARINA;
        case SEQ_FANFARE:
            return SEQ_COUNT_FANFARE;
        case SEQ_SFX:
            return SEQ_COUNT_SFX;
        case SEQ_INSTRUMENT:
            return SEQ_COUNT_INSTRUMENT;
        case SEQ_VOICE:
            return SEQ_COUNT_VOICE;
        case SEQ_ENDING:
            return SEQ_COUNT_ENDING;
        default:
            return 0;
    }
}

static const std::map<int32_t, const char*> audioRandomizerModes = {
    { RANDOMIZE_OFF, "Manual" },
    { RANDOMIZE_ON_NEW_SCENE, "On New Scene" },
    { RANDOMIZE_ON_RANDO_GEN_ONLY, "On Rando Gen Only" },
    { RANDOMIZE_ON_FILE_LOAD, "On File Load" },
    { RANDOMIZE_ON_FILE_LOAD_SEEDED, "On File Load (Seeded)" },
};

// The last BGM sequence the game asked for. Unlike func_800FA0B4 this still works when the area's music is set
// to "None", where the sequence player sits idle because nothing loaded.
static u16 GetCurrentBGM() {
    return gActiveSeqs[SEQ_PLAYER_BGM_MAIN].seqId;
}

// Grabs the current BGM sequence ID and replays it
// which will lookup the proper override, or reset back to vanilla
void ReplayCurrentBGM() {
    u16 curSeqId = GetCurrentBGM();
    // TODO: replace with Audio_StartSeq when the macro is shared
    // The fade time and audio player flags will always be 0 in the case of replaying the BGM, so they are not set here
    Audio_QueueSeqCmd(0x00000000 | curSeqId);
}

// Attempt to update the BGM if it matches the current sequence that is being played
// The seqKey that is passed in should be the vanilla ID, not the override ID
void UpdateCurrentBGM(u16 seqKey, SeqType seqType) {
    if (seqType != SEQ_BGM_WORLD) {
        return;
    }

    u16 curSeqId = GetCurrentBGM();
    if (curSeqId == seqKey) {
        ReplayCurrentBGM();
    }
}

void RandomizeGroup(SeqType type, bool manual = true) {
    std::vector<u16> values;

    uint64_t localRngState = 0;
    uint64_t* shuffleState = nullptr;

    if (!manual) {
        int randomizeMode = CVarGetInteger(CVAR_AUDIO("RandomizeAudioGenModes"), 0);
        if (randomizeMode == RANDOMIZE_ON_FILE_LOAD_SEEDED || randomizeMode == RANDOMIZE_ON_RANDO_GEN_ONLY) {

            uint32_t finalSeed = type + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed()
                                                  : static_cast<uint32_t>(gSaveContext.ship.stats.fileCreatedAt));
            ShipUtils::RandInit(finalSeed, &localRngState);
            shuffleState = &localRngState;
        }
        // For RANDOMIZE_ON_NEW_SCENE, shuffleState remains nullptr, which uses the global RNG
    }

    // An empty IncludedSequences set means that the AudioEditor window has never been drawn
    if (AudioCollection::Instance->GetIncludedSequences().empty()) {
        AudioCollection::Instance->InitializeShufflePool();
    }

    // use a while loop to add duplicates if we don't have enough included sequences
    while (values.size() < AuthenticCountBySequenceType(type)) {
        for (const auto& seqData : AudioCollection::Instance->GetIncludedSequences()) {
            if (seqData->category & type && seqData->canBeUsedAsReplacement) {
                values.push_back(seqData->sequenceId);
            }
        }

        // if we didn't find any, return early without shuffling to prevent an infinite loop
        if (!values.size())
            return;
    }
    ShipUtils::Shuffle(values, shuffleState);
    for (const auto& [seqId, seqData] : AudioCollection::Instance->GetAllSequences()) {
        const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
        const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
        // don't randomize locked entries
        if ((seqData.category & type) && CVarGetInteger(cvarLockKey.c_str(), 0) == 0) {
            // Only save authentic sequence CVars
            if ((((seqData.category & SEQ_BGM_CUSTOM) || seqData.category == SEQ_FANFARE) &&
                 seqData.sequenceId >= MAX_AUTHENTIC_SEQID) ||
                seqData.canBeReplaced == false) {
                continue;
            }
            const int randomValue = values.back();
            CVarSetInteger(cvarKey.c_str(), randomValue);
            values.pop_back();
        }
    }
}

void ResetGroup(const std::map<u16, SequenceInfo>& map, SeqType type) {
    for (const auto& [defaultValue, seqData] : map) {
        if (seqData.category == type) {
            // Only save authentic sequence CVars
            if (seqData.category == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                continue;
            }
            const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
            const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
            if (CVarGetInteger(cvarLockKey.c_str(), 0) == 0) {
                CVarClear(cvarKey.c_str());
            }
        }
    }
}

void LockGroup(const std::map<u16, SequenceInfo>& map, SeqType type) {
    for (const auto& [defaultValue, seqData] : map) {
        if (seqData.category == type) {
            // Only save authentic sequence CVars
            if (seqData.category == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                continue;
            }
            const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
            const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
            CVarSetInteger(cvarLockKey.c_str(), 1);
        }
    }
}

void UnlockGroup(const std::map<u16, SequenceInfo>& map, SeqType type) {
    for (const auto& [defaultValue, seqData] : map) {
        if (seqData.category == type) {
            // Only save authentic sequence CVars
            if (seqData.category == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                continue;
            }
            const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
            const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
            CVarSetInteger(cvarLockKey.c_str(), 0);
        }
    }
}

void DrawPreviewButton(uint16_t sequenceId, std::string sfxKey, SeqType sequenceType) {
    const std::string cvarKey = AudioCollection::Instance->GetCvarKey(sfxKey);
    const std::string hiddenKey = "##" + cvarKey;
    const std::string stopButton = ICON_FA_STOP + hiddenKey;
    const std::string previewButton = ICON_FA_PLAY + hiddenKey;

    if (CVarGetInteger(CVAR_AUDIO("Playing"), 0) == sequenceId) {
        if (UIWidgets::Button(stopButton.c_str(), UIWidgets::ButtonOptions()
                                                      .Size(UIWidgets::Sizes::Inline)
                                                      .Padding(ImVec2(10.0f, 6.0f))
                                                      .Tooltip("Stop Preview")
                                                      .Color(THEME_COLOR))) {
            func_800F5C2C();
            // Abrupt stop: the engine swaps the sequence pointer without tracing each
            // active note through Audio_NoteDisable, so FluidSynth voices left
            // sounding become orphans in their release tail. Cycling many previews
            // accumulates them faster than they drain and exhausts the voice pool, so
            // force an immediate All Notes Off on every channel.
            SOH_MidiTranslator_Reset();
            CVarSetInteger(CVAR_AUDIO("Playing"), 0);
        }
    } else {
        if (UIWidgets::Button(previewButton.c_str(), UIWidgets::ButtonOptions()
                                                         .Size(UIWidgets::Sizes::Inline)
                                                         .Padding(ImVec2(10.0f, 6.0f))
                                                         .Tooltip("Play Preview")
                                                         .Color(THEME_COLOR))) {
            if (CVarGetInteger(CVAR_AUDIO("Playing"), 0) != 0) {
                func_800F5C2C();
                // Same orphan-voice flush as the stop button above.
                SOH_MidiTranslator_Reset();
                CVarSetInteger(CVAR_AUDIO("Playing"), 0);
            } else {
                if (sequenceType == SEQ_SFX || sequenceType == SEQ_VOICE) {
                    Audio_PlaySfxGeneral(sequenceId, &pos, 4, &freqScale, &freqScale, &reverbAdd);
                } else if (sequenceType == SEQ_INSTRUMENT) {
                    AudioOcarina_SetInstrument(sequenceId - INSTRUMENT_OFFSET);
                    AudioOcarina_SetPlaybackSong(9, 1);
                } else {
                    // TODO: Cant do both here, so have to click preview button twice
                    PreviewSequence(sequenceId);
                    CVarSetInteger(CVAR_AUDIO("Playing"), sequenceId);
                }
            }
        }
    }
}

void Draw_SfxTab(const std::string& tabId, SeqType type, const std::string& tabName) {
    const std::map<u16, SequenceInfo>& map = AudioCollection::Instance->GetAllSequences();

    const std::string hiddenTabId = "##" + tabId;
    const std::string resetAllButton = "Reset All" + hiddenTabId;
    const std::string randomizeAllButton = "Randomize All" + hiddenTabId;
    const std::string lockAllButton = "Lock All" + hiddenTabId;
    const std::string unlockAllButton = "Unlock All" + hiddenTabId;

    ImGui::SeparatorText(tabName.c_str());
    if (UIWidgets::Button(resetAllButton.c_str(),
                          UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
        auto currentBGM = GetCurrentBGM();
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        ResetGroup(map, type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    ImGui::SameLine();
    if (UIWidgets::Button(randomizeAllButton.c_str(),
                          UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
        auto currentBGM = GetCurrentBGM();
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        RandomizeGroup(type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    ImGui::SameLine();
    if (UIWidgets::Button(lockAllButton.c_str(),
                          UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
        auto currentBGM = GetCurrentBGM();
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        LockGroup(map, type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    ImGui::SameLine();
    if (UIWidgets::Button(unlockAllButton.c_str(),
                          UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
        auto currentBGM = GetCurrentBGM();
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        UnlockGroup(map, type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }

    auto playingFromMenu = CVarGetInteger(CVAR_AUDIO("Playing"), 0);
    auto currentBGM = GetCurrentBGM();

    // Longest text in Audio Editor
    ImVec2 columnSize = ImGui::CalcTextSize("Navi - Look/Hey/Watchout (Target Enemy)");
    ImGui::BeginTable(tabId.c_str(), 3, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, columnSize.x + 30);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, columnSize.x + 30);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 160.0f);
    for (const auto& [defaultValue, seqData] : map) {
        if (~(seqData.category) & type) {
            continue;
        }
        // Do not display custom sequences in the list
        if ((((seqData.category & SEQ_BGM_CUSTOM) || seqData.category == SEQ_FANFARE) &&
             defaultValue >= MAX_AUTHENTIC_SEQID) ||
            seqData.canBeReplaced == false) {
            continue;
        }

        const std::string initialSfxKey = seqData.sfxKey;
        const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
        const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
        const std::string hiddenKey = "##" + cvarKey;
        const std::string resetButton = ICON_FA_UNDO + hiddenKey;
        const std::string randomizeButton = ICON_FA_RANDOM + hiddenKey;
        const std::string lockedButton = ICON_FA_LOCK + hiddenKey;
        const std::string unlockedButton = ICON_FA_UNLOCK + hiddenKey;
        const int currentValue = CVarGetInteger(cvarKey.c_str(), defaultValue);
        const bool isCurrentlyPlaying = currentValue == playingFromMenu || seqData.sequenceId == currentBGM;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (isCurrentlyPlaying) {
            ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow), "%s %s", ICON_FA_PLAY,
                               seqData.label.c_str());
        } else {
            ImGui::Text("%s", seqData.label.c_str());
        }
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        const int initialValue = map.contains(currentValue) ? currentValue : defaultValue;
        const bool isNone = currentValue == NA_BGM_DISABLED;
        UIWidgets::PushStyleCombobox(THEME_COLOR);
        if (ImGui::BeginCombo(hiddenKey.c_str(), isNone ? "None" : map.at(initialValue).label.c_str())) {
            if (type == SEQ_BGM_WORLD || type == SEQ_SFX || type == SEQ_VOICE) {
                if (ImGui::Selectable("None")) {
                    CVarSetInteger(cvarKey.c_str(), NA_BGM_DISABLED);
                    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                    UpdateCurrentBGM(defaultValue, type);
                }
                if (isNone) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            for (const auto& [value, seqData] : map) {
                // If excluded as a replacement sequence, don't show in other dropdowns except effect's dropdown.
                if (~(seqData.category) & type ||
                    (!seqData.canBeUsedAsReplacement && initialSfxKey != seqData.sfxKey)) {
                    continue;
                }

                if (ImGui::Selectable(seqData.label.c_str())) {
                    CVarSetInteger(cvarKey.c_str(), value);
                    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                    UpdateCurrentBGM(defaultValue, type);
                }

                if (currentValue == value) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        UIWidgets::PopStyleCombobox();
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        // Nothing to preview when set to "None"
        ImGui::BeginDisabled(isNone);
        DrawPreviewButton((type == SEQ_SFX || type == SEQ_VOICE || type == SEQ_INSTRUMENT || isNone) ? defaultValue
                                                                                                     : currentValue,
                          seqData.sfxKey, type);
        ImGui::EndDisabled();
        auto locked = CVarGetInteger(cvarLockKey.c_str(), 0) == 1;
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (UIWidgets::Button(resetButton.c_str(), UIWidgets::ButtonOptions()
                                                       .Size(UIWidgets::Sizes::Inline)
                                                       .Padding(ImVec2(10.0f, 6.0f))
                                                       .Tooltip("Reset to default")
                                                       .Color(THEME_COLOR))) {
            CVarClear(cvarKey.c_str());
            CVarClear(cvarLockKey.c_str());
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            UpdateCurrentBGM(defaultValue, seqData.category);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (UIWidgets::Button(randomizeButton.c_str(), UIWidgets::ButtonOptions()
                                                           .Size(UIWidgets::Sizes::Inline)
                                                           .Padding(ImVec2(10.0f, 6.0f))
                                                           .Tooltip("Randomize this sound")
                                                           .Color(THEME_COLOR))) {
            std::vector<SequenceInfo*> validSequences = {};
            for (const auto seqInfo : AudioCollection::Instance->GetIncludedSequences()) {
                if (seqInfo->category & type) {
                    validSequences.push_back(seqInfo);
                }
            }

            if (validSequences.size()) {
                auto it = validSequences.begin();
                const auto& seqData =
                    *std::next(it, ShipUtils::Random(0, static_cast<uint32_t>(validSequences.size())));
                CVarSetInteger(cvarKey.c_str(), seqData->sequenceId);
                if (locked) {
                    CVarClear(cvarLockKey.c_str());
                }
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                UpdateCurrentBGM(defaultValue, type);
            }
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (UIWidgets::Button(locked ? lockedButton.c_str() : unlockedButton.c_str(),
                              UIWidgets::ButtonOptions()
                                  .Size(UIWidgets::Sizes::Inline)
                                  .Padding(ImVec2(10.0f, 6.0f))
                                  .Tooltip(locked ? "Sound locked" : "Sound unlocked")
                                  .Color(THEME_COLOR))) {
            if (locked) {
                CVarClear(cvarLockKey.c_str());
            } else {
                CVarSetInteger(cvarLockKey.c_str(), 1);
            }
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
    }
    ImGui::EndTable();
}

extern "C" u16 AudioEditor_GetReplacementSeq(u16 seqId) {
    return AudioCollection::Instance->GetReplacementSequence(seqId);
}

std::string GetSequenceTypeName(SeqType type) {
    switch (type) {
        case SEQ_NOSHUFFLE:
            return "No Shuffle";
        case SEQ_BGM_WORLD:
            return "World";
        case SEQ_BGM_EVENT:
            return "Event";
        case SEQ_BGM_BATTLE:
            return "Battle";
        case SEQ_OCARINA:
            return "Ocarina";
        case SEQ_FANFARE:
            return "Fanfare";
        case SEQ_BGM_ERROR:
            return "Error";
        case SEQ_SFX:
            return "SFX";
        case SEQ_VOICE:
            return "Voice";
        case SEQ_INSTRUMENT:
            return "Instrument";
        case SEQ_BGM_CUSTOM:
            return "Custom";
        default:
            return "No Sequence Type";
    }
}

ImVec4 GetSequenceTypeColor(SeqType type) {
    switch (type) {
        case SEQ_BGM_WORLD:
            return ImVec4(0.0f, 0.2f, 0.0f, 1.0f);
        case SEQ_BGM_EVENT:
            return ImVec4(0.3f, 0.0f, 0.15f, 1.0f);
        case SEQ_BGM_BATTLE:
            return ImVec4(0.2f, 0.07f, 0.0f, 1.0f);
        case SEQ_OCARINA:
            return ImVec4(0.0f, 0.0f, 0.4f, 1.0f);
        case SEQ_FANFARE:
            return ImVec4(0.3f, 0.0f, 0.3f, 1.0f);
        case SEQ_SFX:
            return ImVec4(0.4f, 0.33f, 0.0f, 1.0f);
        case SEQ_VOICE:
            return ImVec4(0.3f, 0.42f, 0.09f, 1.0f);
        case SEQ_INSTRUMENT:
            return ImVec4(0.0f, 0.25f, 0.5f, 1.0f);
        case SEQ_BGM_CUSTOM:
            return ImVec4(0.9f, 0.0f, 0.9f, 1.0f);
        default:
            return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}

void DrawTypeChip(SeqType type, std::string sequenceName) {
    ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button, GetSequenceTypeColor(type));
    std::string buttonLabel = GetSequenceTypeName(type) + "##" + sequenceName;
    ImGui::Button(buttonLabel.c_str());
    ImGui::PopStyleColor();
    ImGui::EndDisabled();
}

void AudioEditorRegisterOnSceneInitHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
        if (gSaveContext.gameMode != GAMEMODE_END_CREDITS &&
            CVarGetInteger(CVAR_AUDIO("RandomizeAudioGenModes"), 0) == RANDOMIZE_ON_NEW_SCENE) {

            AudioEditor_AutoRandomizeAll();
        }
    });
}

void AudioEditorRegisterOnGenerationCompletionHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGenerationCompletion>([]() {
        if (CVarGetInteger(CVAR_AUDIO("RandomizeAudioGenModes"), 0) == RANDOMIZE_ON_RANDO_GEN_ONLY) {

            AudioEditor_AutoRandomizeAll();
        }
    });
}

void AudioEditorRegisterOnLoadGameHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        if (CVarGetInteger(CVAR_AUDIO("RandomizeAudioGenModes"), 0) == RANDOMIZE_ON_FILE_LOAD ||
            CVarGetInteger(CVAR_AUDIO("RandomizeAudioGenModes"), 0) == RANDOMIZE_ON_FILE_LOAD_SEEDED) {

            AudioEditor_AutoRandomizeAll();
        }
    });
}

void AudioEditor::InitElement() {
    AudioEditorRegisterOnSceneInitHook();
    AudioEditorRegisterOnGenerationCompletionHook();
    AudioEditorRegisterOnLoadGameHook();
}

void AudioEditor::DrawElement() {
    AudioCollection::Instance->InitializeShufflePool();

    // Pick up Modern audio pipeline CVar transitions regardless of which
    // tab is active right now — the toggle can land here from the Audio
    // Options checkbox, from the FluidSynth tab's Enable button, or from
    // the menu search bar.
    ReconcileModernAudioPipelineIfChanged();

    UIWidgets::Separator();
    if (UIWidgets::Button("Randomize All Groups",
                          UIWidgets::ButtonOptions()
                              .Size(ImVec2(230.0f, 0.0f))
                              .Color(THEME_COLOR)
                              .Tooltip("Randomizes all unlocked music and sound effects across tab groups"))) {
        AudioEditor_RandomizeAll();
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Reset All Groups",
                          UIWidgets::ButtonOptions()
                              .Size(ImVec2(230.0f, 0.0f))
                              .Color(THEME_COLOR)
                              .Tooltip("Resets all unlocked music and sound effects across tab groups"))) {
        AudioEditor_ResetAll();
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Lock All Groups", UIWidgets::ButtonOptions()
                                                 .Size(ImVec2(230.0f, 0.0f))
                                                 .Color(THEME_COLOR)
                                                 .Tooltip("Locks all music and sound effects across tab groups"))) {
        AudioEditor_LockAll();
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Unlock All Groups", UIWidgets::ButtonOptions()
                                                   .Size(ImVec2(230.0f, 0.0f))
                                                   .Color(THEME_COLOR)
                                                   .Tooltip("Unlocks all music and sound effects across tab groups"))) {
        AudioEditor_UnlockAll();
    }

    const std::vector<std::string>& skippedMusic = SOH::GetOotrsSkippedForCustomBank();
    if (!skippedMusic.empty()) {
        UIWidgets::Separator();
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow),
                           "%zu custom music file(s) were skipped: custom soundbanks are not supported yet.",
                           skippedMusic.size());
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            for (const std::string& file : skippedMusic) {
                ImGui::BulletText("%s", file.c_str());
            }
            ImGui::EndTooltip();
        }
    }

    UIWidgets::Separator();

    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTabBar("SfxContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {

        static ImVec2 cellPadding(8.0f, 8.0f);
        if (ImGui::BeginTabItem("Audio Options")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            ImGui::BeginTable("Audio Options", 1, ImGuiTableFlags_SizingStretchSame);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::BeginChild("SfxOptions", ImVec2(0, -8))) {
                SohGui::mSohMenu->MenuDrawItem(lowHpAlarm, THEME_COLOR);
                SohGui::mSohMenu->MenuDrawItem(naviCall, THEME_COLOR);
                SohGui::mSohMenu->MenuDrawItem(enemyProx, THEME_COLOR);
                if (!CVarGetInteger(CVAR_AUDIO("EnemyBGMDisable"), 0)) {
                    SohGui::mSohMenu->MenuDrawItem(leeverProx, THEME_COLOR);
                }
                SohGui::mSohMenu->MenuDrawItem(leadingMusic, THEME_COLOR);
                SohGui::mSohMenu->MenuDrawItem(displaySeqName, THEME_COLOR);
                SohGui::mSohMenu->MenuDrawItem(ovlDuration, THEME_COLOR);
                SohGui::mSohMenu->MenuDrawItem(voicePitch, THEME_COLOR);
                ImGui::SameLine();
                ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 40.f);
                if (UIWidgets::Button("Reset##linkVoiceFreqMultiplier",
                                      UIWidgets::ButtonOptions().Size(ImVec2(80, 36)).Padding(ImVec2(5.0f, 0.0f)))) {
                    CVarSetFloat(CVAR_AUDIO("LinkVoiceFreqMultiplier"), 1.0f);
                }
                SohGui::mSohMenu->MenuDrawItem(randomAudioGenModes, THEME_COLOR);
                SohGui::mSohMenu->MenuDrawItem(lowerOctaves, THEME_COLOR);

                // Master switch for the float audio pipeline. Always compiled; the
                // float pipeline works without FluidSynth. When ENABLE_FLUIDSYNTH is
                // on, a dedicated FluidSynth tab exposes pack selection and overrides.
                // CVar transitions are picked up by ReconcileModernAudioPipelineIfChanged
                // at the top of DrawElement, so a toggle here takes effect on any tab.
                SohGui::mSohMenu->MenuDrawItem(fluidSynthEnabled, THEME_COLOR);

                // (FluidSynth pack selection, mode/volume, and the
                // per-instrument override table live in the dedicated
                // "FluidSynth" tab — see BeginTabItem("FluidSynth") below.)
            }
            ImGui::EndChild();
            ImGui::EndTable();
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

#if ENABLE_FLUIDSYNTH
        if (ImGui::BeginTabItem("FluidSynth")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            ImGui::BeginTable("FluidSynthTab", 1, ImGuiTableFlags_SizingStretchSame);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::BeginChild("FluidSynthChild", ImVec2(0, -8))) {

                const bool pipelineOn = CVarGetInteger(CVAR_AUDIO("ModernAudioPipeline"), 0) != 0;

                if (!pipelineOn) {
                    // Pipeline is the gate for everything in this tab. Offer a
                    // one-click affordance so users who land here from the
                    // menu / docs don't have to flip back to Audio Options to
                    // enable it. The CVar transition is picked up by the
                    // top-of-DrawElement reconcile on the next frame.
                    ImGui::TextWrapped("The Modern audio pipeline is required to use FluidSynth. "
                                       "Enable it to load a synth pack and route engine instruments "
                                       "through it.");
                    ImGui::Spacing();
                    if (ImGui::Button("Enable Modern Pipeline", ImVec2(220, 0))) {
                        CVarSetInteger(CVAR_AUDIO("ModernAudioPipeline"), 1);
                        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                    }
                } else {
                    // ── Status line ──────────────────────────────────────
                    // sLastStatus is written by ApplyFluidSynthFromCVars and
                    // friends so failure messages persist on screen instead
                    // of just landing in spdlog.
                    if (!sLastStatus.message.empty()) {
                        const ImVec4 green(0.40f, 0.85f, 0.45f, 1.0f);
                        const ImVec4 red(0.95f, 0.45f, 0.45f, 1.0f);
                        ImGui::TextColored(sLastStatus.isError ? red : green, "%s", sLastStatus.message.c_str());
                    }
                    // Per-pack skip reasons from the last apply -- amber, one
                    // line each, wrapped so long reasons stay readable.
                    if (!sLastStatus.warnings.empty()) {
                        const ImVec4 amber(0.95f, 0.75f, 0.30f, 1.0f);
                        ImGui::PushStyleColor(ImGuiCol_Text, amber);
                        for (const auto& w : sLastStatus.warnings) {
                            ImGui::TextWrapped("Skipped %s", w.c_str());
                        }
                        ImGui::PopStyleColor();
                    }
                    ImGui::Separator();

                    // ── Synth mods ───────────────────────────────────────
                    // The mods menu decides which mods load and in what order;
                    // this panel only picks the one being edited.
                    static std::vector<SynthPackEntry> packs;
                    static bool packsListed = false;
                    if (!packsListed) {
                        packs = EnumerateSynthPacks();
                        packsListed = true;
                    }
                    const int enabledCount = static_cast<int>(packs.size());
                    std::vector<std::string> synthMods;
                    for (const auto& e : packs)
                        if (std::find(synthMods.begin(), synthMods.end(), e.mod) == synthMods.end())
                            synthMods.push_back(e.mod);

                    auto& translator = SOH::MidiTranslator::Instance();
                    static std::string sModMsg;
                    static bool sModMsgError = false;
                    // The file dialog runs on its own thread; the tab polls it so the
                    // game keeps rendering while it is open.
                    static std::unique_ptr<pfd::open_file> sSoundfontDialog;
                    static bool sSoundfontDialogNewMod = false;
                    auto openSoundfontDialog = [&](bool newMod) {
                        if (!pfd::settings::available()) {
                            sModMsg = "No file dialog available on this system.";
                            sModMsgError = true;
                            return;
                        }
                        sSoundfontDialogNewMod = newMod;
                        sSoundfontDialog = std::make_unique<pfd::open_file>(
                            "Choose a soundfont", "", std::vector<std::string>{ "Soundfonts", "*.sf2 *.sf3" });
                    };
                    if (sSoundfontDialog && sSoundfontDialog->ready(0)) {
                        const auto chosen = sSoundfontDialog->result();
                        sSoundfontDialog.reset();
                        if (!chosen.empty()) {
                            auto mod = AddSoundfontToMod(chosen.front(), sSoundfontDialogNewMod, sModMsg);
                            sModMsgError = !mod.has_value();
                            if (mod) {
                                packs = EnumerateSynthPacks();
                                translator.SetEditMod(*mod);
                                if (CVarGetInteger(CVAR_AUDIO("ModernAudioPipeline"), 0))
                                    ApplyFluidSynthFromCVars();
                            }
                        }
                    }

                    ImGui::Text("Synth mods (%d soundfonts in %d mods)", enabledCount,
                                static_cast<int>(synthMods.size()));
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Rescan##fluidsynthPacks")) {
                        packs = EnumerateSynthPacks();
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Re-list audio/synth/*/soundfont.sf2 / .sf3 in the mounted mods.");
                    }

                    {
                        const std::string& editMod = translator.GetEditMod();
                        ImGui::TextUnformatted("Editing:");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(260.0f);
                        ImGui::BeginDisabled(sEditedModDirty);
                        if (ImGui::BeginCombo("##editMod",
                                              editMod.empty() ? "Nothing (listen only)" : editMod.c_str())) {
                            if (ImGui::Selectable("Nothing (listen only)", editMod.empty()))
                                translator.SetEditMod("");
                            for (const auto& m : synthMods)
                                if (ImGui::Selectable(m.c_str(), m == editMod))
                                    translator.SetEditMod(m);
                            ImGui::EndCombo();
                        }
                        ImGui::EndDisabled();
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                            ImGui::SetTooltip(sEditedModDirty ? "Save or revert the changes first."
                                                              : "Changes go into this mod only. Other mods are\n"
                                                                "never written, whatever plays from them.");
                        }
                        ImGui::SameLine();
                        ImGui::BeginDisabled(!sEditedModDirty);
                        if (ImGui::Button("Save##editMod")) {
                            sModMsgError = !SaveEditedMod(sModMsg);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Revert##editMod")) {
                            ReloadModMappings();
                            sModMsg = "Reloaded the mappings from the mods.";
                            sModMsgError = false;
                        }
                        ImGui::EndDisabled();
                        if (sEditedModDirty) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.95f, 0.75f, 0.30f, 1.0f), "unsaved changes");
                        }

                        ImGui::BeginDisabled(sSoundfontDialog != nullptr);
                        if (ImGui::Button("New mod from soundfont...")) {
                            openSoundfontDialog(true);
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                            ImGui::SetTooltip("Creates mods/<soundfont>.o2r holding the soundfont and an\n"
                                              "empty mapping, loads it, and starts editing it.");
                        }
                        ImGui::SameLine();
                        ImGui::BeginDisabled(editMod.empty());
                        if (ImGui::Button("Add soundfont...")) {
                            openSoundfontDialog(false);
                        }
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                            ImGui::SetTooltip("Adds another soundfont to the mod being edited.");
                        }
                        ImGui::EndDisabled();
                        ImGui::EndDisabled();
                        if (!sModMsg.empty()) {
                            ImGui::TextColored(sModMsgError ? ImVec4(1.0f, 0.4f, 0.4f, 1.0f)
                                                            : ImVec4(0.6f, 0.9f, 0.6f, 1.0f),
                                               "%s", sModMsg.c_str());
                        }
                    }

                    if (packs.empty()) {
                        ImGui::TextDisabled("No synth mods loaded. Create one from a soundfont above, or\n"
                                            "install a mod that ships audio/synth/<name>/soundfont.sf2.");
                    } else {
                        const float rowH = ImGui::GetTextLineHeightWithSpacing();
                        float listH = rowH * static_cast<float>(std::min<size_t>(packs.size(), 10)) + 12.0f;
                        if (ImGui::BeginChild("##synthPackList", ImVec2(420.0f, listH), ImGuiChildFlags_Border)) {
                            for (size_t i = 0; i < packs.size(); i++) {
                                const auto& e = packs[i];
                                ImGui::PushID(static_cast<int>(i));
                                ImGui::TextDisabled("%s", e.mod.c_str());
                                ImGui::SameLine();
                                ImGui::TextUnformatted(e.name.c_str());
                                if (ImGui::IsItemHovered()) {
                                    ImGui::SetTooltip("%s\nmapping: %s", e.sfPath.c_str(), e.mappingPath.c_str());
                                }
                                ImGui::PopID();
                            }
                        }
                        ImGui::EndChild();
                        ImGui::TextDisabled("Where two mods map the same range, the one lower in the\n"
                                            "mods menu wins. Enable and order mods there.");
                    }

                    if (enabledCount == 0) {
                        // Pipeline-only mode: native synthesis runs through
                        // the float path, but no SF substitution happens.
                        // The synth-side controls (mode radio, volume slider,
                        // bypass table) are still relevant only when at
                        // least one pack is active.
                        ImGui::Spacing();
                        ImGui::TextDisabled("No synth packs enabled. The Modern audio pipeline is\n"
                                            "active on its own (no instrument timbre change).");
                    } else {
                        ImGui::Separator();

                        // ── Synth mode + volume ──────────────────────────
                        // Draw the gain slider for the mode that's actually
                        // active, so editing volume targets that mode's trim.
                        {
                            int gmode = CVarGetInteger(CVAR_AUDIO("FluidSynthMode"), 0);
                            SohGui::mSohMenu->MenuDrawItem(
                                gmode == 1 ? fluidSynthGainEnhanced : fluidSynthGainAuthentic, THEME_COLOR);
                            // Push live so dragging the slider updates loudness immediately
                            // (the translator no longer reads the CVar on the audio path).
                            SOH::MidiTranslator::Instance().SetGlobalGain(
                                ComputeSynthGlobalGain(static_cast<SOH::SynthMode>(gmode)));
                        }

                        {
                            int mode = CVarGetInteger(CVAR_AUDIO("FluidSynthMode"), 0);
                            ImGui::TextUnformatted("Synth mode:");
                            ImGui::SameLine();
                            if (ImGui::RadioButton("Authentic##synthMode", mode == 0)) {
                                CVarSetInteger(CVAR_AUDIO("FluidSynthMode"), 0);
                                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Console-style volume curve + console-era reverb.\n"
                                                  "Translator fixes NoteOn velocity at 100 and routes the\n"
                                                  "sqrt(velocity)-shaped value through CC11.");
                            }
                            ImGui::SameLine();
                            if (ImGui::RadioButton("Enhanced##synthMode", mode == 1)) {
                                CVarSetInteger(CVAR_AUDIO("FluidSynthMode"), 1);
                                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Stock SF default modulators + subtle reverb.\n"
                                                  "Translator sends the sqrt(velocity)-shaped value as\n"
                                                  "NoteOn velocity so the SF's own concave attenuation\n"
                                                  "modulator shapes dynamics. Good with musically-curated\n"
                                                  "banks (MuseScore, SC-55, orchestral packs).");
                            }

                            int nowMode = CVarGetInteger(CVAR_AUDIO("FluidSynthMode"), 0);
                            static int sLastMode = nowMode;
                            if (nowMode != sLastMode) {
                                ApplyFluidSynthFromCVars();
                                sLastMode = nowMode;
                            }
                        }

                        ImGui::Separator();

                        // ── Voice budget readout ─────────────────────────
                        // Snapshot the synth's voice state once per frame. When the
                        // active count approaches the polyphony limit, new NoteOns
                        // steal old voices — that's audible as "cuts" on dense
                        // songs. Colour-tier the text so the eye reads "near cap"
                        // without having to do the division.
                        {
                            auto activeSynth = SOH::MidiSynthManager::Instance().GetActiveSynth();
                            uint32_t voiceActive = activeSynth ? activeSynth->GetActiveVoiceCount() : 0u;
                            uint32_t voiceLimit = activeSynth ? activeSynth->GetPolyphonyLimit() : 0u;
                            float ratio = voiceLimit > 0 ? float(voiceActive) / float(voiceLimit) : 0.0f;
                            ImVec4 colour(0.70f, 0.70f, 0.70f, 1.0f); // disabled grey baseline
                            if (ratio >= 0.80f)
                                colour = ImVec4(1.00f, 0.40f, 0.40f, 1.0f); // red
                            else if (ratio >= 0.60f)
                                colour = ImVec4(1.00f, 0.85f, 0.30f, 1.0f); // amber
                            if (voiceLimit > 0) {
                                ImGui::TextColored(colour, "FluidSynth voices: %u / %u", (unsigned)voiceActive,
                                                   (unsigned)voiceLimit);
                            } else {
                                ImGui::TextDisabled("FluidSynth voices: -");
                            }
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Active voices held by FluidSynth out of its polyphony\n"
                                                  "limit (default 256). When this approaches the limit,\n"
                                                  "new NoteOns steal old voices and dense passages cut.\n"
                                                  "If 'cuts' line up with values well below the limit,\n"
                                                  "the bottleneck is audio-thread CPU, not voices.");
                            }

                            // ── Channel pool readout ─────────────────────────
                            // Distinct (fontId, instOrWave) pairs each claim one
                            // MIDI channel out of 64. The pool recycles idle pairs'
                            // channels on exhaustion, so sitting at 64 is normal;
                            // "reclaims" just counts how often a quiet pair handed
                            // its channel to a new one.
                            {
                                uint32_t chUsed = SOH::MidiTranslator::Instance().GetChannelsInUse();
                                uint32_t chMax = SOH::MidiTranslator::kMaxMidiChannels;
                                uint32_t chReclaims = SOH::MidiTranslator::Instance().GetChannelReclaims();
                                ImGui::TextDisabled("Synth channels: %u / %u  (reclaims: %u)", (unsigned)chUsed,
                                                    (unsigned)chMax, (unsigned)chReclaims);
                                if (ImGui::IsItemHovered()) {
                                    ImGui::SetTooltip("MIDI channels handed to sequence channels, one per\n"
                                                      "sequence channel and instrument, out of the 64-channel\n"
                                                      "pool. At the cap, the pool recycles a channel that has\n"
                                                      "gone quiet; 'reclaims' counts how often that happened.\n"
                                                      "Sitting at 64 is normal on a long session.");
                                }
                            }
                            // Throttled log so we can correlate user-reported cuts
                            // with the voice budget without spamming. Game-thread
                            // tick; coarse-grained on purpose.
                            static double sLastVoiceWarnTime = -10.0;
                            const double now = ImGui::GetTime();
                            if (voiceLimit > 0 && ratio >= 0.80f && (now - sLastVoiceWarnTime) > 1.0) {
                                SPDLOG_WARN("[FluidSynth] high voice usage: {} / {} ({:.0f}%)", voiceActive, voiceLimit,
                                            ratio * 100.0);
                                sLastVoiceWarnTime = now;
                            }
                        }

                        // ── Instrument filters ───────────────────────────
                        bool onlyNamed = CVarGetInteger(CVAR_AUDIO("SynthOnlyNamed"), 1) != 0;
                        bool onlyPlayed = CVarGetInteger(CVAR_AUDIO("SynthOnlyPlayed"), 0) != 0;
                        int fontFilter = CVarGetInteger(CVAR_AUDIO("SynthFontFilter"), -1);
                        bool simpleView = CVarGetInteger(CVAR_AUDIO("SynthSimpleView"), 1) != 0;
                        {
                            bool changed = false;
                            if (ImGui::Checkbox("Only named", &onlyNamed)) {
                                CVarSetInteger(CVAR_AUDIO("SynthOnlyNamed"), onlyNamed);
                                changed = true;
                            }
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Hide slots with no registered sample name (usually unused).");
                            ImGui::SameLine();
                            if (ImGui::Checkbox("Only played", &onlyPlayed)) {
                                CVarSetInteger(CVAR_AUDIO("SynthOnlyPlayed"), onlyPlayed);
                                changed = true;
                            }
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Show only instruments that have played. Use Clear to\n"
                                                  "reset the list, then it repopulates as notes sound.");
                            ImGui::SameLine();
                            ImGui::BeginDisabled(!onlyPlayed);
                            if (ImGui::SmallButton("Clear")) {
                                SOH::MidiTranslator::Instance().ClearDiscovered();
                            }
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Reset the played list. Hit this when a new song starts\n"
                                                  "to isolate that song's instruments.");
                            ImGui::EndDisabled();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Font:");
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(180.0f);
                            const char* curF =
                                fontFilter < 0 ? "All" : SOH::GetFontName(static_cast<uint8_t>(fontFilter));
                            if (ImGui::BeginCombo("##synthFontFilter", curF ? curF : "?")) {
                                if (ImGui::Selectable("All", fontFilter < 0)) {
                                    CVarSetInteger(CVAR_AUDIO("SynthFontFilter"), -1);
                                    fontFilter = -1;
                                    changed = true;
                                }
                                int lastFont = -1;
                                for (const auto& inst : SOH::EnumerateRegisteredInstruments()) {
                                    if (inst.fontId == lastFont)
                                        continue; // sorted by fontId -> one row per font
                                    lastFont = inst.fontId;
                                    const char* fn = SOH::GetFontName(inst.fontId);
                                    char lbl[64];
                                    std::snprintf(lbl, sizeof(lbl), "%s##f%d", fn ? fn : "(font)", inst.fontId);
                                    if (ImGui::Selectable(lbl, fontFilter == inst.fontId)) {
                                        CVarSetInteger(CVAR_AUDIO("SynthFontFilter"), inst.fontId);
                                        fontFilter = inst.fontId;
                                        changed = true;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            ImGui::SameLine();
                            if (ImGui::Checkbox("Simple view", &simpleView)) {
                                CVarSetInteger(CVAR_AUDIO("SynthSimpleView"), simpleView);
                                changed = true;
                            }
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Show just the instrument, its name and its preset. Splits\n"
                                                  "and drums stay expandable; tuning and split editing live\n"
                                                  "in the full view.");
                            if (changed)
                                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                        }
                        ImGui::Separator();

                        // ── Per-instrument overrides ─────────────────────
                        // All registered instruments plus discovered drum/SFX slots, filtered.
                        std::vector<SOH::DiscoveredPair> pairs;
                        {
                            SOH::DiscoveredPair disc[SOH::MidiTranslator::kMaxDiscovered];
                            int nd = SOH::MidiTranslator::Instance().DiscoveredSnapshot(
                                disc, SOH::MidiTranslator::kMaxDiscovered);
                            std::set<std::pair<uint8_t, int16_t>> played;
                            for (int i = 0; i < nd; i++)
                                played.insert({ disc[i].fontId, disc[i].instOrWave });
                            std::set<std::pair<uint8_t, int16_t>> seen;
                            auto consider = [&](uint8_t fontId, int16_t io, bool named) {
                                if (!seen.insert({ fontId, io }).second)
                                    return;
                                if (fontFilter >= 0 && fontId != fontFilter)
                                    return;
                                if (onlyPlayed && !played.count({ fontId, io }))
                                    return;
                                if (onlyNamed && !named)
                                    return;
                                pairs.push_back({ fontId, io, false });
                            };
                            for (const auto& inst : SOH::EnumerateRegisteredInstruments()) {
                                const bool named =
                                    !inst.name.empty() || !SOH::MidiTranslator::Instance()
                                                               .GetDisplayName(inst.fontId, inst.instOrWave)
                                                               .empty();
                                consider(inst.fontId, inst.instOrWave, named);
                            }
                            // Drum channels aren't in the melodic registry.
                            for (uint8_t f : SOH::EnumerateDrumFonts())
                                consider(f, 0, true);
                            // Discovered pairs the registry lacks; named so "Only named" keeps them.
                            for (int i = 0; i < nd; i++)
                                consider(disc[i].fontId, disc[i].instOrWave, true);
                            // Single stable order regardless of filters: font, then instrument.
                            std::sort(pairs.begin(), pairs.end(),
                                      [](const SOH::DiscoveredPair& a, const SOH::DiscoveredPair& b) {
                                          if (a.fontId != b.fontId)
                                              return a.fontId < b.fontId;
                                          return a.instOrWave < b.instOrWave;
                                      });
                        }
                        int nPairs = static_cast<int>(pairs.size());

                        bool transSemis = CVarGetInteger(CVAR_AUDIO("FluidSynthTransSemitones"), 0) != 0;
                        const float viewportH = ImGui::GetMainViewport()->Size.y;
                        float bypassTableHeight = viewportH * 0.65f;
                        if (bypassTableHeight < 400.0f)
                            bypassTableHeight = 400.0f;
                        if (bypassTableHeight > 900.0f)
                            bypassTableHeight = 900.0f;
                        // Column-id helper. Postincrement each named slot so
                        // adding/removing columns shifts the rest automatically. The
                        // saved variables (modeCol, presetCol, ...) are the only IDs
                        // referenced by per-row code below, so renumbering is a one-line
                        // edit here, not a hunt-and-replace.
                        uint8_t col = 0;
                        const uint8_t overrideCol = col++;
                        const uint8_t songCol = col++;
                        const uint8_t sampleCol = col++;
                        const uint8_t instCol = col++;
                        const uint8_t modeCol = col++;
                        const uint8_t gainCol = col++;
                        const uint8_t shiftCol = col++;
                        const uint8_t presetCol = col++;
                        // Adv: per-entry advanced popup (Reverb/Chorus/Cutoff/Q). Font and
                        // Source columns were folded into the Song / Preset tooltips; the four
                        // effect columns were folded into this one popup.
                        const uint8_t advCol = col++;
                        const uint8_t kColCount = col;

                        if (ImGui::BeginTable("##bypassTable", kColCount,
                                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                                  ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX |
                                                  ImGuiTableFlags_Resizable,
                                              ImVec2(0.0f, bypassTableHeight))) {
                            // The Override column holds session-only widgets (Solo,
                            // temp volume). It sits first and its widgets are styled
                            // distinctly so the user reads "these don't get saved" at
                            // a glance.
                            // Simple view keeps Override + Song + Sample + Preset. Disabled fully
                            // removes a column (no context-menu entry), unlike DefaultHide.
                            const ImGuiTableColumnFlags hideAdv = simpleView ? ImGuiTableColumnFlags_Disabled : 0;
                            ImGui::TableSetupColumn("Override", ImGuiTableColumnFlags_WidthFixed, 168.0f);
                            // Song stretches wider than Sample by default — the modder UX
                            // wants the song-family label readable at a glance, while the
                            // Sample column's editable rename mostly sits idle and only
                            // expands when typed into.
                            ImGui::TableSetupColumn("Song", ImGuiTableColumnFlags_WidthStretch, 1.6f);
                            ImGui::TableSetupColumn("Sample", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                            ImGui::TableSetupColumn("Inst", ImGuiTableColumnFlags_WidthFixed | hideAdv, 96.0f);
                            ImGui::TableSetupColumn("Mode", ImGuiTableColumnFlags_WidthFixed | hideAdv, 150.0f);
                            ImGui::TableSetupColumn("Gain", ImGuiTableColumnFlags_WidthFixed | hideAdv, 130.0f);
                            ImGui::TableSetupColumn(transSemis ? "Shift (st)" : "Shift (oct)",
                                                    ImGuiTableColumnFlags_WidthFixed | hideAdv, 85.0f);
                            ImGui::TableSetupColumn("Preset", ImGuiTableColumnFlags_WidthFixed, 280.0f);
                            // Adv: a per-entry popup with the effect sends + filter
                            // (Reverb/Chorus/Cutoff/Q). Replaces the four narrow columns that
                            // pushed the table off-screen on smaller monitors.
                            ImGui::TableSetupColumn("Adv", ImGuiTableColumnFlags_WidthFixed | hideAdv, 52.0f);
                            ImGui::TableSetupScrollFreeze(0, 2);

                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(overrideCol);
                            // Clear-overrides button — wipes all session-only state from the
                            // Override column across every row (solo set, native-row mutes,
                            // and every temp volume). Persisted overrides (Gain, Shift,
                            // Preset, effect CCs) are untouched — those have their own
                            // "Reset all" button above the table.
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.20f, 0.05f, 1.00f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.55f, 0.20f, 1.0f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.00f, 0.65f, 0.30f, 1.0f));
                            if (ImGui::SmallButton("Clear##overrideClear")) {
                                sSoloedPairs.clear();
                                sExplicitMutedPairs.clear();
                                sSoloedSlots.clear();
                                sExplicitMutedSlots.clear();
                                SOH::MidiTranslator::Instance().ClearAllTemporaryMutes();
                                SOH::MidiTranslator::Instance().ClearAllTemporarySlotMutes();
                            }
                            ImGui::PopStyleColor(3);
                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("Clear every session-only override across the table:\n"
                                                  "  - Soloed rows (all unsoloed)\n"
                                                  "  - Mute toggles on Native rows\n"
                                                  "  - Temp volume sliders on Synth rows\n"
                                                  "Gain, Shift, Preset and effect CCs belong to the mod and are\n"
                                                  "untouched - use Revert above the table for unsaved edits.");
                            }
                            ImGui::TableSetColumnIndex(shiftCol);
                            {
                                bool transUnit = transSemis;
                                if (ImGui::Checkbox("Semitone##transUnit", &transUnit)) {
                                    CVarSetInteger(CVAR_AUDIO("FluidSynthTransSemitones"), transUnit ? 1 : 0);
                                    Ship::Context::GetRawInstance()
                                        ->GetWindow()
                                        ->GetGui()
                                        ->SaveConsoleVariablesNextFrame();
                                }
                                if (ImGui::IsItemHovered()) {
                                    ImGui::SetTooltip("Display the Shift column in semitones (+/-24 fine)\n"
                                                      "instead of octaves (+/-8 wide). Underlying value is\n"
                                                      "the same; column label and DragInt range switch.");
                                }
                            }

                            ImGui::TableHeadersRow();

                            const ImU32 kSynthTint = IM_COL32(80, 160, 80, 80);
                            const ImU32 kNativeTint = IM_COL32(80, 120, 200, 80);

                            // Per-entry "Adv" button + popup holding the effect sends + filter
                            // (Reverb/Chorus/Cutoff/Q). Shared by the melodic row, drum slots,
                            // and melodic ranges so every entry carries its own effects. The
                            // popup id is scoped by the active PushID (row/entry), so a fixed
                            // string is unique per row. -1 on a slider = "no override".
                            auto drawAdvPopup = [&](int idx) {
                                if (idx < 0) {
                                    ImGui::TextDisabled("-");
                                    return;
                                }
                                if (ImGui::SmallButton("Adv##advbtn"))
                                    ImGui::OpenPopup("advpop");
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip("Per-entry effects: reverb, chorus, cutoff, Q.");
                                if (ImGui::BeginPopup("advpop")) {
                                    const SOH::ConfigEntry& e = SOH::MidiTranslator::Instance().GetEntry(idx);
                                    auto effRow = [&](const char* label, int8_t cur,
                                                      void (SOH::MidiTranslator::*setter)(int, int8_t),
                                                      const char* tip) {
                                        ImGui::TextUnformatted(label);
                                        if (ImGui::IsItemHovered())
                                            ImGui::SetTooltip("%s", tip);
                                        ImGui::SameLine(110.0f);
                                        ImGui::PushID(label);
                                        int v = cur;
                                        ImGui::SetNextItemWidth(90.0f);
                                        if (ImGui::DragInt("##v", &v, 0.5f, -1, 127, cur < 0 ? "off" : "%d")) {
                                            v = std::clamp(v, -1, 127);
                                            (SOH::MidiTranslator::Instance().*setter)(idx, (int8_t)v);
                                        }
                                        if (ImGui::IsItemDeactivatedAfterEdit())
                                            MarkModEdited();
                                        if (ImGui::IsItemHovered())
                                            ImGui::SetTooltip("%s", tip);
                                        ImGui::PopID();
                                    };
                                    effRow("Reverb (CC91)", e.reverb, &SOH::MidiTranslator::SetEntryReverb,
                                           "Reverb send. 0-127; drag below 0 to clear the override\n"
                                           "and restore the channel default.");
                                    effRow("Chorus (CC93)", e.chorus, &SOH::MidiTranslator::SetEntryChorus,
                                           "Chorus send. 0-127; drag below 0 to clear.");
                                    effRow("Cutoff (CC74)", e.cutoff, &SOH::MidiTranslator::SetEntryFilterCutoff,
                                           "Low-pass cutoff. 64 = no shift from the SF default;\n"
                                           "lower darkens, higher brightens. Drag below 0 to clear.");
                                    effRow("Q (CC71)", e.q, &SOH::MidiTranslator::SetEntryFilterResonance,
                                           "Filter resonance. 64 = no shift; higher emphasises the\n"
                                           "cutoff. Drag below 0 to clear.");
                                    ImGui::EndPopup();
                                }
                            };
                            (void)drawAdvPopup;

                            // Per-entry octave/semitone Shift editor, in the Shift column unit
                            // (octaves or +/-24 semitones) chosen by the header toggle. Shared by
                            // the split-range rows so each range gets its own pitch shift; the
                            // unsplit melodic row keeps its own inline copy. idx < 0 -> "-".
                            auto drawShiftEditor = [&](int idx) {
                                if (idx < 0) {
                                    ImGui::TextDisabled("-");
                                    return;
                                }
                                int transStored = SOH::MidiTranslator::Instance().GetEntry(idx).transpose;
                                int curOctaves = transStored / 12;
                                int curRemainder = transStored - curOctaves * 12;
                                int displayValue, displayMin, displayMax;
                                if (transSemis) {
                                    displayValue = transStored;
                                    displayMin = -24;
                                    displayMax = 24;
                                } else {
                                    displayValue = curOctaves;
                                    displayMin = -8;
                                    displayMax = 8;
                                }
                                const bool hasRemainder = (!transSemis && curRemainder != 0);
                                if (hasRemainder)
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(220, 150, 60, 70));
                                char fmt[32];
                                if (transSemis)
                                    std::strcpy(fmt, "%+d st");
                                else if (hasRemainder)
                                    std::snprintf(fmt, sizeof fmt, "%%+d oct (%+d st)", curRemainder);
                                else
                                    std::strcpy(fmt, "%+d oct");
                                ImGui::SetNextItemWidth(-FLT_MIN);
                                if (ImGui::DragInt("##rshift", &displayValue, 0.1f, displayMin, displayMax, fmt)) {
                                    displayValue = std::clamp(displayValue, displayMin, displayMax);
                                    int newSemis;
                                    if (transSemis)
                                        newSemis = displayValue;
                                    else
                                        newSemis = transStored + (displayValue - curOctaves) * 12;
                                    newSemis = std::clamp(newSemis, -127, 127);
                                    SOH::MidiTranslator::Instance().SetEntryTranspose(idx, (int8_t)newSemis);
                                }
                                if (ImGui::IsItemDeactivatedAfterEdit())
                                    MarkModEdited();
                                // Right-click: re-apply the octave Shift derived from this range's
                                // engine sample tuning (auto-seeded on preset pick; this restores it).
                                const SOH::ConfigEntry& re = SOH::MidiTranslator::Instance().GetEntry(idx);
                                int rSuggest =
                                    SOH::SuggestedTranspose(re.fontId, re.instOrWave, re.noteLow, re.noteHigh);
                                if (ImGui::BeginPopupContextItem("##rshiftctx")) {
                                    if (ImGui::MenuItem(rSuggest % 12 == 0 ? "Apply suggested octave"
                                                                           : "Apply suggested shift")) {
                                        SOH::MidiTranslator::Instance().SetEntryTranspose(idx, (int8_t)rSuggest);
                                        MarkModEdited();
                                    }
                                    ImGui::EndPopup();
                                }
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip("Shift this range's notes (%+d st stored). Toggle Semitone\n"
                                                      "precision in the header for +/-24 st fine control.\n"
                                                      "Sample tuning suggests %+d st (right-click to apply).",
                                                      transStored, rSuggest);
                            };
                            (void)drawShiftEditor;

                            // Standardized Solo (S, brown/orange) + Mute (M, red) buttons. The
                            // warm/red palette signals "session-only, not saved". Used for both
                            // the pair-level sets and the per-drum-slot sets.
                            auto soloMuteToggle = [&](const char* sLbl, const char* mLbl, bool solo, bool mute,
                                                      const std::function<void(bool)>& setSolo,
                                                      const std::function<void(bool)>& setMute) {
                                ImGui::PushStyleColor(ImGuiCol_Button, solo ? ImVec4(0.85f, 0.45f, 0.10f, 1.0f)
                                                                            : ImVec4(0.32f, 0.20f, 0.07f, 1.0f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.55f, 0.20f, 1.0f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.00f, 0.65f, 0.30f, 1.0f));
                                if (ImGui::SmallButton(sLbl))
                                    setSolo(!solo);
                                ImGui::PopStyleColor(3);
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip("Solo (session-only, not saved). Plays only soloed\n"
                                                      "rows; everything else is muted.");
                                ImGui::SameLine();
                                ImGui::PushStyleColor(ImGuiCol_Button, mute ? ImVec4(0.80f, 0.12f, 0.12f, 1.0f)
                                                                            : ImVec4(0.32f, 0.10f, 0.08f, 1.0f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.30f, 0.20f, 1.0f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.00f, 0.40f, 0.30f, 1.0f));
                                if (ImGui::SmallButton(mLbl))
                                    setMute(!mute);
                                ImGui::PopStyleColor(3);
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip("Mute (session-only, not saved). Silences this row\n"
                                                      "on both the engine and synth paths.");
                            };
                            auto drawPairSoloMute = [&](const std::pair<uint8_t, int16_t>& key) {
                                soloMuteToggle(
                                    "S##psolo", "M##pmute", sSoloedPairs.count(key) > 0,
                                    sExplicitMutedPairs.count(key) > 0,
                                    [&](bool on) {
                                        if (on)
                                            sSoloedPairs.insert(key);
                                        else
                                            sSoloedPairs.erase(key);
                                    },
                                    [&](bool on) {
                                        if (on)
                                            sExplicitMutedPairs.insert(key);
                                        else
                                            sExplicitMutedPairs.erase(key);
                                    });
                            };
                            auto drawSlotSoloMute = [&](const std::tuple<uint8_t, int16_t, uint8_t>& key) {
                                soloMuteToggle(
                                    "S##ssolo", "M##smute", sSoloedSlots.count(key) > 0,
                                    sExplicitMutedSlots.count(key) > 0,
                                    [&](bool on) {
                                        if (on)
                                            sSoloedSlots.insert(key);
                                        else
                                            sSoloedSlots.erase(key);
                                    },
                                    [&](bool on) {
                                        if (on)
                                            sExplicitMutedSlots.insert(key);
                                        else
                                            sExplicitMutedSlots.erase(key);
                                    });
                            };
                            (void)drawPairSoloMute;
                            (void)drawSlotSoloMute;

                            // Effective-mute apply pass — runs once per frame BEFORE the
                            // per-row drawing so the audible state matches the UI even on
                            // the same frame the user clicks. The model:
                            //   effective_mute(pair) = (anySolo && !inSolo) || inExplicitMute
                            // where anySolo = !sSoloedPairs.empty(), inSolo = membership in
                            // sSoloedPairs, inExplicitMute = membership in sExplicitMutedPairs.
                            {
                                // A soloed drum slot also counts toward the global solo
                                // state, so it mutes other pairs and sibling slots.
                                bool anySolo = !sSoloedPairs.empty() || !sSoloedSlots.empty();
                                SOH::MidiTranslator::Instance().ClearAllTemporarySlotMutes();
                                for (int i = 0; i < nPairs; i++) {
                                    const auto& q = pairs[i];
                                    auto key = std::make_pair(q.fontId, q.instOrWave);
                                    // A soloed sub-unit (drum slot OR melodic range) keeps its parent
                                    // pair audible; scan regardless of pair kind so soloing a melodic
                                    // range doesn't mute its own instrument.
                                    bool pairHasSoloedSlot = false;
                                    for (const auto& t : sSoloedSlots)
                                        if (std::get<0>(t) == q.fontId && std::get<1>(t) == q.instOrWave) {
                                            pairHasSoloedSlot = true;
                                            break;
                                        }
                                    bool inSolo = sSoloedPairs.count(key) > 0 || pairHasSoloedSlot;
                                    bool explicitMute = sExplicitMutedPairs.count(key) > 0;
                                    bool effPair = (anySolo && !inSolo) || explicitMute;
                                    SOH::MidiTranslator::Instance().SetTemporaryMute(q.fontId, q.instOrWave, effPair);

                                    // Per-slot / per-range mutes only matter when the channel itself
                                    // isn't muted (the pair-mute short-circuits before per-slot). Drum
                                    // slots and melodic sub-ranges are both keyed by noteLow; only the
                                    // whole-pair full-range entry (and the Native marker) is excluded.
                                    if (!effPair) {
                                        std::vector<int> idxs;
                                        SOH::MidiTranslator::Instance().GetEntriesForPair(q.fontId, q.instOrWave, idxs);
                                        for (int ei : idxs) {
                                            const auto& ce = SOH::MidiTranslator::Instance().GetEntry(ei);
                                            if (!ce.selected || (ce.noteLow == 0 && ce.noteHigh == 127))
                                                continue;
                                            auto st = std::make_tuple(q.fontId, q.instOrWave, ce.noteLow);
                                            // A soloed slot is more specific than a soloed pair: when the pair
                                            // has any soloed slot, ONLY those slots play (slot-solo wins). A
                                            // pair-solo lights up every slot only when no slot is soloed.
                                            bool slotSolo = sSoloedSlots.count(st) > 0 ||
                                                            (sSoloedPairs.count(key) > 0 && !pairHasSoloedSlot);
                                            bool effSlot = (anySolo && !slotSolo) || sExplicitMutedSlots.count(st) > 0;
                                            if (effSlot)
                                                SOH::MidiTranslator::Instance().SetTemporarySlotMute(
                                                    q.fontId, q.instOrWave, ce.noteLow, true);
                                        }
                                    }
                                }
                            }

                            for (int i = 0; i < nPairs; i++) {
                                const auto& p = pairs[i];
                                ImGui::TableNextRow();
                                // Keyed by instrument, not position: rows shift as instruments are
                                // heard, and an open combo must keep editing the instrument it opened on.
                                ImGui::PushID((p.fontId << 16) | static_cast<uint16_t>(p.instOrWave));

                                // Activity tint: green if synth-active, blue if native-active,
                                // uncoloured otherwise. Split parents (drum/SFX, melodic ranges)
                                // aggregate this from their child entries below; this default
                                // serves the unsplit melodic row.
                                auto setRowTint = [&](bool anySynth, bool anyNative) {
                                    if (anySynth)
                                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kSynthTint);
                                    else if (anyNative)
                                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kNativeTint);
                                    else
                                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, 0);
                                };
                                setRowTint(
                                    SOH::MidiTranslator::Instance().GetSynthActiveCount(p.fontId, p.instOrWave) > 0,
                                    SOH::MidiTranslator::Instance().GetNativeActiveCount(p.fontId, p.instOrWave) > 0);

                                // ── Drum / SFX pair: per-slot tree-row ───────────────
                                // The engine routes all percussion through instOrWave 0
                                // (SFX through 1); the `semitone` byte is a slot index, not a
                                // pitch. Parent row behaves like a melodic row (Solo/Mute,
                                // Native/Synth, a Kit dropdown); expanding reveals one child
                                // row per drum slot (per-slot Solo/Mute, Mode, Drum Sound).
                                // Diverts before the melodic body and continues.
                                bool forcedDrum =
                                    SOH::MidiTranslator::Instance().IsForcedDrumInView(p.fontId, p.instOrWave);
                                if (p.instOrWave == 0 || p.instOrWave == 1 || forcedDrum) {
                                    auto pairKey = std::make_pair(p.fontId, p.instOrWave);
                                    const char* fontName = SOH::GetFontName(p.fontId);
                                    const SOH::ConfigEntry* dActive =
                                        SOH::MidiTranslator::Instance().GetViewEntry(p.fontId, p.instOrWave);
                                    const std::string& editMod = SOH::MidiTranslator::Instance().GetEditMod();
                                    // The per-instrument Native/Synth state is the explicit
                                    // master flag, NOT "any slot enabled" -- so per-slot edits
                                    // never flip the instrument. A forced-drum pair has no
                                    // separate master: the "Treat as drum" flag IS its Synth
                                    // mode (clearing it reverts the pair to melodic).
                                    bool channelSynth = forcedDrum ? true
                                                                   : SOH::MidiTranslator::Instance().IsDrumChannelSynth(
                                                                         p.fontId, p.instOrWave);

                                    // Parent tint aggregates the slots: green if any slot is
                                    // synth-active, blue if any is native-active and none synth,
                                    // uncoloured otherwise. (The per-pair counters would keep this
                                    // permanently blue from the control slot that fires constantly
                                    // with no entry to attribute to.)
                                    {
                                        bool anySynth = false, anyNative = false;
                                        SOH::MidiTranslator::Instance().GetPairEntryActivity(p.fontId, p.instOrWave,
                                                                                             anySynth, anyNative);
                                        // Slots without entries can't reach the counters; the
                                        // recent-hit signal covers them (sampled slots only, so the
                                        // silent control slots don't keep this lit).
                                        if (!anySynth && !anyNative)
                                            anyNative = SOH::MidiTranslator::Instance().AnyDrumSlotRecentlyActive(
                                                p.fontId, p.instOrWave);
                                        setRowTint(anySynth, anyNative);
                                    }

                                    // Override: expand arrow (first), then channel Solo + Mute.
                                    ImGui::TableSetColumnIndex(overrideCol);
                                    bool treeOpen = ImGui::TreeNodeEx(
                                        "##drumtree",
                                        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap, "");
                                    ImGui::SameLine();
                                    drawPairSoloMute(pairKey);

                                    // Song: font name.
                                    ImGui::TableSetColumnIndex(songCol);
                                    ImGui::TextUnformatted(fontName ? fontName : "(font)");

                                    ImGui::TableSetColumnIndex(instCol);
                                    if (forcedDrum)
                                        ImGui::Text("%d (Drum)", (int)p.instOrWave);
                                    else
                                        ImGui::TextUnformatted(p.instOrWave == 0 ? "Drum" : "SFX");
                                    if (ImGui::IsItemHovered()) {
                                        auto s = SOH::MidiTranslator::Instance().GetDebugStats(p.fontId, p.instOrWave);
                                        const char* kind =
                                            forcedDrum ? "forced-drum" : (p.instOrWave == 0 ? "drum" : "SFX");
                                        ImGui::SetTooltip("font %u, %s channel\nNoteOns %u (synth %u / native %u)",
                                                          (unsigned)p.fontId, kind, s.noteOns, s.routedSynth,
                                                          s.routedNative);
                                    }
                                    // Slots discovery lives in the Inst column (next to the
                                    // channel), matching where melodic Split/L-M-H sit.
                                    uint32_t hist[128];
                                    int distinct = SOH::MidiTranslator::Instance().GetDrumSlotHistogram(
                                        p.fontId, p.instOrWave, hist);
                                    char autoBtn[48];
                                    std::snprintf(autoBtn, sizeof autoBtn, "Slots (%d)##autosplit", distinct);
                                    if (ImGui::SmallButton(autoBtn)) {
                                        SOH::MidiTranslator::Instance().AutoSplitDrums(p.fontId, p.instOrWave);
                                        MarkModEdited();
                                    }
                                    if (ImGui::IsItemHovered())
                                        ImGui::SetTooltip("Discover drum slots: create one entry per slot heard\n"
                                                          "(%d so far). Play the song first, then expand to map\n"
                                                          "each slot to a GM percussion sound.",
                                                          distinct);

                                    // Mode: for the intrinsic drum/SFX channels, Native / Synth
                                    // for the whole channel. A forced-drum pair has no separate
                                    // master (the flag IS Synth), so it shows a revert-to-melodic
                                    // button instead.
                                    ImGui::TableSetColumnIndex(modeCol);
                                    ImGui::BeginDisabled(editMod.empty());
                                    if (forcedDrum) {
                                        if (ImGui::SmallButton("Melodic##undrum")) {
                                            SOH::MidiTranslator::Instance().SetForcedDrum(p.fontId, p.instOrWave,
                                                                                          false);
                                            MarkModEdited();
                                        }
                                        if (ImGui::IsItemHovered())
                                            ImGui::SetTooltip("Stop treating this instrument as a drum\n"
                                                              "(revert to normal melodic routing).");
                                    } else {
                                        if (ImGui::RadioButton("Native##dmode", !channelSynth)) {
                                            SOH::MidiTranslator::Instance().SetDrumChannelSynth(p.fontId, p.instOrWave,
                                                                                                false);
                                            MarkModEdited();
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::RadioButton("Synth##dmode", channelSynth)) {
                                            SOH::MidiTranslator::Instance().SetDrumChannelSynth(p.fontId, p.instOrWave,
                                                                                                true);
                                            MarkModEdited();
                                        }
                                        // Dormant-slot warning: per-slot Synth entries do nothing
                                        // while the channel master is Native (the channel plays
                                        // native wholesale), so saved-but-silent slots aren't a
                                        // mystery.
                                        if (!channelSynth) {
                                            int dormant = 0;
                                            std::vector<int> didx;
                                            SOH::MidiTranslator::Instance().GetEntriesForPair(p.fontId, p.instOrWave,
                                                                                              didx);
                                            for (int ei : didx) {
                                                const auto& ce = SOH::MidiTranslator::Instance().GetEntry(ei);
                                                if (ce.enabled && ce.selected && ce.noteLow == ce.noteHigh)
                                                    ++dormant;
                                            }
                                            if (dormant > 0) {
                                                ImGui::SameLine();
                                                ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "(%d dormant)",
                                                                   dormant);
                                                if (ImGui::IsItemHovered())
                                                    ImGui::SetTooltip(
                                                        "%d slot(s) are set to Synth but the instrument is Native,\n"
                                                        "so they play the engine drum. Click Synth to hear them.",
                                                        dormant);
                                            }
                                        }
                                    }

                                    ImGui::EndDisabled();

                                    // Preset: Kit dropdown (bank-128 presets). Mirrors the
                                    // melodic Preset combo: a "None" entry sets the instrument to
                                    // Native; picking a kit sets it to Synth.
                                    ImGui::TableSetColumnIndex(presetCol);
                                    ImGui::SetNextItemWidth(-1.0f);
                                    std::string curKit = !channelSynth ? "None (native)" : "(pick a kit)";
                                    if (channelSynth && dActive && !dActive->packName.empty())
                                        curKit = dActive->presetName.empty() ? dActive->packName : dActive->presetName;
                                    ImGui::BeginDisabled(editMod.empty());
                                    const bool kitComboOpen = ImGui::BeginCombo("##drumkit", curKit.c_str());
                                    ImGui::EndDisabled();
                                    if (kitComboOpen) {
                                        // "None (native)" returns the whole instrument to native:
                                        // the drum-channel master for 0/1, or un-forcing a
                                        // forced-drum pair back to melodic.
                                        if (ImGui::Selectable("None (native)", !channelSynth)) {
                                            if (forcedDrum)
                                                SOH::MidiTranslator::Instance().SetForcedDrum(p.fontId, p.instOrWave,
                                                                                              false);
                                            else
                                                SOH::MidiTranslator::Instance().SetDrumChannelSynth(
                                                    p.fontId, p.instOrWave, false);
                                            MarkModEdited();
                                        }
                                        ImGui::Separator();
                                        for (const auto& lp : sLoadedPresets) {
                                            if (lp.bank != 128 || lp.modName != editMod)
                                                continue;
                                            char lbl[112];
                                            std::snprintf(lbl, sizeof lbl, "%s / %s##kit_%d_%d", lp.packName.c_str(),
                                                          lp.name.c_str(), lp.sfontId, lp.program);
                                            bool sel = channelSynth && dActive && dActive->bank == 128 &&
                                                       dActive->packName == lp.packName &&
                                                       dActive->program == lp.program;
                                            if (ImGui::Selectable(lbl, sel)) {
                                                // Synth first (auto-splits slots if none exist),
                                                // then apply the picked kit to those slots.
                                                SOH::MidiTranslator::Instance().SetDrumChannelSynth(p.fontId,
                                                                                                    p.instOrWave, true);
                                                SOH::MidiTranslator::Instance().SetDrumKit(
                                                    p.fontId, p.instOrWave, lp.packName, (int16_t)lp.program, lp.name);
                                                MarkModEdited();
                                            }
                                            if (sel)
                                                ImGui::SetItemDefaultFocus();
                                        }
                                        ImGui::EndCombo();
                                    }

                                    if (treeOpen) {
                                        // Only the real drum channel has a registered drum array;
                                        // SFX/forced pairs leave the map empty.
                                        std::map<uint8_t, std::string> drumSlotNames;
                                        if (p.instOrWave == 0)
                                            for (const auto& d : SOH::EnumerateDrumSlots(p.fontId))
                                                drumSlotNames[d.slot] = d.name;

                                        // Manual Add slot: configure a slot offline without replaying
                                        // the song to discover it (the fix for forced-drum pairs that
                                        // reload with no slots). Synth master only -- slots resolve
                                        // only then. The slot index is transient, shared across rows.
                                        if (channelSynth) {
                                            ImGui::TableNextRow();
                                            ImGui::TableSetColumnIndex(instCol);
                                            static int sAddSlot = 0;
                                            ImGui::SetNextItemWidth(64.0f);
                                            ImGui::DragInt("##addslotidx", &sAddSlot, 0.3f, 0, 127, "slot %d");
                                            ImGui::SameLine();
                                            if (ImGui::SmallButton("Add##addslot")) {
                                                SOH::MidiTranslator::Instance().AddDrumSlot(
                                                    p.fontId, p.instOrWave, (uint8_t)std::clamp(sAddSlot, 0, 127));
                                                MarkModEdited();
                                            }
                                            if (ImGui::IsItemHovered())
                                                ImGui::SetTooltip("Add a drum slot by index without waiting to\n"
                                                                  "discover it in-game. Created Native; pick a\n"
                                                                  "Drum Sound to make it synth.");
                                        }

                                        // One row per slot: existing entries merged with the font's
                                        // registered drum array, so every named slot shows up front.
                                        struct SlotRow {
                                            uint8_t slot;
                                            int ei; // entry index, or -1 = registry-only
                                        };
                                        std::vector<SlotRow> slotRows;
                                        {
                                            std::vector<int> idxs;
                                            SOH::MidiTranslator::Instance().GetEntriesForPair(p.fontId, p.instOrWave,
                                                                                              idxs);
                                            std::set<uint8_t> haveEntry;
                                            for (int ei : idxs) {
                                                const auto& ce = SOH::MidiTranslator::Instance().GetEntry(ei);
                                                // Drum splits are single-slot; skip full-range
                                                // whole-pair entries so they don't all collapse onto
                                                // slot 0.
                                                if (ce.selected && ce.noteLow == ce.noteHigh) {
                                                    slotRows.push_back({ (uint8_t)ce.noteLow, ei });
                                                    haveEntry.insert((uint8_t)ce.noteLow);
                                                }
                                            }
                                            for (const auto& kv : drumSlotNames)
                                                if (!haveEntry.count(kv.first))
                                                    slotRows.push_back({ kv.first, -1 });
                                            // "Only played" narrows slots like it narrows pairs.
                                            if (onlyPlayed) {
                                                uint32_t hist[128];
                                                SOH::MidiTranslator::Instance().GetDrumSlotHistogram(
                                                    p.fontId, p.instOrWave, hist);
                                                slotRows.erase(
                                                    std::remove_if(slotRows.begin(), slotRows.end(),
                                                                   [&](const SlotRow& r) { return hist[r.slot] == 0; }),
                                                    slotRows.end());
                                            }
                                            std::sort(
                                                slotRows.begin(), slotRows.end(),
                                                [](const SlotRow& a, const SlotRow& b) { return a.slot < b.slot; });
                                        }
                                        if (slotRows.empty()) {
                                            ImGui::TableNextRow();
                                            ImGui::TableSetColumnIndex(instCol);
                                            ImGui::TextDisabled("(no slots - set Synth or click Slots)");
                                        }
                                        for (const auto& rr : slotRows) {
                                            // Registry-only slot: picking a sound creates its entry,
                                            // so unmapped slots never touch the JSON.
                                            if (rr.ei < 0) {
                                                ImGui::TableNextRow();
                                                ImGui::PushID(10000 + rr.slot);
                                                // No entry to count on; the recent-hit signal drives
                                                // the native tint.
                                                if (SOH::MidiTranslator::Instance().DrumSlotRecentlyActive(
                                                        p.fontId, p.instOrWave, rr.slot))
                                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kNativeTint);
                                                ImGui::TableSetColumnIndex(overrideCol);
                                                drawSlotSoloMute(std::make_tuple(p.fontId, p.instOrWave, rr.slot));
                                                ImGui::TableSetColumnIndex(sampleCol);
                                                ImGui::TextDisabled("%s", drumSlotNames[rr.slot].c_str());
                                                ImGui::TableSetColumnIndex(instCol);
                                                ImGui::Text("slot %d", (int)rr.slot);
                                                ImGui::BeginDisabled(!channelSynth);
                                                ImGui::TableSetColumnIndex(presetCol);
                                                ImGui::SetNextItemWidth(-FLT_MIN);
                                                if (ImGui::BeginCombo("##vslotsound", "None (native)",
                                                                      ImGuiComboFlags_HeightLargest)) {
                                                    for (int n = SOH::kGmPercussionLo; n <= SOH::kGmPercussionHi; ++n) {
                                                        const char* nm = SOH::GmPercussionName(n);
                                                        char lbl[48];
                                                        std::snprintf(lbl, sizeof lbl, "%d %s##v%d", n, nm, n);
                                                        if (ImGui::Selectable(lbl, false)) {
                                                            auto& tr = SOH::MidiTranslator::Instance();
                                                            tr.AddDrumSlot(p.fontId, p.instOrWave, rr.slot);
                                                            // Find the just-created entry to set its sound.
                                                            std::vector<int> created;
                                                            tr.GetEntriesForPair(p.fontId, p.instOrWave, created);
                                                            for (int ci : created) {
                                                                const auto& ne = tr.GetEntry(ci);
                                                                if (ne.selected && ne.noteLow == rr.slot &&
                                                                    ne.noteHigh == rr.slot) {
                                                                    tr.SetEntryFixedNote(ci, (int16_t)n);
                                                                    tr.SetEntryRoute(ci, SOH::EntryRoute::Synth);
                                                                    tr.SetEntryEnabled(ci, true);
                                                                    break;
                                                                }
                                                            }
                                                            MarkModEdited();
                                                        }
                                                    }
                                                    ImGui::EndCombo();
                                                }
                                                if (!channelSynth &&
                                                    ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                                                    ImGui::SetTooltip("Pick a Kit first (or set the instrument to "
                                                                      "Synth) to map slots.");
                                                ImGui::EndDisabled();
                                                ImGui::PopID();
                                                continue;
                                            }
                                            int ei = rr.ei;
                                            const SOH::ConfigEntry& ce = SOH::MidiTranslator::Instance().GetEntry(ei);
                                            ImGui::TableNextRow();
                                            ImGui::PushID(ei);

                                            // Per-slot activity tint (green synth / blue native),
                                            // with the recent-hit signal as backstop for hits the
                                            // entry counters miss (e.g. percussive notes already
                                            // released by the time the UI samples them).
                                            if (SOH::MidiTranslator::Instance().GetEntrySynthActive(ei) > 0)
                                                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kSynthTint);
                                            else if (SOH::MidiTranslator::Instance().GetEntryNativeActive(ei) > 0)
                                                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kNativeTint);
                                            else if (SOH::MidiTranslator::Instance().DrumSlotRecentlyActive(
                                                         p.fontId, p.instOrWave, (uint8_t)ce.noteLow))
                                                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                                                       ce.enabled ? kSynthTint : kNativeTint);

                                            // Override: per-slot Solo / Mute.
                                            auto slotKey = std::make_tuple(p.fontId, p.instOrWave, ce.noteLow);
                                            ImGui::TableSetColumnIndex(overrideCol);
                                            drawSlotSoloMute(slotKey);

                                            // Sample name lives in the Sample column (visible in Simple view).
                                            {
                                                auto nmIt = drumSlotNames.find((uint8_t)ce.noteLow);
                                                if (nmIt != drumSlotNames.end() && !nmIt->second.empty()) {
                                                    ImGui::TableSetColumnIndex(sampleCol);
                                                    ImGui::TextDisabled("%s", nmIt->second.c_str());
                                                }
                                            }
                                            ImGui::TableSetColumnIndex(instCol);
                                            ImGui::Text("slot %d", (int)ce.noteLow);

                                            // Slot editing is gated by the instrument master mode:
                                            // when the instrument is Native, the per-slot controls
                                            // are read-only (Solo/Mute above stay live so a native
                                            // drum can still be isolated).
                                            ImGui::BeginDisabled(!channelSynth ||
                                                                 !SOH::MidiTranslator::Instance().IsEditable(ei));

                                            // Native / Synth radio: the enabled flag IS the per-slot
                                            // Native/Synth state. Native plays the engine drum;
                                            // Synth plays the chosen GM sound. Does NOT touch the
                                            // instrument mode.
                                            ImGui::TableSetColumnIndex(modeCol);
                                            if (ImGui::RadioButton("Native##smode", !ce.enabled)) {
                                                SOH::MidiTranslator::Instance().SetEntryEnabled(ei, false);
                                                MarkModEdited();
                                            }
                                            if (!channelSynth &&
                                                ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                                                ImGui::SetTooltip("Instrument is set to Native. Set the "
                                                                  "instrument to Synth to edit slots.");
                                            ImGui::SameLine();
                                            if (ImGui::RadioButton("Synth##smode", ce.enabled)) {
                                                SOH::MidiTranslator::Instance().SetEntryRoute(ei,
                                                                                              SOH::EntryRoute::Synth);
                                                SOH::MidiTranslator::Instance().SetEntryEnabled(ei, true);
                                                MarkModEdited();
                                            }

                                            // Sound dropdown, mirroring the melodic Preset combo:
                                            // a filter box, a "None" entry that reverts to native
                                            // (sets the radio to Native), and the GM percussion
                                            // sounds (each one forces Synth). Disabled slot shows
                                            // "None".
                                            ImGui::TableSetColumnIndex(presetCol);
                                            char preview[48];
                                            if (!ce.enabled)
                                                std::snprintf(preview, sizeof preview, "None (native)");
                                            else if (ce.bank == 128) {
                                                const char* nm = SOH::GmPercussionName(ce.fixedNote);
                                                if (nm[0])
                                                    std::snprintf(preview, sizeof preview, "%d %s", (int)ce.fixedNote,
                                                                  nm);
                                                else
                                                    std::snprintf(preview, sizeof preview, "note %d",
                                                                  (int)ce.fixedNote);
                                            } else
                                                std::snprintf(preview, sizeof preview, "Pitch %d",
                                                              ce.fixedNote < 0 ? 60 : (int)ce.fixedNote);
                                            ImGui::SetNextItemWidth(-FLT_MIN);
                                            if (ImGui::BeginCombo("##slotsound", preview,
                                                                  ImGuiComboFlags_HeightLargest)) {
                                                static char soundFilter[48] = "";
                                                if (ImGui::IsWindowAppearing()) {
                                                    soundFilter[0] = '\0';
                                                    ImGui::SetKeyboardFocusHere();
                                                }
                                                ImGui::SetNextItemWidth(-FLT_MIN);
                                                ImGui::InputTextWithHint("##soundFilter", "Filter sounds", soundFilter,
                                                                         sizeof soundFilter);
                                                ImGui::Separator();
                                                auto containsCi = [](const char* hay, const char* needle) -> bool {
                                                    if (!needle || !*needle)
                                                        return true;
                                                    auto lc = [](char c) {
                                                        return (c >= 'A' && c <= 'Z') ? char(c + 32) : c;
                                                    };
                                                    std::string h, n;
                                                    for (const char* q = hay; *q; ++q)
                                                        h += lc(*q);
                                                    for (const char* q = needle; *q; ++q)
                                                        n += lc(*q);
                                                    return h.find(n) != std::string::npos;
                                                };
                                                bool filterActive = soundFilter[0] != '\0';
                                                if (!filterActive && ImGui::Selectable("None (native)", !ce.enabled)) {
                                                    SOH::MidiTranslator::Instance().SetEntryEnabled(ei, false);
                                                    MarkModEdited();
                                                }
                                                if (ce.bank == 128) {
                                                    for (int n = SOH::kGmPercussionLo; n <= SOH::kGmPercussionHi; ++n) {
                                                        const char* nm = SOH::GmPercussionName(n);
                                                        if (filterActive && !containsCi(nm, soundFilter))
                                                            continue;
                                                        char lbl[48];
                                                        std::snprintf(lbl, sizeof lbl, "%d %s##s%d", n, nm, n);
                                                        bool sel = (ce.enabled && ce.fixedNote == n);
                                                        if (ImGui::Selectable(lbl, sel)) {
                                                            SOH::MidiTranslator::Instance().SetEntryFixedNote(
                                                                ei, (int16_t)n);
                                                            SOH::MidiTranslator::Instance().SetEntryRoute(
                                                                ei, SOH::EntryRoute::Synth);
                                                            SOH::MidiTranslator::Instance().SetEntryEnabled(ei, true);
                                                            MarkModEdited();
                                                        }
                                                        if (sel)
                                                            ImGui::SetItemDefaultFocus();
                                                    }
                                                }
                                                ImGui::EndCombo();
                                            }
                                            // Per-slot Gain.
                                            ImGui::TableSetColumnIndex(gainCol);
                                            {
                                                float g = ce.gain == 0.0f ? 1.0f : ce.gain;
                                                ImGui::SetNextItemWidth(-1.0f);
                                                if (ImGui::DragFloat("##slotgain", &g, 0.01f, 0.0f, 4.0f, "%.2f"))
                                                    SOH::MidiTranslator::Instance().SetEntryGain(ei,
                                                                                                 g < 0.0f ? 0.0f : g);
                                                if (ImGui::IsItemDeactivatedAfterEdit())
                                                    MarkModEdited();
                                            }

                                            // Tuned (non-128) Synth slot: an explicit pitch (Shift col).
                                            if (ce.bank != 128 && ce.enabled) {
                                                ImGui::TableSetColumnIndex(shiftCol);
                                                ImGui::SetNextItemWidth(-1.0f);
                                                int pitch = ce.fixedNote < 0 ? 60 : ce.fixedNote;
                                                if (ImGui::DragInt("##pitch", &pitch, 1.0f, 0, 127))
                                                    SOH::MidiTranslator::Instance().SetEntryFixedNote(ei,
                                                                                                      (int16_t)pitch);
                                                if (ImGui::IsItemDeactivatedAfterEdit())
                                                    MarkModEdited();
                                            }

                                            // Per-slot advanced effects (Reverb/Chorus/Cutoff/Q).
                                            ImGui::TableSetColumnIndex(advCol);
                                            drawAdvPopup(ei);

                                            ImGui::EndDisabled(); // slot-edit gate (instrument mode)
                                            ImGui::PopID();
                                        }
                                        ImGui::TreePop();
                                    }
                                    ImGui::PopID();
                                    continue;
                                }

                                // ── Melodic note-range split: tree-row ───────────────
                                // A melodic pair that has any sub-range entry (noteLow!=0 ||
                                // noteHigh!=127) renders as a collapsible header + one child
                                // row per range (range editor + preset dropdown + Native/
                                // Synth). Unsplit pairs fall through to the normal melodic row.
                                {
                                    std::vector<int> allIdx;
                                    SOH::MidiTranslator::Instance().GetEntriesForPair(p.fontId, p.instOrWave, allIdx);
                                    std::vector<int> ranges;
                                    bool isRangeSplit = false;
                                    for (int ei : allIdx) {
                                        const auto& ce = SOH::MidiTranslator::Instance().GetEntry(ei);
                                        if (!ce.selected)
                                            continue;
                                        // The user Native marker (empty-pack route=Native) is a
                                        // whole-pair native flag, not a split range -- skip it so it
                                        // doesn't surface as a phantom range row.
                                        if (ce.route == SOH::EntryRoute::Native && ce.packName.empty())
                                            continue;
                                        ranges.push_back(ei);
                                        if (ce.noteLow != 0 || ce.noteHigh != 127)
                                            isRangeSplit = true;
                                    }
                                    if (isRangeSplit) {
                                        std::sort(ranges.begin(), ranges.end(), [](int a, int b) {
                                            return SOH::MidiTranslator::Instance().GetEntry(a).noteLow <
                                                   SOH::MidiTranslator::Instance().GetEntry(b).noteLow;
                                        });
                                        auto pairKey = std::make_pair(p.fontId, p.instOrWave);

                                        // Parent tint aggregates the ranges: green if any range is
                                        // synth-active, blue if any is native-active and none synth.
                                        {
                                            bool anySynth = false, anyNative = false;
                                            SOH::MidiTranslator::Instance().GetPairEntryActivity(p.fontId, p.instOrWave,
                                                                                                 anySynth, anyNative);
                                            setRowTint(anySynth, anyNative);
                                        }

                                        // Header: expand arrow (first), Solo/Mute, font, summary,
                                        // Flatten.
                                        ImGui::TableSetColumnIndex(overrideCol);
                                        bool mTreeOpen = ImGui::TreeNodeEx(
                                            "##melsplit",
                                            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap, "");
                                        ImGui::SameLine();
                                        drawPairSoloMute(pairKey);

                                        ImGui::TableSetColumnIndex(songCol);
                                        const char* fName = SOH::GetFontName(p.fontId);
                                        ImGui::TextUnformatted(fName ? fName : "(font)");

                                        ImGui::TableSetColumnIndex(instCol);
                                        ImGui::Text("%d", (int)p.instOrWave);

                                        ImGui::TableSetColumnIndex(presetCol);
                                        // Flag native (disabled) ranges so coverage that isn't synth is
                                        // visible at a glance, not hidden one row deeper.
                                        int nativeRanges = 0;
                                        for (int ei : ranges)
                                            if (!SOH::MidiTranslator::Instance().GetEntry(ei).enabled)
                                                ++nativeRanges;
                                        if (nativeRanges > 0)
                                            ImGui::Text("%d ranges (%d native)", (int)ranges.size(), nativeRanges);
                                        else
                                            ImGui::Text("%d ranges", (int)ranges.size());
                                        ImGui::SameLine();
                                        const bool canEditRanges =
                                            SOH::MidiTranslator::Instance().IsEditable(ranges.front());
                                        ImGui::BeginDisabled(!canEditRanges);
                                        const bool flattenClicked = ImGui::SmallButton("Flatten##melflat");
                                        ImGui::EndDisabled();
                                        if (flattenClicked) {
                                            SOH::MidiTranslator::Instance().FlattenSplits(p.fontId, p.instOrWave);
                                            MarkModEdited();
                                        }
                                        if (ImGui::IsItemHovered())
                                            ImGui::SetTooltip("Merge all ranges back into a single full-range entry.");

                                        if (mTreeOpen) {
                                            for (size_t k = 0; k < ranges.size(); ++k) {
                                                int ei = ranges[k];
                                                const SOH::ConfigEntry& ce =
                                                    SOH::MidiTranslator::Instance().GetEntry(ei);
                                                ImGui::TableNextRow();
                                                ImGui::PushID(ei);

                                                if (SOH::MidiTranslator::Instance().GetEntrySynthActive(ei) > 0)
                                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kSynthTint);
                                                else if (SOH::MidiTranslator::Instance().GetEntryNativeActive(ei) > 0)
                                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, kNativeTint);

                                                // Per-range Solo / Mute (keyed by noteLow, like drum
                                                // slots), so a single melodic range can be isolated.
                                                ImGui::TableSetColumnIndex(overrideCol);
                                                drawSlotSoloMute(std::make_tuple(p.fontId, p.instOrWave, ce.noteLow));
                                                ImGui::BeginDisabled(!canEditRanges);

                                                // Inst (and its boundary editor) is hidden in Simple view;
                                                // show the range read-only so the rows stay tellable apart.
                                                if (simpleView) {
                                                    ImGui::TableSetColumnIndex(sampleCol);
                                                    ImGui::TextDisabled("%d..%d", (int)ce.noteLow, (int)ce.noteHigh);
                                                }

                                                // Inst column: contiguous-range boundary editor. Ranges
                                                // are kept adjacent and non-overlapping by construction:
                                                // the first range's low is pinned to 0 and the last
                                                // range's high to 127 (both grayed); the only editable
                                                // value is the boundary between two ranges, and moving it
                                                // shifts the neighbouring range's edge with it. So a
                                                // range's low mirrors the previous range's high+1 and
                                                // can't be set independently.
                                                const bool isFirst = (k == 0);
                                                const bool isLast = (k + 1 == ranges.size());
                                                const int prevIdx = isFirst ? -1 : ranges[k - 1];
                                                const int nextIdx = isLast ? -1 : ranges[k + 1];
                                                ImGui::TableSetColumnIndex(instCol);
                                                // Low edge (== previous range's high + 1). Editing it moves
                                                // the boundary shared with the previous range.
                                                int lo = (int)ce.noteLow;
                                                ImGui::BeginDisabled(isFirst);
                                                ImGui::SetNextItemWidth(44.0f);
                                                if (ImGui::DragInt("##rlo", &lo, 0.3f, 0, 127, "%d") && !isFirst)
                                                    SOH::MidiTranslator::Instance().SetSplitBoundary(
                                                        prevIdx, ei, (uint8_t)std::clamp(lo - 1, 0, 127));
                                                if (ImGui::IsItemDeactivatedAfterEdit())
                                                    MarkModEdited();
                                                ImGui::EndDisabled();
                                                ImGui::SameLine(0.0f, 4.0f);
                                                ImGui::TextUnformatted("..");
                                                ImGui::SameLine(0.0f, 4.0f);
                                                // High edge (== next range's low - 1). Editing it moves the
                                                // boundary shared with the next range.
                                                int hi = (int)ce.noteHigh;
                                                ImGui::BeginDisabled(isLast);
                                                ImGui::SetNextItemWidth(44.0f);
                                                if (ImGui::DragInt("##rhi", &hi, 0.3f, 0, 127, "%d") && !isLast)
                                                    SOH::MidiTranslator::Instance().SetSplitBoundary(
                                                        ei, nextIdx, (uint8_t)std::clamp(hi, 0, 127));
                                                if (ImGui::IsItemDeactivatedAfterEdit())
                                                    MarkModEdited();
                                                ImGui::EndDisabled();
                                                if (ImGui::IsItemHovered())
                                                    ImGui::SetTooltip(
                                                        "Engine-semitone range. Ranges stay adjacent: the\n"
                                                        "first starts at 0, the last ends at 127, and moving\n"
                                                        "a boundary shifts the neighbouring range with it.");
                                                // Split/Merge under the range editor.
                                                if (ImGui::SmallButton("Split##rsplit")) {
                                                    int mid = ((int)ce.noteLow + (int)ce.noteHigh + 1) / 2;
                                                    SOH::MidiTranslator::Instance().SplitEntry(ei, (uint8_t)mid);
                                                    MarkModEdited();
                                                }
                                                if (ImGui::IsItemHovered())
                                                    ImGui::SetTooltip("Split this range in half.");
                                                ImGui::SameLine();
                                                if (ImGui::SmallButton("Merge##rmerge")) {
                                                    SOH::MidiTranslator::Instance().MergeWithNext(ei);
                                                    MarkModEdited();
                                                }
                                                if (ImGui::IsItemHovered())
                                                    ImGui::SetTooltip("Merge with the next (higher) range.");

                                                // Mode: Native / Synth (enabled flag).
                                                ImGui::TableSetColumnIndex(modeCol);
                                                if (ImGui::RadioButton("Native##rmode", !ce.enabled)) {
                                                    SOH::MidiTranslator::Instance().SetEntryEnabled(ei, false);
                                                    MarkModEdited();
                                                }
                                                ImGui::SameLine();
                                                if (ImGui::RadioButton("Synth##rmode", ce.enabled)) {
                                                    SOH::MidiTranslator::Instance().SetEntryEnabled(ei, true);
                                                    MarkModEdited();
                                                }

                                                // Per-range Gain.
                                                ImGui::TableSetColumnIndex(gainCol);
                                                {
                                                    float g = ce.gain == 0.0f ? 1.0f : ce.gain;
                                                    ImGui::SetNextItemWidth(-1.0f);
                                                    if (ImGui::DragFloat("##rgain", &g, 0.01f, 0.0f, 4.0f, "%.2f"))
                                                        SOH::MidiTranslator::Instance().SetEntryGain(ei, g < 0.0f ? 0.0f
                                                                                                                  : g);
                                                    if (ImGui::IsItemDeactivatedAfterEdit())
                                                        MarkModEdited();
                                                }

                                                // Preset column: preset dropdown (filter + None).
                                                ImGui::TableSetColumnIndex(presetCol);
                                                char rprev[160];
                                                if (!ce.enabled)
                                                    std::snprintf(rprev, sizeof rprev, "None (native)");
                                                else if (ce.packName.empty())
                                                    std::snprintf(rprev, sizeof rprev, "(none)");
                                                else
                                                    std::snprintf(rprev, sizeof rprev, "%s: %s", ce.packName.c_str(),
                                                                  ce.presetName.c_str());
                                                ImGui::SetNextItemWidth(-FLT_MIN);
                                                if (ImGui::BeginCombo("##rpreset", rprev,
                                                                      ImGuiComboFlags_HeightLargest)) {
                                                    static char rFilter[64] = "";
                                                    if (ImGui::IsWindowAppearing()) {
                                                        rFilter[0] = '\0';
                                                        ImGui::SetKeyboardFocusHere();
                                                    }
                                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                                    ImGui::InputTextWithHint("##rfilter", "Filter (preset or pack)",
                                                                             rFilter, sizeof rFilter);
                                                    ImGui::Separator();
                                                    auto ciHas = [](const std::string& hay, const char* n) {
                                                        if (!n || !*n)
                                                            return true;
                                                        auto lc = [](char c) {
                                                            return (c >= 'A' && c <= 'Z') ? char(c + 32) : c;
                                                        };
                                                        std::string h, nn;
                                                        for (char c : hay)
                                                            h += lc(c);
                                                        for (const char* q = n; *q; ++q)
                                                            nn += lc(*q);
                                                        return h.find(nn) != std::string::npos;
                                                    };
                                                    bool fActive = rFilter[0] != '\0';
                                                    if (!fActive && ImGui::Selectable("None (native)", !ce.enabled)) {
                                                        SOH::MidiTranslator::Instance().SetEntryEnabled(ei, false);
                                                        MarkModEdited();
                                                    }
                                                    int lastSf = -2;
                                                    for (const auto& lp : sLoadedPresets) {
                                                        if (lp.bank == 128 || lp.modName != ce.modName)
                                                            continue; // percussion kits aren't melodic
                                                        if (fActive && !ciHas(lp.name, rFilter) &&
                                                            !ciHas(lp.packName, rFilter))
                                                            continue;
                                                        if (lp.sfontId != lastSf) {
                                                            ImGui::Separator();
                                                            ImGui::TextDisabled("%s", lp.packName.c_str());
                                                            lastSf = lp.sfontId;
                                                        }
                                                        char it[200];
                                                        std::snprintf(it, sizeof it, "B%d P%d: %s##%d:%d:%d", lp.bank,
                                                                      lp.program, lp.name.c_str(), lp.sfontId, lp.bank,
                                                                      lp.program);
                                                        bool sel = ce.enabled && ce.packName == lp.packName &&
                                                                   ce.bank == lp.bank && ce.program == lp.program;
                                                        if (ImGui::Selectable(it, sel)) {
                                                            SOH::MidiTranslator::Instance().SetEntryPreset(
                                                                ei, lp.packName, (int16_t)lp.program, (int16_t)lp.bank,
                                                                lp.name);
                                                            MarkModEdited();
                                                        }
                                                        if (sel)
                                                            ImGui::SetItemDefaultFocus();
                                                    }
                                                    ImGui::EndCombo();
                                                }

                                                // Per-range pitch Shift (octaves / semitones), greyed
                                                // when the range plays native (nothing to shift).
                                                ImGui::TableSetColumnIndex(shiftCol);
                                                ImGui::BeginDisabled(!ce.enabled);
                                                drawShiftEditor(ei);
                                                ImGui::EndDisabled();

                                                // Per-range advanced effects (Reverb/Chorus/Cutoff/Q).
                                                ImGui::TableSetColumnIndex(advCol);
                                                drawAdvPopup(ei);
                                                ImGui::EndDisabled();

                                                ImGui::PopID();
                                            }
                                            ImGui::TreePop();
                                        }
                                        ImGui::PopID();
                                        continue;
                                    }
                                }

                                // Hoisted from below: needed by the Override column too.
                                // The Mode/Native vs Synth state comes straight from the
                                // entry resolution — active entry == nullptr means there's
                                // no enabled+resolvable entry for this pair, so native
                                // plays. The "Default:" hint label still references
                                // GetGmPreset since it's the only place to source a
                                // human-readable GM name for the "no override" preset
                                // placeholder.
                                SOH::GmPreset defaultGmForMode = SOH::GetGmPreset(p.fontId, p.instOrWave);
                                auto& tr = SOH::MidiTranslator::Instance();
                                const SOH::ConfigEntry* activeEntry = tr.GetViewEntry(p.fontId, p.instOrWave);
                                const int activeIdx = tr.GetViewEntryIdx(p.fontId, p.instOrWave);
                                const SOH::ConfigEntry* playingEntry = tr.GetActiveEntry(p.fontId, p.instOrWave);
                                const bool playsNative =
                                    playingEntry == nullptr || tr.IsAuditionNative(p.fontId, p.instOrWave);
                                const bool canEditEntry = tr.IsEditable(activeIdx);
                                const std::string& editMod = tr.GetEditMod();

                                // ── Override column (session-only Solo / Mute) ─────────
                                ImGui::TableSetColumnIndex(overrideCol);
                                auto pairKey = std::make_pair(p.fontId, p.instOrWave);
                                drawPairSoloMute(pairKey);

                                ImGui::TableSetColumnIndex(songCol);
                                // Plain text — the font name is almost always accurate
                                // (per-font names come from runtime fontMap[]). The custom
                                // display_name lives in the Sample column instead, where
                                // it actually replaces something that's often empty or
                                // useless. See the InputTextWithHint below.
                                {
                                    const char* font = SOH::GetFontName(p.fontId);
                                    ImGui::TextUnformatted(font ? font : "(modded font)");
                                }

                                // Sample column: shows the SF sample names captured at load
                                // time (L / M / H range splits, deduped when they match).
                                // Modders can type their own label here; the auto name becomes
                                // the hint (shown only when the override is empty), so slots
                                // with no captured name still get a usable label.
                                //
                                // Keyed by (fontId, instOrWave), so a different fontId reusing
                                // the same instOrWave can carry a different name. The hint is
                                // built fresh each frame from the SF sample dataset; the
                                // override comes from the translator's mDisplayName map and is
                                // persisted with the rest of the row.
                                ImGui::TableSetColumnIndex(sampleCol);
                                {
                                    // Build the "auto" sample-name label that doubles as
                                    // the hint placeholder.
                                    char autoBuf[256];
                                    autoBuf[0] = '\0';
                                    {
                                        auto names = SOH::GetInstrumentSampleNames(p.fontId, p.instOrWave);
                                        if (!names.empty()) {
                                            const char* lowName = SOH::StripSamplePathPrefix(names.low);
                                            const char* normalName = SOH::StripSamplePathPrefix(names.normal);
                                            const char* highName = SOH::StripSamplePathPrefix(names.high);
                                            auto isSame = [](const char* a, const char* b) {
                                                return *a && *b && std::strcmp(a, b) == 0;
                                            };
                                            const bool lEmpty = names.low.empty();
                                            const bool nEmpty = names.normal.empty();
                                            const bool hEmpty = names.high.empty();
                                            const bool allMatch = (lEmpty || nEmpty || isSame(lowName, normalName)) &&
                                                                  (nEmpty || hEmpty || isSame(normalName, highName)) &&
                                                                  (lEmpty || hEmpty || isSame(lowName, highName));
                                            if (allMatch) {
                                                const char* shown = !nEmpty ? normalName : !lEmpty ? lowName : highName;
                                                char tag[5] = "(";
                                                int t = 1;
                                                if (!lEmpty)
                                                    tag[t++] = 'L';
                                                if (!nEmpty)
                                                    tag[t++] = 'M';
                                                if (!hEmpty)
                                                    tag[t++] = 'H';
                                                tag[t++] = ')';
                                                tag[t] = '\0';
                                                std::snprintf(autoBuf, sizeof(autoBuf), "%s %s", shown, tag);
                                            } else {
                                                size_t pos = 0;
                                                auto append = [&](const char* prefix, const char* val) {
                                                    if (!*val)
                                                        return;
                                                    int written =
                                                        std::snprintf(autoBuf + pos, sizeof(autoBuf) - pos, "%s%s:%s",
                                                                      pos == 0 ? "" : " ", prefix, val);
                                                    if (written > 0)
                                                        pos += static_cast<size_t>(written);
                                                };
                                                append("L", SOH::StripSamplePathPrefix(names.low));
                                                append("M", SOH::StripSamplePathPrefix(names.normal));
                                                append("H", SOH::StripSamplePathPrefix(names.high));
                                            }
                                        }
                                    }
                                    const char* hint = autoBuf[0] ? autoBuf : "(no sample name)";

                                    const std::string current =
                                        SOH::MidiTranslator::Instance().GetDisplayName(p.fontId, p.instOrWave);
                                    char buf[80];
                                    std::strncpy(buf, current.c_str(), sizeof(buf) - 1);
                                    buf[sizeof(buf) - 1] = '\0';
                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                    ImGui::BeginDisabled(editMod.empty());
                                    if (ImGui::InputTextWithHint("##displayName", hint, buf, sizeof(buf))) {
                                        SOH::MidiTranslator::Instance().SetDisplayName(p.fontId, p.instOrWave,
                                                                                       std::string(buf));
                                    }
                                    if (ImGui::IsItemDeactivatedAfterEdit())
                                        MarkModEdited();
                                    ImGui::EndDisabled();
                                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                        // Always show the engine's three per-range samples,
                                        // whether or not a custom label is set. The custom
                                        // label hides what the engine actually plays per
                                        // pitch range; the tooltip is where the user reads
                                        // back the raw mapping.
                                        auto names = SOH::GetInstrumentSampleNames(p.fontId, p.instOrWave);
                                        const char* lowName = SOH::StripSamplePathPrefix(names.low);
                                        const char* normalName = SOH::StripSamplePathPrefix(names.normal);
                                        const char* highName = SOH::StripSamplePathPrefix(names.high);
                                        ImGui::SetTooltip("Hi:  %s\nMid: %s\nLow: %s",
                                                          (highName && *highName) ? highName : "(empty)",
                                                          (normalName && *normalName) ? normalName : "(empty)",
                                                          (lowName && *lowName) ? lowName : "(empty)");
                                    }
                                }

                                // (Font column removed; the raw fontId lives in the Inst-column
                                // hex display and the [DBG] popup.)

                                ImGui::TableSetColumnIndex(instCol);
                                if (p.instOrWave == 0) {
                                    ImGui::Text("0 (Drum)");
                                } else if (p.instOrWave == 1) {
                                    ImGui::Text("1 (SFX)");
                                } else {
                                    ImGui::Text("%d (0x%02X)", (int)p.instOrWave, (unsigned)(uint8_t)p.instOrWave);
                                }
                                // Debug stats on hover (the [DBG] popup, shown on mouseover).
                                if (ImGui::IsItemHovered()) {
                                    auto s = SOH::MidiTranslator::Instance().GetDebugStats(p.fontId, p.instOrWave);
                                    ImGui::SetTooltip("font %u, inst %d (0x%02X)\n"
                                                      "NoteOns %u  (synth %u / native %u / mute %u)\n"
                                                      "last semitone %u (MIDI %u)",
                                                      (unsigned)p.fontId, (int)p.instOrWave,
                                                      (unsigned)(uint8_t)p.instOrWave, s.noteOns, s.routedSynth,
                                                      s.routedNative, s.routedMute, (unsigned)s.lastSemitone,
                                                      (unsigned)(s.lastSemitone + 21u));
                                }
                                // Split entry points (own line under the inst id so they're
                                // visible and don't fight the narrow column). Manual "Split"
                                // bisects the current preset into two note ranges and works
                                // for any synth pair; "L/M/H" mirrors the engine's sample
                                // ranges and needs the captured boundaries (an .o2r regen).
                                if (activeEntry != nullptr && activeEntry->program >= 0) {
                                    ImGui::BeginDisabled(!canEditEntry);
                                    if (ImGui::SmallButton("Split##manualmel")) {
                                        int mid = ((int)activeEntry->noteLow + (int)activeEntry->noteHigh + 1) / 2;
                                        SOH::MidiTranslator::Instance().SplitEntry(activeIdx, (uint8_t)mid);
                                        MarkModEdited();
                                    }
                                    if (ImGui::IsItemHovered())
                                        ImGui::SetTooltip("Split this instrument into two note ranges (at the\n"
                                                          "midpoint) so the low and high halves can use\n"
                                                          "different presets. Expand the row to edit them.");
                                    // Split into N equal ranges in one click (the common "I want 4
                                    // ranges" case, instead of repeatedly bisecting the lowest).
                                    ImGui::SameLine();
                                    static int sSplitN = 4;
                                    ImGui::SetNextItemWidth(40.0f);
                                    ImGui::DragInt("##splitn", &sSplitN, 0.1f, 2, 16, "%d");
                                    ImGui::SameLine(0.0f, 2.0f);
                                    if (ImGui::SmallButton("Split N##manualmelN")) {
                                        SOH::MidiTranslator::Instance().SplitEntryEven(activeIdx,
                                                                                       std::clamp(sSplitN, 2, 16));
                                        MarkModEdited();
                                    }
                                    if (ImGui::IsItemHovered())
                                        ImGui::SetTooltip("Split this instrument into N equal note ranges at once,\n"
                                                          "duplicating the current preset so each range can be\n"
                                                          "reassigned. Expand the row to edit them.");
                                    auto rng = SOH::GetInstrumentSampleNames(p.fontId, p.instOrWave);
                                    if (rng.hasRange && !(rng.rangeLo == 0 && rng.rangeHi >= 127)) {
                                        ImGui::SameLine();
                                        if (ImGui::SmallButton("L/M/H##autosplitmel")) {
                                            SOH::MidiTranslator::Instance().AutoSplitByEngineRanges(p.fontId,
                                                                                                    p.instOrWave);
                                            MarkModEdited();
                                        }
                                        if (ImGui::IsItemHovered())
                                            ImGui::SetTooltip("Split into the engine's low / normal / high sample\n"
                                                              "ranges (boundaries %d / %d), duplicating the current\n"
                                                              "preset so each range can be reassigned.",
                                                              (int)rng.rangeLo, (int)rng.rangeHi);
                                    }
                                    ImGui::EndDisabled();
                                }
                                // "Treat as drum": route this melodic instrument through the
                                // drum path so each distinct note becomes a slot mapped to a GM
                                // percussion sound. For a melodic slot that's really percussion
                                // (e.g. a song using an instrument slot for a drum hit). An
                                // advanced move, so it's hidden in Simple view.
                                ImGui::BeginDisabled(editMod.empty());
                                const bool asDrumClicked = !simpleView && ImGui::SmallButton("As Drum##forcedrum");
                                ImGui::EndDisabled();
                                if (asDrumClicked) {
                                    SOH::MidiTranslator::Instance().SetForcedDrum(p.fontId, p.instOrWave, true);
                                    MarkModEdited();
                                }
                                if (!simpleView && ImGui::IsItemHovered())
                                    ImGui::SetTooltip("Treat this instrument as a drum/percussion channel.\n"
                                                      "Each distinct note it plays becomes a slot you map to\n"
                                                      "a GM drum sound. Play the song first, then expand the\n"
                                                      "row and click Slots to discover the notes.");
                                ImGui::TableSetColumnIndex(modeCol);
                                if (ImGui::RadioButton("Native##bypass", playsNative)) {
                                    tr.SetAuditionNative(p.fontId, p.instOrWave, true);
                                }
                                if (ImGui::IsItemHovered()) {
                                    ImGui::SetTooltip("Listen to the native instrument for comparison.\n"
                                                      "Not saved: the mods decide what plays.");
                                }
                                ImGui::SameLine();
                                ImGui::BeginDisabled(playingEntry == nullptr);
                                if (ImGui::RadioButton("Synth##bypass", !playsNative)) {
                                    tr.SetAuditionNative(p.fontId, p.instOrWave, false);
                                }
                                ImGui::EndDisabled();

                                ImGui::BeginDisabled(!canEditEntry);
                                auto disabledTooltipIfNative = [&]() {
                                    if (!canEditEntry && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                        ImGui::SetTooltip(editMod.empty()
                                                              ? "Choose a mod to edit above the table."
                                                              : "The mod being edited maps nothing here yet.\n"
                                                                "Pick a preset first.");
                                    }
                                };

                                ImGui::TableSetColumnIndex(gainCol);
                                float gainStored = activeEntry ? activeEntry->gain : 0.0f;
                                float gainShown = (gainStored == 0.0f) ? 1.0f : gainStored;
                                ImGui::SetNextItemWidth(120.0f);
                                if (ImGui::SliderFloat("##gain", &gainShown, 0.0f, 4.0f, "%.2f")) {
                                    if (activeIdx >= 0) {
                                        SOH::MidiTranslator::Instance().SetEntryGain(activeIdx, gainShown);
                                    }
                                }
                                if (ImGui::IsItemDeactivatedAfterEdit())
                                    MarkModEdited();
                                disabledTooltipIfNative();

                                ImGui::TableSetColumnIndex(shiftCol);
                                int transStored = activeEntry ? activeEntry->transpose : 0;

                                // Decompose the stored semitone offset into a coarse octave
                                // count + a leftover remainder in [-11..+11]. Using truncate-
                                // toward-zero (the language default for int /) keeps the
                                // remainder's sign matched to the input — e.g. -13 → -1 oct,
                                // -1 st (NOT -2 oct, +11 st).
                                int curOctaves = transStored / 12;
                                int curRemainder = transStored - curOctaves * 12;
                                int displayValue;
                                int displayMin, displayMax;
                                if (transSemis) {
                                    displayValue = transStored;
                                    displayMin = -24;
                                    displayMax = 24;
                                } else {
                                    displayValue = curOctaves;
                                    displayMin = -8;
                                    displayMax = 8;
                                }

                                // Tint the cell orange when in octave mode AND there's a
                                // non-zero remainder — the displayed integer doesn't tell
                                // the full story so the user needs a visual cue (tooltip
                                // carries the exact remainder). Subtle alpha so it doesn't
                                // clash with the row's synth/native tint.
                                const bool hasRemainder = (!transSemis && curRemainder != 0);
                                if (hasRemainder) {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(220, 150, 60, 70));
                                }

                                // Build a format string that includes the remainder hint in
                                // octave mode so the user sees "+2 oct (+5 st)" inline.
                                // In semitone mode just show the raw "+N st".
                                char fmt[32];
                                if (transSemis) {
                                    std::strcpy(fmt, "%+d st");
                                } else if (hasRemainder) {
                                    std::snprintf(fmt, sizeof(fmt), "%%+d oct (%+d st)", curRemainder);
                                } else {
                                    std::strcpy(fmt, "%+d oct");
                                }

                                ImGui::SetNextItemWidth(-FLT_MIN);
                                if (ImGui::DragInt("##trans", &displayValue, 0.1f, displayMin, displayMax, fmt)) {
                                    displayValue = std::clamp(displayValue, displayMin, displayMax);
                                    int newSemis;
                                    if (transSemis) {
                                        // Direct semitone control — straight assignment.
                                        newSemis = displayValue;
                                    } else {
                                        // Octave control — apply a delta in multiples of 12
                                        // and preserve the remainder so toggling between
                                        // octave/semitone display doesn't silently drop the
                                        // fine offset.
                                        int deltaOctaves = displayValue - curOctaves;
                                        newSemis = transStored + deltaOctaves * 12;
                                    }
                                    newSemis = std::clamp(newSemis, -127, 127);
                                    if (activeIdx >= 0) {
                                        SOH::MidiTranslator::Instance().SetEntryTranspose(
                                            activeIdx, static_cast<int8_t>(newSemis));
                                    }
                                }
                                if (ImGui::IsItemDeactivatedAfterEdit())
                                    MarkModEdited();
                                // Right-click: re-apply the tuning-derived octave Shift (auto-seeded
                                // on preset pick). Only meaningful when there's an active entry.
                                int pairSuggest =
                                    activeEntry ? SOH::SuggestedTranspose(p.fontId, p.instOrWave, activeEntry->noteLow,
                                                                          activeEntry->noteHigh)
                                                : 0;
                                if (activeIdx >= 0 && ImGui::BeginPopupContextItem("##transctx")) {
                                    if (ImGui::MenuItem(pairSuggest % 12 == 0 ? "Apply suggested octave"
                                                                              : "Apply suggested shift")) {
                                        SOH::MidiTranslator::Instance().SetEntryTranspose(activeIdx,
                                                                                          (int8_t)pairSuggest);
                                        MarkModEdited();
                                    }
                                    ImGui::EndPopup();
                                }
                                if (ImGui::IsItemHovered()) {
                                    if (transSemis) {
                                        ImGui::SetTooltip(
                                            "Shift this pair's notes by +/-24 semitones (fine).\n"
                                            "Stored value: %+d st. 0 = no shift. Drums skip this column.\n"
                                            "Sample tuning suggests %+d st (right-click to apply).",
                                            (int)transStored, pairSuggest);
                                    } else if (hasRemainder) {
                                        ImGui::SetTooltip("Shift this pair's notes by +/-8 octaves (whole-scale).\n"
                                                          "Stored value: %+d st = %+d oct %+d st leftover.\n"
                                                          "The leftover semitone offset is preserved when you\n"
                                                          "drag here - octaves move by +/-12 around it. Enable\n"
                                                          "Semitone precision above to edit the leftover.\n"
                                                          "(Cell tinted to flag the leftover.)\n"
                                                          "Sample tuning suggests %+d st (right-click to apply).",
                                                          (int)transStored, curOctaves, curRemainder, pairSuggest);
                                    } else {
                                        ImGui::SetTooltip("Shift this pair's notes by +/-8 octaves (whole-scale).\n"
                                                          "Stored value: %+d st. 0 = no shift. Enable Semitone\n"
                                                          "precision above for +/-24 st fine control.\n"
                                                          "Sample tuning suggests %+d st (right-click to apply).",
                                                          (int)transStored, pairSuggest);
                                    }
                                }
                                disabledTooltipIfNative();

                                ImGui::EndDisabled();

                                ImGui::TableSetColumnIndex(presetCol);
                                char defaultLabel[160];
                                if (defaultGmForMode.program == SOH::kUnmapped) {
                                    std::strcpy(defaultLabel, "Default: None");
                                } else if (defaultGmForMode.bank == 128) {
                                    std::snprintf(defaultLabel, sizeof(defaultLabel), "Default: drum kit %u, slot %u",
                                                  (unsigned)defaultGmForMode.program,
                                                  (unsigned)defaultGmForMode.drumNote);
                                } else {
                                    std::snprintf(defaultLabel, sizeof(defaultLabel), "Default: %u: %s",
                                                  (unsigned)defaultGmForMode.program,
                                                  SOH::kGmProgramNames[defaultGmForMode.program]);
                                }

                                char prgPreview[200];
                                if (activeEntry) {
                                    if (activeEntry->packName.empty()) {
                                        std::strcpy(prgPreview, "(None)");
                                    } else if (activeEntry->bank == 128) {
                                        // GM percussion falls back to native; the picked
                                        // entry is preserved for per-slot routing.
                                        std::snprintf(prgPreview, sizeof(prgPreview), "(drums -> native) P%d: %s",
                                                      activeEntry->program, activeEntry->presetName.c_str());
                                    } else {
                                        std::snprintf(prgPreview, sizeof(prgPreview), "P%d: %s", activeEntry->program,
                                                      activeEntry->presetName.c_str());
                                    }
                                } else {
                                    std::strcpy(prgPreview, defaultLabel);
                                }
                                // Another mod outranks the edited one here, so its pick is not what plays.
                                const bool shadowed =
                                    !editMod.empty() && playingEntry != nullptr && playingEntry->modName != editMod;
                                ImGui::SetNextItemWidth(-FLT_MIN);
                                ImGui::BeginDisabled(editMod.empty());
                                if (shadowed)
                                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.75f, 0.30f, 1.0f));
                                const bool prgComboOpen =
                                    ImGui::BeginCombo("##prgCombo", prgPreview, ImGuiComboFlags_HeightLargest);
                                if (shadowed)
                                    ImGui::PopStyleColor();
                                if (prgComboOpen) {
                                    static char prgFilter[64] = "";
                                    if (ImGui::IsWindowAppearing()) {
                                        prgFilter[0] = '\0';
                                        ImGui::SetKeyboardFocusHere();
                                    }
                                    ImGui::SetNextItemWidth(-FLT_MIN);
                                    ImGui::InputTextWithHint("##prgFilter", "Filter (preset or pack name)", prgFilter,
                                                             sizeof(prgFilter));
                                    ImGui::Separator();

                                    const bool filterActive = prgFilter[0] != '\0';
                                    auto containsCi = [](const std::string& hay, const char* needle) -> bool {
                                        if (!needle || !*needle)
                                            return true;
                                        const size_t nlen = std::strlen(needle);
                                        if (nlen > hay.size())
                                            return false;
                                        auto it = std::search(hay.begin(), hay.end(), needle, needle + nlen,
                                                              [](char a, char b) {
                                                                  return std::tolower(static_cast<unsigned char>(a)) ==
                                                                         std::tolower(static_cast<unsigned char>(b));
                                                              });
                                        return it != hay.end();
                                    };

                                    if (!filterActive && ImGui::Selectable(defaultLabel, activeEntry == nullptr)) {
                                        tr.RemovePairRows(p.fontId, p.instOrWave);
                                        MarkModEdited();
                                    }

                                    int lastSfont = -2;
                                    int shown = 0;
                                    for (const auto& lp : sLoadedPresets) {
                                        if (lp.modName != editMod)
                                            continue;
                                        if (filterActive && !containsCi(lp.name, prgFilter) &&
                                            !containsCi(lp.packName, prgFilter)) {
                                            continue;
                                        }
                                        if (lp.sfontId != lastSfont) {
                                            ImGui::Separator();
                                            ImGui::TextDisabled("%s", lp.packName.c_str());
                                            lastSfont = lp.sfontId;
                                        }
                                        char item[256];
                                        std::snprintf(item, sizeof(item), "B%d P%d: %s##%d:%d:%d", lp.bank, lp.program,
                                                      lp.name.c_str(), lp.sfontId, lp.bank, lp.program);
                                        bool sel = activeEntry && activeEntry->modName == lp.modName &&
                                                   activeEntry->packName == lp.packName &&
                                                   activeEntry->bank == lp.bank && activeEntry->program == lp.program;
                                        if (ImGui::Selectable(item, sel)) {
                                            SOH::MidiTranslator::Instance().PickPreset(
                                                p.fontId, p.instOrWave, lp.packName, static_cast<int16_t>(lp.program),
                                                static_cast<int16_t>(lp.bank), lp.name);
                                            MarkModEdited();
                                        }
                                        shown++;
                                    }
                                    if (shown == 0 && !filterActive) {
                                        ImGui::Separator();
                                        ImGui::TextDisabled("(no soundfont loaded from %s)", editMod.c_str());
                                    } else if (filterActive && shown == 0) {
                                        ImGui::Separator();
                                        ImGui::TextDisabled("(no matches for \"%s\")", prgFilter);
                                    }
                                    ImGui::EndCombo();
                                }
                                ImGui::EndDisabled();
                                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                                    std::string tip = editMod.empty()
                                                          ? "Choose a mod to edit above the table."
                                                          : "Pick a preset from the edited mod's soundfonts. It\n"
                                                            "replaces whatever this mod mapped here before.\n"
                                                            "Default = this mod stops mapping the instrument.";
                                    if (playingEntry != nullptr && (editMod.empty() || shadowed)) {
                                        tip +=
                                            "\n\nPlays from " + playingEntry->modName + ": " + playingEntry->presetName;
                                        if (shadowed)
                                            tip += "\n(that mod is mounted after this one)";
                                    }
                                    if (activeEntry && activeEntry->bank == 128) {
                                        tip += "\n\nGM percussion (bank 128) currently falls back\n"
                                               "to native at play time. The picked preset is kept\n"
                                               "in the JSON so a future per-drum-slot routing\n"
                                               "path can use it.";
                                    }
                                    ImGui::SetTooltip("%s", tip.c_str());
                                }

                                // Advanced effects popup (Reverb/Chorus/Cutoff/Q) for the
                                // active entry.
                                ImGui::TableSetColumnIndex(advCol);
                                ImGui::BeginDisabled(!canEditEntry);
                                drawAdvPopup(activeIdx);
                                ImGui::EndDisabled();
                                ImGui::PopID();
                            }
                            ImGui::EndTable();
                        }
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndTable();
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }
#endif // #if ENABLE_FLUIDSYNTH

        if (ImGui::BeginTabItem("Background Music")) {
            Draw_SfxTab("backgroundMusic", SEQ_BGM_WORLD, "Background Music");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Fanfares")) {
            Draw_SfxTab("fanfares", SEQ_FANFARE, "Fanfares");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Events")) {
            Draw_SfxTab("event", SEQ_BGM_EVENT, "Events");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Battle Music")) {
            Draw_SfxTab("battleMusic", SEQ_BGM_BATTLE, "Battle Music");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Ending")) {
            Draw_SfxTab("ending", SEQ_ENDING, "Ending");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Ocarina")) {
            Draw_SfxTab("instrument", SEQ_INSTRUMENT, "Instruments");
            Draw_SfxTab("ocarina", SEQ_OCARINA, "Ocarina");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound Effects")) {
            Draw_SfxTab("sfx", SEQ_SFX, "Sound Effects");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Voices")) {
            Draw_SfxTab("voice", SEQ_VOICE, "Voices");
            ImGui::EndTabItem();
        }

        static bool excludeTabOpen = false;
        if (ImGui::BeginTabItem("Audio Shuffle Pool Management")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (!excludeTabOpen) {
                excludeTabOpen = true;
            }

            static std::map<SeqType, bool> showType{
                { SEQ_BGM_WORLD, true }, { SEQ_BGM_EVENT, true },  { SEQ_BGM_BATTLE, true },
                { SEQ_OCARINA, true },   { SEQ_FANFARE, true },    { SEQ_SFX, true },
                { SEQ_VOICE, true },     { SEQ_INSTRUMENT, true }, { SEQ_BGM_CUSTOM, true },
            };

            // make temporary sets because removing from the set we're iterating through crashes ImGui
            std::set<SequenceInfo*> seqsToInclude = {};
            std::set<SequenceInfo*> seqsToExclude = {};

            static ImGuiTextFilter sequenceSearch;
            UIWidgets::PushStyleInput(THEME_COLOR);
            sequenceSearch.Draw("Filter (inc,-exc)", 490.0f);
            UIWidgets::PopStyleInput();
            ImGui::SameLine();
            if (UIWidgets::Button("Exclude All",
                                  UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
                for (auto seqInfo : AudioCollection::Instance->GetIncludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->category]) {
                        seqsToExclude.insert(seqInfo);
                    }
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button("Include All",
                                  UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
                for (auto seqInfo : AudioCollection::Instance->GetExcludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->category]) {
                        seqsToInclude.insert(seqInfo);
                    }
                }
            }

            ImGui::BeginTable("sequenceTypes", 9,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_WORLD));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_WORLD).c_str(), &showType[SEQ_BGM_WORLD]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_EVENT));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_EVENT).c_str(), &showType[SEQ_BGM_EVENT]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_BATTLE));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_BATTLE).c_str(), &showType[SEQ_BGM_BATTLE]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_OCARINA));
            ImGui::Selectable(GetSequenceTypeName(SEQ_OCARINA).c_str(), &showType[SEQ_OCARINA]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_FANFARE));
            ImGui::Selectable(GetSequenceTypeName(SEQ_FANFARE).c_str(), &showType[SEQ_FANFARE]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_SFX));
            ImGui::Selectable(GetSequenceTypeName(SEQ_SFX).c_str(), &showType[SEQ_SFX]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_VOICE));
            ImGui::Selectable(GetSequenceTypeName(SEQ_VOICE).c_str(), &showType[SEQ_VOICE]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_INSTRUMENT));
            ImGui::Selectable(GetSequenceTypeName(SEQ_INSTRUMENT).c_str(), &showType[SEQ_INSTRUMENT]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_CUSTOM));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_CUSTOM).c_str(), &showType[SEQ_BGM_CUSTOM]);
            ImGui::PopStyleColor(1);

            ImGui::EndTable();

            if (ImGui::BeginTable("tableAllSequences", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Included", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Excluded", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableHeadersRow();
                ImGui::TableNextRow();

                // COLUMN 1 - INCLUDED SEQUENCES
                ImGui::TableNextColumn();

                ImGui::BeginChild("ChildIncludedSequences", ImVec2(0, -8));
                for (auto seqInfo : AudioCollection::Instance->GetIncludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->category]) {
                        if (UIWidgets::Button(std::string(ICON_FA_TIMES "##" + seqInfo->sfxKey).c_str(),
                                              UIWidgets::ButtonOptions()
                                                  .Size(UIWidgets::Sizes::Inline)
                                                  .Padding(ImVec2(9.0f, 6.0f))
                                                  .Color(THEME_COLOR))) {
                            seqsToExclude.insert(seqInfo);
                        }
                        ImGui::SameLine();
                        DrawPreviewButton(seqInfo->sequenceId, seqInfo->sfxKey, seqInfo->category);
                        ImGui::SameLine();
                        DrawTypeChip(seqInfo->category, seqInfo->label);
                        ImGui::SameLine();
                        ImGui::Text("%s", seqInfo->label.c_str());
                    }
                }
                ImGui::EndChild();

                // remove the sequences we added to the temp set
                for (auto seqInfo : seqsToExclude) {
                    AudioCollection::Instance->RemoveFromShufflePool(seqInfo);
                }

                // COLUMN 2 - EXCLUDED SEQUENCES
                ImGui::TableNextColumn();

                ImGui::BeginChild("ChildExcludedSequences", ImVec2(0, -8));
                for (auto seqInfo : AudioCollection::Instance->GetExcludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->category]) {
                        if (UIWidgets::Button(std::string(ICON_FA_PLUS "##" + seqInfo->sfxKey).c_str(),
                                              UIWidgets::ButtonOptions()
                                                  .Size(UIWidgets::Sizes::Inline)
                                                  .Padding(ImVec2(9.0f, 6.0f))
                                                  .Color(THEME_COLOR))) {
                            seqsToInclude.insert(seqInfo);
                        }
                        ImGui::SameLine();
                        DrawPreviewButton(seqInfo->sequenceId, seqInfo->sfxKey, seqInfo->category);
                        ImGui::SameLine();
                        DrawTypeChip(seqInfo->category, seqInfo->sfxKey);
                        ImGui::SameLine();
                        ImGui::Text("%s", seqInfo->label.c_str());
                    }
                }
                ImGui::EndChild();

                // add the sequences we added to the temp set
                for (auto seqInfo : seqsToInclude) {
                    AudioCollection::Instance->AddToShufflePool(seqInfo);
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        } else {
            excludeTabOpen = false;
        }

        ImGui::EndTabBar();
    }
    UIWidgets::PopStyleTabs();
}

std::vector<SeqType> allTypes = {
    SEQ_BGM_WORLD,  SEQ_BGM_EVENT, SEQ_BGM_BATTLE, SEQ_OCARINA, SEQ_FANFARE,
    SEQ_INSTRUMENT, SEQ_SFX,       SEQ_VOICE,      SEQ_ENDING,
};

// Push the current Master Volume straight onto the live synth's master gain so
// dragging the slider scales synth output in lockstep with native -- no synth
// rebuild. No-op when no synth is installed (native path): GetActiveSynth()
// returns nullptr. Construction-time gain is set from the same CVar in
// ApplyFluidSynthFromCVars, so startup / pipeline-enable / backend-switch are
// already covered; this handles the slider moving while a synth is running.
void AudioEditor_ApplySynthMasterVolume() {
#if ENABLE_FLUIDSYNTH
    if (auto synth = SOH::MidiSynthManager::Instance().GetActiveSynth()) {
        synth->SetMasterGain(SynthMasterGainFromCVar());
    }
#endif
}

void AudioEditor_RandomizeAll() {
    for (auto type : allTypes) {
        RandomizeGroup(type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_AutoRandomizeAll() {
    for (auto type : allTypes) {
        RandomizeGroup(type, false);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_RandomizeGroup(SeqType group) {
    RandomizeGroup(group);

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_ResetAll() {
    for (auto type : allTypes) {
        ResetGroup(AudioCollection::Instance->GetAllSequences(), type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_ResetGroup(SeqType group) {
    ResetGroup(AudioCollection::Instance->GetAllSequences(), group);

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_LockAll() {
    for (auto type : allTypes) {
        LockGroup(AudioCollection::Instance->GetAllSequences(), type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void AudioEditor_UnlockAll() {
    for (auto type : allTypes) {
        UnlockGroup(AudioCollection::Instance->GetAllSequences(), type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void RegisterAudioWidgets() {
    lowHpAlarm = { .name = "Mute Low HP Alarm", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    lowHpAlarm.CVar(CVAR_AUDIO("LowHpAlarm"))
        .Options(CheckboxOptions().Color(THEME_COLOR).Tooltip("Disable the low HP beeping sound."));
    SohGui::mSohMenu->AddSearchWidget({ lowHpAlarm, "Enhancements", "Audio Editor", "Audio Options" });

    naviCall = { .name = "Disable Navi Call Audio", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    naviCall.CVar(CVAR_AUDIO("DisableNaviCallAudio"))
        .Options(CheckboxOptions().Color(THEME_COLOR).Tooltip("Disables the voice audio when Navi calls you."));
    SohGui::mSohMenu->AddSearchWidget({ naviCall, "Enhancements", "Audio Editor", "Audio Options" });

    enemyProx = { .name = "Disable Enemy Proximity Music", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    enemyProx.CVar(CVAR_AUDIO("EnemyBGMDisable"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Disables the music change when getting close to enemies. Useful for hearing "
                              "your custom music for each scene more often."));

    leeverProx = { .name = "Enable Enemy Proximity Music for Leever", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    leeverProx.CVar(CVAR_AUDIO("LeeverEnemyBGM"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Plays the battle music when getting close to a Leever, like in Majora's Mask."));

    leadingMusic = { .name = "Disable Leading Music in Lost Woods", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    leadingMusic.CVar(CVAR_AUDIO("LostWoodsConsistentVolume"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Disables the volume shifting in the Lost Woods. Useful for hearing "
                              "your custom music in the Lost Woods if you don't need the navigation assitance "
                              "the volume changing provides. If toggling this while in the Lost Woods, reload "
                              "the area for the effect to kick in."));
    SohGui::mSohMenu->AddSearchWidget({ leadingMusic, "Enhancements", "Audio Editor", "Audio Options" });

    displaySeqName = { .name = "Display Sequence Name in Notifications", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    displaySeqName.CVar(CVAR_AUDIO("SeqNameNotification"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Emits a notification with the current song name whenever it changes. "
                              "(does not apply to fanfares or enemy BGM)."));
    SohGui::mSohMenu->AddSearchWidget({ displaySeqName, "Enhancements", "Audio Editor", "Audio Options" });

    ovlDuration = { .name = "Sequence Notification Duration: %d seconds", .type = WidgetType::WIDGET_CVAR_SLIDER_INT };
    ovlDuration.CVar(CVAR_AUDIO("SeqNameNotificationDuration"))
        .Options(IntSliderOptions().Color(THEME_COLOR).Min(1).Max(20).DefaultValue(10).Size(ImVec2(300.0f, 0.0f)));
    SohGui::mSohMenu->AddSearchWidget({ ovlDuration, "Enhancements", "Audio Editor", "Audio Options" });

    voicePitch = { .name = "Link's Voice Pitch Multiplier", .type = WidgetType::WIDGET_CVAR_SLIDER_FLOAT };
    voicePitch.CVar(CVAR_AUDIO("LinkVoiceFreqMultiplier"))
        .Options(FloatSliderOptions()
                     .Color(THEME_COLOR)
                     .IsPercentage()
                     .Min(0.4f)
                     .Max(2.5f)
                     .DefaultValue(1.0f)
                     .Size(ImVec2(300.0f, 0.0f)));
    SohGui::mSohMenu->AddSearchWidget({ voicePitch, "Enhancements", "Audio Editor", "Audio Options" });

    randomAudioGenModes = { .name = "Automatically Randomize All Music and Sound Effects",
                            .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    randomAudioGenModes.CVar(CVAR_AUDIO("RandomizeAudioGenModes"))
        .Options(
            ComboboxOptions()
                .DefaultIndex(RANDOMIZE_OFF)
                .ComboMap(audioRandomizerModes)
                .Tooltip(
                    "Set when the music and sound effects is automaticly randomized:\n"
                    "- Manual: Manually randomize music or sound effects by pressing the 'Randomize all Groups' "
                    "button\n"
                    "- On New Scene : Randomizes when you enter a new scene.\n"
                    "- On Rando Gen Only: Randomizes only when you generate a new randomizer.\n"
                    "- On File Load: Randomizes on File Load.\n"
                    "- On File Load (Seeded): Randomizes on file load based on the current randomizer seed/file."));
    SohGui::mSohMenu->AddSearchWidget({ randomAudioGenModes, "Enhancements", "Audio Editor", "Audio Options" });

    lowerOctaves = { .name = "Lower Octaves of Unplayable High Notes", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    lowerOctaves.CVar(CVAR_AUDIO("ExperimentalOctaveDrop"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Some custom sequences may have notes that are too high for the game's audio "
                              "engine to play. Enabling this checkbox will cause these notes to drop a "
                              "couple of octaves so they can still harmonize with the other notes of the "
                              "sequence."));
    SohGui::mSohMenu->AddSearchWidget({ lowerOctaves, "Enhancements", "Audio Editor", "Audio Options" });

#if ENABLE_FLUIDSYNTH
    fluidSynthEnabled = { .name = "Modern audio pipeline (floating point)", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    fluidSynthEnabled.CVar(CVAR_AUDIO("ModernAudioPipeline"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Run the audio path in 32-bit float instead of 16-bit integer.\n"
                              "Removes the resampler's s16 quantisation on its own.\n"
                              "Required for FluidSynth synth packs - see the FluidSynth tab\n"
                              "for pack selection and per-instrument tuning."));
    SohGui::mSohMenu->AddSearchWidget({ fluidSynthEnabled, "Enhancements", "Audio Editor", "Audio Options" });

    // Per-mode synth volume, shown to the user as a percentage relative to the
    // native engine: 100% = matched to native. Each mode's real trim to reach
    // native differs (reverb + curve) and is hidden behind a calibration constant
    // in MidiTranslator, so the slider is a clean relative level defaulting to
    // 100% for both modes -- the user never sees the raw per-mode values.
    auto gainTip = "Synth loudness for this mode, relative to the native engine.\n"
                   "100% = matched to native.";
    fluidSynthGainEnhanced = { .name = "Synth volume (Enhanced)", .type = WidgetType::WIDGET_CVAR_SLIDER_FLOAT };
    fluidSynthGainEnhanced.CVar(CVAR_AUDIO("FluidSynthGainEnhanced"))
        .Options(FloatSliderOptions()
                     .Color(THEME_COLOR)
                     .IsPercentage()
                     .Min(0.0f)
                     .Max(2.0f)
                     .DefaultValue(1.0f)
                     .Size(ImVec2(300.0f, 0.0f))
                     .Tooltip(gainTip));
    SohGui::mSohMenu->AddSearchWidget({ fluidSynthGainEnhanced, "Enhancements", "Audio Editor", "FluidSynth" });

    fluidSynthGainAuthentic = { .name = "Synth volume (Authentic)", .type = WidgetType::WIDGET_CVAR_SLIDER_FLOAT };
    fluidSynthGainAuthentic.CVar(CVAR_AUDIO("FluidSynthGainAuthentic"))
        .Options(FloatSliderOptions()
                     .Color(THEME_COLOR)
                     .IsPercentage()
                     .Min(0.0f)
                     .Max(2.0f)
                     .DefaultValue(1.0f)
                     .Size(ImVec2(300.0f, 0.0f))
                     .Tooltip(gainTip));
    SohGui::mSohMenu->AddSearchWidget({ fluidSynthGainAuthentic, "Enhancements", "Audio Editor", "FluidSynth" });

    // Auto-apply at launch: if the user had the Modern audio pipeline
    // enabled in a previous session, switch the AudioPlayer into float
    // mode now and stack every enabled synth pack so the engine doesn't
    // keep playing on the s16 path until they open the AudioEditor. On
    // hard failure clear the CVar so the checkbox stays honest.
    if (CVarGetInteger(CVAR_AUDIO("ModernAudioPipeline"), 0)) {
        if (!ApplyFluidSynthFromCVars()) {
            CVarSetInteger(CVAR_AUDIO("ModernAudioPipeline"), 0);
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
    }

    ReloadModMappings();
#endif
}

static RegisterMenuInitFunc menuInitFunc(RegisterAudioWidgets);
