---
name: soh-build-test-windows
description: Run reliable Ship of Harkinian build and quick test loops on Windows with lock checks and deterministic commands. Use when compiling release binaries, verifying link locks, and preparing runtime validation.
---

# SOH Build and Test (Windows)

1. Check lock state first.
- Run `scripts/check-build-lock.ps1`.
- Close game process before build.

2. Build with deterministic command.
- Run `scripts/build-release.ps1`.
- Default target is `soh`, config `Release`.

3. Record result and next step.
- On success: run smoke checks.
- On fail: capture first linker/compiler blocker.

## Example (PT-BR)
- "Erro LNK1104 em `soh.exe`: verificar processo rodando e repetir build com `/m:1`."

## Reference
- Read `references/build-matrix.md`.
