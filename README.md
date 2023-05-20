![Ship of Harkinian](docs/shiptitle.darkmode.png#gh-dark-mode-only)
![Ship of Harkinian](docs/shiptitle.lightmode.png#gh-light-mode-only)

## Website

Official Website: https://www.shipofharkinian.com/

## Discord

Official Discord: https://discord.com/invite/shipofharkinian

If you're having any trouble after reading through this `README`, feel free ask for help in the Support text channels. Please keep in mind that we do not condone piracy.

# Quick Start

The Ship does not include any copyrighted assets.  You are required to provide a supported copy of the game.

### 1. Check your `sha1`
You can verify you have a supported copy of the game by checking the `sha1` hash. There are many ways to do this, one of the simplest is using the [`hasher-js`](https://github.com/snarfblam/hasher-js) webapp hosted at https://www.romhacking.net/hash/.

| Supported Version | `sha1` |
| - | - |
| PAL GC | `0227d7c0074f2d0ac935631990da8ec5914597b4` |
| PAL GC (Debug) | `cee6bc3c2a634b41728f2af8da54d9bf8cc14099` |
| PAL MQ (Debug) | `079b855b943d6ad8bd1eb026c0ed169ecbdac7da` |
| PAL MQ (Debug) | `50bebedad9e0f10746a52b07239e47fa6c284d03` | 

### 2. Download The Ship of Harkinian from [Discord](https://discord.com/invite/shipofharkinian)
The latest release is available in the most recent post in the `#downloads` channel.

### 3. Launch the Game!
#### Windows
* Extract the zip
* Launch `soh.exe`

#### Linux
* Place your supported copy of the game in the same folder as the appimage.
* Execute `soh.appimage`.  You may have to `chmod +x` the appimage via terminal.

#### macOS
* Run `soh.app`. When prompted, select your supported copy of the game.
* You should see a notification saying `Processing OTR`, then, once the process is complete, you should get a notification saying `OTR Successfully Generated`, then the game should start.

#### Nintendo Switch
* Run one of the PC releases to generate an `oot.otr` and/or `oot-mq.otr` file. After launching the game on PC, you will be able to find these files in the same directory as `soh.exe` or `soh.appimage`. On macOS, these files can be found in `/Users/<username>/Library/Application Support/com.shipofharkinian.soh/`
* Copy the files to your sd card
```
sdcard
└── switch
    └── soh
        ├── oot-mq.otr
        ├── oot.otr
        ├── soh.nro
        └── soh.otr
```
* Launch via Atmosphere's `Game+R` launcher method.

### 4. Play!

Congratulations, you are now sailing with the Ship of Harkinian! Have fun!

# Configuration

### Default keyboard configuration
| N64 | A | B | Z | Start | Analog stick | C buttons | D-Pad |
| - | - | - | - | - | - | - | - |
| Keyboard | X | C | Z | Space | WASD | Arrow keys | TFGH |

### Other shortcuts
| Keys | Action |
| - | - |
| F1 | Toggle menubar |
| F5 | Save state |
| F6 | Change state |
| F7 | Load state |
| F9 | Toggle Text-to-Speech (Windows and Mac only) |
| F10 | Fullscreen (OpenGL) |
| Tab | Toggle Alternate assets |
| Alt+Enter | Fullscreen (DirectX) |
| Ctrl+R | Reset |

### Graphics Backends
Currently, there are three rendering APIs supported: DirectX11 (Windows), OpenGL (all platforms), and Metal (MacOS). You can change which API to use in the `Settings` menu of the menubar, which requires a restart.  If you're having an issue with crashing, you can change the API in the `shipofharkinian.json` file by finding the line `gfxbackend:""` and changing the value to `sdl` for OpenGL. DirectX 11 is the default on Windows.

# Custom Assets

Custom assets are packed in `.otr` files. To use custom assets, place them in the `mods` folder.

If you're interested in creating and/or packing your own custom asset `.otr` files, check out [**retro**](https://github.com/HarbourMasters64/retro)!

# Development
### Building

If you want to manually compile SoH, please consult the [building instructions](docs/BUILDING.md).

### Playtesting
If you want to playtest a continuous integration build, you can find them at the links below. Keep in mind that these are for playtesting only, and you will likely encounter bugs and possibly crashes. 

* [Windows](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-windows.zip)
* [macOS](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-mac.zip)
* [Linux (performance)](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux-performance.zip) _(requires `glibc 2.35` or newer, but will be more performant than the compatibility build.)_
* [Linux (compatibility)](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux-compatiblity.zip) _(compatible with most Linux distributions, but may not be as performant as the performance build.)_
* [Switch](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-switch.zip)

<a href="https://github.com/Kenix3/libultraship/">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="./docs/poweredbylus.darkmode.png">
    <img alt="Powered by libultraship" src="./docs/poweredbylus.lightmode.png">
  </picture>
</a>
