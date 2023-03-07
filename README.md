# Welcome to the Ship of Harkinian!

A PC port of OoT allowing you to enjoy the game with modern controls, widescreen, high-resolution, gyroscopy and other great features! Setup is simple, let's get started!

The Ship does not include any copyrighted assets.  You are required to provide a supported copy of the game.

## Discord

Official Discord: https://discord.com/invite/BtBmd55HVH

## Quick Start (Windows)

1) Download The Ship of Harkinian from [Discord](https://discord.com/invite/BtBmd55HVH).
2) Requires a supported copy of the game (See supported games below).
3) Use `OTRGui.exe` to generate an `oot.otr` archive file.
4) Launch `soh.exe`

### Supported Games
#### Ocarina of Time Debug PAL GC (not Master Quest)
```
Build team: `zelda@srd022j`
Build date: `03-02-21 00:49:18` (year-month-day)
sha1: cee6bc3c2a634b41728f2af8da54d9bf8cc14099
```
#### Ocarina of Time PAL GameCube
```
sha1: 0227d7c0074f2d0ac935631990da8ec5914597b4
```
#### Ocarina of Time Debug PAL GC MQ (Dungeons will be Master Quest)
```
Build team: `zelda@srd022j`
Build date: `03-02-21 00:16:31` (year-month-day)
sha1: 079b855b943d6ad8bd1eb026c0ed169ecbdac7da (Produced by decomp)
sha1: 50bebedad9e0f10746a52b07239e47fa6c284d03 (Alternate)
```

Congratulations, you are now sailing with the Ship of Harkinian! Have fun!

## About Versions

The Ship of Harkinian's versioning system is a unique system, comprising of a three-numbered version (`x.y.z`) and a version codename.  The main version number `x` signifies major feature updates that require a new `oot.otr` file.  The version number `y` signifies major feature updates and bugfixes that do not require a new `oot.otr` file.  The version number `z` signifies minor updates such as a collection of bugfixes.

The version codename consists of a sci-fi film character followed by a phonetic alphabet code word. The film character represents a major release version which increments with the major `x` or `y` version bumps. The code word represents a minor release version which increments with the minor `z` version bump.

## Additional Setup Information

### Windows Rom Extraction

* Open `OTRGui.exe` and click on "Open OOT ROM", then select your supported copy of the game (see section "Supported Games").
* If a second button already exits then `oot.otr` already exists. To prevent overwriting the old `oot.otr` use this button to choose a new game directory. The new directory must not already contain an `oot.otr` to prevent an error.
* When the process completes, you should have an `oot.otr` or `oot-mq.otr` file in your main directory, depending on the version of the game used to build it.
    (*NOTE:* You can have both `.otr` files, if you want to! This even allows you to shuffle MQ and vanilla dungeons in the randomizer)

This process can take up to several minutes, depending on your system hardware.

Close `OTRGui.exe` when the `Done!` message appears.
If you get another message, then you might have selected the wrong rom. See section "Supported Games".

### Linux Rom Extraction

* Place one of the supported roms in the same folder as the appimage (See section "Supported Games").
* Execute `soh.appimage`.  You may have to `chmod +x` the appimage via terminal.
* When the process completes, you should have an `oot.otr` or `oot-mq.otr` file in the same directory as the appimage, and the game should start automatically.
    (*NOTE:* You can provide both a vanilla and MQ ROM, if you want to! This even allows you to shuffle MQ and vanilla dungeons in the randomizer)

This process can take up to several minutes, depending on your system hardware.

If you get any errors, then you might have selected the wrong rom. See section "Supported Games".

### MacOS Rom Extraction

* Run `soh.app`, and when prompted, select one of the supported roms listed above.
* You should see a notification saying `Processing OTR`, then, once the process is complete, you should get a notification saying `OTR Successfully Generated`, then the game should start.

This process can take up to several minutes, depending on your system hardware.

If you get an error saying `Incompatible ROM hash`, you have selected the wrong rom. See section "Supported Games".

### Nintendo Switch Rom Extraction

* Download the latest PC release of the Ship of Harkinian, and follow the instructions above for generating the `oot.otr` archive on that platform.
* Place the `.nro` and the `oot.otr` archive into a folder called `soh` in your Switch folder on your Switch

### Nintendo Wii U Rom Extraction

* Download the latest PC release of the Ship of Harkinian, and follow the instructions above for generating the `oot.otr` archive on that platform.
* Copy the `.rpx` and the `oot.otr` archive to `wiiu/apps/soh`

---

If you're having any trouble, feel free to join our [Discord Server](https://discord.com/invite/BtBmd55HVH) and ask for help in the Support text channels. Please keep in mind that we do not condone piracy.

### Running The Ship of Harkinian

For Windows, launch `soh.exe`.  For Linux, execute the appimage. For MacOS, launch the app. For Switch, launch the app via Atmosphere's `Game+R` launcher method. For Wii U, launch the `.rpx` file through the homebrew Mii Maker exploit.

Default keyboard configuration:
| N64 | A | B | Z | Start | Analog stick | C buttons | D-Pad |
| - | - | - | - | - | - | - | - |
| Keyboard | X | C | Z | Space | WASD | Arrow keys | TFGH |

Other shortcuts:
| Keys | Action |
| - | - |
| F1 | Toggle menubar |
| F5 | Save state |
| F6 | Change state |
| F7 | Load state |
| F10 | Fullscreen (OpenGL) |
| Alt+Enter | Fullscreen (DirectX) |
| Ctrl+R | Reset |

Currently, there are three rendering APIs supported: DirectX11 (Windows), OpenGL (all platforms), and Metal (MacOS). You can change which API to use in the `Settings` menu of the menubar, which requires a restart.  If you're having an issue with crashing, you can change the API in the `shipofharkinian.json` file by finding the line `gfxbackend:""` and changing the value to `sdl` for OpenGL. DirectX 11 is the default on Windows.

### Custom Music

We support importing custom [Seq64](https://github.com/sauraen/seq64) files to replace the in game music and fanfares (Sound effect and instrument replacement is currently not supported).

First you will need to prepare a folder with the desired sequences. Every sequence requires two files with the same name and different extensions - a `.seq` Seq64 file and a `.meta` plaintext file. These files can be categorically nested in folders if desired, - Retro will recursively search each subfolder it finds.

The `.meta` file requires two lines - the first line is the name that will be displayed in the SFX editor, and the second line is the instrument set number in `base16` format. For example, if there is a sequence file `Foo.seq` then you need a meta file `Foo.meta` that could contain:
```
Awesome Name
C
```

Once you have prepared your sequences folder:
1. Download and open [Retro](https://github.com/HarbourMasters/retro/releases). 
1. Choose the "Create OTR" option. 
1. Choose the "Custom Sequences" option.
1. Using the file selection screen, choose the sequences folder you prepared in the previous instructions.
1. Click the "Stage Files" button.
    (*NOTE:* SoH can handle 1024 custom sequence in total. This includes the original music. Keep that in mind!)
1. Click the "Finalize OTR" text in the green footer.
1. Click "Generate OTR" and, using the file selection screen, save the file to the `mods` folder of your SoH installation.
    - This `mods` folder should be in the same folder as your `oot.otr` file.

Assuming you have done everything correctly, boot up SoH and select the SFX Editor from the enhancements dropdown menu. You should now be able to swap out any of the in game sequences/fanfares for the sequences added in your newly generated OTR file. If you have any trouble with this process, please reach out in the support section of the Discord.

## Building The Ship of Harkinian

If you want to manually compile SoH, please consult the [building instructions](BUILDING.md).

## Nightly Builds
If you want to playtest a nightly build, you can find them at the links below. However, keep in mind that these are for playtesting only, and you will likely encounter bugs.  Rarely, these builds will crash.

Nightly builds of Ship of Harkinian are available here: [Windows](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-windows.zip), [macOS](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-mac.zip), [Linux (compatibility*)](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux-compatiblity.zip), [Linux (performance*)](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux-performance.zip), [Switch](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-switch.zip), [Wii U](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-wiiu.zip)

_*compatibility: compatible with most Linux distributions, but may not be as performant as the perf build._\
_*performance: requires `glibc 2.35` or newer, but will be more performant than the compat build._

## Take The Survey
Want to use cartridge readers in tandem with OTRGui?
Take [this survey](https://retroarchopenhardware.com/survey.php) to increase chances of this becoming reality.

## The Harbour Masters Are...

    Kenix | Lead Developer/Public Relations - Resource Management Programmer, Audio System Programmer, and General Programmer
    Jack Walker | Lead Developer - OTR Format Programmer, Resource Load Programmer, and General Programmer
    Louist103 | Developer - Save System Programmer and General Programmer
    Emil | Developer - Fast3D Programmer
    m4xw | Developer - Shipwright, Throwing Baguettes, and General Programmer
    MelonSpeedruns | Developer - General Programmer
    Rozlette | Developer - General Programmer
    JoshDuMan | Developer - General Programmer
    KiritoDev/Lywx | Developer - General Programmer
    Theo3 | Developer - General Programmer
	Random06457 | Developer - Linux Build

## Special Thanks

    Decomp & ZAPD | Made this project even possible in the first place!
    MNGoldenEagle | Patiently explained audio data formats, encouragement, and founding ZSO which was the first source of the game's code and resource format documentation.
    Rrrrry123 | Speedbunner, encouragement, and community moderation
    Fierce deity | Encouragement and community moderation
    mzxrules | For his contributions to decomp
    zel. | For his contributions to decomp
    Aloxado | Developer - General Programmer
    MegaMech | Developer - General Programmer
	Revo | Tester - GCC support and General Testing
	zfg | Tester - General Testing
	Horseless Headman | Tester - General Testing
    Steven Pritchett | Tester - General Testing
	Trenton May | Tester - General Testing
	Zeldaboy14 | Tester - General Testing, encouragement, and community moderation
	Koby Howell | Tester - General Testing
	Logg | Tester - General Testing
	Taylor Daley | Graphic Design
	Can't Sleep | Graphic Design
	
## Video Credits
    Kenix | Producer / Writer
    briaguya | Writer
    rainbow_fash | Executive Producer
    ReveriePass | Editor
    MicTheMicrophone | Gwonam / The King
    Amphibibro | Link
    AceHeart | Zelda
