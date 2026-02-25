# Memory Policy (Aggressive)

1. Always append, never rewrite old entries.
2. Summary max length: 180 chars.
3. Compact when:
- `memory.log` > 200 entries, or
- oldest live entry > 60 days.
4. After compaction:
- run `rebuild-index.ps1`
- run `validate-memory.ps1`
5. On conflict, `project_state.md` is authoritative.
