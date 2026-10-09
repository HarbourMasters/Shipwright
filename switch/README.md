# Ship of Harkinian 9.3.0 (Dewey Alpha) for Nintendo Switch

Unofficial port. HarbourMasters stopped shipping Switch builds after 8.0.6; this branch restores the
Switch platform layer on top of 9.3.0. Requires a Switch running custom firmware (Atmosphère) and hbmenu.

## Install

1. Unzip this package onto the root of your SD card. It creates `/switch/soh/` containing
   `soh.nro` and `soh.o2r`.
2. Generate the game archive on a PC. The Switch can't do this itself:
   - Download the **9.3.0 Dewey Alpha** PC release (Windows, Linux or macOS) and run it.
   - Point it at your own legally dumped Ocarina of Time ROM when it asks.
     Check that your dump is supported at https://ship.equipment/.
   - It creates `oot.o2r` (and `oot-mq.o2r` for Master Quest) next to the PC executable.
3. Copy `oot.o2r` / `oot-mq.o2r` into `/switch/soh/` on the SD card.
4. Launch hbmenu in **full-memory (title takeover) mode**: hold **R** while starting any game, then
   pick Ship of Harkinian. Album/applet mode does not have enough memory and the game will say so.

Archives must come from the same SoH version as the `.nro`. If you update, regenerate them on the PC.
Mods go in `/switch/soh/mods/`.

## Performance

- **Settings > General > Nintendo Switch > CPU Performance Profile** sets the CPU clock. It defaults to
  Maximum (1785 MHz, the clock the Switch itself uses on loading screens). Clocks drop back to stock on
  the HOME menu and when the game closes. Pick a lower profile to save battery in handheld.
- The game renders at 1080p docked and 720p handheld and switches automatically when you dock.
- Frame interpolation above 30 FPS is the most expensive setting on this hardware. If you see drops,
  lower the interpolated FPS first, and keep MSAA off.
- Controller navigation of the menu is always on. Menu scale defaults to "Large"; change it under
  Settings > General if you play on a TV.

## Known limitations

- ROM extraction isn't available on the console (see step 2).
- Online play (Anchor) uses the console's network connection; it needs internet access from homebrew.
