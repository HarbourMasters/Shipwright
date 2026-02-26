# Doc Contract (v3)

Must be true:
1. `apiVersion` is exactly `3`.
2. Actions are catalog/data-driven (`useItemProfile`, `applyStatus`, `dealDamage`, etc.).
3. IDs are namespaced (`modId:*` or `core:*`).
4. Legacy action names are not documented as valid runtime contract.
5. Capabilities map to actual file paths and schema names.
