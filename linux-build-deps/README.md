# Linux build dependencies

This directory contains plaintext files with package lists and minimum version information for building on Linux systems.

## `apt` vs others

The CI workflows that run on PRs and pushes use GH actions Ubuntu runners, so it is very unlikely `apt.txt` will be missing anything. The other package list files are only verified by the `test-builds-on-distros` workflow, which is triggered manually.

## How you can help

If you run into a missing package issue when building please let us know! A PR updating the appropriate package list file would be wonderful, but opening a GH issue or just saying something on Discord works too!
