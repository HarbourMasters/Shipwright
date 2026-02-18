import "dart:async";
import "dart:io";
import "dart:math" as math;
import "dart:ui" as ui;
import "package:file_picker/file_picker.dart";
import "package:flutter/gestures.dart";
import "package:flutter/material.dart";
import "package:flutter/services.dart";
import "package:flutter_cube/flutter_cube.dart" as cube;

import "../models/editor_entity.dart";
import "../models/scene_patch.dart";
import "../services/asset_indexer_service.dart";
import "../services/o2r_collision_importer_service.dart";
import "../services/o2r_scene_mesh_importer_service.dart";

class SceneStudio3DScreen extends StatefulWidget {
  const SceneStudio3DScreen({
    super.key,
    required this.scene,
    required this.allSceneIds,
    this.romPath,
  });

  final ScenePatch scene;
  final List<String> allSceneIds;
  final String? romPath;

  @override
  State<SceneStudio3DScreen> createState() => _SceneStudio3DScreenState();
}

class _SceneStudio3DScreenState extends State<SceneStudio3DScreen> {
  static const String _allSceneRootOption = "__all_scene_roots__";

  final AssetIndexerService _assetIndexerService = AssetIndexerService();
  final O2rCollisionImporterService _o2rCollisionImporterService =
      O2rCollisionImporterService();
  final O2rSceneMeshImporterService _o2rSceneMeshImporterService =
      O2rSceneMeshImporterService();
  final TextEditingController _assetFilterController = TextEditingController();

  late List<EditorEntity> _entities;
  final Map<String, cube.Object> _renderObjects = <String, cube.Object>{};
  final Set<String> _selectedSceneDefs = <String>{};
  List<O2rCollisionMesh> _romCollisionMeshes = const <O2rCollisionMesh>[];
  List<O2rSceneMesh> _romSceneMeshes = const <O2rSceneMesh>[];
  Map<int, O2rSceneTexture> _romSceneTextures = const <int, O2rSceneTexture>{};
  final Map<int, ui.Image> _romSceneTextureImages = <int, ui.Image>{};
  final Set<int> _loadingTextureHashes = <int>{};
  String? _romCollisionPackagePath;
  String? _romCollisionSceneKey;
  String? _romScenePackagePath;
  bool _showCollisionOverlay = true;
  bool _sanitizeSceneGeometry = true;
  List<String> _romSceneRootPaths = const <String>[];
  String _selectedSceneRootPath = _allSceneRootOption;

  late String _previewSceneId;

  double _scaleGestureStartZoom = 1.0;
  double _pointerPanZoomStartZoom = 1.0;

  IndexedAssets? _indexedAssets;
  cube.Scene? _cubeScene;
  int _selectedIndex = -1;
  String _status =
      "Load a ROM in the home screen, then 3D Studio will auto-scan nearby assets.";
  bool _loadingAssets = false;

  EditorEntity? get _selectedEntity {
    if (_selectedIndex < 0 || _selectedIndex >= _entities.length) {
      return null;
    }
    return _entities[_selectedIndex];
  }

  @override
  void initState() {
    super.initState();
    _previewSceneId = widget.scene.sceneId;
    _entities = _entitiesFromScene(widget.scene);
    for (final asset in widget.scene.assets) {
      if (asset.type == "scene") {
        _selectedSceneDefs.add(asset.path);
      }
    }
    if (_entities.isNotEmpty) {
      _selectedIndex = 0;
    }
    _assetFilterController.addListener(() {
      if (mounted) {
        setState(() {});
      }
    });

    _tryLoadFromRomContextOnOpen();
  }

  @override
  void dispose() {
    _clearTextureImageCache();
    _assetFilterController.dispose();
    super.dispose();
  }

  String _safeCurrentDirectoryPath() {
    try {
      return Directory.current.path;
    } catch (_) {
      return File(Platform.resolvedExecutable).parent.path;
    }
  }

  String _resolveDefaultBasePath() {
    final candidates = _buildDefaultRootCandidates();
    for (final candidate in candidates) {
      if (Directory(candidate).existsSync()) {
        return Directory(candidate).absolute.path;
      }
    }

    return _safeCurrentDirectoryPath();
  }

  List<String> _buildDefaultRootCandidates() {
    final candidates = <String>[];

    void addCandidate(String? path) {
      if (path == null || path.trim().isEmpty) {
        return;
      }
      candidates.add(path);
    }

    final romPath = widget.romPath;
    if (romPath != null && romPath.trim().isNotEmpty) {
      final romParent = File(romPath).parent.path;
      addCandidate(romParent);
      addCandidate(_findWorkspaceRootNear(romParent));
    }

    final executableDir = File(Platform.resolvedExecutable).parent.path;
    addCandidate(executableDir);
    addCandidate(_findWorkspaceRootNear(executableDir));

    final currentDir = _safeCurrentDirectoryPath();
    addCandidate(currentDir);
    addCandidate(_findWorkspaceRootNear(currentDir));

    return _normalizeUniquePaths(candidates);
  }

  String? _findWorkspaceRootNear(String startPath) {
    var current = Directory(startPath).absolute;

    for (var depth = 0; depth < 8; depth++) {
      final hasSoh = Directory(
        _joinPath(current.path, const ["soh"]),
      ).existsSync();
      final hasSceneEditor = Directory(
        _joinPath(current.path, const ["Scene_Editor"]),
      ).existsSync();
      final hasRelease = Directory(
        _joinPath(current.path, const ["x64", "Release"]),
      ).existsSync();

      if (hasSoh && (hasSceneEditor || hasRelease)) {
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

  List<String> _buildExamplesCandidates() {
    final candidates = <String>[];

    for (final root in _buildDefaultRootCandidates()) {
      var current = Directory(root).absolute;
      for (var depth = 0; depth < 4; depth++) {
        candidates.add(_joinPath(current.path, const ["examples"]));
        candidates.add(_joinPath(current.path, const ["docs", "examples"]));

        final parent = current.parent;
        if (parent.path == current.path) {
          break;
        }
        current = parent;
      }
    }

    return _normalizeUniquePaths(candidates);
  }

  Future<void> _pickAssetsFolder() async {
    final path = await FilePicker.platform.getDirectoryPath(
      dialogTitle: "Select extracted assets root folder",
      initialDirectory: _resolveDefaultBasePath(),
    );
    if (path == null || path.isEmpty || !mounted) {
      return;
    }

    await _scanAssetsFolder(path);
  }

  Future<void> _tryLoadFromRomContextOnOpen() async {
    if (!mounted) {
      return;
    }

    final romPath = widget.romPath;
    if (romPath == null || romPath.trim().isEmpty) {
      await _loadRepositoryExamplesFolder();
      return;
    }

    await _loadFromRomContext();
  }

  Future<void> _loadFromRomContext() async {
    final romPath = widget.romPath;
    if (romPath == null || romPath.trim().isEmpty) {
      setState(() {
        _status = "No ROM path provided to 3D Studio.";
      });
      return;
    }

    final candidates = _assetIndexerService.buildRomAwareCandidateRoots(
      romPath: romPath,
      currentPath: _resolveDefaultBasePath(),
    );
    await _scanBestCandidateFolders(candidates);
  }

  Future<void> _loadRepositoryExamplesFolder() async {
    final candidates = _buildExamplesCandidates();

    for (final candidate in candidates) {
      final dir = Directory(candidate);
      if (dir.existsSync()) {
        await _scanAssetsFolder(dir.absolute.path);
        return;
      }
    }

    setState(() {
      _status =
          "Could not find an examples folder near ${_resolveDefaultBasePath()}.";
    });
  }

  Future<void> _scanBestCandidateFolders(List<String> candidates) async {
    final existingCandidates = candidates
        .where((path) => Directory(path).existsSync())
        .toList(growable: false);

    if (existingCandidates.isEmpty) {
      _clearTextureImageCache();
      setState(() {
        _status = "No ROM-adjacent asset folders found.";
        _indexedAssets = null;
        _romCollisionMeshes = const <O2rCollisionMesh>[];
        _romSceneMeshes = const <O2rSceneMesh>[];
        _romSceneTextures = const <int, O2rSceneTexture>{};
        _romCollisionPackagePath = null;
        _romCollisionSceneKey = null;
        _romScenePackagePath = null;
        _showCollisionOverlay = true;
        _romSceneRootPaths = const <String>[];
        _selectedSceneRootPath = _allSceneRootOption;
      });
      _syncRenderObjects();
      return;
    }

    setState(() {
      _loadingAssets = true;
      _status =
          "Scanning ${existingCandidates.length} ROM-related folder(s)...";
    });

    try {
      IndexedAssets? best;
      var bestScore = -1;

      for (final candidate in existingCandidates) {
        final indexed = await _assetIndexerService.scan(candidate);
        final score = _assetScore(indexed);

        if (score > bestScore) {
          best = indexed;
          bestScore = score;
        }
      }

      if (!mounted) {
        return;
      }

      if (best == null) {
        _clearTextureImageCache();
        setState(() {
          _status = "Could not index ROM-adjacent folders.";
          _indexedAssets = null;
          _romCollisionMeshes = const <O2rCollisionMesh>[];
          _romSceneMeshes = const <O2rSceneMesh>[];
          _romSceneTextures = const <int, O2rSceneTexture>{};
          _romCollisionPackagePath = null;
          _romCollisionSceneKey = null;
          _romScenePackagePath = null;
          _showCollisionOverlay = true;
          _romSceneRootPaths = const <String>[];
          _selectedSceneRootPath = _allSceneRootOption;
          _romSceneRootPaths = const <String>[];
          _selectedSceneRootPath = _allSceneRootOption;
        });
        _syncRenderObjects();
        return;
      }

      await _applyIndexedAssets(indexed: best, fromRomContext: true);
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Failed to scan ROM context: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _loadingAssets = false;
        });
      }
    }
  }

  int _assetScore(IndexedAssets indexed) {
    final o2rCount = indexed.packages
        .where((path) => path.toLowerCase().endsWith(".o2r"))
        .length;

    final modelScore = math.min(indexed.models.length, 8000) * 5;
    final textureScore = math.min(indexed.textures.length, 12000) * 2;
    final sceneScore = math.min(indexed.scenes.length, 5000) * 3;
    final packageScore = math.min(indexed.packages.length, 3000) * 8;
    final o2rScore = math.min(o2rCount, 200) * 120;

    return _pathAffinityScore(indexed.rootPath) +
        modelScore +
        textureScore +
        sceneScore +
        packageScore +
        o2rScore;
  }

  int _pathAffinityScore(String path) {
    final lower = path.toLowerCase().replaceAll("/", "\\");
    var score = 0;

    if (lower.contains("\\shipwright\\") || lower.endsWith("\\shipwright")) {
      score += 120000;
    }
    if (lower.contains("\\soh\\assets")) {
      score += 90000;
    }
    if (lower.contains("\\x64\\release")) {
      score += 70000;
    }
    if (lower.contains("\\mods\\")) {
      score += 12000;
    }

    if (lower.contains("\\downloads\\")) {
      score -= 70000;
    }
    if (lower.contains("\\onedrive\\") && lower.contains("\\downloads\\")) {
      score -= 30000;
    }
    if (lower.contains("copper") && !lower.contains("shipwright")) {
      score -= 40000;
    }

    return score;
  }

  Future<void> _scanAssetsFolder(String path) async {
    setState(() {
      _loadingAssets = true;
      _status = "Scanning assets in $path...";
    });

    try {
      final indexed = await _assetIndexerService.scan(path);
      if (!mounted) {
        return;
      }

      await _applyIndexedAssets(indexed: indexed, fromRomContext: false);
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Failed to index assets: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _loadingAssets = false;
        });
      }
    }
  }

  Future<void> _applyIndexedAssets({
    required IndexedAssets indexed,
    required bool fromRomContext,
  }) async {
    final packagePath = _pickCollisionPackage(indexed);

    final collisionFuture = packagePath == null
        ? Future<O2rCollisionLoadResult?>.value(null)
        : _o2rCollisionImporterService.loadSceneCollision(
            packagePath: packagePath,
            sceneId: _previewSceneId,
          );

    final sceneMeshFuture = packagePath == null
        ? Future<O2rSceneMeshLoadResult?>.value(null)
        : _o2rSceneMeshImporterService.loadSceneGeometry(
            packagePath: packagePath,
            sceneId: _previewSceneId,
            sanitizeGeometry: _sanitizeSceneGeometry,
          );

    final collisionPreview = await collisionFuture;
    final sceneMeshPreview = await sceneMeshFuture;

    if (!mounted) {
      return;
    }

    final nextSceneMeshes = sceneMeshPreview?.meshes ?? const <O2rSceneMesh>[];
    final nextSceneTextures =
        sceneMeshPreview?.texturesByHash ?? const <int, O2rSceneTexture>{};
    final nextSceneRootPaths = _collectSceneRootPaths(nextSceneMeshes);
    final nextSceneRootSelection = _resolveSceneRootSelection(
      nextSceneRootPaths,
    );

    _replaceTextureImageCache(nextSceneTextures);

    setState(() {
      _indexedAssets = indexed;
      _romCollisionMeshes =
          collisionPreview?.meshes ?? const <O2rCollisionMesh>[];
      _romSceneMeshes = nextSceneMeshes;
      _romSceneTextures = nextSceneTextures;
      _romSceneRootPaths = nextSceneRootPaths;
      _selectedSceneRootPath = nextSceneRootSelection;
      _romCollisionPackagePath = collisionPreview?.packagePath;
      _romCollisionSceneKey = collisionPreview?.sceneKey;
      _romScenePackagePath = sceneMeshPreview?.packagePath;
      _showCollisionOverlay = nextSceneMeshes.isEmpty;
      _status = _buildIndexedStatus(
        indexed: indexed,
        fromRomContext: fromRomContext,
        collisionPreview: collisionPreview,
        sceneMeshPreview: sceneMeshPreview,
      );
    });

    _warmupSceneTextures();
    _syncRenderObjects();
  }

  String? _pickCollisionPackage(IndexedAssets indexed) {
    final packageCandidates = <String>[
      ...indexed.packages,
      ..._buildPackageFallbacks(indexed.rootPath),
    ];

    final existing = _normalizeUniquePaths(
      packageCandidates,
    ).where((path) => File(path).existsSync()).toList(growable: false);

    if (existing.isEmpty) {
      return null;
    }

    final ranked = [...existing]
      ..sort((a, b) => _packagePriority(a).compareTo(_packagePriority(b)));
    return ranked.first;
  }

  List<String> _buildPackageFallbacks(String indexedRootPath) {
    final roots = <String>{};

    void addRoot(String root) {
      if (root.trim().isEmpty) {
        return;
      }
      roots.add(Directory(root).absolute.path);
    }

    addRoot(indexedRootPath);

    var current = Directory(indexedRootPath).absolute;
    for (var depth = 0; depth < 4; depth++) {
      addRoot(current.path);
      final parent = current.parent;
      if (parent.path == current.path) {
        break;
      }
      current = parent;
    }

    final romPath = widget.romPath;
    if (romPath != null && romPath.trim().isNotEmpty) {
      addRoot(File(romPath).parent.path);
    }

    for (final root in _buildDefaultRootCandidates()) {
      addRoot(root);
    }

    const packageNames = <String>["oot.o2r", "soh.o2r", "oot.otr", "soh.otr"];

    final candidates = <String>[];
    for (final root in roots) {
      for (final packageName in packageNames) {
        candidates.add("$root${Platform.pathSeparator}$packageName");
        candidates.add(
          "$root${Platform.pathSeparator}x64${Platform.pathSeparator}Release${Platform.pathSeparator}$packageName",
        );
      }
    }

    return candidates;
  }

  int _packagePriority(String path) {
    final lower = path.toLowerCase().replaceAll("/", "\\");
    var score = 1000;

    if (lower.endsWith("\\oot.o2r")) {
      score -= 200;
    } else if (lower.endsWith("\\soh.o2r")) {
      score -= 180;
    } else if (lower.endsWith(".o2r")) {
      score -= 150;
    } else if (lower.endsWith(".otr")) {
      score -= 50;
    }

    if (lower.contains("\\x64\\release\\")) {
      score -= 40;
    }

    if (lower.contains("\\soh\\assets\\")) {
      score += 70;
    }

    return score;
  }

  List<String> _normalizeUniquePaths(List<String> candidates) {
    final unique = <String>[];
    final seen = <String>{};

    for (final candidate in candidates) {
      final trimmed = candidate.trim();
      if (trimmed.isEmpty) {
        continue;
      }

      final normalized = trimmed
          .replaceAll("/", Platform.pathSeparator)
          .replaceAll("\\", Platform.pathSeparator);
      final absolute = File(normalized).absolute.path;
      final key = Platform.isWindows ? absolute.toLowerCase() : absolute;

      if (seen.add(key)) {
        unique.add(absolute);
      }
    }

    return unique;
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

  List<String> _collectSceneRootPaths(List<O2rSceneMesh> meshes) {
    final roots = meshes.map((mesh) => mesh.resourcePath).toSet().toList();
    roots.sort();
    return roots;
  }

  String _resolveSceneRootSelection(List<String> nextSceneRootPaths) {
    if (nextSceneRootPaths.isEmpty) {
      return _allSceneRootOption;
    }

    if (nextSceneRootPaths.contains(_selectedSceneRootPath)) {
      return _selectedSceneRootPath;
    }

    // Keep preview constrained to a single root to avoid mixed scene overdraw.
    return nextSceneRootPaths.first;
  }

  List<O2rSceneMesh> _visibleRomSceneMeshes() {
    if (_romSceneRootPaths.isEmpty) {
      return const <O2rSceneMesh>[];
    }

    final selectedRoot = _romSceneRootPaths.contains(_selectedSceneRootPath)
        ? _selectedSceneRootPath
        : _romSceneRootPaths.first;

    return _romSceneMeshes
        .where((mesh) => mesh.resourcePath == selectedRoot)
        .toList(growable: false);
  }

  List<String> _previewSceneOptions() {
    final options = <String>{
      ...widget.allSceneIds.map((item) => item.trim().toLowerCase()),
      _previewSceneId.trim().toLowerCase(),
    }..removeWhere((item) => item.isEmpty);

    final sorted = options.toList(growable: false)..sort();
    return sorted;
  }

  Future<void> _reloadPreviewScene({
    required String sceneId,
    bool force = false,
  }) async {
    final indexed = _indexedAssets;
    if (indexed == null) {
      return;
    }

    final normalized = sceneId.trim().toLowerCase();
    if (normalized.isEmpty) {
      return;
    }

    if (!force && normalized == _previewSceneId && _romSceneMeshes.isNotEmpty) {
      return;
    }

    setState(() {
      _previewSceneId = normalized;
      _loadingAssets = true;
      _status = "Loading preview scene '$normalized'...";
    });

    try {
      await _applyIndexedAssets(indexed: indexed, fromRomContext: true);
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Failed to load preview scene '$normalized': $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _loadingAssets = false;
        });
      }
    }
  }

  String _buildIndexedStatus({
    required IndexedAssets indexed,
    required bool fromRomContext,
    required O2rCollisionLoadResult? collisionPreview,
    required O2rSceneMeshLoadResult? sceneMeshPreview,
  }) {
    final prefix = fromRomContext
        ? "ROM context loaded from ${indexed.rootPath}"
        : "Indexed ${indexed.recognizedFiles}/${indexed.totalFiles} files";

    final assetSummary =
        "$prefix: ${indexed.models.length} renderable models, ${indexed.scenes.length} scenes, ${indexed.textures.length} textures, ${indexed.packages.length} packages.";

    final details = <String>[
      "Preview scene id: '$_previewSceneId'.",
      "Geometry sanitizer: ${_sanitizeSceneGeometry ? "on" : "off"}.",
    ];

    if (collisionPreview == null) {
      details.add("No .o2r/.otr package found for collision preview.");
    } else if (collisionPreview.hasGeometry) {
      final sceneKey = collisionPreview.sceneKey ?? _previewSceneId;
      details.add(
        "Collision preview: ${collisionPreview.meshes.length} mesh(es), ${collisionPreview.triangleCount} triangle(s), scene '$sceneKey', source ${_baseName(collisionPreview.packagePath)}.",
      );
    } else {
      details.add(collisionPreview.message);
    }

    if (sceneMeshPreview == null) {
      details.add("No display list mesh preview available.");
    } else if (sceneMeshPreview.hasGeometry) {
      details.add(
        "Scene mesh preview: ${sceneMeshPreview.meshes.length} mesh(es), ${sceneMeshPreview.triangleCount} triangle(s), source ${_baseName(sceneMeshPreview.packagePath)}.",
      );
    } else {
      details.add(sceneMeshPreview.message);
    }

    return "$assetSummary ${details.join(' ')}";
  }

  void _replaceTextureImageCache(Map<int, O2rSceneTexture> nextTextures) {
    final keep = nextTextures.keys.toSet();
    final staleHashes = _romSceneTextureImages.keys
        .where((hash) => !keep.contains(hash))
        .toList(growable: false);

    for (final hash in staleHashes) {
      _romSceneTextureImages.remove(hash)?.dispose();
    }

    _loadingTextureHashes.removeWhere((hash) => !keep.contains(hash));
  }

  void _clearTextureImageCache() {
    for (final image in _romSceneTextureImages.values) {
      image.dispose();
    }
    _romSceneTextureImages.clear();
    _loadingTextureHashes.clear();
  }

  void _warmupSceneTextures() {
    final neededHashes = _visibleRomSceneMeshes()
        .map((mesh) => mesh.textureHash)
        .whereType<int>()
        .toSet();

    var queued = 0;
    for (final hash in neededHashes) {
      if (queued >= 48) {
        break;
      }
      _ensureSceneTextureDecoded(hash);
      queued++;
    }
  }

  Future<void> _ensureSceneTextureDecoded(int textureHash) async {
    if (_romSceneTextureImages.containsKey(textureHash) ||
        _loadingTextureHashes.contains(textureHash)) {
      return;
    }

    final texture = _romSceneTextures[textureHash];
    if (texture == null) {
      return;
    }

    _loadingTextureHashes.add(textureHash);
    try {
      final image = await _decodeSceneTextureImage(texture);
      if (!mounted || !_romSceneTextures.containsKey(textureHash)) {
        image.dispose();
        return;
      }

      setState(() {
        _romSceneTextureImages[textureHash] = image;
      });
      _syncRenderObjects();
    } catch (_) {
      // Keep viewport functional even if decoding fails for a texture.
    } finally {
      _loadingTextureHashes.remove(textureHash);
    }
  }

  Future<ui.Image> _decodeSceneTextureImage(O2rSceneTexture texture) {
    final completer = Completer<ui.Image>();
    ui.decodeImageFromPixels(
      texture.rgbaBytes,
      texture.width,
      texture.height,
      ui.PixelFormat.rgba8888,
      (image) {
        if (!completer.isCompleted) {
          completer.complete(image);
        }
      },
    );
    return completer.future;
  }

  void _onSceneCreated(cube.Scene scene) {
    _cubeScene = scene;
    scene.camera.near = 0.01;
    scene.camera.far = 50000;
    scene.camera.zoom = 1.0;
    scene.camera.position.setValues(4, 5, 14);
    scene.camera.target.setValues(0, 0, 0);
    scene.camera.up.setValues(0, 1, 0);
    scene.light.position.setValues(0, 15, 20);
    _syncRenderObjects();
  }

  void _onViewportPointerSignal(PointerSignalEvent event) {
    if (event is! PointerScrollEvent) {
      return;
    }

    final scene = _cubeScene;
    if (scene == null) {
      return;
    }

    final scroll = event.scrollDelta;
    final preferPan =
        _isShiftPressed() || scroll.dx.abs() > (scroll.dy.abs() * 1.2);

    if (preferPan) {
      _panCameraByScreenDelta(scroll.dx * 0.9, scroll.dy * 0.9);
      return;
    }

    final zoomFactor = math.exp(-scroll.dy * 0.0015);
    _setCameraZoom(scene.camera.zoom * zoomFactor);
  }

  void _onViewportPointerPanZoomStart(PointerPanZoomStartEvent event) {
    final scene = _cubeScene;
    if (scene == null) {
      return;
    }

    _pointerPanZoomStartZoom = scene.camera.zoom;
  }

  void _onViewportPointerPanZoomUpdate(PointerPanZoomUpdateEvent event) {
    if (_cubeScene == null) {
      return;
    }

    _setCameraZoom(_pointerPanZoomStartZoom * event.scale);
    _panCameraByScreenDelta(event.panDelta.dx, event.panDelta.dy);
  }

  void _onViewportPointerMove(PointerMoveEvent event) {
    final buttons = event.buttons;
    final isPanDrag =
        (buttons & kSecondaryMouseButton) != 0 ||
        (buttons & kMiddleMouseButton) != 0;
    if (!isPanDrag) {
      return;
    }

    _panCameraByScreenDelta(event.delta.dx, event.delta.dy);
  }

  void _onViewportScaleStart(ScaleStartDetails details) {
    final scene = _cubeScene;
    if (scene == null) {
      return;
    }
    _scaleGestureStartZoom = scene.camera.zoom;
  }

  void _onViewportScaleUpdate(ScaleUpdateDetails details) {
    if (_cubeScene == null) {
      return;
    }

    if (details.pointerCount >= 2) {
      _setCameraZoom(_scaleGestureStartZoom * details.scale);
      _panCameraByScreenDelta(
        details.focalPointDelta.dx,
        details.focalPointDelta.dy,
      );
      return;
    }

    if (_isShiftPressed()) {
      _panCameraByScreenDelta(
        details.focalPointDelta.dx,
        details.focalPointDelta.dy,
      );
      return;
    }

    _rotateCameraByScreenDelta(
      details.focalPointDelta.dx,
      details.focalPointDelta.dy,
    );
  }

  bool _isShiftPressed() {
    return HardwareKeyboard.instance.logicalKeysPressed.contains(
          LogicalKeyboardKey.shiftLeft,
        ) ||
        HardwareKeyboard.instance.logicalKeysPressed.contains(
          LogicalKeyboardKey.shiftRight,
        );
  }

  void _setCameraZoom(double value) {
    final scene = _cubeScene;
    if (scene == null) {
      return;
    }

    scene.camera.zoom = value.clamp(0.06, 80.0);
    scene.update();
  }

  void _rotateCameraByScreenDelta(double dx, double dy) {
    final scene = _cubeScene;
    if (scene == null) {
      return;
    }

    final camera = scene.camera;
    final offset = camera.position - camera.target;
    var distance = offset.length;
    if (distance <= 0.0001) {
      distance = 10.0;
    }

    var yaw = math.atan2(offset.x, offset.z);
    final pitchRatio = (offset.y / distance).clamp(-0.99, 0.99);
    var pitch = math.asin(pitchRatio);

    yaw -= dx * 0.01;
    pitch = (pitch + dy * 0.01).clamp(-1.45, 1.45);

    final cosPitch = math.cos(pitch);
    final rotated = cube.Vector3(
      distance * math.sin(yaw) * cosPitch,
      distance * math.sin(pitch),
      distance * math.cos(yaw) * cosPitch,
    );

    camera.position.setFrom(camera.target + rotated);
    camera.up.setValues(0, 1, 0);
    scene.update();
  }

  void _panCameraByScreenDelta(double dx, double dy) {
    final scene = _cubeScene;
    if (scene == null) {
      return;
    }

    final camera = scene.camera;
    var forward = camera.target - camera.position;
    var distance = forward.length;
    if (distance <= 0.0001) {
      distance = 10.0;
      forward = cube.Vector3(0, 0, -1);
    }
    forward = forward.normalized();

    var right = forward.cross(camera.up);
    if (right.length2 <= 0.000001) {
      right = cube.Vector3(1, 0, 0);
    } else {
      right = right.normalized();
    }

    var up = right.cross(forward);
    if (up.length2 <= 0.000001) {
      up = cube.Vector3(0, 1, 0);
    } else {
      up = up.normalized();
    }

    final scale = (distance * 0.0018) / math.max(0.4, camera.zoom);
    final move = (right * (-dx * scale)) + (up * (dy * scale));

    camera.position.setFrom(camera.position + move);
    camera.target.setFrom(camera.target + move);
    scene.update();
  }

  void _fitCameraToEntities(cube.Scene scene) {
    var minX = double.infinity;
    var minY = double.infinity;
    var minZ = double.infinity;
    var maxX = -double.infinity;
    var maxY = -double.infinity;
    var maxZ = -double.infinity;
    var hasBounds = false;

    void includePoint(double x, double y, double z) {
      minX = math.min(minX, x);
      minY = math.min(minY, y);
      minZ = math.min(minZ, z);
      maxX = math.max(maxX, x);
      maxY = math.max(maxY, y);
      maxZ = math.max(maxZ, z);
      hasBounds = true;
    }

    for (final entity in _entities) {
      final halfX = _safeExtent(entity.scaleX);
      final halfY = _safeExtent(entity.scaleY);
      final halfZ = _safeExtent(entity.scaleZ);

      includePoint(
        entity.positionX - halfX,
        entity.positionY - halfY,
        entity.positionZ - halfZ,
      );
      includePoint(
        entity.positionX + halfX,
        entity.positionY + halfY,
        entity.positionZ + halfZ,
      );
    }

    for (final mesh in _visibleRomSceneMeshes()) {
      for (final vertex in mesh.vertices) {
        includePoint(vertex.x, vertex.y, vertex.z);
      }
    }

    for (final mesh in _romCollisionMeshes) {
      for (final vertex in mesh.vertices) {
        includePoint(vertex.x, vertex.y, vertex.z);
      }
    }

    if (!hasBounds) {
      scene.camera.target.setValues(0, 0, 0);
      scene.camera.position.setValues(4, 5, 14);
      scene.update();
      return;
    }

    final centerX = (minX + maxX) / 2;
    final centerY = (minY + maxY) / 2;
    final centerZ = (minZ + maxZ) / 2;

    final spanX = (maxX - minX).abs();
    final spanY = (maxY - minY).abs();
    final spanZ = (maxZ - minZ).abs();
    final span = math.max(spanX, math.max(spanY, spanZ));
    final distance = math.max(10.0, math.min(2000.0, span * 2.4 + 8.0));

    scene.camera.target.setValues(centerX, centerY, centerZ);
    scene.camera.position.setValues(
      centerX + distance * 0.28,
      centerY + distance * 0.35,
      centerZ + distance,
    );
    scene.update();
  }

  double _safeExtent(double scale) {
    final normalized = scale.abs();
    if (normalized <= 0.0001) {
      return 1.0;
    }
    return math.min(40.0, math.max(0.5, normalized / 2));
  }

  List<EditorEntity> _entitiesFromScene(ScenePatch scene) {
    final entities = <EditorEntity>[];

    for (final door in scene.doors) {
      entities.add(
        EditorEntity(
          id: door.id,
          name: door.name,
          type: EditorEntityType.portal,
          positionX: door.position.x,
          positionY: door.position.y,
          positionZ: door.position.z,
          rotationX: 0,
          rotationY: 0,
          rotationZ: 0,
          scaleX: 1,
          scaleY: 2,
          scaleZ: 0.3,
          targetSceneId: door.targetSceneId,
          targetSpawn: door.targetSpawn,
        ),
      );
    }

    for (final collision in scene.collisions) {
      final centerX = (collision.min.x + collision.max.x) / 2;
      final centerY = (collision.min.y + collision.max.y) / 2;
      final centerZ = (collision.min.z + collision.max.z) / 2;
      final sizeX = (collision.max.x - collision.min.x).abs();
      final sizeY = (collision.max.y - collision.min.y).abs();
      final sizeZ = (collision.max.z - collision.min.z).abs();
      entities.add(
        EditorEntity(
          id: collision.id,
          name: collision.name,
          type: EditorEntityType.collision,
          positionX: centerX,
          positionY: centerY,
          positionZ: centerZ,
          rotationX: 0,
          rotationY: 0,
          rotationZ: 0,
          scaleX: sizeX <= 0 ? 1 : sizeX,
          scaleY: sizeY <= 0 ? 1 : sizeY,
          scaleZ: sizeZ <= 0 ? 1 : sizeZ,
        ),
      );
    }

    for (final hook in scene.hooks) {
      if (!hook.action.startsWith("spawn_npc:")) {
        continue;
      }
      final parts = hook.action.split(":");
      final npcId = parts.length >= 2 ? parts[1] : "npc_generic";
      final x = parts.length >= 3 ? double.tryParse(parts[2]) ?? 0.0 : 0.0;
      final y = parts.length >= 4 ? double.tryParse(parts[3]) ?? 0.0 : 0.0;
      final z = parts.length >= 5 ? double.tryParse(parts[4]) ?? 0.0 : 0.0;

      entities.add(
        EditorEntity(
          id: hook.id,
          name: hook.name,
          type: EditorEntityType.npc,
          positionX: x,
          positionY: y,
          positionZ: z,
          rotationX: 0,
          rotationY: 0,
          rotationZ: 0,
          scaleX: 0.7,
          scaleY: 1.6,
          scaleZ: 0.7,
          npcId: npcId,
        ),
      );
    }

    return entities;
  }

  ScenePatch _buildScenePatch() {
    final doors = <SceneDoorLink>[];
    final collisions = <CollisionBox>[];
    final hooks = <SceneHook>[];
    final assets = <SceneAssetRef>[...widget.scene.assets];

    final assetPathSet = assets.map((asset) => asset.path).toSet();

    for (final entity in _entities) {
      if (entity.type == EditorEntityType.portal) {
        doors.add(
          SceneDoorLink(
            id: entity.id,
            name: entity.name,
            position: Vec3(
              x: entity.positionX,
              y: entity.positionY,
              z: entity.positionZ,
            ),
            targetSceneId: entity.targetSceneId ?? "kokiri_forest",
            targetSpawn: entity.targetSpawn ?? 0,
            oneWay: false,
          ),
        );
      } else if (entity.type == EditorEntityType.collision) {
        final halfX =
            (entity.scaleX.abs() <= 0 ? 1.0 : entity.scaleX.abs()) / 2.0;
        final halfY =
            (entity.scaleY.abs() <= 0 ? 1.0 : entity.scaleY.abs()) / 2.0;
        final halfZ =
            (entity.scaleZ.abs() <= 0 ? 1.0 : entity.scaleZ.abs()) / 2.0;
        collisions.add(
          CollisionBox(
            id: entity.id,
            name: entity.name,
            min: Vec3(
              x: entity.positionX - halfX,
              y: entity.positionY - halfY,
              z: entity.positionZ - halfZ,
            ),
            max: Vec3(
              x: entity.positionX + halfX,
              y: entity.positionY + halfY,
              z: entity.positionZ + halfZ,
            ),
            climbable: false,
            lethal: false,
          ),
        );
      } else if (entity.type == EditorEntityType.npc) {
        hooks.add(
          SceneHook(
            id: entity.id,
            name: entity.name,
            event: "on_scene_load",
            action:
                "spawn_npc:${entity.npcId ?? "npc_generic"}:${entity.positionX}:${entity.positionY}:${entity.positionZ}",
            enabled: true,
          ),
        );
      } else if (entity.type == EditorEntityType.staticObject) {
        hooks.add(
          SceneHook(
            id: entity.id,
            name: entity.name,
            event: "on_scene_load",
            action:
                "spawn_object:${entity.modelPath ?? "unknown"}:${entity.positionX}:${entity.positionY}:${entity.positionZ}:${entity.rotationX}:${entity.rotationY}:${entity.rotationZ}:${entity.scaleX}:${entity.scaleY}:${entity.scaleZ}",
            enabled: true,
          ),
        );
      }

      if (entity.modelPath != null &&
          entity.modelPath!.isNotEmpty &&
          !assetPathSet.contains(entity.modelPath)) {
        assets.add(
          SceneAssetRef(
            id: generateEntityId("asset"),
            path: entity.modelPath!,
            type: "object",
          ),
        );
        assetPathSet.add(entity.modelPath!);
      }

      if (entity.texturePath != null &&
          entity.texturePath!.isNotEmpty &&
          !assetPathSet.contains(entity.texturePath)) {
        assets.add(
          SceneAssetRef(
            id: generateEntityId("asset"),
            path: entity.texturePath!,
            type: "texture",
          ),
        );
        assetPathSet.add(entity.texturePath!);
      }
    }

    for (final scenePath in _selectedSceneDefs) {
      if (scenePath.isEmpty || assetPathSet.contains(scenePath)) {
        continue;
      }
      assets.add(
        SceneAssetRef(
          id: generateEntityId("asset"),
          path: scenePath,
          type: "scene",
        ),
      );
      assetPathSet.add(scenePath);
    }

    final passthroughHooks = widget.scene.hooks
        .where((hook) => !hook.action.startsWith("spawn_npc:"))
        .toList();

    return widget.scene.copyWith(
      assets: assets,
      doors: doors,
      collisions: collisions,
      hooks: [...passthroughHooks, ...hooks],
    );
  }

  void _syncRenderObjects() {
    final scene = _cubeScene;
    if (scene == null) {
      return;
    }

    scene.world.children.clear();
    _renderObjects.clear();

    final grid = _createGridFloor();
    scene.world.add(grid);
    _renderObjects["__grid__"] = grid;

    final visibleSceneMeshes = _visibleRomSceneMeshes();

    for (var i = 0; i < visibleSceneMeshes.length; i++) {
      final sceneObject = _buildRomSceneRenderObject(visibleSceneMeshes[i], i);
      scene.world.add(sceneObject);
      _renderObjects["__rom_scene__$i"] = sceneObject;
    }

    if (_showCollisionOverlay || visibleSceneMeshes.isEmpty) {
      for (var i = 0; i < _romCollisionMeshes.length; i++) {
        final collisionObject = _buildRomCollisionRenderObject(
          _romCollisionMeshes[i],
          i,
        );
        scene.world.add(collisionObject);
        _renderObjects["__rom_collision__$i"] = collisionObject;
      }
    }

    for (final entity in _entities) {
      final renderObject = _buildRenderObject(entity);
      scene.world.add(renderObject);
      _renderObjects[entity.id] = renderObject;
    }

    _fitCameraToEntities(scene);
    scene.updateTexture();
    scene.update();
  }

  cube.Object _buildRomSceneRenderObject(O2rSceneMesh mesh, int index) {
    const palette = <Color>[
      Color.fromARGB(255, 120, 176, 218),
      Color.fromARGB(255, 141, 202, 179),
      Color.fromARGB(255, 199, 173, 132),
      Color.fromARGB(255, 188, 157, 186),
    ];

    final color = palette[index % palette.length];
    final textureHash = mesh.textureHash;
    final texture = textureHash == null ? null : _romSceneTextures[textureHash];
    final textureImage = textureHash == null
        ? null
        : _romSceneTextureImages[textureHash];

    if (textureHash != null && texture != null && textureImage == null) {
      _ensureSceneTextureDecoded(textureHash);
    }

    return cube.Object(
      name: "__rom_scene__$index",
      mesh: _createSceneMesh(
        mesh,
        color: color,
        texture: texture,
        textureImage: textureImage,
      ),
      backfaceCulling: true,
      lighting: textureImage != null,
      position: cube.Vector3.zero(),
      rotation: cube.Vector3.zero(),
      scale: cube.Vector3.all(1),
    );
  }

  cube.Mesh _createSceneMesh(
    O2rSceneMesh mesh, {
    required Color color,
    required O2rSceneTexture? texture,
    required ui.Image? textureImage,
  }) {
    final vertices = mesh.vertices
        .map((vertex) => cube.Vector3(vertex.x, vertex.y, vertex.z))
        .toList(growable: false);

    final indices = <cube.Polygon>[];
    for (var i = 0; i + 2 < mesh.triangleIndices.length; i += 3) {
      indices.add(
        cube.Polygon(
          mesh.triangleIndices[i],
          mesh.triangleIndices[i + 1],
          mesh.triangleIndices[i + 2],
        ),
      );
    }

    final colors = mesh.vertices
        .map((vertex) => _sceneVertexColor(vertex, fallback: color))
        .toList(growable: false);

    final hasTexture =
        texture != null &&
        textureImage != null &&
        texture.width > 0 &&
        texture.height > 0;

    final texcoords = hasTexture
        ? mesh.vertices
              .map(
                (vertex) => _sceneVertexTexCoord(
                  vertex: vertex,
                  textureWidth: texture.width,
                  textureHeight: texture.height,
                ),
              )
              .toList(growable: false)
        : const <Offset>[];

    return cube.Mesh(
      vertices: vertices,
      indices: indices,
      colors: colors,
      texcoords: texcoords,
      texture: textureImage,
    );
  }

  Color _sceneVertexColor(O2rSceneVertex vertex, {required Color fallback}) {
    final hasVertexColor = vertex.r != 0 || vertex.g != 0 || vertex.b != 0;
    if (!hasVertexColor) {
      return fallback;
    }

    final blend = 0.15;
    final red =
        (vertex.r * (1.0 - blend) + (_colorChannelToByte(fallback.r) * blend))
            .round();
    final green =
        (vertex.g * (1.0 - blend) + (_colorChannelToByte(fallback.g) * blend))
            .round();
    final blue =
        (vertex.b * (1.0 - blend) + (_colorChannelToByte(fallback.b) * blend))
            .round();
    final alpha = vertex.a.clamp(96, 255);

    return Color.fromARGB(alpha, red, green, blue);
  }

  int _colorChannelToByte(double channel) {
    return (channel * 255.0).round().clamp(0, 255);
  }

  Offset _sceneVertexTexCoord({
    required O2rSceneVertex vertex,
    required int textureWidth,
    required int textureHeight,
  }) {
    final width = math.max(1, textureWidth);
    final height = math.max(1, textureHeight);

    final rawU = vertex.s / (32.0 * width);
    final rawV = 1.0 - (vertex.t / (32.0 * height));

    return Offset(_normalizeUv(rawU), _normalizeUv(rawV));
  }

  double _normalizeUv(double value) {
    var wrapped = value % 1.0;
    if (wrapped < 0) {
      wrapped += 1.0;
    }
    return wrapped;
  }

  cube.Object _buildRomCollisionRenderObject(O2rCollisionMesh mesh, int index) {
    const palette = <Color>[
      Color.fromARGB(165, 152, 198, 255),
      Color.fromARGB(165, 159, 232, 210),
      Color.fromARGB(165, 244, 199, 127),
      Color.fromARGB(165, 255, 174, 174),
    ];

    final color = palette[index % palette.length];

    return cube.Object(
      name: "__rom_collision__$index",
      mesh: _createCollisionMesh(mesh, color: color),
      backfaceCulling: false,
      lighting: false,
      position: cube.Vector3.zero(),
      rotation: cube.Vector3.zero(),
      scale: cube.Vector3.all(1),
    );
  }

  cube.Mesh _createCollisionMesh(
    O2rCollisionMesh mesh, {
    required Color color,
  }) {
    final vertices = mesh.vertices
        .map((vertex) => cube.Vector3(vertex.x, vertex.y, vertex.z))
        .toList(growable: false);

    final indices = <cube.Polygon>[];
    for (var i = 0; i + 2 < mesh.triangleIndices.length; i += 3) {
      indices.add(
        cube.Polygon(
          mesh.triangleIndices[i],
          mesh.triangleIndices[i + 1],
          mesh.triangleIndices[i + 2],
        ),
      );
    }

    return cube.Mesh(
      vertices: vertices,
      indices: indices,
      colors: List<Color>.filled(vertices.length, color),
    );
  }

  cube.Object _buildRenderObject(EditorEntity entity) {
    if (entity.type == EditorEntityType.staticObject &&
        entity.modelPath != null &&
        entity.modelPath!.isNotEmpty &&
        entity.modelPath!.toLowerCase().endsWith(".obj")) {
      final modelFile = File(entity.modelPath!);
      if (modelFile.existsSync()) {
        return cube.Object(
          name: entity.id,
          fileName: entity.modelPath,
          isAsset: false,
          normalized: true,
          lighting: false,
          backfaceCulling: true,
          position: cube.Vector3(
            entity.positionX,
            entity.positionY,
            entity.positionZ,
          ),
          rotation: cube.Vector3(
            entity.rotationX,
            entity.rotationY,
            entity.rotationZ,
          ),
          scale: cube.Vector3(
            entity.scaleX == 0 ? 1 : entity.scaleX,
            entity.scaleY == 0 ? 1 : entity.scaleY,
            entity.scaleZ == 0 ? 1 : entity.scaleZ,
          ),
        );
      }
    }

    return _buildPrimitiveRenderObject(entity);
  }

  cube.Object _buildPrimitiveRenderObject(EditorEntity entity) {
    final color = switch (entity.type) {
      EditorEntityType.portal => const Color.fromARGB(255, 52, 177, 255),
      EditorEntityType.collision => const Color.fromARGB(255, 255, 107, 107),
      EditorEntityType.npc => const Color.fromARGB(255, 255, 214, 10),
      EditorEntityType.staticObject => const Color.fromARGB(255, 180, 180, 180),
    };

    return cube.Object(
      name: entity.id,
      mesh: _createBoxMesh(color: color),
      backfaceCulling: false,
      lighting: false,
      position: cube.Vector3(
        entity.positionX,
        entity.positionY,
        entity.positionZ,
      ),
      rotation: cube.Vector3(
        entity.rotationX,
        entity.rotationY,
        entity.rotationZ,
      ),
      scale: cube.Vector3(
        entity.scaleX == 0 ? 1 : entity.scaleX,
        entity.scaleY == 0 ? 1 : entity.scaleY,
        entity.scaleZ == 0 ? 1 : entity.scaleZ,
      ),
    );
  }

  cube.Mesh _createBoxMesh({required Color color}) {
    final vertices = <cube.Vector3>[
      cube.Vector3(-0.5, -0.5, -0.5),
      cube.Vector3(0.5, -0.5, -0.5),
      cube.Vector3(0.5, 0.5, -0.5),
      cube.Vector3(-0.5, 0.5, -0.5),
      cube.Vector3(-0.5, -0.5, 0.5),
      cube.Vector3(0.5, -0.5, 0.5),
      cube.Vector3(0.5, 0.5, 0.5),
      cube.Vector3(-0.5, 0.5, 0.5),
    ];

    final indices = <cube.Polygon>[
      cube.Polygon(0, 1, 2),
      cube.Polygon(0, 2, 3),
      cube.Polygon(1, 5, 6),
      cube.Polygon(1, 6, 2),
      cube.Polygon(5, 4, 7),
      cube.Polygon(5, 7, 6),
      cube.Polygon(4, 0, 3),
      cube.Polygon(4, 3, 7),
      cube.Polygon(3, 2, 6),
      cube.Polygon(3, 6, 7),
      cube.Polygon(4, 5, 1),
      cube.Polygon(4, 1, 0),
    ];

    return cube.Mesh(
      vertices: vertices,
      indices: indices,
      colors: List<Color>.filled(vertices.length, color),
    );
  }

  cube.Object _createGridFloor() {
    final size = 16.0;
    final vertices = <cube.Vector3>[
      cube.Vector3(-size, -0.5, -size),
      cube.Vector3(size, -0.5, -size),
      cube.Vector3(size, -0.5, size),
      cube.Vector3(-size, -0.5, size),
    ];

    final indices = <cube.Polygon>[
      cube.Polygon(0, 1, 2),
      cube.Polygon(0, 2, 3),
    ];

    return cube.Object(
      name: "__grid__",
      mesh: cube.Mesh(
        vertices: vertices,
        indices: indices,
        colors: List<Color>.filled(
          vertices.length,
          const Color.fromARGB(56, 78, 94, 110),
        ),
      ),
      backfaceCulling: true,
      lighting: false,
      position: cube.Vector3.zero(),
      scale: cube.Vector3.all(1),
    );
  }

  void _addEntity(EditorEntity entity) {
    setState(() {
      _entities = [..._entities, entity];
      _selectedIndex = _entities.length - 1;
    });
    _syncRenderObjects();
  }

  void _addPortal() {
    _addEntity(
      EditorEntity(
        id: generateEntityId("portal"),
        name: "Portal",
        type: EditorEntityType.portal,
        positionX: 0,
        positionY: 0,
        positionZ: 0,
        rotationX: 0,
        rotationY: 0,
        rotationZ: 0,
        scaleX: 1,
        scaleY: 2,
        scaleZ: 0.3,
        targetSceneId: widget.allSceneIds.isEmpty
            ? "kokiri_forest"
            : widget.allSceneIds.first,
        targetSpawn: 0,
      ),
    );
  }

  void _addCollision() {
    _addEntity(
      EditorEntity(
        id: generateEntityId("collision"),
        name: "Collision Box",
        type: EditorEntityType.collision,
        positionX: 0,
        positionY: 0,
        positionZ: 0,
        rotationX: 0,
        rotationY: 0,
        rotationZ: 0,
        scaleX: 2,
        scaleY: 2,
        scaleZ: 2,
      ),
    );
  }

  void _addNpc() {
    _addEntity(
      EditorEntity(
        id: generateEntityId("npc"),
        name: "NPC",
        type: EditorEntityType.npc,
        positionX: 0,
        positionY: 0,
        positionZ: 0,
        rotationX: 0,
        rotationY: 0,
        rotationZ: 0,
        scaleX: 0.7,
        scaleY: 1.6,
        scaleZ: 0.7,
        npcId: "npc_generic",
      ),
    );
  }

  void _addModelEntity(String modelPath) {
    if (!_assetIndexerService.isRenderableModelPath(modelPath)) {
      setState(() {
        _status =
            "This file is not a renderable 3D model. Supported formats: .obj, .gltf, .glb, .fbx, .dae.";
      });
      return;
    }

    final fileName = _baseName(modelPath);
    _addEntity(
      EditorEntity(
        id: generateEntityId("obj"),
        name: fileName,
        type: EditorEntityType.staticObject,
        positionX: 0,
        positionY: 0,
        positionZ: 0,
        rotationX: 0,
        rotationY: 0,
        rotationZ: 0,
        scaleX: 1,
        scaleY: 1,
        scaleZ: 1,
        modelPath: modelPath,
      ),
    );
  }

  void _removeSelectedEntity() {
    if (_selectedIndex < 0 || _selectedIndex >= _entities.length) {
      return;
    }
    setState(() {
      final next = [..._entities]..removeAt(_selectedIndex);
      _entities = next;
      if (_entities.isEmpty) {
        _selectedIndex = -1;
      } else {
        _selectedIndex = _selectedIndex.clamp(0, _entities.length - 1);
      }
    });
    _syncRenderObjects();
  }

  void _updateSelectedEntity(
    EditorEntity updated, {
    bool rebuildObjects = false,
  }) {
    if (_selectedIndex < 0 || _selectedIndex >= _entities.length) {
      return;
    }
    setState(() {
      final next = [..._entities];
      next[_selectedIndex] = updated;
      _entities = next;
    });

    if (rebuildObjects) {
      _syncRenderObjects();
      return;
    }

    final object = _renderObjects[updated.id];
    if (object != null) {
      object.position.setValues(
        updated.positionX,
        updated.positionY,
        updated.positionZ,
      );
      object.rotation.setValues(
        updated.rotationX,
        updated.rotationY,
        updated.rotationZ,
      );
      object.scale.setValues(
        updated.scaleX == 0 ? 1 : updated.scaleX,
        updated.scaleY == 0 ? 1 : updated.scaleY,
        updated.scaleZ == 0 ? 1 : updated.scaleZ,
      );
      object.updateTransform();
      _cubeScene?.update();
    }
  }

  String _baseName(String path) {
    final segments = path.split(RegExp(r"[/\\]"));
    if (segments.isEmpty) {
      return path;
    }
    return segments.last;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("3D Studio - ${widget.scene.sceneName}"),
        actions: [
          TextButton.icon(
            onPressed: () => Navigator.of(context).pop(),
            icon: const Icon(Icons.close),
            label: const Text("Cancel"),
          ),
          const SizedBox(width: 8),
          FilledButton.icon(
            onPressed: () => Navigator.of(context).pop(_buildScenePatch()),
            icon: const Icon(Icons.save_outlined),
            label: const Text("Apply To Scene"),
          ),
          const SizedBox(width: 12),
        ],
      ),
      body: DecoratedBox(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [Color(0xFFF2ECDF), Color(0xFFE8DFD0)],
          ),
        ),
        child: Padding(
          padding: const EdgeInsets.all(12),
          child: LayoutBuilder(
            builder: (context, constraints) {
              if (constraints.maxWidth < 1200) {
                return Column(
                  children: [
                    SizedBox(height: 260, child: _buildAssetsPanel()),
                    const SizedBox(height: 8),
                    Expanded(child: _buildViewportPanel()),
                    const SizedBox(height: 8),
                    SizedBox(height: 320, child: _buildEntitiesPanel()),
                  ],
                );
              }

              return Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  SizedBox(width: 360, child: _buildAssetsPanel()),
                  const SizedBox(width: 8),
                  Expanded(child: _buildViewportPanel()),
                  const SizedBox(width: 8),
                  SizedBox(width: 420, child: _buildEntitiesPanel()),
                ],
              );
            },
          ),
        ),
      ),
    );
  }

  Widget _buildAssetsPanel() {
    final indexed = _indexedAssets;
    final filter = _assetFilterController.text.trim().toLowerCase();
    final models =
        indexed?.models
            .where(
              (path) => filter.isEmpty || path.toLowerCase().contains(filter),
            )
            .toList() ??
        const <String>[];
    final textures =
        indexed?.textures
            .where(
              (path) => filter.isEmpty || path.toLowerCase().contains(filter),
            )
            .toList() ??
        const <String>[];
    final scenes =
        indexed?.scenes
            .where(
              (path) => filter.isEmpty || path.toLowerCase().contains(filter),
            )
            .toList() ??
        const <String>[];
    final packages =
        indexed?.packages
            .where(
              (path) => filter.isEmpty || path.toLowerCase().contains(filter),
            )
            .toList() ??
        const <String>[];

    final totalModels = indexed?.models.length ?? 0;
    final totalScenes = indexed?.scenes.length ?? 0;
    final totalTextures = indexed?.textures.length ?? 0;
    final totalPackages = indexed?.packages.length ?? 0;
    final previewSceneOptions = _previewSceneOptions();

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(10),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Wrap(
              spacing: 8,
              runSpacing: 8,
              children: [
                FilledButton.icon(
                  onPressed: _loadingAssets ? null : _pickAssetsFolder,
                  icon: const Icon(Icons.folder_open),
                  label: const Text("Load Assets Folder"),
                ),
                OutlinedButton.icon(
                  onPressed: _loadingAssets
                      ? null
                      : _loadRepositoryExamplesFolder,
                  icon: const Icon(Icons.dataset),
                  label: const Text("Load Repository Examples"),
                ),
                OutlinedButton.icon(
                  onPressed: _loadingAssets || widget.romPath == null
                      ? null
                      : _loadFromRomContext,
                  icon: const Icon(Icons.sd_card_outlined),
                  label: const Text("Load From ROM"),
                ),
              ],
            ),
            const SizedBox(height: 8),
            TextField(
              controller: _assetFilterController,
              decoration: const InputDecoration(
                labelText: "Filter assets",
                border: OutlineInputBorder(),
                isDense: true,
              ),
            ),
            const SizedBox(height: 8),
            Text(
              _status,
              maxLines: 4,
              overflow: TextOverflow.fade,
              style: Theme.of(context).textTheme.bodySmall,
            ),
            if (_romCollisionPackagePath != null)
              Padding(
                padding: const EdgeInsets.only(top: 4),
                child: Text(
                  "Collision source: ${_baseName(_romCollisionPackagePath!)} (${_romCollisionSceneKey ?? _previewSceneId})",
                  maxLines: 2,
                  overflow: TextOverflow.ellipsis,
                  style: Theme.of(context).textTheme.labelSmall,
                ),
              ),
            if (_romScenePackagePath != null)
              Padding(
                padding: const EdgeInsets.only(top: 2),
                child: Text(
                  "Scene mesh source: ${_baseName(_romScenePackagePath!)}",
                  maxLines: 2,
                  overflow: TextOverflow.ellipsis,
                  style: Theme.of(context).textTheme.labelSmall,
                ),
              ),
            if (previewSceneOptions.isNotEmpty)
              Padding(
                padding: const EdgeInsets.only(top: 6),
                child: DropdownButtonFormField<String>(
                  initialValue: previewSceneOptions.contains(_previewSceneId)
                      ? _previewSceneId
                      : previewSceneOptions.first,
                  isDense: true,
                  decoration: const InputDecoration(
                    labelText: "Preview Scene",
                    border: OutlineInputBorder(),
                    isDense: true,
                  ),
                  items: previewSceneOptions
                      .map(
                        (sceneId) => DropdownMenuItem<String>(
                          value: sceneId,
                          child: Text(sceneId, overflow: TextOverflow.ellipsis),
                        ),
                      )
                      .toList(growable: false),
                  onChanged: _loadingAssets
                      ? null
                      : (value) async {
                          if (value == null) {
                            return;
                          }
                          await _reloadPreviewScene(
                            sceneId: value,
                            force: true,
                          );
                        },
                ),
              )
            else
              Padding(
                padding: const EdgeInsets.only(top: 2),
                child: Text(
                  "Preview scene: $_previewSceneId",
                  maxLines: 1,
                  overflow: TextOverflow.ellipsis,
                  style: Theme.of(context).textTheme.labelSmall,
                ),
              ),
            if (_romSceneRootPaths.isNotEmpty)
              Padding(
                padding: const EdgeInsets.only(top: 6),
                child: DropdownButtonFormField<String>(
                  initialValue:
                      _romSceneRootPaths.contains(_selectedSceneRootPath)
                      ? _selectedSceneRootPath
                      : _romSceneRootPaths.first,
                  isDense: true,
                  decoration: const InputDecoration(
                    labelText: "Scene Root",
                    border: OutlineInputBorder(),
                    isDense: true,
                  ),
                  items: [
                    ..._romSceneRootPaths.map(
                      (path) => DropdownMenuItem<String>(
                        value: path,
                        child: Text(
                          _baseName(path),
                          overflow: TextOverflow.ellipsis,
                        ),
                      ),
                    ),
                  ],
                  onChanged: (value) {
                    if (value == null) {
                      return;
                    }
                    setState(() {
                      _selectedSceneRootPath = value;
                    });
                    _syncRenderObjects();
                  },
                ),
              ),
            const SizedBox(height: 6),
            if (_romCollisionMeshes.isNotEmpty)
              Align(
                alignment: Alignment.centerLeft,
                child: TextButton.icon(
                  onPressed: () {
                    setState(() {
                      _showCollisionOverlay = !_showCollisionOverlay;
                    });
                    _syncRenderObjects();
                  },
                  icon: Icon(
                    _showCollisionOverlay
                        ? Icons.layers_clear_outlined
                        : Icons.layers_outlined,
                  ),
                  label: Text(
                    _showCollisionOverlay
                        ? "Hide Collision Overlay"
                        : "Show Collision Overlay",
                  ),
                ),
              ),
            if (_romScenePackagePath != null)
              Align(
                alignment: Alignment.centerLeft,
                child: TextButton.icon(
                  onPressed: () async {
                    setState(() {
                      _sanitizeSceneGeometry = !_sanitizeSceneGeometry;
                    });
                    await _reloadPreviewScene(
                      sceneId: _previewSceneId,
                      force: true,
                    );
                  },
                  icon: Icon(
                    _sanitizeSceneGeometry
                        ? Icons.shield_outlined
                        : Icons.warning_amber_outlined,
                  ),
                  label: Text(
                    _sanitizeSceneGeometry
                        ? "Sanitizer On (anti-explode)"
                        : "Sanitizer Off (raw)",
                  ),
                ),
              ),
            Wrap(
              spacing: 6,
              runSpacing: 6,
              children: [
                _countChip(
                  "Models",
                  models.length,
                  const Color(0xFFD7F2E8),
                  total: totalModels,
                ),
                _countChip(
                  "Scenes",
                  scenes.length,
                  const Color(0xFFE6E3FB),
                  total: totalScenes,
                ),
                _countChip(
                  "Textures",
                  textures.length,
                  const Color(0xFFFBE8D8),
                  total: totalTextures,
                ),
                _countChip(
                  "Packages",
                  packages.length,
                  const Color(0xFFE4EEF8),
                  total: totalPackages,
                ),
                _countChip(
                  "Scene Meshes",
                  _romSceneMeshes.length,
                  const Color(0xFFD8EAF8),
                ),
                _countChip(
                  "Visible Meshes",
                  _visibleRomSceneMeshes().length,
                  const Color(0xFFD9EFFB),
                ),
                _countChip(
                  "Scene Roots",
                  _romSceneRootPaths.length,
                  const Color(0xFFE2EDF9),
                ),
                _countChip(
                  "ROM Textures",
                  _romSceneTextures.length,
                  const Color(0xFFE3F4F0),
                ),
                _countChip(
                  "Decoded",
                  _romSceneTextureImages.length,
                  const Color(0xFFF0E8FB),
                ),
                if (_romSceneMeshes.isNotEmpty)
                  _countChip(
                    "Scene Tris",
                    _romSceneMeshes.fold<int>(
                      0,
                      (sum, mesh) => sum + mesh.triangleCount,
                    ),
                    const Color(0xFFE0F6EA),
                  ),
                _countChip(
                  "Collision",
                  _romCollisionMeshes.length,
                  const Color(0xFFDDECF9),
                ),
                if (_romCollisionMeshes.isNotEmpty)
                  _countChip(
                    "Collision Tris",
                    _romCollisionMeshes.fold<int>(
                      0,
                      (sum, mesh) => sum + mesh.triangleCount,
                    ),
                    const Color(0xFFE6F7E6),
                  ),
              ],
            ),
            const SizedBox(height: 8),
            Row(
              children: [
                Expanded(
                  child: OutlinedButton.icon(
                    onPressed: _addPortal,
                    icon: const Icon(Icons.login),
                    label: const Text("Portal"),
                  ),
                ),
                const SizedBox(width: 6),
                Expanded(
                  child: OutlinedButton.icon(
                    onPressed: _addCollision,
                    icon: const Icon(Icons.crop_square),
                    label: const Text("Collision"),
                  ),
                ),
                const SizedBox(width: 6),
                Expanded(
                  child: OutlinedButton.icon(
                    onPressed: _addNpc,
                    icon: const Icon(Icons.person_add_alt_1),
                    label: const Text("NPC"),
                  ),
                ),
              ],
            ),
            const SizedBox(height: 8),
            Expanded(
              child: DefaultTabController(
                length: 4,
                child: Column(
                  children: [
                    const TabBar(
                      tabs: [
                        Tab(text: "Models"),
                        Tab(text: "Scenes"),
                        Tab(text: "Textures"),
                        Tab(text: "Packages"),
                      ],
                    ),
                    Expanded(
                      child: TabBarView(
                        children: [
                          _buildAssetList(
                            items: models,
                            onAdd: _addModelEntity,
                          ),
                          _buildSceneList(scenes),
                          _buildTextureList(textures),
                          _buildPackageList(packages),
                        ],
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildAssetList({
    required List<String> items,
    required ValueChanged<String> onAdd,
  }) {
    if (items.isEmpty) {
      return const Center(child: Text("No assets loaded."));
    }
    return ListView.builder(
      itemCount: items.length,
      itemBuilder: (context, index) {
        final item = items[index];
        return ListTile(
          dense: true,
          title: Text(_baseName(item)),
          subtitle: Text(item, maxLines: 1, overflow: TextOverflow.ellipsis),
          trailing: IconButton(
            tooltip: "Add to scene",
            onPressed: () => onAdd(item),
            icon: const Icon(Icons.add),
          ),
        );
      },
    );
  }

  Widget _countChip(String label, int count, Color color, {int? total}) {
    final display = total != null && total != count
        ? "$count/$total"
        : "$count";
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
      decoration: BoxDecoration(
        color: color,
        borderRadius: BorderRadius.circular(999),
      ),
      child: Text("$label: $display"),
    );
  }

  Widget _buildTextureList(List<String> items) {
    final selected = _selectedEntity;
    if (items.isEmpty) {
      return const Center(child: Text("No textures loaded."));
    }
    return ListView.builder(
      itemCount: items.length,
      itemBuilder: (context, index) {
        final item = items[index];
        return ListTile(
          dense: true,
          title: Text(_baseName(item)),
          subtitle: Text(item, maxLines: 1, overflow: TextOverflow.ellipsis),
          trailing: IconButton(
            tooltip: "Assign texture",
            onPressed: selected == null
                ? null
                : () {
                    _updateSelectedEntity(
                      selected.copyWith(texturePath: item),
                      rebuildObjects: false,
                    );
                  },
            icon: const Icon(Icons.texture_outlined),
          ),
        );
      },
    );
  }

  Widget _buildSceneList(List<String> items) {
    if (items.isEmpty) {
      return const Center(child: Text("No scene definitions loaded."));
    }

    return ListView.builder(
      itemCount: items.length,
      itemBuilder: (context, index) {
        final item = items[index];
        final selected = _selectedSceneDefs.contains(item);
        return ListTile(
          dense: true,
          title: Text(_baseName(item)),
          subtitle: Text(item, maxLines: 1, overflow: TextOverflow.ellipsis),
          trailing: IconButton(
            tooltip: selected
                ? "Remove scene from patch"
                : "Add scene to patch",
            onPressed: () {
              setState(() {
                if (selected) {
                  _selectedSceneDefs.remove(item);
                } else {
                  _selectedSceneDefs.add(item);
                }
              });
            },
            icon: Icon(
              selected ? Icons.check_circle : Icons.add_circle_outline,
            ),
          ),
        );
      },
    );
  }

  Widget _buildPackageList(List<String> items) {
    if (items.isEmpty) {
      return const Center(child: Text("No .otr/.o2r/.zip packages loaded."));
    }

    return ListView.builder(
      itemCount: items.length,
      itemBuilder: (context, index) {
        final item = items[index];
        return ListTile(
          dense: true,
          title: Text(_baseName(item)),
          subtitle: Text(item, maxLines: 1, overflow: TextOverflow.ellipsis),
          trailing: const Icon(Icons.inventory_2_outlined),
        );
      },
    );
  }

  Widget _buildViewportPanel() {
    return Card(
      child: Column(
        children: [
          Padding(
            padding: const EdgeInsets.fromLTRB(10, 8, 10, 4),
            child: Row(
              children: [
                const Icon(Icons.threed_rotation),
                const SizedBox(width: 6),
                Expanded(
                  child: Text(
                    "3D Viewport (LMB rotate, Shift+LMB/RMB/MMB pan, wheel/pinch zoom, trackpad pan)",
                    style: Theme.of(context).textTheme.titleSmall,
                  ),
                ),
                IconButton(
                  tooltip: "Reset camera",
                  onPressed: () {
                    final scene = _cubeScene;
                    if (scene == null) {
                      return;
                    }
                    _fitCameraToEntities(scene);
                  },
                  icon: const Icon(Icons.center_focus_strong),
                ),
              ],
            ),
          ),
          Expanded(
            child: Padding(
              padding: const EdgeInsets.all(8),
              child: DecoratedBox(
                decoration: BoxDecoration(
                  borderRadius: BorderRadius.circular(8),
                  gradient: const LinearGradient(
                    begin: Alignment.topCenter,
                    end: Alignment.bottomCenter,
                    colors: [Color(0xFF121E2A), Color(0xFF0A1119)],
                  ),
                ),
                child: Listener(
                  onPointerSignal: _onViewportPointerSignal,
                  onPointerPanZoomStart: _onViewportPointerPanZoomStart,
                  onPointerPanZoomUpdate: _onViewportPointerPanZoomUpdate,
                  onPointerMove: _onViewportPointerMove,
                  child: GestureDetector(
                    behavior: HitTestBehavior.opaque,
                    onScaleStart: _onViewportScaleStart,
                    onScaleUpdate: _onViewportScaleUpdate,
                    child: cube.Cube(
                      interactive: false,
                      onSceneCreated: _onSceneCreated,
                      onObjectCreated: (object) {
                        _cubeScene?.update();
                      },
                    ),
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildEntitiesPanel() {
    final selected = _selectedEntity;
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(10),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                Text(
                  "Entities",
                  style: Theme.of(context).textTheme.titleMedium,
                ),
                const Spacer(),
                IconButton(
                  tooltip: "Delete selected",
                  onPressed: selected == null ? null : _removeSelectedEntity,
                  icon: const Icon(Icons.delete_outline),
                ),
              ],
            ),
            Expanded(
              child: Row(
                children: [
                  SizedBox(width: 180, child: _buildEntityList()),
                  const SizedBox(width: 8),
                  Expanded(
                    child: selected == null
                        ? const Center(child: Text("Select entity"))
                        : _buildInspector(selected),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildEntityList() {
    if (_entities.isEmpty) {
      return const Center(child: Text("No entities."));
    }

    return ListView.builder(
      itemCount: _entities.length,
      itemBuilder: (context, index) {
        final entity = _entities[index];
        final selected = index == _selectedIndex;
        return Card(
          color: selected
              ? Theme.of(context).colorScheme.primaryContainer
              : null,
          child: ListTile(
            dense: true,
            title: Text(
              entity.name,
              maxLines: 1,
              overflow: TextOverflow.ellipsis,
            ),
            subtitle: Text(entity.type.name),
            selected: selected,
            onTap: () {
              setState(() {
                _selectedIndex = index;
              });
            },
          ),
        );
      },
    );
  }

  Widget _buildInspector(EditorEntity entity) {
    return SingleChildScrollView(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          TextField(
            controller: TextEditingController(text: entity.name),
            decoration: const InputDecoration(
              labelText: "Name",
              border: OutlineInputBorder(),
              isDense: true,
            ),
            onSubmitted: (value) {
              _updateSelectedEntity(
                entity.copyWith(
                  name: value.trim().isEmpty ? entity.name : value.trim(),
                ),
              );
            },
          ),
          const SizedBox(height: 8),
          _vectorEditor(
            title: "Position",
            x: entity.positionX,
            y: entity.positionY,
            z: entity.positionZ,
            onChanged: (x, y, z) {
              _updateSelectedEntity(
                entity.copyWith(positionX: x, positionY: y, positionZ: z),
              );
            },
          ),
          const SizedBox(height: 8),
          _vectorEditor(
            title: "Rotation",
            x: entity.rotationX,
            y: entity.rotationY,
            z: entity.rotationZ,
            onChanged: (x, y, z) {
              _updateSelectedEntity(
                entity.copyWith(rotationX: x, rotationY: y, rotationZ: z),
              );
            },
          ),
          const SizedBox(height: 8),
          _vectorEditor(
            title: "Scale",
            x: entity.scaleX,
            y: entity.scaleY,
            z: entity.scaleZ,
            onChanged: (x, y, z) {
              _updateSelectedEntity(
                entity.copyWith(scaleX: x, scaleY: y, scaleZ: z),
              );
            },
          ),
          const SizedBox(height: 8),
          _buildNudgeButtons(entity),
          const SizedBox(height: 8),
          _buildTypeSpecificFields(entity),
        ],
      ),
    );
  }

  Widget _buildNudgeButtons(EditorEntity entity) {
    return Wrap(
      spacing: 6,
      runSpacing: 6,
      children: [
        OutlinedButton(
          onPressed: () {
            _updateSelectedEntity(
              entity.copyWith(positionX: entity.positionX - 1),
            );
          },
          child: const Text("X-"),
        ),
        OutlinedButton(
          onPressed: () {
            _updateSelectedEntity(
              entity.copyWith(positionX: entity.positionX + 1),
            );
          },
          child: const Text("X+"),
        ),
        OutlinedButton(
          onPressed: () {
            _updateSelectedEntity(
              entity.copyWith(positionY: entity.positionY - 1),
            );
          },
          child: const Text("Y-"),
        ),
        OutlinedButton(
          onPressed: () {
            _updateSelectedEntity(
              entity.copyWith(positionY: entity.positionY + 1),
            );
          },
          child: const Text("Y+"),
        ),
        OutlinedButton(
          onPressed: () {
            _updateSelectedEntity(
              entity.copyWith(positionZ: entity.positionZ - 1),
            );
          },
          child: const Text("Z-"),
        ),
        OutlinedButton(
          onPressed: () {
            _updateSelectedEntity(
              entity.copyWith(positionZ: entity.positionZ + 1),
            );
          },
          child: const Text("Z+"),
        ),
      ],
    );
  }

  Widget _buildTypeSpecificFields(EditorEntity entity) {
    if (entity.type == EditorEntityType.portal) {
      return Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          DropdownButtonFormField<String>(
            initialValue:
                entity.targetSceneId ??
                (widget.allSceneIds.isEmpty ? null : widget.allSceneIds.first),
            decoration: const InputDecoration(
              labelText: "Target Scene",
              border: OutlineInputBorder(),
              isDense: true,
            ),
            items: widget.allSceneIds
                .map(
                  (sceneId) => DropdownMenuItem<String>(
                    value: sceneId,
                    child: Text(sceneId),
                  ),
                )
                .toList(growable: false),
            onChanged: (value) {
              if (value == null) {
                return;
              }
              _updateSelectedEntity(entity.copyWith(targetSceneId: value));
            },
          ),
          const SizedBox(height: 8),
          _integerField(
            label: "Target Spawn",
            value: entity.targetSpawn ?? 0,
            onSubmitted: (value) {
              _updateSelectedEntity(entity.copyWith(targetSpawn: value));
            },
          ),
        ],
      );
    }

    if (entity.type == EditorEntityType.npc) {
      return TextField(
        controller: TextEditingController(text: entity.npcId ?? "npc_generic"),
        decoration: const InputDecoration(
          labelText: "NPC Id",
          border: OutlineInputBorder(),
          isDense: true,
        ),
        onSubmitted: (value) {
          _updateSelectedEntity(
            entity.copyWith(
              npcId: value.trim().isEmpty ? "npc_generic" : value.trim(),
            ),
          );
        },
      );
    }

    if (entity.type == EditorEntityType.staticObject) {
      return Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          _readonlyInfo("Model", entity.modelPath ?? "none"),
          const SizedBox(height: 6),
          _readonlyInfo("Texture", entity.texturePath ?? "none"),
        ],
      );
    }

    return const SizedBox.shrink();
  }

  Widget _readonlyInfo(String label, String value) {
    return Container(
      width: double.infinity,
      padding: const EdgeInsets.all(8),
      decoration: BoxDecoration(
        border: Border.all(color: Theme.of(context).dividerColor),
        borderRadius: BorderRadius.circular(6),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(label, style: Theme.of(context).textTheme.labelSmall),
          Text(value, maxLines: 2, overflow: TextOverflow.ellipsis),
        ],
      ),
    );
  }

  Widget _vectorEditor({
    required String title,
    required double x,
    required double y,
    required double z,
    required void Function(double x, double y, double z) onChanged,
  }) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(title, style: Theme.of(context).textTheme.labelLarge),
        const SizedBox(height: 4),
        Row(
          children: [
            Expanded(
              child: _doubleField(
                label: "X",
                value: x,
                onSubmitted: (newX) => onChanged(newX, y, z),
              ),
            ),
            const SizedBox(width: 6),
            Expanded(
              child: _doubleField(
                label: "Y",
                value: y,
                onSubmitted: (newY) => onChanged(x, newY, z),
              ),
            ),
            const SizedBox(width: 6),
            Expanded(
              child: _doubleField(
                label: "Z",
                value: z,
                onSubmitted: (newZ) => onChanged(x, y, newZ),
              ),
            ),
          ],
        ),
      ],
    );
  }

  Widget _doubleField({
    required String label,
    required double value,
    required ValueChanged<double> onSubmitted,
  }) {
    return TextField(
      controller: TextEditingController(text: value.toStringAsFixed(3)),
      decoration: InputDecoration(
        labelText: label,
        border: const OutlineInputBorder(),
        isDense: true,
      ),
      onSubmitted: (input) {
        final parsed = double.tryParse(input.trim());
        if (parsed != null) {
          onSubmitted(parsed);
        }
      },
    );
  }

  Widget _integerField({
    required String label,
    required int value,
    required ValueChanged<int> onSubmitted,
  }) {
    return TextField(
      controller: TextEditingController(text: value.toString()),
      decoration: InputDecoration(
        labelText: label,
        border: const OutlineInputBorder(),
        isDense: true,
      ),
      onSubmitted: (input) {
        final parsed = int.tryParse(input.trim());
        if (parsed != null) {
          onSubmitted(parsed);
        }
      },
    );
  }
}
