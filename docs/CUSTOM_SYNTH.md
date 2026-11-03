# Custom SoundFonts using FluidSynth

SoH can play its music through [FluidSynth](https://www.fluidsynth.org/)
using SoundFont (`.sf2` / `.sf3`) files shipped inside mods. A mod maps game
instruments to presets of its soundfonts; the game still plays the notes, so
the sequence's own dynamics and layering survive. This page opens with the
player's side; the authoring workflow and the file reference follow.

> FluidSynth is a **compile-time** option. You need a build made with
> `-DENABLE_FLUIDSYNTH=ON` (see [BUILDING](BUILDING.md)). Without it, the
> FluidSynth tab does not appear.

---

## For players

1. In **Audio Editor -> Audio Options**, tick **Modern audio pipeline
   (floating point)**. FluidSynth needs it; the FluidSynth tab also has a
   one-click button if you land there first.
2. Drop the mod's `.o2r` into `mods/` and restart. That is all: a mod in
   `mods/` is active, and the mods menu orders it against the others.

Where two mods map the same instrument, the one **higher in the mods menu**
wins, the same rule as for textures and every other file a mod replaces.

### Settings

- **Modern audio pipeline** - switches the audio path to 32-bit float. A
  quality win on its own (less rounding in mixing and resampling), and
  required for FluidSynth.
- **Authentic** (default) - a console-style volume curve (adapted from
  [ANMP](https://github.com/derselbst/ANMP)), with synth notes run through a
  port of the engine's own reverb.
- **Enhanced** - the SoundFont author's own dynamics and a subtle reverb.
  Best with musically-curated banks (orchestral, SC-55, MuseScore).
- **Volume** - per mode, since each is calibrated to a different baseline.
  100% aims to match native loudness - parity, not a boost.

---

## For modders

### Starting a mod

In **Audio Editor -> FluidSynth**:

- **New mod from soundfont...** picks a `.sf2` / `.sf3`, creates
  `mods/<soundfont name>.o2r` holding it and an empty mapping, loads it, and
  starts editing it.
- **Add soundfont...** puts another soundfont into the mod being edited. A mod
  can hold several.

### Editing

**Editing:** chooses the one mod your changes go into. Other mods are never
written, whatever plays from them; with **Nothing (listen only)** the table is
read-only. Edits mark the mod **unsaved changes**; **Save** writes them into
its `.o2r`, **Revert** reloads every mod's mapping. The Editing combo is
locked while changes are unsaved.

The table lists every game instrument (grouped by font), whether or not a song
has played it yet. A row tints **green** when it is sounding through the synth
and **blue** when sounding native. Filters keep it manageable:

- **Simple view** (on by default) - just each instrument, its name and its
  preset; untick it for the tuning columns and split editors.
- **Only named** - hide slots with no detected sample name (usually unused).
- **Only played** - instruments heard since launch. **Clear** resets the
  list, so hitting it when a song starts isolates that song's instruments.
- **Font** - scope the list to a single font.

| Column   | Saved | What it does |
|----------|-------|--------------|
| Override | no    | **Solo** / **Mute**, for auditioning. |
| Song     | -     | Engine font name. A font is shared across several songs. |
| Sample   | yes   | Friendly label. The hint shows the detected sample name. |
| Inst     | yes   | Engine slot id; `0 (Drum)` / `1 (SFX)` are special. Holds **Split** / **L/M/H** / **As Drum**. |
| Mode     | no    | **Native** / **Synth**: hear the instrument either way, for comparison. The mods decide what plays. |
| Gain     | yes   | Synth volume for the instrument (0..4x). |
| Shift    | yes   | Pitch shift. Seeded from the sample's tuning when you pick a preset. |
| Preset   | yes   | A preset from the edited mod's soundfonts. Picking one replaces whatever this mod mapped there; **Default** removes this mod's mapping for the instrument. |
| Adv      | yes   | Reverb (CC91), Chorus (CC93), Cutoff (CC74), Q (CC71). |

When another mod is what plays for an instrument, the preview turns amber and
the Preset tooltip names that mod.

**Drums** are a row like any other: expand one to see every slot its font
defines, named after the game sample it plays. Pick a **Kit** on the parent
row, then map any slot to a percussion sound - no need to play the song first.

**Advanced** (untick Simple view):

- **Note-range splits.** Many instruments load different samples at low /
  normal / high pitch, so one preset sounds right at one octave and wrong at
  another. **Split** bisects a row; **L/M/H** splits at the engine's own
  boundaries. A split row expands to one preset per range. Ranges stay
  contiguous and cover the whole keyboard; drag the boundary between two.
  **Flatten** merges them back. Unlike the instrument's own Mode, a range's
  Native / Synth is saved: Native leaves that range to other mods.
- **SFX and As-Drum slots.** These have no fixed slot table - their slots
  appear as they play, or via the manual **Add** row.
- **As Drum.** Some songs play percussion through a melodic slot. **As Drum**
  routes that row through the drum path so each note becomes a mappable slot;
  **Melodic** reverts it.

### Mod layout

```paths
<MyMod.o2r>/audio/synth/<pack>/soundfont.sf3   (or .sf2)
<MyMod.o2r>/audio/synth/<pack>/mapping.json
```

Each `audio/synth/<pack>/` folder is one soundfont and its mapping; the folder
name is the pack name the mapping refers to. An `.o2r` is a zip, so one can be
built by hand:

```bash
cd my-mod/          # contains audio/synth/MyPack/{soundfont.sf3,mapping.json}
zip -r ../MyMod.o2r audio/
```

### `mapping.json`

Written by **Save**; editable by hand. Schema **version 2**.

```json
{
  "version": 2,
  "entries": [
    {
      "fontId": 6,
      "instOrWave": 12,
      "bank": 0,
      "program": 46,
      "preset_name": "Orchestral Harp",
      "gain": 0.85,
      "transpose": 0,
      "reverb": 64,
      "chorus": 16,
      "filter_cutoff": 96,
      "filter_q": 32
    },
    { "fontId": 6, "instOrWave": 12, "display_name": "Lush strings" }
  ],
  "drum_channels_synth": [ { "fontId": 9, "instOrWave": 0 } ],
  "forced_drums":        [ { "fontId": 9, "instOrWave": 3 } ]
}
```

| Key             | Type                         | Meaning |
|-----------------|------------------------------|---------|
| `version`       | int                          | Schema version. Use `2`. |
| `fontId`        | int 0-63                     | Engine sound-font index. 0-37 are vanilla. |
| `font`          | string                       | Font resource path. Modded fonts get their number from mod load order, so entries resolve by this name first; without it, only vanilla `fontId`s are honored. Save writes it. |
| `instOrWave`    | int 0-255                    | Engine instrument slot. `0` = drum bank, `1` = SFX bank. |
| `bank`          | int 0-255                    | SF bank. Default 0; 128 = percussion. |
| `program`       | int 0-127                    | SF program inside that bank. An entry without one only carries `display_name`. |
| `preset_name`   | string                       | Human-readable preset name. Metadata only. |
| `display_name`  | string                       | Label shown in the Sample column. |
| `gain`          | float                        | Volume multiplier. Omit = 1.0x. |
| `transpose`     | int (semitones)              | Pitch shift. Omit = 0. |
| `reverb`        | int 0-127                    | CC91 reverb send. |
| `chorus`        | int 0-127                    | CC93 chorus send. |
| `filter_cutoff` | int 0-127                    | CC74 low-pass cutoff (64 = SF default). |
| `filter_q`      | int 0-127                    | CC71 low-pass resonance (64 = SF default). |
| `note_low`      | int 0-127                    | Low end of the entry's range. Omit = 0. |
| `note_high`     | int 0-127                    | High end of the range. Omit = 127. |
| `fixed_note`    | int 0-127                    | Play this exact note instead of the engine pitch (drum slots). |
| `route`         | "synth" / "native" / "mute"  | What the range plays. Omit = synth. |
| `enabled`       | bool                         | `false` keeps the entry but leaves the range to other mods. Omit = true. |

Entries for one instrument with distinct `note_low` values are note-range
splits or drum slots; a mod holds one entry per range, and when two repeat a
range the later one is used. `drum_channels_synth` lists drum channels set to
Synth; `forced_drums` lists melodic instruments flagged **As Drum**. Unknown
keys, such as an older file's `pack_name`, are ignored.

### What plays

For each note, among the loaded mods' entries covering its pitch:

1. A narrower range beats a wider one, whichever mod it is in.
2. Then the mod higher in the mods menu wins.
3. Nothing resolvable plays native. A synth entry needs its soundfont loaded
   with that `(bank, program)`; `route: "native"` claims the range for the
   engine's own instrument, over any mod that ranks below.

So a mod can refine another one - split an instrument another mod maps whole -
without copying its rows.

### Limits of the approach

- **A generic SoundFont cannot fully match the original samples.** The engine
  uses per-key sample splits with their own envelopes, filter and reverb
  shaping, while a preset plays one sample set across the range. Slot meaning
  is **font-specific** (the same instrument index differs between fonts), so
  mapping is per-font work.
- **Drum / SFX banks use slot indices, not pitches.** Map per slot, not with
  the melodic Preset combo. SFX is usually best left native.
- **Single-NoteOn polyphonic samples** (a chord baked into one sample) cannot
  be reproduced one voice per event - leave the row native.
- **Native instruments have no gain or effects** here; those controls apply to
  synth rows only.
- **Never ship samples extracted from the game.** A soundfont built from the
  game's own audio is game data, not original work; leave those instruments
  native instead.
- **Custom engine fonts** resolve by the `font` name. If the mod that adds the
  font isn't loaded, those entries sit dormant and the instruments play native.
  Total fonts are capped at 64.

---

## See also

- [derselbst/**ANMP**](https://github.com/derselbst/ANMP) - a FluidSynth-based
  player for sequenced console-era music; origin of the Authentic volume curve.
  Its wiki's [Reproducing N64 OSTs
  accurately](https://github.com/derselbst/ANMP/wiki/Reproducing-N64-OSTs-accurately)
  page informs the Authentic reverb default.
