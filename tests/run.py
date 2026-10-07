"""Run ROM-free heart-piece regression tests against production game types."""
import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
GAME = "soh"


def run():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sanitize", action="store_true", help="Enable address and undefined-behavior sanitizers")
    args = parser.parse_args()
    includes = [ROOT, ROOT / GAME, ROOT / GAME / "include", ROOT / GAME / "src",
                ROOT / GAME / "assets", ROOT / "libultraship/include"]
    compiler = shlex.split(os.environ.get("CXX", "clang++"))
    command = compiler + ["-std=c++20", "-DF3DEX_GBI_2", "-DCONTROLLERBUTTONS_T=uint32_t",
                          "-Wno-macro-redefined", "-Wno-c++11-narrowing"]
    for directory in includes:
        command += ["-I", str(directory)]
    if args.sanitize:
        command += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]

    with tempfile.TemporaryDirectory(prefix="heart-piece-tests-") as temporary:
        build = Path(temporary)

        def check(name, source, extra=()):
            executable = build / name
            subprocess.run(command + [str(source), *map(str, extra), "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)

        check("flags", ROOT / "tests/heart_piece_flags.cpp")
        check("reward_edit", ROOT / "tests/heart_reward_edit.cpp")


if __name__ == "__main__":
    run()
