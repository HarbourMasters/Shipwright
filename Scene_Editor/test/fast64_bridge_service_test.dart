import "dart:convert";
import "dart:io";

import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/models/toolchain_config.dart";
import "package:scene_editor/services/fast64_bridge_service.dart";
import "package:scene_editor/services/toolchain_config_service.dart";

void main() {
  test("validateToolchain fails when pinned tag mismatches lock", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_fast64_validate_fail_",
    );

    try {
      final workspace = Directory("${tempRoot.path}/workspace")
        ..createSync(recursive: true);
      Directory("${workspace.path}/Scene_Editor").createSync(recursive: true);
      Directory("${workspace.path}/scripts/fast64").createSync(recursive: true);
      await File(
        "${workspace.path}/scripts/fast64/export_scene_bundle.py",
      ).writeAsString("print('ok')");

      final appData = Directory("${tempRoot.path}/appdata")
        ..createSync(recursive: true);
      final lockPath = "${workspace.path}/Scene_Editor/fast64.lock.json";
      await File(
        lockPath,
      ).writeAsString(jsonEncode(<String, dynamic>{"testedTag": "v2.1.0"}));

      final blenderExe = File("${workspace.path}/blender.exe")
        ..createSync(recursive: true);
      final fast64Install = Directory("${workspace.path}/fast64")
        ..createSync(recursive: true);
      final projectRoot = Directory("${workspace.path}/project")
        ..createSync(recursive: true);

      final configService = ToolchainConfigService(
        appDataPathOverride: appData.path,
        workspaceRootOverride: workspace.path,
        lockFilePathOverride: lockPath,
      );

      await configService.save(
        ToolchainConfig(
          blenderExePath: blenderExe.path,
          fast64InstallPath: fast64Install.path,
          fast64PinnedTag: "v1.0.0",
          fast64ProjectRoot: projectRoot.path,
        ),
      );

      final bridge = Fast64BridgeService(
        toolchainConfigService: configService,
        workspaceRootOverride: workspace.path,
        processExecutor:
            (
              String executable,
              List<String> arguments, {
              String? workingDirectory,
            }) async {
              return ProcessResult(0, 0, "Blender 4.0", "");
            },
      );

      final result = await bridge.validateToolchain();
      expect(result.success, isFalse);
      expect(
        result.errors.any((err) => err.contains("does not match lock")),
        isTrue,
      );
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });

  test(
    "exportSceneBundle returns bundle paths when script run succeeds",
    () async {
      final tempRoot = await Directory.systemTemp.createTemp(
        "scene_editor_fast64_export_success_",
      );

      try {
        final workspace = Directory("${tempRoot.path}/workspace")
          ..createSync(recursive: true);
        Directory("${workspace.path}/Scene_Editor").createSync(recursive: true);
        Directory(
          "${workspace.path}/scripts/fast64",
        ).createSync(recursive: true);
        await File(
          "${workspace.path}/scripts/fast64/export_scene_bundle.py",
        ).writeAsString("print('ok')");

        final appData = Directory("${tempRoot.path}/appdata")
          ..createSync(recursive: true);
        final lockPath = "${workspace.path}/Scene_Editor/fast64.lock.json";
        await File(
          lockPath,
        ).writeAsString(jsonEncode(<String, dynamic>{"testedTag": "v2.1.0"}));

        final blenderExe = File("${workspace.path}/blender.exe")
          ..createSync(recursive: true);
        final fast64Install = Directory("${workspace.path}/fast64")
          ..createSync(recursive: true);
        final projectRoot = Directory("${workspace.path}/project")
          ..createSync(recursive: true);

        final configService = ToolchainConfigService(
          appDataPathOverride: appData.path,
          workspaceRootOverride: workspace.path,
          lockFilePathOverride: lockPath,
        );

        await configService.save(
          ToolchainConfig(
            blenderExePath: blenderExe.path,
            fast64InstallPath: fast64Install.path,
            fast64PinnedTag: "v2.1.0",
            fast64ProjectRoot: projectRoot.path,
          ),
        );

        final bridge = Fast64BridgeService(
          toolchainConfigService: configService,
          workspaceRootOverride: workspace.path,
          processExecutor:
              (
                String executable,
                List<String> arguments, {
                String? workingDirectory,
              }) async {
                if (arguments.contains("--version")) {
                  return ProcessResult(0, 0, "Blender 4.1.0", "");
                }

                final outputIndex = arguments.indexOf("--output-dir");
                final outputDir = Directory(arguments[outputIndex + 1]);
                await outputDir.create(recursive: true);
                await File(
                  "${outputDir.path}/scene_manifest.json",
                ).writeAsString(
                  jsonEncode(<String, dynamic>{"sceneKey": "kokiri_forest"}),
                );
                await File(
                  "${outputDir.path}/entrance_map.json",
                ).writeAsString("{}");
                await File(
                  "${outputDir.path}/scene.glb",
                ).writeAsBytes(<int>[0, 1]);
                await File(
                  "${outputDir.path}/collision.glb",
                ).writeAsBytes(<int>[2, 3]);
                return ProcessResult(0, 0, "ok", "");
              },
        );

        final result = await bridge.exportSceneBundle("kokiri_forest");
        expect(result.success, isTrue);
        expect(result.bundlePath, isNotNull);
        expect(result.sceneManifestPath, isNotNull);
        expect(File(result.sceneManifestPath!).existsSync(), isTrue);
        expect(File(result.entranceMapPath!).existsSync(), isTrue);
      } finally {
        await tempRoot.delete(recursive: true);
      }
    },
  );

  test("buildModAssetArchive copies parkour_room.otr from bundle", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_fast64_archive_copy_",
    );

    try {
      final workspace = Directory("${tempRoot.path}/workspace")
        ..createSync(recursive: true);
      Directory("${workspace.path}/Scene_Editor").createSync(recursive: true);
      Directory("${workspace.path}/scripts/fast64").createSync(recursive: true);
      await File(
        "${workspace.path}/scripts/fast64/export_scene_bundle.py",
      ).writeAsString("print('ok')");

      final appData = Directory("${tempRoot.path}/appdata")
        ..createSync(recursive: true);
      final lockPath = "${workspace.path}/Scene_Editor/fast64.lock.json";
      await File(
        lockPath,
      ).writeAsString(jsonEncode(<String, dynamic>{"testedTag": "v2.1.0"}));

      final configService = ToolchainConfigService(
        appDataPathOverride: appData.path,
        workspaceRootOverride: workspace.path,
        lockFilePathOverride: lockPath,
      );

      await configService.save(
        ToolchainConfig(
          blenderExePath: "${workspace.path}/blender.exe",
          fast64InstallPath: "${workspace.path}/fast64",
          fast64PinnedTag: "v2.1.0",
          fast64ProjectRoot: "${workspace.path}/project",
        ),
      );

      File("${workspace.path}/blender.exe").createSync(recursive: true);
      Directory("${workspace.path}/fast64").createSync(recursive: true);
      Directory("${workspace.path}/project").createSync(recursive: true);

      final bundle = Directory("${workspace.path}/bundle")
        ..createSync(recursive: true);
      final source = File("${bundle.path}/assets/parkour_room.otr")
        ..createSync(recursive: true);
      await source.writeAsBytes(<int>[7, 8, 9]);

      final bridge = Fast64BridgeService(
        toolchainConfigService: configService,
        workspaceRootOverride: workspace.path,
        processExecutor:
            (
              String executable,
              List<String> arguments, {
              String? workingDirectory,
            }) async {
              return ProcessResult(0, 0, "ok", "");
            },
      );

      final outputPath = "${tempRoot.path}/parkour_room.otr";
      final result = await bridge.buildModAssetArchive(
        "kokiri_forest",
        bundlePath: bundle.path,
        outputArchivePath: outputPath,
      );

      expect(result.success, isTrue);
      expect(result.outputArchivePath, outputPath);
      expect(File(outputPath).existsSync(), isTrue);
      expect(await File(outputPath).readAsBytes(), <int>[7, 8, 9]);
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });
}
