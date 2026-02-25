# Error Map v3

## Unsupported macro
Symptom:
- `unsupported macros while converting Fast64 model`
Action:
- Map to known command when safe.
- Soft-ignore non-structural state macros with warning.
- Hard-fail structural macros (geometry flow, texture load base).

## Archive finalize failure
Symptom:
- `failed to finalize .o2r archive`
Action:
- Verify texture byte size, dimensions, and emitted root display list path.

## Texture blur or mismatch
Symptom:
- Visual blur despite point request.
Action:
- Confirm explicit `modelTextureFilter`.
- Enforce deterministic override when set to `point` or `bilerp`.

## Runtime crash in resource load
Symptom:
- `MemoryStream::Read` out-of-range / exception.
Action:
- Validate serialized offsets and lengths; reject invalid payload before archive registration.
