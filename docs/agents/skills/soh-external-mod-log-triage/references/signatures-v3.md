# Signature Map v3

## Parser / Contract
- `Missing or invalid field:` -> invalid schema or missing required file for declared capability.
- `Unsupported apiVersion` -> `apiVersion` is not `3`.
- `Unknown schemaVersion` -> unsupported catalog schema.

## Runtime / Actions
- `Runtime disabled for ...` -> fatal validation or runtime structural failure in that mod.
- `Unknown action` -> behavior action not in v3 contract.
- `condition ... invalid` -> malformed behavior condition payload.

## Fast64 / Render
- `unsupported macros while converting Fast64 model` -> converter mapping gap.
- `failed to finalize .o2r archive` -> conversion output corruption or unresolved resources.
- `LoadResource ... out_of_range` -> invalid texture bytes/offset/size.

## Crash Stack
- `MemoryStream::Read` -> invalid serialized resource size/path.
- `ExternalModManager::ExecuteActions` -> malformed action payload or runtime target resolution error.
