import "dart:convert";
import "dart:io";

import "../models/toolchain_config.dart";

class ToolchainConfigService {
  ToolchainConfigService({
    this.appDataPathOverride,
    this.workspaceRootOverride,
    this.lockFilePathOverride,
  });

  static const String defaultLockTag = "fast64-tag-not-configured";

  final String? appDataPathOverride;
  final String? workspaceRootOverride;
  final String? lockFilePathOverride;

  Future<ToolchainConfig> load() async {
    final defaultPinnedTag = await _readPinnedTagFromLock();
    final configFile = _configFile();

    if (!configFile.existsSync()) {
      return ToolchainConfig.empty(defaultPinnedTag: defaultPinnedTag);
    }

    try {
      final raw = await configFile.readAsString();
      final decoded = jsonDecode(raw);
      if (decoded is! Map<String, dynamic>) {
        return ToolchainConfig.empty(defaultPinnedTag: defaultPinnedTag);
      }

      return ToolchainConfig.fromJson(
        decoded,
        defaultPinnedTag: defaultPinnedTag,
      );
    } catch (_) {
      return ToolchainConfig.empty(defaultPinnedTag: defaultPinnedTag);
    }
  }

  Future<void> save(ToolchainConfig config) async {
    final file = _configFile();
    await file.parent.create(recursive: true);

    final encoder = const JsonEncoder.withIndent("  ");
    await file.writeAsString(encoder.convert(config.toJson()), flush: true);
  }

  Future<String> readConfigPath() async {
    return _configFile().absolute.path;
  }

  Future<String> readPinnedTagFromLock() async {
    return _readPinnedTagFromLock();
  }

  Future<String> _readPinnedTagFromLock() async {
    final lockPath = _resolveLockFilePath();
    final lockFile = File(lockPath);
    if (!lockFile.existsSync()) {
      return defaultLockTag;
    }

    try {
      final raw = await lockFile.readAsString();
      final decoded = jsonDecode(raw);
      if (decoded is! Map<String, dynamic>) {
        return defaultLockTag;
      }

      final tag = decoded["testedTag"];
      if (tag is String && tag.trim().isNotEmpty) {
        return tag.trim();
      }

      return defaultLockTag;
    } catch (_) {
      return defaultLockTag;
    }
  }

  File _configFile() {
    final appData = _resolveAppDataPath();
    final configDir = Directory(_joinPath(appData, const ["scene_editor"]));
    return File(_joinPath(configDir.path, const ["toolchain_config.json"]));
  }

  String _resolveAppDataPath() {
    final override = appDataPathOverride;
    if (override != null && override.trim().isNotEmpty) {
      return Directory(override).absolute.path;
    }

    final env = Platform.environment;
    if (Platform.isWindows) {
      final appData = env["APPDATA"];
      if (appData != null && appData.trim().isNotEmpty) {
        return Directory(appData).absolute.path;
      }
    }

    final home = env["HOME"];
    if (home != null && home.trim().isNotEmpty) {
      return Directory(home).absolute.path;
    }

    return Directory.current.absolute.path;
  }

  String _resolveLockFilePath() {
    final override = lockFilePathOverride;
    if (override != null && override.trim().isNotEmpty) {
      return File(override).absolute.path;
    }

    final root = _resolveWorkspaceRoot();
    return _joinPath(root, const ["Scene_Editor", "fast64.lock.json"]);
  }

  String _resolveWorkspaceRoot() {
    final override = workspaceRootOverride;
    if (override != null && override.trim().isNotEmpty) {
      return Directory(override).absolute.path;
    }

    var current = Directory.current.absolute;
    for (var depth = 0; depth < 8; depth++) {
      final sceneEditor = Directory(
        _joinPath(current.path, const ["Scene_Editor"]),
      );
      final soh = Directory(_joinPath(current.path, const ["soh"]));
      if (sceneEditor.existsSync() && soh.existsSync()) {
        return current.path;
      }

      final parent = current.parent;
      if (parent.path == current.path) {
        break;
      }
      current = parent;
    }

    return Directory.current.absolute.path;
  }

  String _joinPath(String base, List<String> parts) {
    var value = base;
    for (final part in parts) {
      if (value.endsWith("/") || value.endsWith("\\")) {
        value = "$value$part";
      } else {
        value = "$value${Platform.pathSeparator}$part";
      }
    }
    return value;
  }
}
