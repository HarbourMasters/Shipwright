# How to use the sample overrides feature

## Creating compatible samples
- There are three things you have to make sure for a sample to sound properly in the game:
  1. The sample must be mono. From my experience, stereo samples just don't work properly. If you have a stereo sample, you might wanna either merge the channels or just pick whichever channel sounds best.
  2. The sample rate must be 32000Hz. Anything else will cause issues and will not sound good.
  3. The sample must be transposed to the corresponding note in the original sample file. In order to do that, you will need access to the actual samples to hear how they sound and see what note each one is tuned to. That's a problem because the OoT soundfont cannot be shared for legal reasons, so you're on your own for this one at least until someone figures out a legitimate way to convert the samples from the OTR into actual wavefiles. In the meantime, I'll try to provide a mapping from the samples to the notes they correspond to.
- Also do keep in mind some of the original samples are kinda *loud*, so you might need to apply some gain to your sound file in order for it not to get drowned by the other instruments.

### Using polyphone to extract and tune your samples from a sonudfont of your choosing
- TODO: Explain how polyphone works (it's quite intuitive though and it's multiplatform, free software so just give it a try)

## Creating your assets override file
1. Make a file named `overrides.txt` inside your `mods` folder
2. On each line, include the internal OTR path for the sample to be overriden inside double quotes, followed by the path to the `.wav` sample in your filesystem inside double quotes, e.g.:
```
"audio/samples/Trumpet" "samples/trumpet.wav"
"audio/samples/Trombone" "samples/trombone.wav"
"audio/samples/Tuba" "samples/tuba.wav"
```
If you use a relative path, it will be relative to the directory in which your soh executable is located. If the file path supplied doesn't exist, your game will crash as you open it.
[Here's a list with all the sample names as seen in the OTR](https://gist.github.com/RaelCappra/77f4651343c61b1d6f6b662e6a87e434)

## Tips for making a good sample
TODO
