import "dart:io";
import "dart:typed_data";

import "package:archive/archive.dart";
import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/services/o2r_collision_importer_service.dart";

void main() {
  test("loads collision mesh from o2r zip using scene aliases", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_o2r_collision_test_",
    );

    try {
      final o2rFile = File("${tempRoot.path}/oot.o2r");
      final archive = Archive()
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot04_scene/spot04_sceneCollisionHeader_000000",
            _buildCollisionResource(),
          ),
        )
        ..addFile(
          ArchiveFile.bytes(
            "scenes/shared/spot01_scene/spot01_sceneCollisionHeader_000000",
            _buildCollisionResource(),
          ),
        );

      final encoded = ZipEncoder().encode(archive);
      await o2rFile.writeAsBytes(encoded, flush: true);

      final service = O2rCollisionImporterService();
      final result = await service.loadSceneCollision(
        packagePath: o2rFile.path,
        sceneId: "kokiri_forest",
      );

      expect(result.hasGeometry, isTrue);
      expect(result.meshes.length, 1);
      expect(result.sceneKey, "spot04_scene");
      expect(result.triangleCount, 2);
      expect(result.vertexCount, 4);
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });

  test(
    "returns empty result when scene token has no matching collision",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_o2r_collision_test_missing_",
      );

      try {
        final o2rFile = File("${tempRoot.path}/oot.o2r");
        final archive = Archive()
          ..addFile(
            ArchiveFile.bytes(
              "scenes/shared/spot04_scene/spot04_sceneCollisionHeader_000000",
              _buildCollisionResource(),
            ),
          );

        final encoded = ZipEncoder().encode(archive);
        await o2rFile.writeAsBytes(encoded, flush: true);

        final service = O2rCollisionImporterService();
        final result = await service.loadSceneCollision(
          packagePath: o2rFile.path,
          sceneId: "custom_unknown_scene",
        );

        expect(result.hasGeometry, isFalse);
        expect(result.meshes, isEmpty);
        expect(result.scannedCollisionEntries, 0);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );
}

Uint8List _buildCollisionResource() {
  final bytes = BytesBuilder(copy: false);

  final header = Uint8List(0x40);
  final headerView = ByteData.sublistView(header);
  header[0] = 0x00;
  headerView.setUint32(4, 0x4F434F4C, Endian.little);
  bytes.add(header);

  void writeInt16(int value) {
    final data = ByteData(2);
    data.setInt16(0, value, Endian.little);
    bytes.add(data.buffer.asUint8List());
  }

  void writeUint16(int value) {
    final data = ByteData(2);
    data.setUint16(0, value, Endian.little);
    bytes.add(data.buffer.asUint8List());
  }

  void writeInt32(int value) {
    final data = ByteData(4);
    data.setInt32(0, value, Endian.little);
    bytes.add(data.buffer.asUint8List());
  }

  void writeUint32(int value) {
    final data = ByteData(4);
    data.setUint32(0, value, Endian.little);
    bytes.add(data.buffer.asUint8List());
  }

  writeInt16(-100);
  writeInt16(0);
  writeInt16(-100);
  writeInt16(100);
  writeInt16(80);
  writeInt16(100);

  writeInt32(4);

  writeInt16(-100);
  writeInt16(0);
  writeInt16(-100);

  writeInt16(100);
  writeInt16(0);
  writeInt16(-100);

  writeInt16(100);
  writeInt16(0);
  writeInt16(100);

  writeInt16(-100);
  writeInt16(0);
  writeInt16(100);

  writeUint32(2);

  writeUint16(0);
  writeUint16(0);
  writeUint16(1);
  writeUint16(2);
  writeInt16(0);
  writeInt16(32767);
  writeInt16(0);
  writeInt16(0);

  writeUint16(0);
  writeUint16(0);
  writeUint16(2);
  writeUint16(3);
  writeInt16(0);
  writeInt16(32767);
  writeInt16(0);
  writeInt16(0);

  writeUint32(0);
  writeUint32(0);
  writeInt32(0);
  writeInt32(0);

  return bytes.toBytes();
}
