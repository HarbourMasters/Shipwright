import "dart:convert";
import "dart:io";

import "package:archive/archive.dart";

class IndexedAssets {
  const IndexedAssets({
    required this.rootPath,
    required this.models,
    required this.textures,
    required this.scenes,
    required this.packages,
    required this.totalFiles,
  });

  final String rootPath;
  final List<String> models;
  final List<String> textures;
  final List<String> scenes;
  final List<String> packages;
  final int totalFiles;

  int get recognizedFiles =>
      models.length + textures.length + scenes.length + packages.length;
}

class AssetIndexerService {
  static const Set<String> _modelExtensions = {
    ".obj",
    ".gltf",
    ".glb",
    ".fbx",
    ".dae",
  };

  static const Set<String> _textureExtensions = {
    ".png",
    ".jpg",
    ".jpeg",
    ".bmp",
  };

  static const Set<String> _sceneExtensions = {".scene", ".zmap"};

  static const Set<String> _packageExtensions = {".otr", ".o2r"};

  static const String _zipExtension = ".zip";
  static const int _maxZipManifestProbeBytes = 64 * 1024 * 1024;

  static const List<String> _assetKeywordHints = [
    "soh",
    "ship",
    "harkinian",
    "oot",
    "asset",
    "copper",
  ];

  List<String> buildRomAwareCandidateRoots({
    required String romPath,
    String? currentPath,
  }) {
    final romDirectory = File(romPath).parent.path;
    final workingDirectory = currentPath ?? Directory.current.path;
    final workspaceRoot = _findWorkspaceRoot(workingDirectory);

    final rawCandidates = <String>[
      _joinPath(romDirectory, const ["soh", "assets"]),
      _joinPath(romDirectory, const ["assets"]),
      _joinPath(romDirectory, const ["x64", "Release"]),
      _joinPath(romDirectory, const ["mods"]),
      _joinPath(romDirectory, const ["external_mods"]),
      _joinPath(romDirectory, const ["examples"]),
      _joinPath(romDirectory, const ["extracted_assets"]),
      _joinPath(romDirectory, const ["oot_assets"]),
      ..._findLikelySiblingAssetDirs(romDirectory),
    ];

    if (workspaceRoot != null) {
      rawCandidates.addAll([
        _joinPath(workspaceRoot, const ["soh", "assets"]),
        _joinPath(workspaceRoot, const ["x64", "Release"]),
        _joinPath(workspaceRoot, const ["examples"]),
        _joinPath(workspaceRoot, const ["mods"]),
        _joinPath(workspaceRoot, const ["docs", "examples"]),
      ]);
    }

    return _normalizeUniquePaths(rawCandidates);
  }

  bool isRenderableModelPath(String path) {
    final lower = path.toLowerCase();
    return _modelExtensions.any(lower.endsWith);
  }

  Future<IndexedAssets> scan(String rootPath) async {
    final models = <String>[];
    final textures = <String>[];
    final scenes = <String>[];
    final packages = <String>[];
    var totalFiles = 0;

    final root = Directory(rootPath);
    if (!root.existsSync()) {
      return IndexedAssets(
        rootPath: rootPath,
        models: const [],
        textures: const [],
        scenes: const [],
        packages: const [],
        totalFiles: 0,
      );
    }

    await for (final entity in root.list(recursive: true, followLinks: false)) {
      if (entity is! File) {
        continue;
      }

      totalFiles++;
      final path = entity.path;
      final lower = path.toLowerCase();
      final fileName = entity.uri.pathSegments.isEmpty
          ? lower
          : entity.uri.pathSegments.last.toLowerCase();

      if (_hasAnyExtension(lower, _modelExtensions)) {
        models.add(path);
        continue;
      }

      if (_hasAnyExtension(lower, _textureExtensions)) {
        textures.add(path);
        continue;
      }

      if (_hasAnyExtension(lower, _sceneExtensions)) {
        scenes.add(path);
        continue;
      }

      if (_hasAnyExtension(lower, _packageExtensions)) {
        packages.add(path);
        continue;
      }

      if (lower.endsWith(_zipExtension) &&
          await _looksLikeExternalModZip(entity)) {
        packages.add(path);
        continue;
      }

      if (lower.endsWith(".xml")) {
        if (_looksLikeSceneXml(lower, fileName)) {
          scenes.add(path);
        } else if (_looksLikeTextureXml(lower, fileName)) {
          textures.add(path);
        }
      }
    }

    models.sort();
    textures.sort();
    scenes.sort();
    packages.sort();

    return IndexedAssets(
      rootPath: rootPath,
      models: models,
      textures: textures,
      scenes: scenes,
      packages: packages,
      totalFiles: totalFiles,
    );
  }

  bool _hasAnyExtension(String lowerPath, Set<String> extensions) {
    for (final extension in extensions) {
      if (lowerPath.endsWith(extension)) {
        return true;
      }
    }
    return false;
  }

  Future<bool> _looksLikeExternalModZip(File file) async {
    try {
      final size = await file.length();
      if (size <= 0 || size > _maxZipManifestProbeBytes) {
        return false;
      }

      final bytes = await file.readAsBytes();
      final archive = ZipDecoder().decodeBytes(bytes, verify: false);

      ArchiveFile? manifestFile;
      for (final entry in archive.files) {
        if (entry.isFile && _normalizeZipEntryPath(entry.name) == "mod.json") {
          manifestFile = entry;
          break;
        }
      }

      if (manifestFile == null) {
        return false;
      }

      final manifestRaw = _archiveFileAsString(manifestFile);
      final decoded = jsonDecode(manifestRaw);
      if (decoded is! Map<String, dynamic>) {
        return false;
      }

      final apiVersion = decoded["apiVersion"];
      final entryScript = decoded["entryScript"];

      final validApiVersion = apiVersion is int && apiVersion == 1;
      final validEntryScript =
          entryScript is String && entryScript.trim().isNotEmpty;

      return validApiVersion && validEntryScript;
    } catch (_) {
      return false;
    }
  }

  String _archiveFileAsString(ArchiveFile file) {
    final dynamic content = file.content;
    if (content is String) {
      return content;
    }
    if (content is List<int>) {
      return utf8.decode(content, allowMalformed: true);
    }
    return "";
  }

  String _normalizeZipEntryPath(String path) {
    var normalized = path.replaceAll("\\", "/").trim();
    while (normalized.startsWith("./")) {
      normalized = normalized.substring(2);
    }
    normalized = normalized.replaceAll(RegExp(r"^/+"), "");
    return normalized;
  }

  bool _looksLikeSceneXml(String lowerPath, String lowerFileName) {
    return lowerPath.contains("\\scenes\\") ||
        lowerPath.contains("/scenes/") ||
        lowerPath.contains("\\rooms\\") ||
        lowerPath.contains("/rooms/") ||
        lowerFileName.contains("_scene") ||
        lowerFileName.contains("scene_") ||
        lowerFileName.contains("_room");
  }

  bool _looksLikeTextureXml(String lowerPath, String lowerFileName) {
    return lowerPath.contains("\\textures\\") ||
        lowerPath.contains("/textures/") ||
        lowerPath.contains("texture") ||
        lowerFileName.contains("tex");
  }

  List<String> _findLikelySiblingAssetDirs(String rootPath) {
    final root = Directory(rootPath);
    if (!root.existsSync()) {
      return const [];
    }

    final candidates = <String>[];
    for (final entity in root.listSync(followLinks: false)) {
      if (entity is! Directory) {
        continue;
      }

      final segments = entity.path
          .split(RegExp(r"[\\/]"))
          .where((segment) => segment.isNotEmpty)
          .toList(growable: false);
      final name = (segments.isEmpty ? entity.path : segments.last)
          .toLowerCase();

      final hasSohHint =
          name.contains("soh") ||
          name.contains("ship") ||
          name.contains("harkinian");
      final hasAssetHint =
          name.contains("asset") ||
          name.contains("oot") ||
          name.contains("copper");

      if (hasSohHint && hasAssetHint) {
        candidates.add(entity.path);
        continue;
      }

      if (_assetKeywordHints.any(name.contains) && name.contains("asset")) {
        candidates.add(entity.path);
      }
    }

    return candidates;
  }

  String? _findWorkspaceRoot(String startPath) {
    var current = Directory(startPath).absolute;

    for (var depth = 0; depth < 8; depth++) {
      final hasSoh = Directory(
        _joinPath(current.path, const ["soh"]),
      ).existsSync();
      final hasSceneEditor = Directory(
        _joinPath(current.path, const ["Scene_Editor"]),
      ).existsSync();

      if (hasSoh && hasSceneEditor) {
        return current.path;
      }

      final parent = current.parent;
      if (parent.path == current.path) {
        break;
      }
      current = parent;
    }

    return null;
  }

  List<String> _normalizeUniquePaths(List<String> candidates) {
    final normalized = <String>[];
    final seen = <String>{};

    for (final candidate in candidates) {
      final trimmed = candidate.trim();
      if (trimmed.isEmpty) {
        continue;
      }

      final cleaned = trimmed
          .replaceAll("/", Platform.pathSeparator)
          .replaceAll("\\", Platform.pathSeparator);
      final canonical = Directory(cleaned).absolute.path;
      final key = Platform.isWindows ? canonical.toLowerCase() : canonical;

      if (seen.add(key)) {
        normalized.add(canonical);
      }
    }

    return normalized;
  }

  String _joinPath(String base, List<String> parts) {
    var built = base;
    for (final part in parts) {
      if (built.endsWith("/") || built.endsWith("\\")) {
        built = "$built$part";
      } else {
        built = "$built${Platform.pathSeparator}$part";
      }
    }
    return built;
  }
}
