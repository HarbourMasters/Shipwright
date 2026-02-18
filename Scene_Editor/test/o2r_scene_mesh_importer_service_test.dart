import "dart:io";
import "dart:typed_data";

import "package:archive/archive.dart";
import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/services/o2r_scene_mesh_importer_service.dart";

void main() {
  test("loads scene mesh triangles from ODLT+OARR resources", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_o2r_scene_mesh_test_",
    );

    try {
      final o2rFile = File("${tempRoot.path}/oot.o2r");

      const vertexHash = 0x1122334455667788;
      const dlistHash = 0x8877665544332211;

      final archive = Archive()
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0Vtx_000000",
            _buildVertexArrayResource(hash: vertexHash),
          ),
        )
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0DL_000000",
            _buildDisplayListResource(hash: dlistHash, vertexHash: vertexHash),
          ),
        );

      final encoded = ZipEncoder().encode(archive);
      await o2rFile.writeAsBytes(encoded, flush: true);

      final service = O2rSceneMeshImporterService();
      final result = await service.loadSceneGeometry(
        packagePath: o2rFile.path,
        sceneId: "kokiri_forest",
      );

      expect(result.hasGeometry, isTrue);
      expect(result.meshes, isNotEmpty);
      expect(result.triangleCount, greaterThan(0));
      expect(result.meshes.first.vertices.length, greaterThanOrEqualTo(3));
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });

  test(
    "resolves hash commands via CRC64(path) when resource header id is DEADBEEF",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_o2r_scene_mesh_crc_test_",
      );

      try {
        final o2rFile = File("${tempRoot.path}/oot.o2r");

        const vertexPath = "scenes/shared/spot04_scene/spot04_room_0Vtx_001160";
        const dlistPath = "scenes/shared/spot04_scene/spot04_room_0DL_002900";
        const texturePath =
            "scenes/shared/spot04_scene/spot04_room_0Tex_00BF08";

        final vertexHash = _crc64PathHash(vertexPath);
        final dlistHash = _crc64PathHash(dlistPath);
        final textureHash = _crc64PathHash(texturePath);

        final archive = Archive()
          ..addFile(
            ArchiveFile.bytes(
              vertexPath,
              _buildVertexArrayResource(
                hash: vertexHash,
                headerIdOverride: 0xDEADBEEFDEADBEEF,
              ),
            ),
          )
          ..addFile(
            ArchiveFile.bytes(
              texturePath,
              _buildTextureResource(
                hash: textureHash,
                headerIdOverride: 0xDEADBEEFDEADBEEF,
              ),
            ),
          )
          ..addFile(
            ArchiveFile.bytes(
              dlistPath,
              _buildDisplayListResource(
                hash: dlistHash,
                vertexHash: vertexHash,
                textureHash: textureHash,
                headerIdOverride: 0xDEADBEEFDEADBEEF,
              ),
            ),
          );

        final encoded = ZipEncoder().encode(archive);
        await o2rFile.writeAsBytes(encoded, flush: true);

        final service = O2rSceneMeshImporterService();
        final result = await service.loadSceneGeometry(
          packagePath: o2rFile.path,
          sceneId: "kokiri_forest",
        );

        expect(result.hasGeometry, isTrue);
        expect(result.triangleCount, greaterThan(0));
        expect(result.texturesByHash, contains(textureHash));
        expect(result.texturesByHash[textureHash]?.width, 32);
        expect(result.texturesByHash[textureHash]?.height, 32);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test(
    "resolves texture by resource id hash when command uses header id",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_o2r_scene_mesh_texture_id_test_",
      );

      try {
        final o2rFile = File("${tempRoot.path}/oot.o2r");

        const vertexPath = "scenes/shared/spot04_scene/spot04_room_0Vtx_00AB00";
        const dlistPath = "scenes/shared/spot04_scene/spot04_room_0DL_00AB00";
        const texturePath =
            "scenes/shared/spot04_scene/spot04_room_0Tex_00AB00";
        const textureResourceId = 0x1234567890ABCDEF;

        final vertexHash = _crc64PathHash(vertexPath);
        final dlistHash = _crc64PathHash(dlistPath);
        final texturePathHash = _crc64PathHash(texturePath);

        final archive = Archive()
          ..addFile(
            ArchiveFile.bytes(
              vertexPath,
              _buildVertexArrayResource(hash: vertexHash),
            ),
          )
          ..addFile(
            ArchiveFile.bytes(
              texturePath,
              _buildTextureResource(
                hash: texturePathHash,
                headerIdOverride: textureResourceId,
              ),
            ),
          )
          ..addFile(
            ArchiveFile.bytes(
              dlistPath,
              _buildDisplayListResource(
                hash: dlistHash,
                vertexHash: vertexHash,
                textureHash: textureResourceId,
              ),
            ),
          );

        final encoded = ZipEncoder().encode(archive);
        await o2rFile.writeAsBytes(encoded, flush: true);

        final service = O2rSceneMeshImporterService();
        final result = await service.loadSceneGeometry(
          packagePath: o2rFile.path,
          sceneId: "kokiri_forest",
        );

        expect(result.hasGeometry, isTrue);
        expect(result.texturesByHash, contains(textureResourceId));
        expect(result.textureCount, greaterThan(0));
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test("decodes TRI2 command with f3dex2 packed byte indices", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_o2r_scene_mesh_tri2_test_",
    );

    try {
      final o2rFile = File("${tempRoot.path}/oot.o2r");

      const vertexHash = 0xAABBCCDDEEFF0011;
      const dlistHash = 0x1100FFEEDDCCBBAA;

      final archive = Archive()
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0Vtx_000100",
            _buildVertexArrayResourceWithVertices(
              hash: vertexHash,
              vertices: const <List<int>>[
                [0, 0, 0],
                [100, 0, 0],
                [0, 100, 0],
                [100, 100, 0],
              ],
            ),
          ),
        )
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0DL_000100",
            _buildDisplayListTri2Resource(
              hash: dlistHash,
              vertexHash: vertexHash,
            ),
          ),
        );

      final encoded = ZipEncoder().encode(archive);
      await o2rFile.writeAsBytes(encoded, flush: true);

      final service = O2rSceneMeshImporterService();
      final result = await service.loadSceneGeometry(
        packagePath: o2rFile.path,
        sceneId: "kokiri_forest",
        sanitizeGeometry: false,
      );

      expect(result.hasGeometry, isTrue);
      expect(result.triangleCount, 2);
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });
  test("returns empty mesh preview when scene tokens do not match", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_o2r_scene_mesh_test_missing_",
    );

    try {
      final o2rFile = File("${tempRoot.path}/oot.o2r");
      final archive = Archive()
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0DL_000000",
            _buildDisplayListResource(hash: 1, vertexHash: 2),
          ),
        )
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0Vtx_000000",
            _buildVertexArrayResource(hash: 2),
          ),
        );

      final encoded = ZipEncoder().encode(archive);
      await o2rFile.writeAsBytes(encoded, flush: true);

      final service = O2rSceneMeshImporterService();
      final result = await service.loadSceneGeometry(
        packagePath: o2rFile.path,
        sceneId: "custom_unknown_scene",
      );

      expect(result.hasGeometry, isFalse);
      expect(result.meshes, isEmpty);
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });

  test("drops exploded triangles when sanitizer is enabled", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_o2r_scene_mesh_sanitize_test_",
    );

    try {
      final o2rFile = File("${tempRoot.path}/oot.o2r");

      const vertexHash = 0x1111222233334444;
      const dlistHash = 0x4444333322221111;

      final archive = Archive()
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0Vtx_000000",
            _buildVertexArrayResourceWithVertices(
              hash: vertexHash,
              vertices: const <List<int>>[
                [0, 0, 0],
                [100, 0, 0],
                [0, 100, 0],
                [30000, 0, 0],
              ],
            ),
          ),
        )
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_room_0DL_000000",
            _buildDisplayListTwoTrianglesResource(
              hash: dlistHash,
              vertexHash: vertexHash,
            ),
          ),
        );

      final encoded = ZipEncoder().encode(archive);
      await o2rFile.writeAsBytes(encoded, flush: true);

      final service = O2rSceneMeshImporterService();
      final sanitized = await service.loadSceneGeometry(
        packagePath: o2rFile.path,
        sceneId: "kokiri_forest",
      );
      final raw = await service.loadSceneGeometry(
        packagePath: o2rFile.path,
        sceneId: "kokiri_forest",
        sanitizeGeometry: false,
      );

      expect(raw.triangleCount, 2);
      expect(sanitized.triangleCount, 1);
      expect(sanitized.message, contains("Sanitizer removed"));
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });
}

Uint8List _buildVertexArrayResource({
  required int hash,
  int? headerIdOverride,
}) {
  return _buildVertexArrayResourceWithVertices(
    hash: hash,
    headerIdOverride: headerIdOverride,
    vertices: const <List<int>>[
      [0, 0, 0],
      [100, 0, 0],
      [0, 100, 0],
    ],
  );
}

Uint8List _buildVertexArrayResourceWithVertices({
  required int hash,
  required List<List<int>> vertices,
  int? headerIdOverride,
}) {
  final payload = BytesBuilder(copy: false);

  void writeUint32(int value) {
    final data = ByteData(4);
    data.setUint32(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  void writeInt16(int value) {
    final data = ByteData(2);
    data.setInt16(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  void writeUint16(int value) {
    final data = ByteData(2);
    data.setUint16(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  void writeByte(int value) {
    payload.add([value & 0xFF]);
  }

  writeUint32(25); // ArrayResourceType::Vertex
  writeUint32(vertices.length);

  for (final vertex in vertices) {
    writeInt16(vertex[0]);
    writeInt16(vertex[1]);
    writeInt16(vertex[2]);
    writeUint16(0);
    writeInt16(0);
    writeInt16(0);
    writeByte(255);
    writeByte(255);
    writeByte(255);
    writeByte(255);
  }

  return _buildResource(
    type: 0x4F415252,
    hash: hash,
    payload: payload.toBytes(),
    headerIdOverride: headerIdOverride,
  );
}

Uint8List _buildTextureResource({required int hash, int? headerIdOverride}) {
  final payload = BytesBuilder(copy: false);

  void writeUint32(int value) {
    final data = ByteData(4);
    data.setUint32(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  writeUint32(2); // RGBA16
  writeUint32(32);
  writeUint32(32);
  writeUint32(32 * 32 * 2);

  final texData = Uint8List(32 * 32 * 2);
  for (var i = 0; i < texData.length; i += 2) {
    texData[i] = 0xFF;
    texData[i + 1] = 0xFF;
  }
  payload.add(texData);

  return _buildResource(
    type: 0x4F544558,
    hash: hash,
    payload: payload.toBytes(),
    headerIdOverride: headerIdOverride,
  );
}

Uint8List _buildDisplayListTwoTrianglesResource({
  required int hash,
  required int vertexHash,
  int? headerIdOverride,
}) {
  final payload = BytesBuilder(copy: false);

  void writeByte(int value) {
    payload.add([value & 0xFF]);
  }

  void writeUint32(int value) {
    final data = ByteData(4);
    data.setUint32(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  writeByte(4); // ucode_f3dex2
  for (var i = 0; i < 7; i++) {
    writeByte(0);
  }

  final vtxCmdW0 = (0x32 << 24) | (4 << 12) | (4 << 1);
  writeUint32(vtxCmdW0);
  writeUint32(0);
  writeUint32((vertexHash >> 32) & 0xFFFFFFFF);
  writeUint32(vertexHash & 0xFFFFFFFF);

  final tri1W0 = (0x05 << 24) | (0 << 16) | (2 << 8) | 4; // 0,1,2
  writeUint32(tri1W0);
  writeUint32(0);

  final tri2W0 = (0x05 << 24) | (0 << 16) | (4 << 8) | 6; // 0,2,3
  writeUint32(tri2W0);
  writeUint32(0);

  writeUint32(0xDF << 24);
  writeUint32(0);

  return _buildResource(
    type: 0x4F444C54,
    hash: hash,
    payload: payload.toBytes(),
    headerIdOverride: headerIdOverride,
  );
}

Uint8List _buildDisplayListTri2Resource({
  required int hash,
  required int vertexHash,
  int? headerIdOverride,
}) {
  final payload = BytesBuilder(copy: false);

  void writeByte(int value) {
    payload.add([value & 0xFF]);
  }

  void writeUint32(int value) {
    final data = ByteData(4);
    data.setUint32(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  writeByte(4); // ucode_f3dex2
  for (var i = 0; i < 7; i++) {
    writeByte(0);
  }

  final vtxCmdW0 = (0x32 << 24) | (4 << 12) | (4 << 1);
  writeUint32(vtxCmdW0);
  writeUint32(0);
  writeUint32((vertexHash >> 32) & 0xFFFFFFFF);
  writeUint32(vertexHash & 0xFFFFFFFF);

  final tri2CmdW0 = (0x06 << 24) | (0 << 16) | (2 << 8) | 4;
  final tri2CmdW1 = (0 << 16) | (4 << 8) | 6;
  writeUint32(tri2CmdW0);
  writeUint32(tri2CmdW1);

  writeUint32(0xDF << 24);
  writeUint32(0);

  return _buildResource(
    type: 0x4F444C54,
    hash: hash,
    payload: payload.toBytes(),
    headerIdOverride: headerIdOverride,
  );
}

Uint8List _buildDisplayListResource({
  required int hash,
  required int vertexHash,
  int? textureHash,
  int? headerIdOverride,
}) {
  final payload = BytesBuilder(copy: false);

  void writeByte(int value) {
    payload.add([value & 0xFF]);
  }

  void writeUint32(int value) {
    final data = ByteData(4);
    data.setUint32(0, value, Endian.little);
    payload.add(data.buffer.asUint8List());
  }

  writeByte(4); // ucode_f3dex2
  for (var i = 0; i < 7; i++) {
    writeByte(0);
  }

  if (textureHash != null) {
    final setTimgCmdW0 = (0x20 << 24) | (0 << 21) | (2 << 19) | 32;
    writeUint32(setTimgCmdW0);
    writeUint32(0);
    writeUint32((textureHash >> 32) & 0xFFFFFFFF);
    writeUint32(textureHash & 0xFFFFFFFF);
  }

  final vtxCmdW0 = (0x32 << 24) | (3 << 12) | (3 << 1);
  writeUint32(vtxCmdW0);
  writeUint32(0);
  writeUint32((vertexHash >> 32) & 0xFFFFFFFF);
  writeUint32(vertexHash & 0xFFFFFFFF);

  final triCmdW0 = (0x05 << 24) | (0 << 16) | (2 << 8) | 4;
  writeUint32(triCmdW0);
  writeUint32(0);

  writeUint32(0xDF << 24);
  writeUint32(0);

  return _buildResource(
    type: 0x4F444C54,
    hash: hash,
    payload: payload.toBytes(),
    headerIdOverride: headerIdOverride,
  );
}

Uint8List _buildResource({
  required int type,
  required int hash,
  required Uint8List payload,
  int? headerIdOverride,
}) {
  final header = Uint8List(0x40);
  final view = ByteData.sublistView(header);
  header[0] = 0x00; // little endian
  header[1] = 0x00;
  view.setUint32(4, type, Endian.little);
  view.setUint32(8, 0, Endian.little);
  view.setUint64(12, headerIdOverride ?? hash, Endian.little);

  final bytes = BytesBuilder(copy: false)
    ..add(header)
    ..add(payload);
  return bytes.toBytes();
}

int _crc64PathHash(String path) {
  const poly = 0x42F0E1EBA9EA3693;
  var crc = 0xFFFFFFFFFFFFFFFF;

  for (final codeUnit in path.codeUnits) {
    final byte = codeUnit & 0xFF;
    crc ^= (byte << 56);

    for (var i = 0; i < 8; i++) {
      if ((crc & 0x8000000000000000) != 0) {
        crc = ((crc << 1) ^ poly) & 0xFFFFFFFFFFFFFFFF;
      } else {
        crc = (crc << 1) & 0xFFFFFFFFFFFFFFFF;
      }
    }
  }

  return crc & 0xFFFFFFFFFFFFFFFF;
}
