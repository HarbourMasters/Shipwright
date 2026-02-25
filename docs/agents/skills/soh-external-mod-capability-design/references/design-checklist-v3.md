# Design Checklist v3 (No Legacy)

1. Capability declaration is explicit in `mod.json`.
2. Required file exists when capability is declared.
3. `schemaVersion` is supported; unknown value is fatal for that mod.
4. Every ID is namespaced: `modId:*` or `core:*`.
5. Validation rules define:
- min/max/clamp behavior
- fatal vs warning outcome
- default values
6. Runtime action is deterministic and budget-aware.
7. Logs include `modId`, `file`, `jsonPath`, `capability`, `reason`.
8. Reload flow is safe (`Reload External Mods` does not leak runtime state).
9. Demo and docs are updated in the same change.
10. Memory protocol is updated (`decision` + refs).
