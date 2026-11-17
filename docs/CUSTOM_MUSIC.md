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
    - This `mods` folder should be in the same folder as your `oot.o2r` file.

Assuming you have done everything correctly, boot up SoH and select the SFX Editor from the enhancements dropdown menu. You should now be able to swap out any of the in game sequences/fanfares for the sequences added in your newly generated OTR file. If you have any trouble with this process, please reach out in the support section of the Discord.

### Streamed audio

Recorded songs (wav, ogg, mp3, flac) are packed with [future](https://github.com/louist103/future)'s streamed audio window instead, which transcodes them to opus and writes one `custom/music/<name>_SEQ.xml` per song (`_SEQ.xml_fanfare` for fanfares).

#### Fades

A streamed song is one long note, so by default the game's cuts stop it dead. Two optional attributes on the `<Sequence>` element of its `_SEQ.xml` smooth this out, in seconds (decimals allowed, 0 to 60, default 0):

```xml
<Sequence Index="0" Medium="Cart" CachePolicy="Temporary" Size="0" Streamed="true" FadeIn="1.5" FadeOut="3" Length="209" Looped="true" Stereo="true">
```

- `FadeIn` - used when the game starts the song without a fade of its own.
- `FadeOut` - used when the game cuts the song, or switches to another song without a fade. When a sequence player is free the two songs crossfade; otherwise the next song waits for the fade to finish.

Fades the game asks for itself are kept as they are. The attributes are ignored on non-streamed sequences and by older SoH versions.
