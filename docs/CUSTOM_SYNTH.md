# Custom SoundFonts using FluidSynth

SoH can play its music through [FluidSynth](https://www.fluidsynth.org/)
using SoundFont (`.sf2` / `.sf3`) files as **synth packs**. Each game
instrument is routed to a SoundFont preset, and can be tuned further if you
want. This page opens with a quick start; the deeper controls and the modder
reference follow, so skip ahead only if you need them.

> FluidSynth is a **compile-time** option. You need a build made with
> `-DENABLE_FLUIDSYNTH=ON` (see [BUILDING](BUILDING.md)). Without it, the
> FluidSynth tab does not appear.

---

## Quick start

1. In **Audio Editor -> Audio Options**, tick **Modern audio pipeline
   (floating point)**. FluidSynth needs it; the FluidSynth tab also has a
   one-click button if you land there first.
2. Drop a `.sf2` or `.sf3` into `<SoH-config>/synth-packs/` (create the folder
   next to your `mods` folder).
3. Open **Audio Editor -> FluidSynth** and enable your pack in the pack list.
   Hit **Rescan** if it does not appear.
4. The list shows every game instrument. For each one, pick a preset in the
   **Preset** column. For drums, expand the row, pick a **Kit**, and map each
   named slot to a GM percussion sound. That is sample replacement - you are
   done.
5. Want to fine-tune (volume, pitch, per-range splits, effects)? Untick
   **Simple view** to reveal the advanced columns.

There is no Save button - every edit is written to
`<SoH-config>/fluidsynth_overrides.json` automatically.

---

## For users

### The Modern audio pipeline

Ticking **Modern audio pipeline (floating point)** switches the audio path to
32-bit float. It is a quality win on its own (less rounding during mixing and
resampling), and it is required for FluidSynth.

### Adding SoundFont files (SF2 / SF3)

Both `.sf2` and `.sf3` work. SF3 is the same format with Ogg/Vorbis-compressed
samples - a fraction of the size for essentially the same sound. An `.sf3`
that cannot be decoded is skipped with a reason shown in the tab.

Two ways to supply a pack:

- **Loose folder** - drop `*.sf2` / `*.sf3` (plus an optional sibling
  `<basename>.json` for tuning) into `<SoH-config>/synth-packs/`.
- **Mod archive** - inside any mounted `.o2r`, at
  `audio/synth/<pack-name>/soundfont.sf3` (plus an optional `mapping.json`).

Both show in the pack list tagged `[loose]` or `[mod]`. **Rescan**
re-enumerates without a restart. Untick a pack to disable it. With more than
one pack enabled, **last loaded wins** on `(bank, program)` collisions;
picking a preset pins that row to the pack that owns it.

### The instrument list

Every game instrument is listed (grouped by font), whether or not a song has
played it yet. A row tints **green** when it is currently sounding through the
synth and **blue** when sounding native.

**Drums** are a row like any other: expand one to see every slot its font
defines, each named after the game sample it plays. Pick a **Kit** on the
parent row, then map any slot to a GM percussion sound - no need to play the
song first.

Filters above the list keep it manageable:

- **Simple view** (on by default) - shows just each instrument, its name and
  its preset; drums and splits stay expandable. Untick it for the tuning
  columns (Mode / Gain / Shift / effects) and the split editors.
- **Only named** - hide slots with no detected sample name (usually unused).
- **Only played** - limit the list to instruments heard since launch. The
  **Clear** button next to it resets that list, so hitting Clear when a new
  song starts isolates just that song's instruments.
- **Font** - scope the list to a single font.

Above the list, **Reset all** drops your personal overrides back to the active
pack's defaults, and **Export pack mapping...** publishes a pack (see the
modder section).

In the full (non-Simple) view each row has these columns:

| Column   | What it does |
|----------|--------------|
| Song     | Engine font name. Read-only. (A font is shared across several songs.) |
| Sample   | Editable label - type a friendly name. The hint shows the detected SF sample name. |
| Inst     | Engine slot id; `0 (Drum)` / `1 (SFX)` are special. Holds the **Split** / **L/M/H** / **As Drum** buttons. |
| Mode     | **Native** (engine synth) vs **Synth** (FluidSynth). |
| Gain     | Per-instrument synth volume (0..4x). |
| Shift    | Pitch shift. Header toggles octaves vs semitones. Auto-seeded from the sample's tuning when you pick a preset. |
| Preset   | Pick an SF preset from any loaded pack. Selecting also pins the row to that pack. |
| Adv      | Per-entry popup: Reverb (CC91), Chorus (CC93), Cutoff (CC74), Q (CC71). |

There is also a session-only **Override** column (Solo / Mute) for auditioning;
it is never saved.

### Authentic vs Enhanced, and volume

Two radio buttons choose how velocity/volume is shaped:

- **Authentic** (default) - halved-amount velocity/CC7/CC11 modulators (curve
  adapted from [ANMP](https://github.com/derselbst/ANMP)) for a console-era
  feel. Pairs with a heavier reverb.
- **Enhanced** - the SoundFont author's own dynamics. Best with
  musically-curated banks (orchestral, SC-55, MuseScore).

The **Volume** slider is **per mode** (each is calibrated to a different
baseline). 100% aims to match native loudness - parity, not a boost.

### Advanced tuning (untick Simple view)

- **Note-range splits (melodic).** Many instruments load different samples at
  low / normal / high pitch, so one preset sounds right at one octave and
  wrong at another. **Split** bisects a row into note ranges; **L/M/H** splits
  into the engine's own low/normal/high boundaries. A split row becomes a
  collapsible header - expand it to set a different preset per range. Ranges are
  always contiguous and cover the whole keyboard; you drag the boundary between
  two ranges. **Flatten** collapses back to one entry.
- **SFX and As-Drum slots.** Unlike drums, SFX (slot `1 (SFX)`) and As-Drum
  pairs have no fixed slot table - their slots appear as they play, or via the
  manual **Add** row.
- **As Drum.** Some songs play percussion through a melodic slot. **As Drum**
  routes that row through the drum path so each note becomes a mappable slot;
  **Melodic** reverts it.

### Known limitations

A generic SoundFont cannot fully match the original samples - the engine uses
per-key sample splits with their own envelopes, filter and reverb shaping,
while a GM program plays one sample across the range. Slot meaning is also
**font-specific** (the same instrument index differs between fonts), so tuning
is per-font work. A SoundFont purpose-built for the game's layout is the
realistic path to "better than native".

---

## For modders

### Pack layout

Loose form (for iterating while authoring):

```paths
<SoH-config>/synth-packs/MyPack.sf3   (or .sf2)
<SoH-config>/synth-packs/MyPack.json  (optional, sibling)
```

Archive form (for distribution - what **Export .o2r** builds):

```paths
<MyPack.o2r>/audio/synth/MyPack/soundfont.sf3   (or .sf2)
<MyPack.o2r>/audio/synth/MyPack/mapping.json    (optional)
```

The `<pack-name>` segment is the user-visible id - a short ASCII slug like
`HD-Orchestra`. It must match the soundfont name; that is how the loader ties
the mapping to the soundfont.

### Authoring loop

1. Drop your SF into the loose folder; **Rescan** if needed.
2. Enable just your pack. Play the songs you are tuning against.
3. Walk the list: pick a **Preset** per row (which pins it to your SF), then
   tune Gain / Shift / Adv effects and split or map drums where one preset is
   not enough. Type a friendly **Sample** label as you go.
4. Every edit auto-saves. When happy, **Export pack...** writes a shippable mod.

### Export pack

**Export pack mapping...** (above the list) exports the pack's **effective
mapping** - every entry currently routing through synth for that pack, whether
hand-picked or from the pack's own `mapping.json`. It strips runtime-only flags
and writes the pack name once as a `pack_name` header. Two outputs:

- **Export .o2r** (recommended) - zips soundfont + mapping into
  `<SoH-config>/mods/<pack_name>.o2r`, ready to share. Loads on next launch.
- **JSON only** - writes just the mapping beside your loose soundfont
  (`<SoH-config>/synth-packs/<pack_name>.json`); picked up on Rescan.

### `mapping.json` schema

Same shape as the user's `fluidsynth_overrides.json` (schema **version 2**).
Your pack's mapping overlays the defaults; the user's file overlays yours.

```json
{
  "version": 2,
  "pack_name": "MyPack",
  "entries": [
    {
      "fontId": 6,
      "instOrWave": 12,
      "bank": 0,
      "program": 46,
      "preset_name": "Orchestral Harp",
      "display_name": "Lush strings",
      "gain": 0.85,
      "transpose": 0,
      "reverb": 64,
      "chorus": 16,
      "filter_cutoff": 96,
      "filter_q": 32
    }
  ],
  "drum_channels_synth": [ { "fontId": 9, "instOrWave": 0 } ],
  "forced_drums":        [ { "fontId": 9, "instOrWave": 3 } ]
}
```

| Key             | Type               | Meaning |
|-----------------|--------------------|---------|
| `version`       | int                | Schema version. Use `2`. |
| `pack_name`     | string             | Names the pack (added by Export). The loader also derives it from the file/folder name, so a rename is safe. |
| `fontId`        | int 0-63           | Engine sound-font index. Required. 0-37 are vanilla. |
| `font`          | string             | Font resource path (e.g. `custom/fonts/MyFont`). Modded fonts get their number from mod load order, so entries resolve by this name first; without it, only vanilla `fontId`s (0-37) are honored. Export writes it automatically. |
| `instOrWave`    | int 0-255          | Engine instrument slot. Required. `0` = drum bank, `1` = SFX bank. |
| `bank`          | int 0-255          | SF bank. Default 0 (GM melodic); 128 = GM drums. |
| `program`       | int 0-127          | SF program inside that bank. |
| `preset_name`   | string             | Human-readable preset name. Metadata only (drift detection). |
| `display_name`  | string             | Friendly label shown in the Sample column. |
| `gain`          | float              | Per-pair volume multiplier. Omit = 1.0x. |
| `transpose`     | int (semitones)    | Pitch shift. Omit = 0. |
| `reverb`        | int 0-127          | CC91 reverb send. |
| `chorus`        | int 0-127          | CC93 chorus send. |
| `filter_cutoff` | int 0-127          | CC74 low-pass cutoff (64 = SF default). |
| `filter_q`      | int 0-127          | CC71 low-pass resonance (64 = SF default). |
| `note_low`      | int 0-127          | Low end of this entry's range. Omit = 0. |
| `note_high`     | int 0-127          | High end of the range. Omit = 127. |
| `fixed_note`    | int 0-127          | Play this exact note instead of the engine pitch (drum slots). |
| `route`         | "synth" / "native" | Per-entry route. Omit = synth. |

Omit any field you do not want to set - partial entries layer cleanly.
Multiple entries can share one `(fontId, instOrWave)`: distinct `note_low`
values become note-range splits or drum slots. `drum_channels_synth` lists the
drum channels whose master is set to Synth; `forced_drums` lists melodic pairs
flagged **As Drum**.

### Resolution model

At play time, for each engine `(fontId, instOrWave)` pair: keep the enabled,
resolvable entries (pinned pack loaded and it has that `(bank, program)`); zero
matches plays native; for a split/drum pair the entry whose
`[note_low..note_high]` covers the incoming semitone wins; on a plain collision
the **last-loaded** pack wins. Pinning uses FluidSynth's `program_select`, so
your pack wins for the rows you pinned even if another SF shares the same
`(bank, program)`.

The override chain, highest priority last: built-in defaults (empty) ->
pack `mapping.json` files in enabled order -> user's
`fluidsynth_overrides.json`. So a user's edit always wins over a pack default.

### Packaging by hand

**Export .o2r** is the easy path. To build one manually, lay out the inner
shape and zip it (standard `zip` produces a valid `.o2r`):

```bash
cd my-pack/          # contains audio/synth/MyPack/{soundfont.sf3,mapping.json}
zip -r ../MyPack.o2r audio/
```

Each `audio/synth/<pack>/` folder is one enableable pack; ship two soundfonts
as two folders (or two `.o2r` files). A loose `<name>.sf3` and a shipped
`<name>.o2r` share the same internal pack name - keep only one enabled at a
time.

### Engine quirks the substitution model can't paper over

These are limits of the approach, not bugs:

- **Drum / SFX banks use slot indices, not pitches.** Use the per-slot Drum
  Sound mapping, not the melodic Preset combo. SFX is usually best left Native.
- **Some percussion lives on melodic rows.** Mapping those to GM percussion
  cleanly is not yet first-class - leave Native or approximate with a tuned
  preset.
- **Single-NoteOn polyphonic samples** (a chord baked into one sample) cannot
  be reproduced one-voice-per-event - leave the row Native.
- **Custom engine fonts** get their `fontId` from mod load order, so entries
  for them resolve by the `font` name (written automatically). If the owning
  mod isn't loaded, those entries sit dormant and the instruments play native;
  they come back when the mod does. Vanilla fonts (0-37) are stable by id.
  Total fonts are capped at 64.

---

## See also

- [derselbst/**ANMP**](https://github.com/derselbst/ANMP) - a FluidSynth-based
  player for sequenced console-era music; origin of the Authentic volume curve.
  Its wiki's [Reproducing N64 OSTs
  accurately](https://github.com/derselbst/ANMP/wiki/Reproducing-N64-OSTs-accurately)
  page informs the Authentic reverb default.
