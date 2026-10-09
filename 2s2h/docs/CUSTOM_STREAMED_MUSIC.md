
### Custom Audio
We support importing streamed (MP3, WAV, OGG, etc.) files to replace in game music, fanfares, sound effects, and instruments.

First you will need to prepare a folder with the desired songs. These can be `.wav`, `.ogg`, `.mp3`, and `.flac`. Place these files in an empty folder and change their name to what you want to appear in game. No `.meta` files are required for streamed songs.


Once you have prepared your sequences folder:
1. Download and open [Future] (https://github.com/louist103/future/). 
1. Choose the "Create OTR/O2R" option. 
1. Choose the "Custom Audio" option.
1. Choose the "Streamed" option
1. Using the file selection screen, choose the sequences folder you prepared in the previous instructions.
1. Click the slider to pack an archive instead.
1. Select either OTR or O2R.
1. Click the 'Set Save Path' button to set the save location of the archive, if in create archive mode.
    (*NOTE:* 2Ship can handle 1024 custom sequence in total. This includes the original music. Keep that in mind!)
1. Click the "Pack Archive" button..
1. An archive will be created in the location selected with the 'Set Save Path' step. If that location wasn't the 'mods' folder, move the archive there. The mods folder must be in the same location as the game executable and mm.o2r.

Replacing sound effects and instruments it a little more complicated.
Both of these store extra data in the first sound font, currently named `Soundfont_0`.
1. First open Soundfont_0. This file is an XML document.
2. Next find the sample or sound effect you want to replace.
3. Replace the `SampleRef` field with the path of the new audio file. 
4. Set the `Tuning` value using this calculation: $\dfrac{sample rate}{32000} * channels$. This is handled by future for songs.
5. Save the Soundfont XML and replace the one in the original archive.
6. Save the sample file in the archive, giving it the same path as the `SampleRef`

Assuming you have done everything correctly, boot up 2Ship and select the Audio Editor from the enhancements dropdown menu. You should now be able to swap out any of the in game sequences/fanfares for the sequences added in your newly generated OTR/O2R file. If you have any trouble with this process, please reach out in the support section of the Discord.
