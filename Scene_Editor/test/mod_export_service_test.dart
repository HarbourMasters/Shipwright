import "dart:convert";
import "dart:io";
import "dart:typed_data";

import "package:archive/archive.dart";
import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/models/editable_mod_file.dart";
import "package:scene_editor/models/rom_metadata.dart";
import "package:scene_editor/models/scene_patch.dart";
import "package:scene_editor/services/mod_export_service.dart";

void main() {
  test(
    "strict validation blocks export when parkour asset archive is missing",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_mod_export_missing_asset_",
      );

      try {
        final outputPath = "${tempRoot.path}/missing_asset_mod.zip";
        final service = ModExportService();

        final result = await service.exportModZip(
          modName: "",
          rom: _testRom(),
          scenes: <ScenePatch>[
            _buildScenePatch("kokiri_forest", changed: false),
          ],
          editableFiles: const <EditableModFile>[],
          options: const ModExportOptions(strictValidation: true),
          outputPathOverride: outputPath,
        );

        expect(result.success, isFalse);
        expect(result.message, contains("Export blocked by validation"));
        expect(File(outputPath).existsSync(), isFalse);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test(
    "exports runtime layout with mod.json scripts/init.json and parkour_room.otr",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_mod_export_runtime_",
      );

      try {
        final outputPath = "${tempRoot.path}/runtime_mod.zip";
        final assetPath = "${tempRoot.path}/parkour_room.otr";
        await File(
          assetPath,
        ).writeAsBytes(Uint8List.fromList(<int>[1, 2, 3, 4]));

        final service = ModExportService();

        final result = await service.exportModZip(
          modName: "Kokiri Parkour",
          rom: _testRom(),
          scenes: <ScenePatch>[
            _buildScenePatch("kokiri_forest", changed: true),
            _buildScenePatch("kakariko_village", changed: false),
          ],
          editableFiles: const <EditableModFile>[
            EditableModFile(
              id: "file_1",
              relativePath: "mods/config/settings.json",
              content: "{\"parkour\":true}",
            ),
          ],
          options: ModExportOptions(
            includeUnchangedScenes: false,
            includeReadme: true,
            includeValidationReport: true,
            strictValidation: true,
            parkourAssetArchivePath: assetPath,
            forceKokiriTemplate: true,
          ),
          outputPathOverride: outputPath,
        );

        expect(result.success, isTrue);
        expect(File(outputPath).existsSync(), isTrue);

        final archive = ZipDecoder().decodeBytes(
          await File(outputPath).readAsBytes(),
        );

        expect(_hasEntry(archive, "mod.json"), isTrue);
        expect(_hasEntry(archive, "scripts/init.json"), isTrue);
        expect(_hasEntry(archive, "assets/parkour_room.otr"), isTrue);
        expect(_hasEntry(archive, "assets/scene_manifest.json"), isTrue);
        expect(_hasEntry(archive, "assets/entrance_map.json"), isTrue);
        expect(_hasEntry(archive, "reports/validation_report.json"), isTrue);
        expect(_hasEntry(archive, "README_MOD.txt"), isTrue);

        final manifest =
            jsonDecode(_entryAsString(archive, "mod.json"))
                as Map<String, dynamic>;
        expect(manifest["id"], "com.scene_editor.kokiri_parkour");
        expect(manifest["entryScript"], "scripts/init.json");
        expect(
          (manifest["assets"] as List<dynamic>).first,
          "assets/parkour_room.otr",
        );

        final script =
            jsonDecode(_entryAsString(archive, "scripts/init.json"))
                as Map<String, dynamic>;
        expect(script["apiVersion"], 1);
        expect(
          (script["onFrameTriggers"] as List<dynamic>).length,
          greaterThanOrEqualTo(1),
        );
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test(
    "exports logic-only runtime package when strict validation is disabled",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_mod_export_logic_only_",
      );

      try {
        final outputPath = "${tempRoot.path}/logic_only_mod.zip";
        final service = ModExportService();

        final result = await service.exportModZip(
          modName: "logic_only_mod",
          rom: _testRom(),
          scenes: <ScenePatch>[
            _buildScenePatch("kokiri_forest", changed: false),
          ],
          editableFiles: const <EditableModFile>[],
          options: const ModExportOptions(
            strictValidation: false,
            includeUnchangedScenes: true,
            forceKokiriTemplate: true,
          ),
          outputPathOverride: outputPath,
        );

        expect(result.success, isTrue);
        final archive = ZipDecoder().decodeBytes(
          await File(outputPath).readAsBytes(),
        );

        expect(_hasEntry(archive, "mod.json"), isTrue);
        expect(_hasEntry(archive, "scripts/init.json"), isTrue);
        expect(_hasEntry(archive, "assets/parkour_room.otr"), isFalse);
        expect(_hasEntry(archive, "assets/README.txt"), isTrue);

        final manifest =
            jsonDecode(_entryAsString(archive, "mod.json"))
                as Map<String, dynamic>;
        expect(manifest["assets"], isEmpty);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );
}

RomMetadata _testRom() {
  return const RomMetadata(
    path: "C:/roms/oot11.z64",
    fileName: "oot11.z64",
    sizeBytes: 33554432,
    sha1: "dummysha1",
    byteOrder: N64ByteOrder.bigEndian,
    title: "THE LEGEND OF ZELDA",
    gameCode: "CZLE",
    version: 1,
  );
}

ScenePatch _buildScenePatch(String sceneId, {required bool changed}) {
  final doors = changed
      ? const <SceneDoorLink>[
          SceneDoorLink(
            id: "door_1",
            name: "Test Door",
            position: Vec3(x: 0, y: 0, z: 0),
            targetSceneId: "kakariko_village",
            targetSpawn: 0,
            oneWay: false,
          ),
        ]
      : const <SceneDoorLink>[];

  return ScenePatch(
    sceneId: sceneId,
    sceneName: sceneId,
    description: "",
    assets: const <SceneAssetRef>[],
    doors: doors,
    collisions: const <CollisionBox>[],
    hooks: const <SceneHook>[],
  );
}

bool _hasEntry(Archive archive, String path) {
  return archive.findFile(path) != null;
}

String _entryAsString(Archive archive, String path) {
  final file = archive.findFile(path);
  if (file == null) {
    throw StateError("Missing archive entry: $path");
  }

  final dynamic content = file.content;
  if (content is String) {
    return content;
  }
  if (content is Uint8List) {
    return utf8.decode(content);
  }
  if (content is List<int>) {
    return utf8.decode(content);
  }

  throw StateError("Unsupported archive entry content for '$path'.");
}
