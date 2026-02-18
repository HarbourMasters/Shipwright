import "dart:convert";
import "dart:io";

import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/models/toolchain_config.dart";
import "package:scene_editor/services/toolchain_config_service.dart";

void main() {
  test("loads default config with pinned tag from lock file", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_toolchain_defaults_",
    );

    try {
      final appData = Directory("${tempRoot.path}/appdata")
        ..createSync(recursive: true);
      final lockPath = "${tempRoot.path}/fast64.lock.json";
      await File(
        lockPath,
      ).writeAsString(jsonEncode(<String, dynamic>{"testedTag": "v9.9.9"}));

      final service = ToolchainConfigService(
        appDataPathOverride: appData.path,
        workspaceRootOverride: tempRoot.path,
        lockFilePathOverride: lockPath,
      );

      final config = await service.load();
      expect(config.fast64PinnedTag, "v9.9.9");
      expect(config.isConfigured, isFalse);
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });

  test("saves and reloads toolchain config", () async {
    final tempRoot = await Directory.systemTemp.createTemp(
      "scene_editor_toolchain_save_",
    );

    try {
      final appData = Directory("${tempRoot.path}/appdata")
        ..createSync(recursive: true);
      final lockPath = "${tempRoot.path}/fast64.lock.json";
      await File(
        lockPath,
      ).writeAsString(jsonEncode(<String, dynamic>{"testedTag": "v2.1.0"}));

      final service = ToolchainConfigService(
        appDataPathOverride: appData.path,
        workspaceRootOverride: tempRoot.path,
        lockFilePathOverride: lockPath,
      );

      const config = ToolchainConfig(
        blenderExePath: "C:/Tools/Blender/blender.exe",
        fast64InstallPath: "C:/Tools/fast64",
        fast64PinnedTag: "v2.1.0",
        fast64ProjectRoot: "D:/projects/shipwright_fast64",
        pythonPath: "C:/Python/python.exe",
      );

      await service.save(config);
      final loaded = await service.load();

      expect(loaded.blenderExePath, config.blenderExePath);
      expect(loaded.fast64InstallPath, config.fast64InstallPath);
      expect(loaded.fast64PinnedTag, config.fast64PinnedTag);
      expect(loaded.fast64ProjectRoot, config.fast64ProjectRoot);
      expect(loaded.pythonPath, config.pythonPath);
      expect(loaded.isConfigured, isTrue);

      final configPath = await service.readConfigPath();
      expect(File(configPath).existsSync(), isTrue);
    } finally {
      await tempRoot.delete(recursive: true);
    }
  });
}
