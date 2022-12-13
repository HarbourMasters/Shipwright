# Welcome to the Ship of Harkinian!

A PC port of OoT allowing you to enjoy the game with modern controls, widescreen, high-resolution, gyroscopy and other great features! Setup is simple, let's get started!

The Ship does not include assets and as such requires a prior copy of the game to play.

## Quick Start (Windows)

1) Download The Ship of Harkinian from [Discord](https://discord.com/invite/BtBmd55HVH).
2) Requires a supported copy of the game (See supported games below).
3) Use the OTRGui to generate an `oot.otr` archive file.
4) Launch `soh.exe`

### Supported Games
#### Ocarina of Time Debug PAL GC (not Master Quest)
> Currently the recommended option
```
Build team: `zelda@srd022j`
Build date: `03-02-21 00:49:18` (year-month-day)
sha1: cee6bc3c2a634b41728f2af8da54d9bf8cc14099
```
#### Ocarina of Time PAL GameCube
> May lead to crashes and instability
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

## Additional Setup Information

The Ship of Harkinian uses a proprietary versioning system consisting of a sci-fi film character followed by a phonetic alphabet code word. The film character represents a major release version which increments with the addition of many new features and bug fixes. The code word represents a minor release version which increments with small updates mainly comprised of bug fixes. For example, `DECKARD ALFA`.

### Windows Rom Extraction

* Open OTRGui.exe, and select one of the supported roms listed above, to generate the `oot.otr` archive file.
* If a second button already exits then `oot.otr` already exists. To prevent overwriting the old `oot.otr` use this button to choose a new game directory. The new directory must not already contain an `oot.otr` to prevent an error.
* When the process completes, place `oot.otr` beside `soh.exe` if it is not already.

This packaging process can take up to **5 minutes**.

Close the OTRGui when the `Done!` message appears.
If you get another message, then you might have selected the wrong rom. Make sure to use a rom consistent with the above checksum.

### Linux Rom Extraction

* Place one of the supported roms in the same folder as the appimage.
* When you run the soh appimage, it should begin generating the `oot.otr` archive file.
* When the process completes, place `oot.otr` in the same folder as the appimage, if it is not already, then run the appimage.

The packaging process can take up to **5 minutes**.

If you get any errors, then you might have selected the wrong rom. Make sure to use a rom consistent with the above checksum.

### MacOS Rom Extraction

* Run `soh.app`, and when prompted, select one of the supported roms listed above.
* You should see a notification saying `Processing OTR`, then, once the process is complete, you should get a notification saying `OTR Successfully Generated`, then the game should start.

The packing process can take up to **5 minutes**.

If you get an error saying `Incompatible ROM hash`, you have selected the wrong rom, make sure the checksum matches one of the ones listed above.

### Nintendo Switch Rom Extraction

* Download the latest PC release of the Ship of Harkinian, and follow the instructions above for generating the `oot.otr` archive on that platform.
* Place the `.nro` and the `oot.otr` archive into a folder called `soh` in your Switch folder on your Switch

### Nintendo Wii U Rom Extraction

* Download the latest PC release of the Ship of Harkinian, and follow the instructions above for generating the `oot.otr` archive on that platform.
* Copy the `.rpx` and the `oot.otr` archive to `wiiu/apps/soh`

---

If you still cannot get the tool to work, join our [Discord Server](https://discord.com/invite/BtBmd55HVH) and ask for help in the `#support` text channel. Keep-in-mind that we do not condone piracy in any way.

### Running The Ship of Harkinian

Launch the game. If the window immediately closes, or if there are visual artifacts, you may have selected the wrong rom in the OTRGui tool.

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

Currently, DirectX 11 and OpenGL are supported. Change the renderer by opening the `shipofharkinian.json` configuration file in notepad and add `sdl` to the quotes in `"GfxBackend": ""` for OpenGL or leave blank for DirectX.

### Custom Music

We support importing custom [Seq64](https://github.com/sauraen/seq64) files to replace the in game music and fanfares (Not sound effects yet currently).

First you will need to prepare a folder with the desired sequences, in which every sequence will have two files with the same name and different extensions, a `.seq` Seq64 file and a `.meta` plaintext file. These files can be categorically nested in folders if desired, they will still be consumed as if they were all at the top level.

The meta file will have two lines, the first line is the name that will be displayed in the SFX editor, and the second line will be the instrument set number, in base 16. For example, if there is a sequence file `Foo.seq` then you need a meta file `Foo.meta` that could contain:
```
Awesome Name
C
```

Once you have prepared your sequences folder:
1. Download and open [Retro](https://github.com/HarbourMasters/retro/releases). 
1. Choose the "Create OTR" option 
1. Choose the "Custom Sequences" option
1. Using the file select, choose the sequences folder you prepared in the previous instructions.
1. Click the "Stage Files" button (Reminder: SoH can handle 1024 custom sequence in total, this number include the count of the original music with it so be sure to take this into account)
1. Click the "Finalize OTR" text in the green footer.
1. Finally click "Generate OTR" and for the source destination it needs to go in the `mods` folder of your SoH installation.
    - This `mods` folder should be a sibling of your `oot.otr` file.

Assuming you have done everything correctly, boot up SoH and open up the SFX Editor (In the Enhancements dropdown). You should now be able to swap out any of the in game sequences/fanfares for the sequences added in your newly generated OTR file. If you have any trouble with this process please reach out in the support section of the Discord

## Take The Survey
Want to use cartridge readers in tandem with the OTRGui?
Take [this survey](https://retroarchopenhardware.com/survey.php) to increase chances of this becoming reality.

## Discord

Official Discord: https://discord.com/invite/BtBmd55HVH

## Building The Ship of Harkinian

Refer to the [building instructions](BUILDING.md) to compile SoH.

## Getting CI to work on your fork

The CI works via [Github Actions](https://github.com/features/actions) where we mostly make use of machines hosted by Github; except for the very first step of the CI process called "Extract assets". This steps extracts assets from the game file and generates an "assets" folder in `soh/`.

To get this step working on your fork, you'll need to add a machine to your own repository as a self-hosted runner via "Settings > Actions > Runners" in your repository settings. If you're on macOS or Linux take a look at `macports-deps.txt` or `apt-deps.txt` to see the dependencies expected to be on your machine. For Windows, deps get installed as part of the CI process. To setup your runner as a service read the docs [here](https://docs.github.com/en/actions/hosting-your-own-runners/configuring-the-self-hosted-runner-application-as-a-service?platform=linux).


## Troubleshooting The Exporter
- Confirm that you have an `/assets` folder filled with XMLs in the same directory as OTRGui.exe
- Confirm that `zapd.exe` exists in the `/assets/extractor` folder

## Nightly Builds
Nightly builds of Ship of Harkinian are available here: [Windows](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-windows.zip), [macOS](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-mac.zip), [Linux](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-linux.zip), [Switch](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-switch.zip), [Wii U](https://nightly.link/HarbourMasters/Shipwright/workflows/generate-builds/develop/soh-wiiu.zip)


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
