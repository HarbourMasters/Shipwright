#!/usr/bin/env bash
# Create, but never mount or overwrite, an Aroma SD-card staging tree.
set -euo pipefail

repo_dir="$(cd "$(dirname "$0")" && pwd)"
stage_dir="${1:-$repo_dir/wiiu-staging}"
build_dir="${SOH_WIIU_BUILD_DIR:-build-wiiu}"
cd "$repo_dir"

[[ "$(git rev-parse HEAD)" == "cb71e22a79bc5d1f688fa881795bbd93094895fc" ]] || {
  echo "Refusing: source is not the recorded 9.2.3 commit." >&2; exit 1;
}
[[ "$build_dir" != /* ]] || { echo "SOH_WIIU_BUILD_DIR must be relative to the repository." >&2; exit 1; }
for artifact in "$build_dir/soh/soh.wuhb" "$build_dir/gamecontrollerdb.txt" oot.o2r soh.o2r; do
  [[ -s "$artifact" ]] || { echo "Missing build artifact: $artifact" >&2; exit 1; }
done
command -v unzip >/dev/null
unzip -t oot.o2r >/dev/null
unzip -t soh.o2r >/dev/null

app_dir="$stage_dir/wiiu/apps/soh"
if [[ -e "$app_dir" ]]; then
  echo "Refusing to overwrite existing staging app directory: $app_dir" >&2
  exit 1
fi
mkdir -p "$app_dir/mods" "$app_dir/logs"
cp "$build_dir/soh/soh.wuhb" "$app_dir/soh.wuhb"
cp "$build_dir/gamecontrollerdb.txt" "$app_dir/"
cp oot.o2r soh.o2r "$app_dir/"

echo "Created staging tree: $stage_dir (using $build_dir)"
find "$stage_dir" -type f -maxdepth 5 -print
echo "To copy safely, first back up any SD-card wiiu/apps/soh directory, then review:"
echo "  rsync -av --dry-run '$stage_dir/' '/Volumes/<YOUR_SD_NAME>/'"
echo "Only after reviewing the dry run:"
echo "  rsync -av '$stage_dir/' '/Volumes/<YOUR_SD_NAME>/'"
