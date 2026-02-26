# Crash Signatures

1. `MemoryStream::Read` / `ResourceLoader::LoadResource`
- Usually invalid serialized size/offset, corrupted archive payload, or mismatched texture metadata.

2. `ExternalModManager::ExecuteActions`
- Action payload mismatch, invalid target resolution, or null actor access.

3. `Fast::Interpreter::Run` with custom resource frames
- Display list references unresolved resource path or malformed command payload.

4. `_CxxThrowException` + `std::vector::_Xrange`
- Out-of-range container access, usually due to bad parse assumptions.
