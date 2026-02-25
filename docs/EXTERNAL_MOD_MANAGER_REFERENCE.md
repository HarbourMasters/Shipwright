# External Mod Manager Reference (Sylian Foundry Modloader)

Technical reference for runtime behavior in `ExternalModManager` and related modules.

> Contract baseline: `apiVersion: 3`

## Source locations

- `soh/soh/Enhancements/external-mods/ExternalModManager.h` + `soh/soh/Enhancements/external-mods/ExternalModManager.cpp`
- `soh/soh/Enhancements/external-mods/ExternalModTypes.h`
- `soh/soh/Enhancements/external-mods/ExternalModContentRegistry.h` + `soh/soh/Enhancements/external-mods/ExternalModContentRegistry.cpp`
- `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.h` + `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp`

## Runtime reference exports

Generated machine-readable references:

- `docs/catalogs.json`
- `docs/actions.json`
- `docs/events.json`

Regenerate with:

```powershell
tools/external_mods/export_runtime_reference.ps1
```

## Manager responsibilities

1. Discover external packages (folder/zip abstractions).
2. Validate manifests/capability files.
3. Register catalogs and behavior subscriptions.
4. Route hook events to behaviors/WASM.
5. Apply per-mod runtime budgets and isolation rules.
6. Cleanup state on reload/shutdown/disable.

## Capability/file enforcement

If a capability is declared, its file/field contract must be valid for that mod.

Canonical capability mapping is exported in `docs/catalogs.json`.

## Conflict resolution

For overlapping catalog IDs across mods:

1. Higher `loadPriority` wins.
2. Tie-breaker: alphabetical `mod.id`.
3. Conflicts are logged with context.

## Runtime budgets (defaults)

Current defaults (see `docs/catalogs.json` runtime.budgetDefaults):

- `maxCallMs = 2`
- `maxFrameBudgetMs = 2`
- `maxHookCallsPerFrame = 256`
- `maxActorInstances = 64`
- `maxActiveStatuses = 256`

## Hook and behavior observability

Runtime tracks and surfaces:

- hook calls per frame
- behavior/WASM execution counts
- budget drops and disable reasons

Use runtime logs and external-mod UI diagnostics for triage.

## Safety model

- Invalid/over-budget mod -> disable that mod, not the whole system.
- Reload must clear mod-owned runtime state deterministically.

