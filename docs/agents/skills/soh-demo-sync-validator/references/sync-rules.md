# Sync Rules

Source of truth:
- `docs/examples/external_mods`

Runtime mirror:
- `x64/Release/mods`

Rules:
1. Directory name must match exactly.
2. `mod.json` must exist in source and destination.
3. `apiVersion` must be `3`.
4. Sync is overwrite for tracked demo directories.
5. Validation fails if destination contains stale demo not present in source (unless ignored).
