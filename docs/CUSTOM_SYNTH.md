# Custom SoundFonts using FluidSynth

SoH can play its music through [FluidSynth](https://www.fluidsynth.org/)
using one or more SoundFont (`.sf2` or `.sf3`) files supplied as **synth
packs**.
Each engine instrument can be routed to a specific SF preset, tuned per
pair (gain, pitch shift, reverb / chorus / filter), split across note
ranges, mapped per drum slot, and labeled with a friendly name. This doc
is the user + modder guide.

---

## For users

### Modern audio pipeline (F32 instead of S16)

In **Audio Editor -> Audio Options**, tick **Modern audio pipeline
(floating point)**. This switches the audio path to 32-bit float.

The float pipeline is its own quality win even with no SF loaded: it
processes audio at higher precision, reducing rounding artifacts and
preserving more detail during mixing and resampling. FluidSynth requires
it, so the FluidSynth tab also offers a one-click **Enable Modern
Pipeline** button if you land there first.

### Enabling the FluidSynth pipeline

FluidSynth must be enabled at compile time. At the time of writing the
only way to get it is to compile Shipwright from source. Read
[BUILDING](BUILDING.md) for the basics, install
[FluidSynth](https://www.fluidsynth.org/download/), and add
`-DENABLE_FLUIDSYNTH=ON` to the CMake configure command (or set the
option after the first generation).

With it compiled in, the **Audio Editor** gains a dedicated **FluidSynth**
tab. That's where everything below lives.

### Adding SoundFont files (SF2 / SF3)

Both `.sf2` and `.sf3` are accepted. SF3 is the same format with its
samples Ogg/Vorbis-compressed - typically a fraction of the size (a large
GM SoundFont can drop from ~200 MB to ~40 MB) for essentially the same
sound. FluidSynth decompresses SF3 transparently, **provided the
FluidSynth it was linked against was built with libsndfile + Vorbis**
(most distro packages are; a minimal build may not). An `.sf3` that can't
be decoded is skipped with a reason shown in the FluidSynth tab rather
than failing silently.

Two ways to supply a pack:

**Loose folder.** Drop a `*.sf2` or `*.sf3` into:

```paths
<SoH-config>/synth-packs/
```

Optionally include a sibling `<basename>.json` for per-instrument tuning
(e.g. `MyPack.sf2` + `MyPack.json`). Create the `synth-packs` folder next
to your `mods` folder.

**Mod archive.** Files inside any mounted `.o2r` archive at:

```paths
audio/synth/<pack-name>/soundfont.sf2   (or soundfont.sf3)
audio/synth/<pack-name>/mapping.json    (optional)
```

Both sources show up in the FluidSynth tab's pack list, tagged `[loose]`
or `[mod]`. Loose packs are listed after mod-supplied ones, both
alphabetised. **Rescan** re-enumerates without a restart, so a
freshly-dropped file becomes visible. Untick a pack to disable it; the
header shows `(N enabled / M discovered)`.

The float pipeline can run with no packs enabled (no timbre change) -
the synth controls below only matter once at least one pack is on.

### Stacking multiple packs

When more than one pack is enabled, FluidSynth searches the last-loaded
pack first for any `(bank, program)` lookup - like the wider mod stack,
**last loaded wins** on collisions. Per-pair pinning (see "Source state"
below) lets a specific pack win regardless of order: picking a preset
binds the row to the pack that owns it.

### Synth mode and volume

Two radio buttons at the top of the active-pack controls:

- **Authentic** - replaces the SF's default velocity / CC7 / CC11
  attenuation modulators with halved-amount versions (curve adapted from
  [ANMP](https://github.com/derselbst/ANMP)). Fixes NoteOn velocity at 100
  and routes the shaped value through CC11. Pairs with a console-era reverb
  preset. Default.
- **Enhanced** - stock SF modulators; sends the shaped value as NoteOn
  velocity so the SF author's own dynamics apply. Pairs with a subtle
  reverb. Use this with musically-curated banks (orchestral, SC-55,
  MuseScore) where the author's dynamics are what you want.

Just above the mode buttons is a **Volume** slider. It is **per mode** -
Authentic and Enhanced are calibrated to different baselines (Authentic's
reverb makes it the louder mode), so the slider edits whichever mode is
active. 100% is tuned to roughly match native loudness; the goal is
parity, not a boost. Drag live to hear it.

### Voice and channel gauges

Two readouts sit under the mode controls:

- **FluidSynth voices: A / B** - active voices held by FluidSynth out of
  its polyphony limit (default 256). As A approaches B, new NoteOns steal
  old voices and dense passages "cut". The text tiers grey -> amber ->
  red as you near the cap. If cuts line up with values well *below* the
  limit, the bottleneck is audio-thread CPU, not voices.
- **Synth channels: A / B (reclaims: R)** - distinct routed
  `(instrument)` pairs each claim one MIDI channel out of 64. At the cap
  the pool recycles a channel from a pair that has gone quiet; `reclaims`
  counts how often that happened. Sitting at 64 on a long session is
  normal.

### The per-instrument table

Each row is one engine instrument pair the audio engine has actually
played at least once. The row background tints **green** when it is
currently sounding through the synth, **blue** when sounding native.
Above the table: **Clear list** (forget discovered rows), **Reset all**
(drop personal overrides, restore the active pack's defaults; auto-saved),
and **Export pack mapping...** (modder publishing, see below).

Columns:

| Column   | What it does                                                                                  |
|----------|----------------------------------------------------------------------------------------------|
| Override | Session-only **Solo** (S) / **Mute** (M) buttons. Warm/red palette = *not saved*.            |
| Song     | Engine font name (e.g. "Main Theme"). Read-only.                                              |
| Sample   | Editable label - type a name like "Lush strings". Hint shows the auto-detected SF sample name; hover for the engine's Low/Mid/High samples. |
| Inst     | Engine slot id; `0 (Drum)` / `1 (SFX)` are special. Holds the **Split** / **L/M/H** / **As Drum** (and, for drum rows, **Slots**) buttons; hover for NoteOn stats. |
| Mode     | **Native** (engine synth) vs **Synth** (FluidSynth).                                          |
| Gain     | Per-instrument synth volume (0..4x).                                                          |
| Shift    | Pitch shift. Header has a **Semitone** checkbox to switch the column between octaves (+/-8) and semitones (+/-24). Auto-seeded from the engine sample's tuning when you pick a preset (so a substitute lands in the original octave); right-click to re-apply the suggested shift. |
| Preset   | Pick an SF preset from any loaded pack (filterable). Selecting also pins the row to that pack. |
| Adv      | Per-entry **Adv** popup: Reverb (CC91), Chorus (CC93), Cutoff (CC74), Q (CC71). Drag a slider below 0 to clear that override. |

**Override column (session-only).** Solo adds the row to the solo set;
while that set is non-empty, every non-soloed row is muted (solo multiple
rows to play them side by side). Mute silences just that row, on both the
engine and synth paths. Neither is saved. The **Clear** button in the
column header wipes all solo/mute state across the table without touching
persisted edits.

Everything **outside** the Override column auto-saves to
`<SoH-config>/fluidsynth_overrides.json` on every edit. There is no Save
button. To roll back persisted edits, use **Reset all** above the table.

### Note-range splits (melodic)

Many engine instruments load different samples at low / normal / high
pitch, so a single GM preset sounds right at one octave and wrong at
another. Split a melodic row to assign a different preset per range:

- **Split** (in the Inst column) bisects the row's current preset into
  two note ranges.
- **L/M/H** appears when the engine captured the instrument's own
  low/normal/high sample boundaries; it splits into exactly those three
  ranges, duplicating the current preset so each range can be reassigned.

A split row becomes a collapsible header (`N ranges`). Expand it to edit
each range independently: Native/Synth, Gain, **Shift** (per-range octave/
semitone), Preset, and Adv effects, plus per-range **Split** / **Merge**.
**Flatten** collapses everything back to one full-range entry.

Ranges are always contiguous and non-overlapping, so the boundary between
two ranges is the only editable value. The first range's low is pinned to
0 and the last range's high to 127 (both grayed); dragging any boundary
moves the neighbouring range's edge with it. **Merge** absorbs the next
(higher) range into this one.

### Drums

Engine drums live on instrument slot `0 (Drum)` (sound effects on
`1 (SFX)`). For these the engine's `semitone` byte is a *slot index*, not
a pitch, so they get their own collapsible tree-row:

- The parent row has a per-instrument **Native / Synth** master and a
  **Kit** dropdown (lists the bank-128 percussion presets from loaded
  packs). Picking a kit switches the instrument to Synth and applies the
  kit to its slots; "None (native)" switches it back.
- The **Slots (N)** button discovers drum slots - it creates one child
  entry per slot heard so far. Play the song first, then click it.
- Expand to get one child row per slot: per-slot Solo/Mute, Native/Synth,
  a **Drum Sound** dropdown (filterable GM percussion names), per-slot
  Gain, and Adv effects. Slot rows are editable only while the
  instrument master is Synth; Native greys them out but still lets you
  Solo/Mute to isolate a native drum.

### Treat a melodic instrument as a drum

Some songs play a percussion hit through a *melodic* instrument slot
rather than the drum bank. The **As Drum** button (Inst column, next to
Split / L/M/H) routes any melodic instrument through the drum path: each
distinct note it plays becomes a slot you map to a GM percussion sound.

Click **As Drum**, play the part so the notes are heard, then expand the
row and use **Slots (N)** to discover them - the row now behaves exactly
like a drum tree-row (Kit dropdown, per-slot Drum Sound, Solo/Mute). The
**Melodic** button in the Mode column reverts it to normal melodic
routing. Notes you haven't mapped keep playing the original instrument.

### Source state (pin behavior)

Picking a preset pins the row to the SF that owns it. The pin persists
through pack toggles; the Preset cell and its tooltip reflect the current
resolution:

- **Live**: the pinned pack is loaded and the preset is found - the row
  plays through that pack.
- **Drift**: the pinned pack is loaded, but a *different* pack now
  resolves the same `(bank, program)` first because the load order
  changed. Re-pick the preset to refresh.
- **Dead** / missing: the pinned pack is disabled or the preset is gone.
  The row plays native until the pack is re-enabled; your tuning waits on
  disk.

### Known limitations

A generic SoundFont can't fully match the original samples:

- The engine uses **per-key sample splits** with their own ADSR / pan /
  attenuation per range, plus baked filter and reverb shaping. A GM
  program plays one sample across the whole range with one envelope. The
  note-range split tools above help, but only so far.
- **Slot semantics are font-specific.** The same instrument index means
  different things in different engine fonts. Tuning is per-font work -
  there's no shared "bank 1 layout".
- A SoundFont *purpose-built* for the game's instrument layout is the
  realistic path to "better than native". Synth packs are how those
  community projects ship.

---

## For modders

### Pack layout

Loose form (for iterating during authoring - no zip step):

```paths
<SoH-config>/synth-packs/MyPack.sf3   (or .sf2)
<SoH-config>/synth-packs/MyPack.json  (optional, sibling)
```

Archive form (for distribution - what **Export .o2r** builds for you):

```paths
<MyPack.o2r>/
  audio/synth/MyPack/soundfont.sf3   (or .sf2)
  audio/synth/MyPack/mapping.json    (optional)
```

The `<pack-name>` segment is the user-visible identifier - a short ASCII
slug like `HD-Orchestra` or `FluidR3-Default`.

### Authoring loop

1. Drop your SF into the loose folder. The pack appears in the
   FluidSynth tab (hit **Rescan** if needed).
2. Enable just your pack, disable the rest. Open the game and play the
   songs you're tuning against (the sequence-preview tab works too).
3. Walk the per-instrument table. For each row:
   - Pick a preset from the **Preset** combo. The row is now pinned to
     your SF.
   - Tune Gain, Shift, and the Adv effect CCs by ear. Split melodic rows
     by range, and map drum slots, where the single-preset approach
     falls short.
   - Type a friendly label in **Sample** so the table reads coherently
     when you come back to it.
   - Use **Solo** to hear one voice; Mute to drop one out.
4. Every edit auto-saves to `fluidsynth_overrides.json`. When happy, use
   **Export pack...** to write a shippable mod.

### Export pack

The **Export pack...** button (above the table) opens a dialog prefilled
with the pack's name. It exports the pack's **effective mapping** - every
entry currently **enabled** (routing through synth) for that pack, whether
you hand-picked it or it came from the pack's own loaded `mapping.json`. It
strips the runtime-only flags (`enabled` / `selected`) and writes the pack
name **once** as a `pack_name` header (entries no longer repeat it). The
dialog previews the entry count before you commit. Two outputs:

- **Export .o2r** (recommended) - zips your soundfont and the mapping into
  a single shareable mod at:

  ```paths
  <SoH-config>/mods/<pack_name>.o2r
  ```

  with the files placed at `audio/synth/<pack_name>/soundfont.sf3` (or
  `.sf2`) and `.../mapping.json`. This is the artifact you upload. It
  loads on the **next launch** (mods are mounted at startup).

- **JSON only** - writes just the mapping beside your loose soundfont at:

  ```paths
  <SoH-config>/synth-packs/<pack_name>.json
  ```

  Picked up on **Rescan**, no restart needed - handy for iterating before
  you build the final `.o2r`.

The pack name must match the soundfont's name (`<pack_name>.sf3` loose, or
the `audio/synth/<pack_name>/` folder in the archive); that name is what
the loader uses to tie the mapping to the soundfont.

### `mapping.json` schema

Same shape as the user's `fluidsynth_overrides.json` (schema **version
2**). Your pack's mapping is a chain layer that overlays defaults; the
user's file overlays yours. Pack-shipped entries reload on every
pack-enable and are never written back to the user file.

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
  "drum_channels_synth": [
    { "fontId": 9, "instOrWave": 0 }
  ],
  "forced_drums": [
    { "fontId": 9, "instOrWave": 3 }
  ]
}
```

| Key             | Type               | Meaning                                                              |
|-----------------|--------------------|---------------------------------------------------------------------|
| `version`       | int                | Schema version. Use `2`.                                            |
| `pack_name`     | string             | Top-level header naming the pack (added by Export). This is the authoritative owner for every entry; the loader also derives it from the pack's file/folder name, so a rename is safe. |
| `fontId`        | int 0-63           | Engine sound-font index. Required. 0-37 are vanilla.               |
| `instOrWave`    | int 0-255          | Engine instrument slot. Required. `0` = drum bank, `1` = SFX bank.  |
| `pack`          | string             | Per-entry pack owner. **Optional / legacy** - `pack_name` (or the file name) supplies this now. Only read when there is no `pack_name` header. |
| `bank`          | int 0-255          | SF bank. Default 0 (GM melodic); 128 = GM drums.                   |
| `program`       | int 0-127          | SF program inside that bank.                                       |
| `preset_name`   | string             | Human-readable SF preset name. Metadata only - the UI uses it to detect drift. |
| `display_name`  | string             | Friendly label shown in the Sample column.                          |
| `gain`          | float              | Per-pair volume multiplier. Omit = 1.0x.                            |
| `transpose`     | int (semitones)    | Pitch shift. Omit = 0.                                              |
| `reverb`        | int 0-127          | CC91 reverb send. Omit to leave the channel default.               |
| `chorus`        | int 0-127          | CC93 chorus send.                                                  |
| `filter_cutoff` | int 0-127          | CC74 low-pass cutoff (64 = no shift from the SF default).         |
| `filter_q`      | int 0-127          | CC71 low-pass resonance (64 = no shift).                           |
| `note_low`      | int 0-127          | Low end of this entry's engine-semitone range. Omit = 0 (full range). |
| `note_high`     | int 0-127          | High end of the range. Omit = 127.                                 |
| `fixed_note`    | int 0-127          | Play this exact note instead of the engine pitch (drum slots / tuned percussion). Omit = derive from pitch. |
| `route`         | "synth" / "native" | Per-entry route. Omit = synth.                                     |

`drum_channels_synth` is a separate top-level array listing the
`(fontId, instOrWave)` drum channels whose per-instrument master is set
to Synth (absent = Native).

`forced_drums` is a separate top-level array listing the melodic pairs
(`instOrWave >= 2`) flagged **As Drum** - each plays through the drum path
with its notes mapped to GM percussion via per-slot `fixed_note` entries.

Omit any field you don't want to set - partial entries layer cleanly over
the chain below them. Multiple entries can share one `(fontId,
instOrWave)` pair: distinct `note_low` values become note-range splits or
drum slots.

### Resolution model

At play time, for each engine `(fontId, instOrWave)` pair:

1. Among the pair's entries, keep those that are enabled and resolvable
   (the pinned pack is loaded AND it actually has that `(bank, program)`).
2. Zero matches -> the row plays native.
3. For a split/drum pair, the entry whose `[note_low..note_high]` covers
   the incoming semitone wins; ranges are kept adjacent and
   non-overlapping.
4. On a plain collision, the entry from the **last-loaded** pack wins.

Pinning uses FluidSynth's `program_select`, so even if another loaded SF
has the same `(bank, program)`, your pack wins for the rows you pinned.
A dead entry (pack disabled / preset gone) stays on disk and springs back
when the source pack is re-enabled.

### Override chain (highest priority wins)

1. Reset-to-factory state
2. Built-in defaults (currently empty)
3. Pack `mapping.json` files, in enabled-pack order
4. User's `fluidsynth_overrides.json`

So a user's per-pair edit always wins over your pack's defaults, and a
later-loaded pack's mapping wins over an earlier one's. Users can
re-tune individual pairs to taste without touching your pack.

### Packaging

The easiest path is the **Export .o2r** button (see Export pack above) -
it writes a ready-to-share `mods/<pack_name>.o2r` with your soundfont and
mapping already at the right paths inside. Users drop that `.o2r` into
their own `<SoH-config>/mods/` and it loads on next launch.

To build one by hand instead, lay out the inner shape and zip it -
standard `zip` produces a valid `.o2r`:

```bash
cd my-pack/          # contains audio/synth/MyPack/{soundfont.sf3,mapping.json}
zip -r ../MyPack.o2r audio/
```

#### Shipping more than one soundfont

Each soundfont is its own pack (one `audio/synth/<pack>/` folder = one
enableable row). **Export .o2r** writes one pack per file, so the simple
way to ship two soundfonts is two `.o2r` files - export each pack, ship
both. They are independent: users can enable either or both.

If you'd rather hand-pack them into a single `.o2r`, put each in its own
folder and zip together - the archive name itself doesn't matter, only the
inner folders:

```paths
<MyMod.o2r>/
  audio/synth/SoundBankA/{soundfont.sf2, mapping.json}
  audio/synth/SoundBankB/{soundfont.sf3, mapping.json}
```

They still appear as two separate rows (SoundBankA, SoundBankB), not one
merged pack.

> **Loose + mod with the same name.** A loose `<name>.sf3` and a shipped
> `<name>.o2r` both show in the list (tagged `[loose]` and `[mod]`) and
> enable/disable independently. They do share the same internal pack name,
> so if you leave *both* enabled their mappings overlay and routing is
> ambiguous - keep only one of the two enabled. The usual flow is to author
> against the loose copy, then disable it once the `.o2r` is built.

### Caveats

- **New songs with custom engine fonts are not reliably supported.**
  Custom fonts (under `custom/fonts/`) are assigned a `fontId` by archive
  listing order, which is effectively arbitrary. Two mods with custom
  fonts can shift each other's `fontId` unpredictably, making any mapping
  against those IDs fragile. A new song that only references vanilla fonts
  (0-37) is not affected.
- Total fonts are capped at **64** (38 vanilla + 26 modded slots).
- **Sample names**: many engine slots have no captured sample name in the
  binary asset. The Sample column shows `(no sample name)` then - which
  is where `display_name` becomes especially useful for walking the
  table. Labels can be shipped in the mapping.

#### Engine quirks the substitution model can't paper over

These are limits of the SF-substitution approach, not bugs. A pack
author hits them as "this row refuses to behave".

- **Drum and SFX banks use slot indices, not pitches.** On `0 (Drum)`
  and `1 (SFX)` the engine's `semitone` byte selects a sample from a
  per-font drum/SFX table; it is not a chromatic pitch. The drum tree-row
  handles this: discover the slots, then map each to a GM percussion
  sound (or a tuned pitch). A plain melodic preset on these rows would
  play "right rhythm, wrong sound at random pitches", so use the per-slot
  Drum Sound mapping, not the melodic Preset combo. SFX is usually best
  left Native.

- **Some audible percussion lives on melodic-instrument rows.** A few
  songs author a drum as a regular melodic instrument whose sample slots
  hold drum hits (the engine pitch then has no relation to the sample's
  real fundamental). Mapping those to GM percussion cleanly is not yet
  first-class - for now, leave such a row Native or approximate it with a
  tuned preset.

- **Single-NoteOn polyphonic samples cannot be substituted.** A few
  instruments bake a chord into one sample - one engine NoteOn fires what
  sounds like a multi-voice stack. The substitution model fires one
  preset voice per engine event, so reproducing this needs either a
  custom SF preset whose single voice already contains the chord, or a
  sequence rewrite emitting N parallel NoteOns. Neither is in this tool's
  scope - leave the row Native.

---

## See also

- [derselbst/**ANMP**](https://github.com/derselbst/ANMP) - the closest
  analog: a FluidSynth-based player for sequenced console-era music.
  Originator of the halved-attenuation volume curve used by Authentic mode.
- [**ANMP**'s wiki](https://github.com/derselbst/ANMP/wiki), in particular
  its [Reproducing console OSTs
  accurately](https://github.com/derselbst/ANMP/wiki/Reproducing-N64-OSTs-accurately)
  page - reverb-preset research that informs the Authentic mode default.
