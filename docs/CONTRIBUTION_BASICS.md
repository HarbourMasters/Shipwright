# Contributing Basics

This guide is made for absolute beginners in software development to guide them through setting up a fork, making basic changes and PRing it to the main repo. MMore experianced users will likely want to skip this guide and check BUILDING.md instead for more direct instructions.

## Creating and setting up your fork.

1. If you have not already, create a Github account.

2. On the [Shipwright main github page](https://github.com/HarbourMasters/Shipwright), click Fork, and create a fork with you as the owner. When you want to make changes, you will first make them on your fork, which you can edit freely, and then use a Pull Request to submit those changes for review before possibly being merged.

3. If you have not already, download and install git. It is likely already installed on linux systems, so this step will focus on the [windows installer](https://git-scm.com/). Most of the settings should be left as defaults, however I recommend at least changing the following:
- When prompted to select a text editor, select one you are comfortable with. If you are not sure, [Notepad++](https://notepad-plus-plus.org/) is a common choice among developers, but will need to be downloaded separately.
- Setting pull behaviour to `Rebase` helps avoid unnecessary commits on your tree, which can cause conflicts and other issues in the future. If you expect to collaborate with others on your fork, `fast-forward-only` may help avoid surprises, but may also complicate the pulling process.
- While it is correctly set by default, always set line endings to either `checkout Windows-style, commit Unix-style` or `Checkout as-is, commit Unix-style`. Some windows word processors may struggle with the latter, but development tools should be fine. This is needed to maintain consistency in development.

4. (Optional) Windows users not comfortable with the command line may wish to download a more visual git client. The common reccomendation for this is [TortoiseGit](https://tortoisegit.org/). This program adds commands to the right click menu of folders that bring up GUIs for various git commands, while being more powerful and flexible than Github Desktop. Be sure to install the dependencies link to by the download page. this guide will include instructions for both the Command Line (which can be accessed on windows via Git Bash, which can also be accessed via the right-click menu if git is installed from the above link.) or with TortoiseGit.

5. Find your fork's .git url by going to your fork's github pag and clicking the green `Code` button, and clone it. to do this...
- On Command line: `git clone <url> --recursive`. If you forget the --recusive, enter `git submodule update --init` to download submodules manually.
- With TortoiseGit: RightClick _inside_ the folder you wish to place ship's development files andclick git clone. Enter the .git url of your fork into the URL field of the new window, tick "Recursive", and click OK. If you forget to click Recursive, right click the newly created folder and select `TortoiseGit` > `Submodule Update...` and click OK on the popup.

6. (Optional) To develop using Visual Studio you need to use cmake to generate the solution file:
```powershell
# Generates Ship.sln at `build' for Visual Studio 2022
& cmake -S . -B "build" -G "Visual Studio 17 2022" -A x64
```

Now you have the files, Proceed to [BUILDING.md](https://github.com/HarbourMasters/Shipwright/blob/develop/docs/BUILDING.md) to build the initial state of Ship.

## Updating your fork

1. In order to update your git, you have to first the main repo as a remote. You will need the .git url of the main ship repo, obtained similarly to your fork in Building Step 5.
- On Command line: git remote add <name> <url>
- with TortoiseGit: Rightclick the Shipwright folder, click git sync, then `Manage` next to the Remote URL box. then enter the name you wish to use to refer to it in the Name box and the .git url of the main repo in URL, then click `Add New/Save`. You should not need to disable tags for the main repo.

Step 1 only needs performing once, on subsquent updates you can skip to step 2.

2. Fetch the Main Repo's data, then rebase it onto the current branch.
- On Command line: `git fetch --all`, then `git rebase <name>/<branch>`
- with TortoiseGit: Rightclick the Shipwright folder, click `git fetch`, choose the Main Repo under remote, then OK. Then right click again, select TortoiseGit > 'Rebase', select the main repo's version of your current branch (visible in the left dropdown), and click `Start Rebase`. If you selected the correct branches, it should say fastforward.

If you are on a feature branch which has some of your own changes, you will need to merge instead of rebase. The process is the same, except you use merge instead of rebase, and TortoiseGit has a different UI. You may also get a conflict, read below for a quick guide to solving conflicts.

3. You can then push your updated fork to github:
- On Command line: `git push`
- with TortoiseGit: In the right-click menu, click push, then assuming you want to push to the same branch you pulled from, click OK.

## Setting up VS Cod(ium) for development

This guide will explain 2 possible configurations for building and debugging Ship in VS code or it's forks. The first is the full Microsoft stack and the second is one based on clangd and lldb. These are not the only configurations, just those I have personally tested. If anyone has others that work better or require fewer downloads, or even that remove reliance on MSVC on Windows, please make them known.

The Microsoft stack is simpler to install, but is tied to VC Code specifically due to strict licences in an attempt to force people off of forks, while the LLVM has some fake errors with C on Windows that have yet to be resolved. (anyone who does should let the team know so it can be added)

### Microsoft stack

1. Ensure you are on telemetry-enabled VS code and not a fork.
2. Install the C/C++ extension.

### LLVM Stack

1. Install the latest version of LLVM
2. Install the clangd and CodeLLDB extensions in Vs Cod(ium). If clangd does not find the version LLVM installed, it should offer a popup to install it's own version, accept it.
3. Ensure that step 3 of VS Cod(ium) building has been performed, and build in that state at least once to generate required files.
4. Open Shipwright/.vscode/launch.json (.vscode is a hidden folder and may require a settings change to see) and set lldb's program to the location of the built files.

Regardless you should now be able to debug by going to the debugging sidebar and clicking the play button at the top, near the dropdown.

## Making a Basic Feature Branch and turning it into a PR

In order to make a feature branch, you must first choose which branch to use as a base. Generally there are 2 options, `develop` which is the latest nightly that intends to be the next version of ship, and `develop-<release name>`, which handles bugfix patches for the latest release. Fixes added to the fix branch will be merged into develop at the maintainer's convinience. If it has been a long time since the last release, the maintainer may request fixes go to develop instead.

1. Update your fork as described above.
2. Switch to the branch you want to use as a base.
- on Command Line: `git checkout <base repo name>/<branch name>`, `git branch <new branch name>` then `git checkout <new branch name>`
- with TortoiseGit: Right click, TortoiseGit > Switch/Checkout, select <base repo name>/<branch name>, enter the name of your new branch in the `Create New Branch` checkbox and click OK.
3. Make the changes you want to make. Be sure to test them by building and running the program in debug mode.
4. Run clang-format-14 and other formatting tools in order to clean up your changes to the repo's standards. This is described [here](https://github.com/HarbourMasters/Shipwright/blob/develop/docs/FORMATTING.md).
5. Commit your final changes. do this by:
- On Command Line: For each new file you want to add, `git add <path to the file>` or right click it in VS Cod(ium)'s git sidebar, right click the files you want to add and click `Stage Changes`. Then input `git commit -a`, enter the message in the text editor that appears, where the cursor is, save the file, and close the editor to finish the commit.
- with Tortoise Git: Click `Git Commit` from the right click menu, enter the commit message in the upper text box then tick any additional files you want to add to the repo in the lower box. Once you are done, click `Commit`.
6. Now that the branch is in the final state, it needs pushing to github so it can be made into a PR. Regardless of how you do this you'd be prompted to authenticate the push.
- On Command line: `git push`. The first time you do this for a branch, you'll be prompted with a command to set the origin. do so.
- with TortoiseGit: Right click, TortoiseGit > `Push`, select the branch you are pushing from the first dropdown, then select `origin` in the first dropdown in the "Destination" box. Then click OK.

You can also use steps 5 and 6 to save your current work onto the cloud, where other people can see and it'll be backed up in case of issues with your computer.

7.  Go to the [Main Repo's Github page, go to the Pull Request tab](https://github.com/HarbourMasters/Shipwright/pulls), and you should see a pop up telling you about your recently pushed branch, otherwise click `New Pull Request`. In the new menu select the branch you are making into a PR in the right hand dropdown, and select the branch you originally created your branch from on the left. Then name and describe your pull request and click `Create Pull Request` to make the PR.
8. While waiting for merge, a conflict between your PR and new additions to the repo may occur, if this happens, update your PR and resolve the conflicts as described below.

### Resolving Merge Conflicts

If the main repo has changed the same lines of code as your feature branch, you may have a conflict. If this happens you will need to tell git how to resolve the conflict.

1. Go into VS Code(ium) and click the branch symbol on the left sidebar. This is the git sidebar, and shows you every conflicted file(s) with a red !. Click 1 of them, then `Resolve in Merge Editor`
2. The new interface will have 3 code windows. The upper left window is what the Base repo has done while the upper right is what your branch has done. For each conflict, whose changes will be highlighted, you can choose to either accept 1 side of the conflict by clicking `Accept Incoming` or `Accept Current`, you can sometimes quickly merge them by clicking `Accept Combination`, or you can use the lower box to enter the result manually. You can also accept 1 or both and then edit what merge left over for you.
3. On the right side, between the windows, is a button that shows how many conflicts are remaining. You can click it to jump to the next one. When it says there are no conflicts left, click `Complete Merge`.
4. Once all the files with conflicts have completed thier merge, click continue in the sidebar to commit the conflict resolution.