### Custom Music

We support importing custom [Seq64](https://github.com/sauraen/seq64) files to replace the in game music and fanfares (Sound effect and instrument replacement is currently not supported).

First you will need to prepare a folder with the desired sequences. Every sequence requires two files with the same name and different extensions - a `.seq` Seq64 file and a `.meta` plaintext file.

The `.meta` file requires two lines - the first line is the name that will be displayed in the SFX editor, the second line is the instrument set number in `base16` format, and the third line optionally contains 'bgm' or 'fanfare'. For example, if there is a sequence file `Foo.seq` then you need a meta file `Foo.meta` that could contain:
```
Awesome Name
C
```

Once you have prepared your sequences folder:
1. Download and open [Future] (https://github.com/louist103/future/). 
1. Choose the "Create OTR/O2R" option. 
1. Choose the "Custom Audio" option.
1. Choose the "Sequenced" option
1. Using the file selection screen, choose the sequences folder you prepared in the previous instructions.
1. Click the slider to pack an archive instead.
1. Select either OTR or O2R.
1. Click the 'Set Save Path' button to set the save location of the archive, if in create archive mode.
    (*NOTE:* 2Ship can handle 1024 custom sequence in total. This includes the original music. Keep that in mind!)
1. Click the "Pack Archive" button..
1. An archive will be created in the location selected with the 'Set Save Path' step. If that location wasn't the 'mods' folder, move the archive there. The mods folder must be in the same location as the game executable and mm.o2r.

Assuming you have done everything correctly, boot up 2Ship and select the SFX Editor from the enhancements dropdown menu. You should now be able to swap out any of the in game sequences/fanfares for the sequences added in your newly generated OTR file. If you have any trouble with this process, please reach out in the support section of the Discord.
