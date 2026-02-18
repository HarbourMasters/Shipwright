import "dart:convert";
import "dart:io";

import "package:archive/archive.dart";
import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/services/asset_indexer_service.dart";

void main() {
  test(
    "asset indexer recognizes renderable models and xml scene/texture patterns",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_assets_test_",
      );
      try {
        final objXml = File(
          "${tempRoot.path}/assets/xml/GC_MQ_D/objects/object_box.xml",
        );
        final sceneXml = File(
          "${tempRoot.path}/assets/xml/GC_MQ_D/scenes/spot04_scene.xml",
        );
        final texXml = File(
          "${tempRoot.path}/assets/xml/GC_MQ_D/textures/forest_tex.xml",
        );
        final modelObj = File("${tempRoot.path}/custom/room.obj");
        final texPng = File("${tempRoot.path}/custom/brick.png");
        final packOtr = File("${tempRoot.path}/mods/soh.otr");

        await objXml.create(recursive: true);
        await sceneXml.create(recursive: true);
        await texXml.create(recursive: true);
        await modelObj.create(recursive: true);
        await texPng.create(recursive: true);
        await packOtr.create(recursive: true);

        final indexer = AssetIndexerService();
        final indexed = await indexer.scan(tempRoot.path);

        expect(indexed.totalFiles, 6);
        expect(indexed.models.length, 1);
        expect(indexed.scenes.length, 1);
        expect(indexed.textures.length, 2);
        expect(indexed.packages.length, 1);
        expect(indexed.recognizedFiles, 5);
        expect(indexer.isRenderableModelPath(modelObj.path), isTrue);
        expect(indexer.isRenderableModelPath(objXml.path), isFalse);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test(
    "asset indexer only accepts zip package when root mod.json is valid",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_zip_mod_test_",
      );

      try {
        final validZipPath =
            "${tempRoot.path}${Platform.pathSeparator}mods${Platform.pathSeparator}valid_mod.zip";
        final invalidZipPath =
            "${tempRoot.path}${Platform.pathSeparator}mods${Platform.pathSeparator}invalid_mod.zip";

        await Directory(
          "${tempRoot.path}${Platform.pathSeparator}mods",
        ).create(recursive: true);

        await _writeZip(validZipPath, <String, String>{
          "mod.json": jsonEncode(<String, dynamic>{
            "id": "com.example.valid",
            "name": "Valid",
            "version": "0.1.0",
            "apiVersion": 1,
            "entryScript": "scripts/init.json",
            "assets": <String>["assets/main.otr"],
          }),
          "scripts/init.json": "{}",
        });

        await _writeZip(invalidZipPath, <String, String>{
          "folder/mod.json": "{\"apiVersion\":1}",
        });

        final indexer = AssetIndexerService();
        final indexed = await indexer.scan(tempRoot.path);

        expect(indexed.packages, contains(validZipPath));
        expect(indexed.packages, isNot(contains(invalidZipPath)));
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test(
    "asset indexer builds ROM-aware roots without scanning ROM parent root",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_rom_roots_test_",
      );

      try {
        final romHome = Directory(
          "${tempRoot.path}${Platform.pathSeparator}rom",
        )..createSync(recursive: true);

        final likelySibling = Directory(
          "${romHome.path}${Platform.pathSeparator}SoH-Copper-Assets",
        )..createSync(recursive: true);

        final workspaceRoot = Directory(
          "${tempRoot.path}${Platform.pathSeparator}workspace${Platform.pathSeparator}Shipwright",
        )..createSync(recursive: true);
        Directory(
          "${workspaceRoot.path}${Platform.pathSeparator}soh",
        ).createSync(recursive: true);
        Directory(
          "${workspaceRoot.path}${Platform.pathSeparator}Scene_Editor",
        ).createSync(recursive: true);

        final romPath =
            "${romHome.path}${Platform.pathSeparator}Legend of Zelda, The - Ocarina of Time (USA) (Rev 1).z64";

        final indexer = AssetIndexerService();
        final roots = indexer.buildRomAwareCandidateRoots(
          romPath: romPath,
          currentPath:
              "${workspaceRoot.path}${Platform.pathSeparator}Scene_Editor",
        );

        expect(roots, isNotEmpty);
        expect(
          roots,
          contains(
            Directory(
              "${romHome.path}${Platform.pathSeparator}assets",
            ).absolute.path,
          ),
        );
        expect(roots, contains(likelySibling.absolute.path));
        expect(
          roots,
          contains(
            Directory(
              "${workspaceRoot.path}${Platform.pathSeparator}soh${Platform.pathSeparator}assets",
            ).absolute.path,
          ),
        );
        expect(roots, isNot(contains(Directory(romHome.path).absolute.path)));
        expect(
          roots,
          isNot(contains(Directory(workspaceRoot.path).absolute.path)),
        );
        expect(roots.length, roots.toSet().length);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );
}

Future<void> _writeZip(String outputPath, Map<String, String> entries) async {
  final archive = Archive();
  for (final entry in entries.entries) {
    archive.addFile(ArchiveFile.string(entry.key, entry.value));
  }

  final bytes = ZipEncoder().encode(archive);
  if (bytes.isEmpty) {
    throw StateError("Failed to encode test zip at $outputPath");
  }

  final file = File(outputPath);
  await file.create(recursive: true);
  await file.writeAsBytes(bytes, flush: true);
}
