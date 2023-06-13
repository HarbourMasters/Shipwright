# Modding

> So you would like to create a code mod? _BUCKLE UP_

## Preface

Git is required to be installed. Knowing how to use git is going to help, I will list out commands that should set you on the right but without a general understanding you will likely get stuck if you deviate from the happy path.

General coding knowledge is also usually a requirement. You might be able to get by without, but the more knowledgeable the better, as it will allow you to find what you are looking for and troubleshoot much more easily.

## Making a fork in the road

Your first step is to fork the repository. To do this, you will need a github account. Assuming you have a github account you can navigate to the Shipwright repo [here](https://github.com/HarbourMasters/Shipwright) and press the `Fork` button in the top right of the screen. When that process is complete you should land on a page that looks similar to the original repo, but under your username (eg: `https://github.com/<GITHUB USERNAME>/Shipwright`).

On this page you should see a green `Code` button, click this and copy the URL within. (You may use the github desktop app here as well, but I will not provide instructions for it). Then in your terminal/command prompt you will git clone to a local development folder using the copied URL

```bash
cd <path to where you'll clone (eg, ~/Documents)>
git clone https://github.com/<GITHUB USERNAME>/Shipwright.git
```

At this point, I will now direct you to our [BUILDING](building.md) guide, it will have the most up to date documentation on getting the Ship running on your local machine. Once you have successfully built and launched the executable you may return here.

## Look at all those branches!

Congrats, if you have made it this far! Before we start making changes, you will need to create a new branch. It's recommended that you cut all your branches from the `develop` branch, as that tends to be the most up-to-date. Before cutting a branch make sure you are on the `develop` branch with the following command:

```bash
git checkout develop
```

Then cut your branch with the following command:

```bash
git checkout -b <BRANCH NAME>
```

You can name your branch whatever you want, but it's recommended to name it something that describes the feature you are working on. For example, if you are adding a new feature to the ship, you might name your branch `feature/ship-new-feature`. If you are fixing a bug, you might name your branch `bugfix/ship-bugfix`. If you are adding a new mod, you might name your branch `mod/ship-new-mod`. The important thing is to be descriptive and consistent.

## Making the changes

The limit is your imagination. You can add new features, fix bugs, add new mods, or even change the way the game works. We will demonstrate this by creating a mod that changes the speed of the day/night cycle.

Let's being by finding where the time is updated. Thankfully in the save editor we have a slider already hooked up to the time of day so we can check there for reference. The save editor file is at `soh/soh/Enhancements/debugger/debugSaveEditor.cpp`, if we do a quick search within that file for time we will find the following at line 400:

```cpp
const uint16_t dayTimeMin = 0;
const uint16_t dayTimeMax = 0xFFFF;
ImGui::SliderScalar("Time", ImGuiDataType_U16, &gSaveContext.dayTime, &dayTimeMin, &dayTimeMax);
```

So this tells us that `gSaveContext.dayTime` is what we're looking for. Let's now do a global search for this to see if we can find where it is updated. We find the following in `soh/src/code/z_kankyo.c` line 925:

```cpp
if (IS_DAY || gTimeIncrement >= 0x190) {
    gSaveContext.dayTime += gTimeIncrement;
} else {
    gSaveContext.dayTime += gTimeIncrement * 2; // time moves twice as fast at night
}
```

We can make a quick change to this code to verify this is indeed what we are looking for, lets multiply the the gTimeIncrement by 10:

```diff
if (IS_DAY || gTimeIncrement >= 0x190) {
-    gSaveContext.dayTime += gTimeIncrement;
+    gSaveContext.dayTime += gTimeIncrement * 10;
} else {
-    gSaveContext.dayTime += gTimeIncrement * 2; // time moves twice as fast at night
+    gSaveContext.dayTime += gTimeIncrement * 2 * 10; // time moves twice as fast at night
}
```

Rebuild the game and launch it, then load a save file. You should see that the time of day is now moving much faster. Terrific! While we could wrap this up and call it a day, we could make this user configurable by making a few more changes. I think a slider would be good for this, there's a slider in the cheat menu that we can use as a reference. Let's find it in `soh/soh/SohMenuBar.cpp` around line 1120:

```cpp
UIWidgets::EnhancementSliderFloat("Hookshot Reach Multiplier: %.1fx", "##gCheatHookshotReachMultiplier", "gCheatHookshotReachMultiplier", 1.0f, 5.0f, "", 1.0f, false);
```

The float values being passed in here are `minimum`, `maximum`, and `default` respectively. We'll make our minimum 0.2 to allow it to move slower, and our maximum 5.0 to allow it to move up to 5x faster. We'll also set the default to 1.0 so that it doesn't change the behavior by default. Copy this line and paste it below, then make the relevant changes:

```cpp
UIWidgets::EnhancementSliderFloat("Time Multiplier: %.1fx", "##gCheatTimeMultiplier", "gCheatTimeMultiplier", 0.2f, 5.0f, "", 1.0f, false);
```

Now we need to replace our hard coded values with the new variable. We can do this by replacing the `10` with a cvar call

```diff
if (IS_DAY || gTimeIncrement >= 0x190) {
-    gSaveContext.dayTime += gTimeIncrement * 10;
+    gSaveContext.dayTime += gTimeIncrement * CVarGetFloat("gCheatTimeMultiplier", 1.0f);
} else {
-    gSaveContext.dayTime += gTimeIncrement * 2 * 10;
+    gSaveContext.dayTime += gTimeIncrement * 2 * CVarGetFloat("gCheatTimeMultiplier", 1.0f);
}
```

After rebuilding and launching the game, you should now see a new slider in the cheat menu that allows you to change the speed of the day/night cycle. Nice!
If you're ever not sure about how something would be implemented, you can always look at external resources like the [Cloudmodding Wiki](https://wiki.cloudmodding.com/oot/Main_Page) to get a better perspective.
## Are you committed?

Now that we have made our changes, we need to commit them. First we need to add the files we changed to the staging area. We can do this with the following command:

```bash
git add .
```

This will add all the files we changed to the staging area. If you want to add specific files you can do so by replacing the `.` with the file path. For example, if we only wanted to add the `soh/soh/SohMenuBar.cpp` file we would do the following:

```bash
git add soh/soh/SohMenuBar.cpp
```

Now that we have added our files to the staging area, we need to commit them. We can do this with the following command:

```bash
git commit -m "Add time multiplier cheat"
```

You can verify everything was committed correctly by running the following command:

```bash
git status
```

Now push your changes to your fork with the following command:

```bash
git push origin <BRANCH NAME>
```

## Sharing the treasure

Now that you have made your changes, you can share them with the world! You can do this by creating a pull request to your own fork. You can navigate around in the Github UI to find this, or you can use the following replacing the relevant info:
```
https://github.com/<GITHUB USERNAME>/Shipwright/compare/develop...<BRANCH NAME>
```

From there you should see all of your changes listed, and a big green `Create pull request` button. You can fill out relevant information and a title and create the pull request. Once you have done this the CI will begin building distributables for your changes, and when they are ready they will be added to the bottom of your Pull request description! (See other PRs for examples)

Note: DO NOT MERGE THIS PULL REQUEST. You will want your fork's develop branch to stay in sync with the upstream develop branch. We will go over this in the next section, but all changes should stay on their own branches, with open PR's to continue generating distributables.

## Maintaining your fork'in mod

A month has passed, and new features have been added upstream that you want included in your distribution. You can do this by rebasing your branch on top of the upstream develop branch. You can do this with the following commands:

```bash
# This command will add the upstream repository as a remote, only needs to be done once
git remote add upstream https://github.com/HarbourMasters/Shipwright.git

git checkout develop
git pull upstream develop
git checkout <BRANCH NAME>
git pull origin develop --rebase
```

If you happen to run into merge conflicts, it is outside the scope of this tutorial to explain how to resolve them. If you want to abort the rebase you can run the following command:

```bash
git rebase --abort
```

Assuming all went well, you can now push your changes to your fork with the following command:

```bash
git push origin <BRANCH NAME> --force
```

## Combining multiple mods

You have been working on your mod for a while, and you want to combine it with another mod. You can do this by adding the other mod as a remote, and then merging it into your branch. You can do this with the following commands:

```bash
# This command will add the other repository as a remote, only needs to be done once
git remote add <MOD AUTHOR NAME> https://github.com/<MOD AUTHOR>/Shipwright.git

git checkout <YOUR BRANCH NAME>
git pull <MOD AUTHOR NAME> <MOD AUTHOR BRANCH NAME>
```

If you happen to run into merge conflicts, it is outside the scope of this tutorial to explain how to resolve them. If you want to abort the merge you can run the following command:

```bash
git merge --abort
```

Assuming all went well, you can now push your changes to your fork with the following command:

```bash
git push origin <YOUR BRANCH NAME>
```