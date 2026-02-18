import "dart:io";

import "toolchain_config_service.dart";

typedef ProcessExecutor =
    Future<ProcessResult> Function(
      String executable,
      List<String> arguments, {
      String? workingDirectory,
    });

class ToolchainValidationResult {
  const ToolchainValidationResult({
    required this.success,
    required this.errors,
    required this.warnings,
    required this.details,
  });

  final bool success;
  final List<String> errors;
  final List<String> warnings;
  final Map<String, String> details;
}

class Fast64ExportResult {
  const Fast64ExportResult({
    required this.success,
    required this.message,
    this.bundlePath,
    this.sceneManifestPath,
    this.entranceMapPath,
    this.exitCode,
    this.warnings = const <String>[],
  });

  final bool success;
  final String message;
  final String? bundlePath;
  final String? sceneManifestPath;
  final String? entranceMapPath;
  final int? exitCode;
  final List<String> warnings;
}

class ArchiveBuildResult {
  const ArchiveBuildResult({
    required this.success,
    required this.message,
    this.outputArchivePath,
  });

  final bool success;
  final String message;
  final String? outputArchivePath;
}

class Fast64BridgeService {
  Fast64BridgeService({
    required this.toolchainConfigService,
    this.workspaceRootOverride,
    ProcessExecutor? processExecutor,
  }) : _processExecutor = processExecutor ?? _defaultProcessExecutor;

  static const String defaultExportScriptRelativePath =
      "scripts/fast64/export_scene_bundle.py";

  static Future<ProcessResult> _defaultProcessExecutor(
    String executable,
    List<String> arguments, {
    String? workingDirectory,
  }) {
    return Process.run(
      executable,
      arguments,
      workingDirectory: workingDirectory,
      runInShell: true,
    );
  }

  final ToolchainConfigService toolchainConfigService;
  final String? workspaceRootOverride;
  final ProcessExecutor _processExecutor;

  Future<ToolchainValidationResult> validateToolchain() async {
    final config = await toolchainConfigService.load();
    final errors = <String>[];
    final warnings = <String>[];
    final details = <String, String>{};

    final testedTag = await toolchainConfigService.readPinnedTagFromLock();
    details["testedTag"] = testedTag;
    details["configuredTag"] = config.fast64PinnedTag;

    if (config.fast64PinnedTag.trim().isEmpty) {
      errors.add("fast64PinnedTag is empty in toolchain config.");
    } else if (config.fast64PinnedTag.trim() != testedTag.trim()) {
      errors.add(
        "Configured fast64PinnedTag '${config.fast64PinnedTag}' does not match lock testedTag '$testedTag'.",
      );
    }

    _validateFilePath(
      errors,
      details,
      key: "blenderExePath",
      value: config.blenderExePath,
      expectsFile: true,
    );
    _validateFilePath(
      errors,
      details,
      key: "fast64InstallPath",
      value: config.fast64InstallPath,
      expectsFile: false,
    );
    _validateFilePath(
      errors,
      details,
      key: "fast64ProjectRoot",
      value: config.fast64ProjectRoot,
      expectsFile: false,
    );

    final pythonPath = config.pythonPath;
    if (pythonPath != null && pythonPath.trim().isNotEmpty) {
      _validateFilePath(
        errors,
        details,
        key: "pythonPath",
        value: pythonPath,
        expectsFile: true,
      );
    }

    final exportScriptPath = _resolveExportScriptPath();
    details["exportScriptPath"] = exportScriptPath;
    if (!File(exportScriptPath).existsSync()) {
      errors.add("Fast64 export script not found: $exportScriptPath");
    }

    if (errors.isEmpty) {
      final versionResult = await _processExecutor(
        config.blenderExePath,
        const ["--version"],
      );
      if (versionResult.exitCode != 0) {
        errors.add(
          "Failed to execute blender --version (exit ${versionResult.exitCode}).",
        );
      } else {
        final output = "${versionResult.stdout}".trim().split(
          RegExp(r"[\r\n]+"),
        );
        if (output.isNotEmpty && output.first.trim().isNotEmpty) {
          details["blenderVersion"] = output.first.trim();
        }
      }
    }

    if (testedTag == ToolchainConfigService.defaultLockTag) {
      warnings.add("Fast64 lock file is missing or invalid.");
    }

    return ToolchainValidationResult(
      success: errors.isEmpty,
      errors: errors,
      warnings: warnings,
      details: details,
    );
  }

  Future<Fast64ExportResult> exportSceneBundle(
    String sceneKey, {
    String? roomKey,
    String? outputRoot,
  }) async {
    final sceneKeySanitized = sceneKey.trim().toLowerCase();
    if (sceneKeySanitized.isEmpty) {
      return const Fast64ExportResult(
        success: false,
        message: "sceneKey is required.",
      );
    }

    final validation = await validateToolchain();
    if (!validation.success) {
      return Fast64ExportResult(
        success: false,
        message: "Toolchain validation failed.",
        warnings: validation.errors,
      );
    }

    final config = await toolchainConfigService.load();
    final exportScriptPath = _resolveExportScriptPath();

    final root = outputRoot == null || outputRoot.trim().isEmpty
        ? _joinPath(_resolveWorkspaceRoot(), const [
            "Scene_Editor",
            "build",
            "fast64_exports",
          ])
        : Directory(outputRoot).absolute.path;

    final timestamp = DateTime.now().toUtc().toIso8601String().replaceAll(
      RegExp(r"[^0-9]"),
      "",
    );
    final bundlePath = _joinPath(root, ["${sceneKeySanitized}_$timestamp"]);
    await Directory(bundlePath).create(recursive: true);

    final args = <String>[
      "-b",
      "--python",
      exportScriptPath,
      "--",
      "--project-root",
      config.fast64ProjectRoot,
      "--fast64-install",
      config.fast64InstallPath,
      "--scene-key",
      sceneKeySanitized,
      "--output-dir",
      bundlePath,
      "--pinned-tag",
      config.fast64PinnedTag,
    ];

    if (roomKey != null && roomKey.trim().isNotEmpty) {
      args.addAll(["--room-key", roomKey.trim().toLowerCase()]);
    }

    final result = await _processExecutor(
      config.blenderExePath,
      args,
      workingDirectory: _resolveWorkspaceRoot(),
    );

    if (result.exitCode != 0) {
      final stderr = "${result.stderr}".trim();
      return Fast64ExportResult(
        success: false,
        message: stderr.isEmpty
            ? "Fast64 export failed with exit ${result.exitCode}."
            : stderr,
        bundlePath: bundlePath,
        exitCode: result.exitCode,
      );
    }

    final sceneManifestPath = _joinPath(bundlePath, const [
      "scene_manifest.json",
    ]);
    final entranceMapPath = _joinPath(bundlePath, const ["entrance_map.json"]);

    final warnings = <String>[];
    if (!File(sceneManifestPath).existsSync()) {
      return Fast64ExportResult(
        success: false,
        message: "Fast64 export did not generate scene_manifest.json.",
        bundlePath: bundlePath,
        exitCode: result.exitCode,
      );
    }
    if (!File(entranceMapPath).existsSync()) {
      warnings.add(
        "entrance_map.json not found; using defaults during export.",
      );
    }

    final hasSceneGlb = File(
      _joinPath(bundlePath, const ["scene.glb"]),
    ).existsSync();
    final hasCollisionGlb = File(
      _joinPath(bundlePath, const ["collision.glb"]),
    ).existsSync();

    if (!hasSceneGlb) {
      warnings.add("scene.glb not found in exported bundle.");
    }
    if (!hasCollisionGlb) {
      warnings.add("collision.glb not found in exported bundle.");
    }

    return Fast64ExportResult(
      success: true,
      message: "Fast64 bundle exported.",
      bundlePath: bundlePath,
      sceneManifestPath: sceneManifestPath,
      entranceMapPath: entranceMapPath,
      exitCode: result.exitCode,
      warnings: warnings,
    );
  }

  Future<ArchiveBuildResult> buildModAssetArchive(
    String sceneKey, {
    String? bundlePath,
    String? outputArchivePath,
  }) async {
    final normalizedScene = sceneKey.trim().toLowerCase();
    if (normalizedScene.isEmpty) {
      return const ArchiveBuildResult(
        success: false,
        message: "sceneKey is required.",
      );
    }

    final resolvedBundle = bundlePath == null || bundlePath.trim().isEmpty
        ? _findLatestBundlePath(normalizedScene)
        : Directory(bundlePath).absolute.path;

    if (resolvedBundle == null || !Directory(resolvedBundle).existsSync()) {
      return ArchiveBuildResult(
        success: false,
        message: "Bundle path not found for scene '$normalizedScene'.",
      );
    }

    final sourceCandidates = <String>[
      _joinPath(resolvedBundle, const ["assets", "parkour_room.otr"]),
      _joinPath(resolvedBundle, const ["parkour_room.otr"]),
    ];

    String? source;
    for (final candidate in sourceCandidates) {
      if (File(candidate).existsSync()) {
        source = candidate;
        break;
      }
    }

    if (source == null) {
      return ArchiveBuildResult(
        success: false,
        message:
            "No parkour_room.otr found in bundle. Build the archive with your Fast64/OTR pipeline first.",
      );
    }

    final outputPath =
        outputArchivePath == null || outputArchivePath.trim().isEmpty
        ? _joinPath(_resolveWorkspaceRoot(), const [
            "docs",
            "examples",
            "external_mods",
            "kokiri_parkour",
            "assets",
            "parkour_room.otr",
          ])
        : File(outputArchivePath).absolute.path;

    final target = File(outputPath);
    await target.parent.create(recursive: true);
    await File(source).copy(target.path);

    return ArchiveBuildResult(
      success: true,
      message: "Asset archive copied from bundle.",
      outputArchivePath: target.path,
    );
  }

  String _resolveExportScriptPath() {
    final root = _resolveWorkspaceRoot();
    return _joinPath(root, const [
      "scripts",
      "fast64",
      "export_scene_bundle.py",
    ]);
  }

  String _resolveWorkspaceRoot() {
    final override = workspaceRootOverride;
    if (override != null && override.trim().isNotEmpty) {
      return Directory(override).absolute.path;
    }

    var current = Directory.current.absolute;
    for (var depth = 0; depth < 8; depth++) {
      final hasSceneEditor = Directory(
        _joinPath(current.path, const ["Scene_Editor"]),
      ).existsSync();
      final hasScripts = Directory(
        _joinPath(current.path, const ["scripts"]),
      ).existsSync();
      if (hasSceneEditor && hasScripts) {
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

  String? _findLatestBundlePath(String sceneKey) {
    final exportRoot = Directory(
      _joinPath(_resolveWorkspaceRoot(), const [
        "Scene_Editor",
        "build",
        "fast64_exports",
      ]),
    );
    if (!exportRoot.existsSync()) {
      return null;
    }

    final candidates = exportRoot
        .listSync(followLinks: false)
        .whereType<Directory>()
        .where(
          (dir) => _baseName(dir.path).toLowerCase().startsWith("${sceneKey}_"),
        )
        .toList(growable: false);

    if (candidates.isEmpty) {
      return null;
    }

    final sorted = [...candidates]
      ..sort((a, b) => b.statSync().modified.compareTo(a.statSync().modified));

    return sorted.first.absolute.path;
  }

  String _baseName(String path) {
    final normalized = path.replaceAll("\\", "/");
    final parts = normalized.split("/");
    return parts.isEmpty ? normalized : parts.last;
  }

  void _validateFilePath(
    List<String> errors,
    Map<String, String> details, {
    required String key,
    required String value,
    required bool expectsFile,
  }) {
    final trimmed = value.trim();
    details[key] = trimmed;

    if (trimmed.isEmpty) {
      errors.add("$key is not configured.");
      return;
    }

    if (expectsFile) {
      if (!File(trimmed).existsSync()) {
        errors.add("$key not found: $trimmed");
      }
      return;
    }

    if (!Directory(trimmed).existsSync()) {
      errors.add("$key not found: $trimmed");
    }
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
