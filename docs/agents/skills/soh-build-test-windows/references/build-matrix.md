# Build Matrix (Windows)

Primary:
- `cmake --build build/x64 --config Release --target soh -- /m:1`

Alternatives:
- Full parallel: `/m`
- Specific target: `--target OTRExporter` or `--target libultraship`

Pre-check:
- Ensure no running `soh.exe`.
- Ensure output binary path is writable.
