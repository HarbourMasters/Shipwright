# Freeze Dome Staff Demo (API v3)

A hammer-triggered staff example:

- Trigger: `useTrigger = hammerGroundImpact` (real hammer ground impact)
- Visual: blue shockwave + ice smoke (`spawnShockwave`)
- Gameplay: AoE freeze (`spawnAoE`) scoped to `enemies_bosses`

## How to test

1. Load the mod and equip **Freeze Dome Staff** on a C button.
2. Use hammer attack and hit the ground.
3. Expected: blue dome-like burst appears; nearby enemies/bosses get freeze status; player is not frozen.
