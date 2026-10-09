#!/usr/bin/env python3
"""
Generate a git-style patch that shows changes in decompilation between 2Ship (new)
and the official decomp repo (old).
"""
import difflib
import os
import sys

ZELDARET_URL = "https://raw.githubusercontent.com/zeldaret/mm/master/"

OLD_DISPLAY = "zeldaret/mm"
NEW_DISPLAY = "2Ship"

try:
    # Prefer requests if available for nicer error handling
    import requests
except Exception:
    requests = None
    import urllib.request

def read_file(path):
    with open(path, "r", encoding="utf-8", errors="surrogateescape") as f:
        return f.read().splitlines(keepends=False)

def fetch_url(url):
    if requests:
        r = requests.get(url)
        r.raise_for_status()
        return r.text.splitlines(keepends=False)
    else:
        with urllib.request.urlopen(url) as resp:
            data = resp.read()
            # decode with utf-8 fallback
            text = data.decode("utf-8", errors="surrogateescape")
            return text.splitlines(keepends=False)

def main():
    if len(sys.argv) < 2:
        print("Usage: python decomp_diff.py <local_file> [decomp_relative_path]", file=sys.stderr)
        sys.exit(2)

    local_path = sys.argv[1]
    if not os.path.exists(local_path):
        print(f"Local file not found: {local_path}", file=sys.stderr)
        sys.exit(2)

    if not local_path.endswith((".c", ".h")):
        print("Can only compare .c and .h files", file=sys.stderr)
        sys.exit(2)

    # Read local file (new)
    local_path = os.path.abspath(local_path)
    new_lines = read_file(local_path)

    repo_path = os.path.join(os.path.dirname(__file__), "mm")
    relative_path = os.path.relpath(local_path, repo_path)

    if len(sys.argv) > 2:
        decomp_url = sys.argv[2]
    else:
        decomp_url = None

    print(f"Comparing '{relative_path}' to{f" '{decomp_url}' in" if decomp_url else ""} zeldaret's decomp...")

    if not decomp_url:
        decomp_url = relative_path.replace("\\", "/")

    # Read zeldaret file (old)
    try:
        old_lines = fetch_url(ZELDARET_URL + decomp_url)
    except Exception as e:
        print(f"Failed to fetch URL {ZELDARET_URL + decomp_url}: {e}", file=sys.stderr)
        sys.exit(3)

    # produce unified diff (old -> new)
    patch_lines = list(difflib.unified_diff(old_lines, new_lines,
                                   fromfile=f"{OLD_DISPLAY} ({decomp_url})",
                                   tofile=f"{NEW_DISPLAY} ({relative_path})",
                                   lineterm=""))

    # append --- and +++ lines are already present in unified diff output; but ensure they exist
    # if unified_diff returned an empty list and files are identical, print nothing
    if not patch_lines:
        print("No differences found.")
        return

    out_text = "\n".join(patch_lines) + "\n"
    sys.stdout.write(out_text)

if __name__ == "__main__":
    main()
