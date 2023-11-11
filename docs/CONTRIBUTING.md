# Modding

> You'd like to contribute? That's great! Read here to learn how we recommend going about it.

## Preface

The information in [MODDING.md](https://github.com/HarbourMasters/Shipwright/blob/develop/docs/MODDING.md) is just as relevant to this. This document aims more at giving you best practices for code format, modification documentation, and repo/branch management. All of the information there will help with this aspect.

## Guiding principles

The main idea behind contributions that get merged into SoH's main repo is well-built, easy to maintain changes that most often modify, extend, or improve built-in functionality. Things like custom items, for example, conflict with vanilla, and thus should remain as custom builds. However, you never know what might be considered for merging, so feel free to PR, or you can talk to devs about your idea on the [Discord](https://discord.gg/shipofharkinian).

## Repo management

In addition to principles outlined in MODDING.md, the importance of maintaining clean copies of the main repos (develop, and develop-{release}) cannot be overstated. It is easier to create new branches and update existing ones with changes from SoH using branches on your fork that are always exact mirrors of the primary branches on HM64's repo. All changes should be committed to new branches on your own repo, and those branches form the base of PRs you then submit to HM64.

## Formatting

While we are still in the process of codifying code formats, whitespacing, etc, generally you can follow the precedents set around where you're making changes for information on how to indent lines, opening bracket positions, capitalization, variable naming, etc. Be specific, be as concisely elaborate/accurate as you can.  For example:

```c++
    if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0)) {
        this->frogSongTimer = sTimerFrogSong[index] * CVarGetInteger("gFrogsModifyFailTime", 1);
    } else {
        this->frogSongTimer = sTimerFrogSong[index];
    }
```

Here you see we open brackets on the same line, spacing outside the parentheses but not inside, same line else and opening bracket, etc. Also, all if...else blocks should be fully encapsulated with brackets, even if they're single-line blocks (which technically would be legal code).

## Documentation

We want to maintain the readability and integrity of vanilla codeflow as much as possible going forward. To do this, we have decided that we will want any modifications to vanilla code to be well notated and maintained separately from modified sections. Given this original code:

```c++
        this->frogSongTimer = 40;
        this->ocarinaNoteIndex = 0;
        func_8010BD58(play, OCARINA_ACTION_FROGS);
        this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
        EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
        this->actionFunc = EnFr_ContinueFrogSong;
```

If you wanted to modify this block with an enhancement to add difficulty options to the frog ocarina game, the first thing you would do is encapsulate it in a comment region as such:

```c++
        //Enhancement: Customize Frogs Ocarina Game
        if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0) == 1) {
            this->frogSongTimer = 40;
            this->ocarinaNoteIndex = 0;
            func_8010BD58(play, OCARINA_ACTION_FROGS);
            this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
            EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
            this->actionFunc = EnFr_ContinueFrogSong;
        } else { // vanilla code
            this->frogSongTimer = 40;
            this->ocarinaNoteIndex = 0;
            func_8010BD58(play, OCARINA_ACTION_FROGS);
            this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
            EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
            this->actionFunc = EnFr_ContinueFrogSong;
        }
        // Enhancement end
```

Incorporate as much of the section as is applicable in a block behind a CVar, with comment mentioning what type of addition it is (enhancement/fix/restoration, etc), label the else block as vanilla code, and put a closing comment at the end. Then, make your changes in the if block. In this instance, it ended up looking like this:

```c++
        if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0) == 1) {
            this->frogSongTimer = 40 * CVarGetInteger("gFrogsModifyFailTime", 1);
            if (CVarGetInteger("gInstantFrogsGameWin", 0) == 1) {
                this->actor.textId = 0x40AC;
                EnFr_SetupReward(this, play, false);
            } else {
                this->ocarinaNoteIndex = 0;
                func_8010BD58(play, OCARINA_ACTION_FROGS);
                this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
                EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
                this->actionFunc = EnFr_ContinueFrogSong;
            }
```

This does result in some code duplication, but this is just a step to where we want to eventually be where enhancement additions are encapsulated in source files housed elsewhere that are called from within the enhancement CVar check, and is more allowable in a situation like we're in, where preserving vanilla code is important to us. The final block ends up looking as such:

```c++
        //Enhancement: Customize Frogs Ocarina Game
        if (CVarGetInteger("gCustomizeFrogsOcarinaGame", 0) == 1) {
            this->frogSongTimer = 40 * CVarGetInteger("gFrogsModifyFailTime", 1);
            if (CVarGetInteger("gInstantFrogsGameWin", 0) == 1) {
                this->actor.textId = 0x40AC;
                EnFr_SetupReward(this, play, false);
            } else {
                this->ocarinaNoteIndex = 0;
                func_8010BD58(play, OCARINA_ACTION_FROGS);
                this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
                EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
                this->actionFunc = EnFr_ContinueFrogSong;
            }
        } else { // vanilla code
            this->frogSongTimer = 40;
            this->ocarinaNoteIndex = 0;
            func_8010BD58(play, OCARINA_ACTION_FROGS);
            this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
            EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
            this->actionFunc = EnFr_ContinueFrogSong;
        }
        // Enhancement end
```

In the case of alternative/replacement functions and variables, a simple comment mentioning the change it belongs to (Enhancement, Cheat, Debug, Randomizer Mechanic, etc) will suffice, as almost all the time, those replacements will be notated where they're used in the code following the above principles. The names of these replacements should also mirror the names of what they're replacing, with a short addition referring to the type of change.