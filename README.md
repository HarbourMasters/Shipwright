# Welcome to the Ship of Harkinian!

A PC port of OoT allowing you to enjoy the game with modern controls, enhanced resolution, and a bunch of new features! Setup is simple, let's get started!

# Requirements

The Ship of Harkinian cannot be played by itself. It needs assets from an OoT ROM to be able to run. To get the needed assets, we are going to use a tool that does all the work for you. To use it, you will need a few things.

### Here's what's needed to run the Ship of Harkinian:

- The latest version of the Ship of Harkinian: ([Available here](https://github.com/HarbourMasters/Shipwright/releases/latest))
- A ZRET OoT Debug ROM.

Build date: `zelda@srd022j 03-02-21 00:49:18`

sha1: `CEE6BC3C2A634B41728F2AF8DA54D9BF8CC14099`

The Ship of Harkinian uses a proprietary versioning system consisting of a sci-fi film name and a color. The motion-picture name represents a **Major** release version which increments with the addition of many new features and bug fixes. The color represents a **Minor** release version which increments with small updates mainly comprised of bug fixes.

## Extracting the Ship of Harkinian

The Ship of Harkinian tool will be packed in a `.zip` file. It is ***paramount*** that you extract that `.zip` file. We recommend extracting its contents somewhere safe, like in a folder on your Desktop.

Once that is done, we are going to run the OTRGUI tool, which will allow us to extract the needed assets from an OoT ROM. Doing this will allow us to play the Ship of Harkinian.

## Using the extraction tool

Launch `OTRGui.exe` by double-clicking it. Once the OTRGUI tool has launched, you will be met with a classy, yet easy-to-use interface.
There will be 2 buttons:

 - Open OoT Rom
 - Open Ship Folder

We will start by clicking on `Open OoT Rom`. After doing that, a window should open, asking you to open an OoT ROM. Simply navigate through your folders and find your ZRET OoT Debug ROM that you have obtained legitimately. Simply double-click on the file, and the prompt should close.

After that, click on `Open Ship Folder`. Another window will open, but this time, it will ask you to select a folder. Simply select the folder that you extracted from the Ship of Harkinian `.zip` file.

After that, a command-line window should open. Don't worry, this is perfectly normal. The tool will proceed to extract the assets from the ROM and compile them into a format the Ship of Harkinian can understand. 

The whole process can take up to **5 minutes**. Depending on your PC specs, this process might be shorter.

After that, a `Done!` message should show up. At that point, simply press on the `X` of the OTRGUI tool at the top-right.
If you get another message, then you might have selected a wrong ROM. Simply retry again with another ROM.

If you still cannot get the tool to work, join our [Discord Server](https://discord.com/invite/BtBmd55HVH) and ask for help in the `#support` text channel. Keep in mind that we do not condome piracy in any way.

## Running the Ship of Harkinian

Simply double-click on `soh.exe` to launch the game. If the window immediately closes, or if there are visual artifacts, you might have selected a wrong ROM in the OTRGUI tool. Simply retry again with another ROM. 

Congratulations, you are now sailing with the Ship of Harkinian! Have fun!

# Building the Ship of Harkinian

Please only follow those steps if you are a developer planning on helping with the project.

 1. Install [Python](https://www.python.org/ftp/python/3.10.2/python-3.10.2-amd64.exe)
 2. Install [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/community/)
 2b. In the Visual Studio Installer, install `MSVC v142 - VS 2019 C++`.
 4. Clone the Ship of Harkinian repository.
 5. Put your 2020 OoT Debug ROM in the `soh` folder.
 6. Launch `soh/fixbaserom.py`.
 7. Launch `soh/extract_baserom.py`.
 8. Copy the `baserom` folder from the `soh` folder into the `OTRExporter` folder.
 9. Run `OTRExporter/OTRExporter.sln`.
 10. Switch the solution to `Release x64`.
 11. Build the solution. 
 12. Launching `OTRExporter/extract_assets.py` will generate an `oot.otr` archive file in `OTRExporter/oot.otr`.
 13. Run `soh/soh.sln`
 14. Switch the solution to `Release x86`.
 15. Build the solution.
 16. Copy the `OTRExporter/oot.otr` archive file to `soh/Release`.
 17. Launch `soh.exe`.

# Troubleshooting the exporter
- Affirm that you have an `/assets` folder filled with XMLs in the same directory as otrgui.exe
- Affirm that `zapd.exe` exists in the `/assets/extractor` folder


# The harbour masters are...

    Kenix 			| Lead Developer/Public Relations - Resource Management Programmer, Audio System Programmer, and General Programmer
    Jack Walker 		| Lead Developer - OTR Format Programmer, Resource Load Programmer, and General Programmer
    Louist103 		| Developer - Save System Programmer and General Programmer
    Emil 			| Developer - Fast3D Programmer
    m4xw 			| Developer - Shipwright, Throwing Baguettes, and General Programmer
    MelonSpeedruns 		| Developer - General Programmer
    Rozlette 		| Developer - General Programmer
    JoshDuMan 		| Developer - General Programmer
    KiritoDev/Lywx 		| Developer - General Programmer
    Theo3 			| Developer - General Programmer
	Random06457  		| Developer - Linux Build

# Special Thanks

    Decomp & ZAPD 		| Made this project even possible in the first place!
    MNGoldenEagle 		| Patiently explained audio data formats, encouragement, and founding ZSO which was the first source of the game's code and resource format documentation.
    Rrrrry123 		| Speedbunner, encouragement, and community moderation
    Fierce deity 		| Encouragement and community moderation
    mzxrules		| For his contributions to decomp
    Aloxado 		| Developer - General Programmer
    MegaMech 		| Developer - General Programmer
	Revo 			| Tester - GCC support and General Testing
	zfg 			| Tester - General Testing
	Horseless Headman 	| Tester - General Testing
    Steven Pritchett        | Tester - General Testing
	Trenton May 		| Tester - General Testing
	Zeldaboy14 		| Tester - General Testing, encouragement, and community moderation
	Koby Howell 		| Tester - General Testing
	Logg 			| Tester - General Testing
	Taylor Daley		| Graphic Design
	Can't Sleep 		| Graphic Design
    MicTheMicrophone 	| Voice actor for the King
    Amphibibro		| Voice actor for Link
