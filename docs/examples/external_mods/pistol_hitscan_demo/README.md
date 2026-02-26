# Pistol Hitscan Demo

Demonstrates:
- `combat.targeting.v1` (`raycast`)
- `combat.damage.v1`
- `items.use_profiles.v1`

Current setup:
- Uses `SLOT_SLINGSHOT` with placeholder assets:
  - `assets/pistol_model` (Fast64 model folder)
  - `assets/pistol_icon_placeholder.png`
  - `assets/pistol_reticle_placeholder.png`
- `useMode=augment` to keep free vanilla shooting behavior.
- Adds data-driven hitscan damage on each shot.
- `aimReticleTextureAsset` is used while aiming with slingshot context.
