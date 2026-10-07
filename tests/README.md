# Heart reward tests

Run `python3 tests/run.py --sanitize` from the repository root with Python 3,
Clang, and initialized submodules. No ROM or extracted game assets are needed.
Set `CXX` to choose a different C++20 compiler. Omit `--sanitize` for compilers
without AddressSanitizer/UndefinedBehaviorSanitizer support.

Tests use the production save types, flag reader, and editor. They cover all 36
pieces and eight boss containers, independent flag writes, active-scene precedence,
preservation of unrelated data, piece rollover/borrow, health limits, reward
statistics, Hurt Container mode, and flag-only edits. Assertions remain enabled.
They do not replace live pickup, save/reload, or UI checks.
