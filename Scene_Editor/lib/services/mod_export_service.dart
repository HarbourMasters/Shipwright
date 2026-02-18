import "dart:convert";
import "dart:io";

import "package:archive/archive.dart";
import "package:file_picker/file_picker.dart";

import "../models/editable_mod_file.dart";
import "../models/rom_metadata.dart";
import "../models/scene_patch.dart";

class ModExportResult {
  const ModExportResult({
    required this.success,
    required this.message,
    this.outputPath,
  });

  final bool success;
  final String message;
  final String? outputPath;
}

class ModExportOptions {
  const ModExportOptions({
    this.includeUnchangedScenes = false,
    this.includeReadme = true,
    this.includeValidationReport = true,
    this.strictValidation = true,
    this.includeLegacyScenePatches = true,
    this.forceKokiriTemplate = true,
    this.parkourAssetArchivePath,
    this.sceneManifestPath,
    this.entryScriptPath,
    this.loadOrder = 120,
  });

  final bool includeUnchangedScenes;
  final bool includeReadme;
  final bool includeValidationReport;
  final bool strictValidation;
  final bool includeLegacyScenePatches;
  final bool forceKokiriTemplate;
  final String? parkourAssetArchivePath;
  final String? sceneManifestPath;
  final String? entryScriptPath;
  final int loadOrder;

  ModExportOptions copyWith({
    bool? includeUnchangedScenes,
    bool? includeReadme,
    bool? includeValidationReport,
    bool? strictValidation,
    bool? includeLegacyScenePatches,
    bool? forceKokiriTemplate,
    String? parkourAssetArchivePath,
    String? sceneManifestPath,
    String? entryScriptPath,
    int? loadOrder,
    bool clearParkourAssetArchivePath = false,
    bool clearSceneManifestPath = false,
    bool clearEntryScriptPath = false,
  }) {
    return ModExportOptions(
      includeUnchangedScenes:
          includeUnchangedScenes ?? this.includeUnchangedScenes,
      includeReadme: includeReadme ?? this.includeReadme,
      includeValidationReport:
          includeValidationReport ?? this.includeValidationReport,
      strictValidation: strictValidation ?? this.strictValidation,
      includeLegacyScenePatches:
          includeLegacyScenePatches ?? this.includeLegacyScenePatches,
      forceKokiriTemplate: forceKokiriTemplate ?? this.forceKokiriTemplate,
      parkourAssetArchivePath: clearParkourAssetArchivePath
          ? null
          : (parkourAssetArchivePath ?? this.parkourAssetArchivePath),
      sceneManifestPath: clearSceneManifestPath
          ? null
          : (sceneManifestPath ?? this.sceneManifestPath),
      entryScriptPath: clearEntryScriptPath
          ? null
          : (entryScriptPath ?? this.entryScriptPath),
      loadOrder: loadOrder ?? this.loadOrder,
    );
  }

  Map<String, dynamic> toJson() => {
    "includeUnchangedScenes": includeUnchangedScenes,
    "includeReadme": includeReadme,
    "includeValidationReport": includeValidationReport,
    "strictValidation": strictValidation,
    "includeLegacyScenePatches": includeLegacyScenePatches,
    "forceKokiriTemplate": forceKokiriTemplate,
    "hasParkourAssetArchivePath":
        parkourAssetArchivePath != null && parkourAssetArchivePath!.isNotEmpty,
    "hasSceneManifestPath":
        sceneManifestPath != null && sceneManifestPath!.isNotEmpty,
    "hasEntryScriptPath":
        entryScriptPath != null && entryScriptPath!.isNotEmpty,
    "loadOrder": loadOrder,
  };
}

class _ModExportValidationReport {
  const _ModExportValidationReport({
    required this.errors,
    required this.warnings,
    required this.notes,
  });

  final List<String> errors;
  final List<String> warnings;
  final List<String> notes;

  bool get hasErrors => errors.isNotEmpty;

  Map<String, dynamic> toJson() => {
    "errorCount": errors.length,
    "warningCount": warnings.length,
    "errors": errors,
    "warnings": warnings,
    "notes": notes,
  };
}

class _ResolvedRuntimeArtifacts {
  const _ResolvedRuntimeArtifacts({
    required this.entryScript,
    required this.sceneManifest,
    required this.entranceMap,
    required this.assetArchiveBytes,
    required this.assetArchiveSource,
  });

  final Map<String, dynamic> entryScript;
  final Map<String, dynamic> sceneManifest;
  final Map<String, dynamic> entranceMap;
  final List<int>? assetArchiveBytes;
  final String? assetArchiveSource;

  bool get hasAssetArchive =>
      assetArchiveBytes != null && assetArchiveBytes!.isNotEmpty;
}

class ModExportService {
  Future<ModExportResult> exportModZip({
    required String modName,
    required RomMetadata rom,
    required List<ScenePatch> scenes,
    required List<EditableModFile> editableFiles,
    ModExportOptions options = const ModExportOptions(),
    String? outputPathOverride,
  }) async {
    final normalizedModName = _sanitizeName(modName);
    final modId = normalizedModName.isEmpty
        ? "external_mod"
        : normalizedModName;

    final scenesToExport = _normalizeScenesForExport(
      scenes,
      includeUnchangedScenes: options.includeUnchangedScenes,
    );
    final changedScenes = scenes
        .where((scene) => scene.hasChanges)
        .toList(growable: false);
    final normalizedFiles = _normalizeEditableFiles(editableFiles);

    final runtimeArtifacts = await _resolveRuntimeArtifacts(
      scenesToExport: scenesToExport,
      options: options,
    );

    final validation = _validateExport(
      rom: rom,
      modName: modName,
      modId: modId,
      scenesToExport: scenesToExport,
      changedScenes: changedScenes,
      normalizedFiles: normalizedFiles,
      runtimeArtifacts: runtimeArtifacts,
      options: options,
    );

    if (options.strictValidation && validation.hasErrors) {
      return ModExportResult(
        success: false,
        message: "Export blocked by validation: ${validation.errors.first}",
      );
    }

    final outputPath =
        outputPathOverride ??
        await FilePicker.platform.saveFile(
          dialogTitle: "Export External Mod ZIP",
          fileName: "$modId.zip",
          type: FileType.custom,
          allowedExtensions: const ["zip"],
        );

    if (outputPath == null || outputPath.isEmpty) {
      return const ModExportResult(success: false, message: "Export canceled.");
    }

    final archive = Archive();
    final nowUtc = DateTime.now().toUtc().toIso8601String();
    final encoder = const JsonEncoder.withIndent("  ");

    final modManifest = <String, dynamic>{
      "id": "com.scene_editor.$modId",
      "name": modName.trim().isEmpty ? modId : modName.trim(),
      "version": "0.1.0",
      "apiVersion": 1,
      "loadOrder": options.loadOrder,
      "entryScript": "scripts/init.json",
      "assets": runtimeArtifacts.hasAssetArchive
          ? <String>["assets/parkour_room.otr"]
          : <String>[],
      "dependencies": const <String>[],
      "generatedAtUtc": nowUtc,
      "generatedBy": "Scene Editor MVP",
      "rom": {
        "title": rom.title,
        "gameCode": rom.gameCode,
        "versionByte": rom.version,
        "sha1": rom.sha1,
      },
      "validation": validation.toJson(),
      "exportOptions": options.toJson(),
    };

    archive.addFile(
      ArchiveFile.string("mod.json", encoder.convert(modManifest)),
    );

    archive.addFile(
      ArchiveFile.string(
        "scripts/init.json",
        encoder.convert(runtimeArtifacts.entryScript),
      ),
    );

    archive.addFile(
      ArchiveFile.string(
        "assets/scene_manifest.json",
        encoder.convert(runtimeArtifacts.sceneManifest),
      ),
    );

    archive.addFile(
      ArchiveFile.string(
        "assets/entrance_map.json",
        encoder.convert(runtimeArtifacts.entranceMap),
      ),
    );

    if (runtimeArtifacts.hasAssetArchive) {
      final bytes = runtimeArtifacts.assetArchiveBytes!;
      archive.addFile(
        ArchiveFile("assets/parkour_room.otr", bytes.length, bytes),
      );
    } else {
      archive.addFile(
        ArchiveFile.string(
          "assets/README.txt",
          "Missing parkour_room.otr. Provide ModExportOptions.parkourAssetArchivePath.",
        ),
      );
    }

    if (options.includeLegacyScenePatches) {
      final sceneIndex = <String, dynamic>{
        "sceneCount": scenesToExport.length,
        "changedSceneCount": changedScenes.length,
        "scenes": scenesToExport
            .map((scene) => scene.sceneId)
            .toList(growable: false),
      };
      archive.addFile(
        ArchiveFile.string("scenes/index.json", encoder.convert(sceneIndex)),
      );

      for (final scene in scenesToExport) {
        archive.addFile(
          ArchiveFile.string(
            "scenes/${scene.sceneId}.json",
            encoder.convert(scene.toJson()),
          ),
        );
      }

      if (normalizedFiles.isNotEmpty) {
        final fileIndex = <String, dynamic>{
          "fileCount": normalizedFiles.length,
          "files": normalizedFiles
              .map(
                (file) => {
                  "relativePath": file.relativePath,
                  "contentLength": file.content.length,
                },
              )
              .toList(growable: false),
        };

        archive.addFile(
          ArchiveFile.string(
            "file_patches/index.json",
            encoder.convert(fileIndex),
          ),
        );

        for (final file in normalizedFiles) {
          archive.addFile(
            ArchiveFile.string(
              "file_patches/files/${file.relativePath}",
              file.content,
            ),
          );
        }
      }
    }

    if (options.includeValidationReport) {
      final validationReport = <String, dynamic>{
        "generatedAtUtc": nowUtc,
        "modId": "com.scene_editor.$modId",
        "rom": {
          "title": rom.title,
          "gameCode": rom.gameCode,
          "versionByte": rom.version,
          "sha1": rom.sha1,
        },
        "scenePatchCount": scenesToExport.length,
        "changedSceneCount": changedScenes.length,
        "filePatchCount": normalizedFiles.length,
        "runtime": {
          "entryScriptPath": options.entryScriptPath,
          "sceneManifestPath": options.sceneManifestPath,
          "assetArchivePath": runtimeArtifacts.assetArchiveSource,
        },
        "validation": validation.toJson(),
      };
      archive.addFile(
        ArchiveFile.string(
          "reports/validation_report.json",
          encoder.convert(validationReport),
        ),
      );
    }

    if (options.includeReadme) {
      archive.addFile(
        ArchiveFile.string(
          "README_MOD.txt",
          _buildReadme(
            modName: modName,
            modId: modId,
            changedSceneCount: changedScenes.length,
            exportedSceneCount: scenesToExport.length,
            filePatchCount: normalizedFiles.length,
            hasAssetArchive: runtimeArtifacts.hasAssetArchive,
          ),
        ),
      );
    }

    final zipData = ZipEncoder().encode(archive);
    if (zipData.isEmpty) {
      return const ModExportResult(
        success: false,
        message: "Failed to encode ZIP archive.",
      );
    }

    final outFile = File(outputPath);
    await outFile.create(recursive: true);
    await outFile.writeAsBytes(zipData, flush: true);

    final warningSuffix = validation.warnings.isNotEmpty
        ? " Warnings: ${validation.warnings.length}."
        : "";

    return ModExportResult(
      success: true,
      message:
          "Mod ZIP exported with runtime layout (${runtimeArtifacts.hasAssetArchive ? "asset+logic" : "logic-only"}).$warningSuffix",
      outputPath: outputPath,
    );
  }

  Future<_ResolvedRuntimeArtifacts> _resolveRuntimeArtifacts({
    required List<ScenePatch> scenesToExport,
    required ModExportOptions options,
  }) async {
    final entryScript = await _resolveEntryScript(
      scenesToExport: scenesToExport,
      options: options,
    );

    final sceneManifest = await _resolveSceneManifest(
      scenesToExport: scenesToExport,
      options: options,
    );

    final entranceMap = await _resolveEntranceMap(
      scenesToExport: scenesToExport,
      entryScript: entryScript,
      options: options,
    );

    final assetPath = options.parkourAssetArchivePath;
    List<int>? assetBytes;
    String? assetSource;

    if (assetPath != null && assetPath.trim().isNotEmpty) {
      final file = File(assetPath);
      if (file.existsSync()) {
        assetBytes = await file.readAsBytes();
        assetSource = file.absolute.path;
      }
    }

    return _ResolvedRuntimeArtifacts(
      entryScript: entryScript,
      sceneManifest: sceneManifest,
      entranceMap: entranceMap,
      assetArchiveBytes: assetBytes,
      assetArchiveSource: assetSource,
    );
  }

  Future<Map<String, dynamic>> _resolveEntryScript({
    required List<ScenePatch> scenesToExport,
    required ModExportOptions options,
  }) async {
    final overridePath = options.entryScriptPath;
    if (overridePath != null && overridePath.trim().isNotEmpty) {
      final file = File(overridePath);
      if (file.existsSync()) {
        final raw = await file.readAsString();
        final decoded = jsonDecode(raw);
        if (decoded is Map<String, dynamic>) {
          return decoded;
        }
      }
    }

    if (options.forceKokiriTemplate || scenesToExport.isEmpty) {
      return _buildKokiriTemplateEntryScript();
    }

    final triggers = <Map<String, dynamic>>[];
    final onSceneInit = <Map<String, dynamic>>[];

    for (final scene in scenesToExport) {
      final sceneAlias = _sceneAliasForId(scene.sceneId);
      onSceneInit.add({
        "scene": sceneAlias,
        "actions": [
          {
            "action": "showNotification",
            "text": "Loaded scene ${scene.sceneName}",
          },
        ],
      });

      for (var i = 0; i < scene.doors.length; i++) {
        final door = scene.doors[i];
        final bounds = i < scene.collisions.length
            ? scene.collisions[i]
            : _boundsFromDoor(door);

        final destination =
            _entranceAliasForScene(door.targetSceneId) ?? door.targetSpawn;

        triggers.add({
          "id": door.id.isEmpty ? "${scene.sceneId}_door_$i" : door.id,
          "scene": sceneAlias,
          "bounds": {
            "min": [bounds.min.x, bounds.min.y, bounds.min.z],
            "max": [bounds.max.x, bounds.max.y, bounds.max.z],
          },
          "cooldownFrames": 90,
          "actions": [
            {"action": "teleportToEntrance", "entrance": destination},
          ],
        });
      }
    }

    if (triggers.isEmpty) {
      return _buildKokiriTemplateEntryScript();
    }

    return {
      "apiVersion": 1,
      "onGameLoaded": [
        {"action": "showNotification", "text": "External mod loaded."},
      ],
      "onSceneInit": onSceneInit,
      "onFrameTriggers": triggers,
    };
  }

  Future<Map<String, dynamic>> _resolveSceneManifest({
    required List<ScenePatch> scenesToExport,
    required ModExportOptions options,
  }) async {
    final overridePath = options.sceneManifestPath;
    if (overridePath != null && overridePath.trim().isNotEmpty) {
      final file = File(overridePath);
      if (file.existsSync()) {
        final raw = await file.readAsString();
        final decoded = jsonDecode(raw);
        if (decoded is Map<String, dynamic>) {
          return decoded;
        }
      }
    }

    final sceneIds =
        scenesToExport
            .map((scene) => scene.sceneId.trim())
            .where((id) => id.isNotEmpty)
            .toSet()
            .toList(growable: false)
          ..sort();

    final mainScene = sceneIds.isEmpty ? "kokiri_forest" : sceneIds.first;

    return {
      "schemaVersion": 1,
      "sceneKey": mainScene,
      "sceneRoots": sceneIds,
      "generatedAtUtc": DateTime.now().toUtc().toIso8601String(),
    };
  }

  Future<Map<String, dynamic>> _resolveEntranceMap({
    required List<ScenePatch> scenesToExport,
    required Map<String, dynamic> entryScript,
    required ModExportOptions options,
  }) async {
    final manifestPath = options.sceneManifestPath;
    if (manifestPath != null && manifestPath.trim().isNotEmpty) {
      final sibling = File(manifestPath).parent.path;
      final candidate = File(
        "$sibling${Platform.pathSeparator}entrance_map.json",
      );
      if (candidate.existsSync()) {
        final raw = await candidate.readAsString();
        final decoded = jsonDecode(raw);
        if (decoded is Map<String, dynamic>) {
          return decoded;
        }
      }
    }

    final entries = <Map<String, dynamic>>[];

    final triggers = entryScript["onFrameTriggers"];
    if (triggers is List) {
      for (final item in triggers) {
        if (item is! Map<String, dynamic>) {
          continue;
        }

        final actions = item["actions"];
        if (actions is! List) {
          continue;
        }

        for (final action in actions) {
          if (action is! Map<String, dynamic>) {
            continue;
          }
          if (action["action"] != "teleportToEntrance") {
            continue;
          }
          entries.add({
            "triggerId": item["id"],
            "scene": item["scene"],
            "entrance": action["entrance"],
          });
        }
      }
    }

    if (entries.isEmpty) {
      for (final scene in scenesToExport) {
        for (final door in scene.doors) {
          entries.add({
            "triggerId": door.id,
            "scene": _sceneAliasForId(scene.sceneId),
            "entrance":
                _entranceAliasForScene(door.targetSceneId) ?? door.targetSpawn,
          });
        }
      }
    }

    return {
      "schemaVersion": 1,
      "generatedAtUtc": DateTime.now().toUtc().toIso8601String(),
      "entries": entries,
    };
  }

  _ModExportValidationReport _validateExport({
    required RomMetadata rom,
    required String modName,
    required String modId,
    required List<ScenePatch> scenesToExport,
    required List<ScenePatch> changedScenes,
    required List<EditableModFile> normalizedFiles,
    required _ResolvedRuntimeArtifacts runtimeArtifacts,
    required ModExportOptions options,
  }) {
    final errors = <String>[];
    final warnings = <String>[];
    final notes = <String>[];

    if (modName.trim().isEmpty) {
      warnings.add("Mod name was empty. Using fallback id '$modId'.");
    }

    if (!rom.looksLikeUsa11) {
      warnings.add(
        "Loaded ROM header does not match expected OOT USA 1.1 profile.",
      );
    }

    if (options.strictValidation && !runtimeArtifacts.hasAssetArchive) {
      errors.add(
        "Missing assets/parkour_room.otr. Set ModExportOptions.parkourAssetArchivePath.",
      );
    } else if (!runtimeArtifacts.hasAssetArchive) {
      warnings.add("Exporting logic-only ZIP without parkour_room.otr asset.");
    }

    if (runtimeArtifacts.entryScript["apiVersion"] != 1) {
      errors.add("scripts/init.json apiVersion must be 1.");
    }

    final frameTriggers = runtimeArtifacts.entryScript["onFrameTriggers"];
    if (frameTriggers is! List || frameTriggers.isEmpty) {
      warnings.add("Entry script has no onFrameTriggers.");
    }

    if (runtimeArtifacts.sceneManifest["sceneKey"] == null) {
      errors.add("scene_manifest.json must include sceneKey.");
    }

    if (scenesToExport.isEmpty && normalizedFiles.isEmpty) {
      notes.add("No legacy scene/file patch payload included.");
    }

    if (changedScenes.isEmpty && scenesToExport.isNotEmpty) {
      notes.add("No scene changes detected; exporting scene templates only.");
    }

    final sceneIdSet = <String>{};
    for (final scene in scenesToExport) {
      final sceneId = scene.sceneId.trim();
      if (sceneId.isEmpty) {
        errors.add("A scene has empty sceneId.");
        continue;
      }
      final key = sceneId.toLowerCase();
      if (!sceneIdSet.add(key)) {
        warnings.add(
          "Duplicate scene id '$sceneId' detected. Last entry was kept.",
        );
      }
    }

    for (final file in normalizedFiles) {
      if (file.content.length > 2 * 1024 * 1024) {
        warnings.add("File patch '${file.relativePath}' is larger than 2MB.");
      }
    }

    return _ModExportValidationReport(
      errors: errors,
      warnings: warnings,
      notes: notes,
    );
  }

  List<ScenePatch> _normalizeScenesForExport(
    List<ScenePatch> scenes, {
    required bool includeUnchangedScenes,
  }) {
    final selected = includeUnchangedScenes
        ? scenes
        : scenes.where((scene) => scene.hasChanges).toList(growable: false);

    final deduped = <String, ScenePatch>{};
    for (final scene in selected) {
      final sceneId = scene.sceneId.trim();
      if (sceneId.isEmpty) {
        continue;
      }
      deduped[sceneId.toLowerCase()] = scene.copyWith(sceneId: sceneId);
    }

    final normalized = deduped.values.toList(growable: false)
      ..sort(
        (a, b) => a.sceneId.toLowerCase().compareTo(b.sceneId.toLowerCase()),
      );
    return normalized;
  }

  List<EditableModFile> _normalizeEditableFiles(List<EditableModFile> files) {
    final deduped = <String, EditableModFile>{};
    for (final file in files) {
      if (file.content.isEmpty) {
        continue;
      }
      final relativePath = _sanitizeArchiveRelativePath(file.relativePath);
      deduped[relativePath.toLowerCase()] = file.copyWith(
        relativePath: relativePath,
      );
    }

    final sorted = deduped.values.toList(growable: false)
      ..sort(
        (a, b) => a.relativePath.toLowerCase().compareTo(
          b.relativePath.toLowerCase(),
        ),
      );
    return sorted;
  }

  String _sanitizeArchiveRelativePath(String path) {
    var normalized = path.replaceAll("\\", "/").trim();
    normalized = normalized.replaceAll(RegExp(r"^/+"), "");

    final parts = normalized
        .split("/")
        .where((part) => part.isNotEmpty && part != "." && part != "..")
        .toList(growable: false);

    if (parts.isEmpty) {
      return "unnamed.txt";
    }

    return parts.join("/");
  }

  String _sanitizeName(String input) {
    final cleaned = input.trim().toLowerCase().replaceAll(
      RegExp(r"[^a-z0-9]+"),
      "_",
    );
    return cleaned
        .replaceAll(RegExp(r"_+"), "_")
        .replaceAll(RegExp(r"^_|_$"), "");
  }

  Map<String, dynamic> _buildKokiriTemplateEntryScript() {
    return {
      "apiVersion": 1,
      "onGameLoaded": [
        {"action": "showNotification", "text": "Kokiri Parkour mod active."},
      ],
      "onSceneInit": [
        {
          "scene": "SCENE_KOKIRI_FOREST",
          "actions": [
            {
              "action": "showNotification",
              "text": "Enter behind Link's house to reach the parkour room.",
            },
          ],
        },
        {
          "scene": "SCENE_LINKS_HOUSE",
          "actions": [
            {
              "action": "showNotification",
              "text": "Use portals to Kokiri or Kakariko.",
            },
          ],
        },
      ],
      "onFrameTriggers": [
        {
          "id": "kokiri_backdoor_to_parkour",
          "scene": "SCENE_KOKIRI_FOREST",
          "bounds": {
            "min": [-1040.0, 350.0, -1325.0],
            "max": [-880.0, 520.0, -1160.0],
          },
          "cooldownFrames": 90,
          "actions": [
            {
              "action": "teleportToEntrance",
              "entrance": "ENTR_LINKS_HOUSE_CHILD_SPAWN",
            },
          ],
        },
        {
          "id": "parkour_exit_to_kokiri",
          "scene": "SCENE_LINKS_HOUSE",
          "bounds": {
            "min": [-90.0, 0.0, 150.0],
            "max": [90.0, 180.0, 260.0],
          },
          "cooldownFrames": 90,
          "actions": [
            {
              "action": "teleportToEntrance",
              "entrance": "ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE",
            },
          ],
        },
        {
          "id": "parkour_exit_to_kakariko",
          "scene": "SCENE_LINKS_HOUSE",
          "bounds": {
            "min": [180.0, 0.0, -30.0],
            "max": [320.0, 220.0, 120.0],
          },
          "cooldownFrames": 90,
          "actions": [
            {
              "action": "teleportToEntrance",
              "entrance": "ENTR_KAKARIKO_VILLAGE_FRONT_GATE",
            },
          ],
        },
      ],
    };
  }

  CollisionBox _boundsFromDoor(SceneDoorLink door) {
    const pad = 80.0;
    return CollisionBox(
      id: "auto_${door.id}",
      name: "Auto bounds for ${door.name}",
      min: Vec3(
        x: door.position.x - pad,
        y: door.position.y - pad,
        z: door.position.z - pad,
      ),
      max: Vec3(
        x: door.position.x + pad,
        y: door.position.y + pad,
        z: door.position.z + pad,
      ),
      climbable: false,
      lethal: false,
    );
  }

  String _sceneAliasForId(String sceneId) {
    switch (sceneId.trim().toLowerCase()) {
      case "kokiri_forest":
      case "spot04":
      case "spot04_scene":
        return "SCENE_KOKIRI_FOREST";
      case "links_house":
      case "links_house_interior":
      case "link_home":
        return "SCENE_LINKS_HOUSE";
      case "kakariko_village":
      case "spot01":
      case "spot01_scene":
        return "SCENE_KAKARIKO_VILLAGE";
      default:
        return sceneId;
    }
  }

  Object? _entranceAliasForScene(String sceneId) {
    switch (sceneId.trim().toLowerCase()) {
      case "kokiri_forest":
      case "spot04":
      case "spot04_scene":
        return "ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE";
      case "links_house":
      case "links_house_interior":
      case "link_home":
        return "ENTR_LINKS_HOUSE_CHILD_SPAWN";
      case "kakariko_village":
      case "spot01":
      case "spot01_scene":
        return "ENTR_KAKARIKO_VILLAGE_FRONT_GATE";
      default:
        return null;
    }
  }

  String _buildReadme({
    required String modName,
    required String modId,
    required int changedSceneCount,
    required int exportedSceneCount,
    required int filePatchCount,
    required bool hasAssetArchive,
  }) {
    final displayName = modName.trim().isEmpty ? modId : modName.trim();
    return """
$displayName
========================

Mod ID: com.scene_editor.$modId
Generated by Scene Editor MVP.

Contents
- Runtime script: scripts/init.json
- Scene manifest: assets/scene_manifest.json
- Entrance map: assets/entrance_map.json
- Asset archive: ${hasAssetArchive ? "assets/parkour_room.otr" : "(missing - logic only)"}
- Legacy scenes exported: $exportedSceneCount
- Scenes with changes: $changedSceneCount
- File patches: $filePatchCount

Install
1. Copy this ZIP into Shipwright's mods folder.
2. Start Shipwright and load a save.
3. Check reports/validation_report.json for warnings.

Required runtime entries
- mod.json
- scripts/init.json
- assets/parkour_room.otr
- reports/validation_report.json
- README_MOD.txt
""";
  }
}
