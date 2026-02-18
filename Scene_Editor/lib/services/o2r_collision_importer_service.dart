import "dart:io";
import "dart:typed_data";

import "package:archive/archive.dart";

class O2rCollisionVertex {
  const O2rCollisionVertex({required this.x, required this.y, required this.z});

  final double x;
  final double y;
  final double z;
}

class O2rCollisionMesh {
  const O2rCollisionMesh({
    required this.resourcePath,
    required this.sceneKey,
    required this.vertices,
    required this.triangleIndices,
  });

  final String resourcePath;
  final String sceneKey;
  final List<O2rCollisionVertex> vertices;
  final List<int> triangleIndices;

  int get triangleCount => triangleIndices.length ~/ 3;
}

class O2rCollisionLoadResult {
  const O2rCollisionLoadResult({
    required this.packagePath,
    required this.sceneId,
    required this.sceneTokens,
    required this.meshes,
    required this.scannedCollisionEntries,
    required this.message,
  });

  final String packagePath;
  final String sceneId;
  final List<String> sceneTokens;
  final List<O2rCollisionMesh> meshes;
  final int scannedCollisionEntries;
  final String message;

  bool get hasGeometry => meshes.isNotEmpty;

  int get triangleCount =>
      meshes.fold<int>(0, (sum, mesh) => sum + mesh.triangleCount);

  int get vertexCount =>
      meshes.fold<int>(0, (sum, mesh) => sum + mesh.vertices.length);

  String? get sceneKey => meshes.isEmpty ? null : meshes.first.sceneKey;
}

class O2rCollisionImporterService {
  static const int _resourceHeaderSize = 0x40;
  static const int _vertexIndexMask = 0x1FFF;
  static const int _maxVertices = 200000;
  static const int _maxPolygons = 400000;
  static const int _maxMeshesPerScene = 4;

  static const Map<String, List<String>> _sceneAliases = {
    "kokiri_forest": ["spot04_scene", "spot04"],
    "links_house_interior": ["link_home_scene", "link_home"],
    "kakariko_village": ["spot01_scene", "spot01", "kakariko_scene"],
    "lost_woods": ["woods_scene", "woods"],
    "kokiri_parkour_room": ["parkour_room", "kokiri_parkour_room"],
  };

  Future<O2rCollisionLoadResult> loadSceneCollision({
    required String packagePath,
    required String sceneId,
  }) async {
    final file = File(packagePath);
    final sceneTokens = buildSceneSearchTokens(sceneId);

    if (!file.existsSync()) {
      return O2rCollisionLoadResult(
        packagePath: packagePath,
        sceneId: sceneId,
        sceneTokens: sceneTokens,
        meshes: const [],
        scannedCollisionEntries: 0,
        message: "Package not found: $packagePath",
      );
    }

    try {
      final archiveBytes = await file.readAsBytes();
      final archive = ZipDecoder().decodeBytes(archiveBytes, verify: false);
      final entries = _findCandidateEntries(archive, sceneTokens);

      if (entries.isEmpty) {
        return O2rCollisionLoadResult(
          packagePath: packagePath,
          sceneId: sceneId,
          sceneTokens: sceneTokens,
          meshes: const [],
          scannedCollisionEntries: 0,
          message:
              "No collision headers found in ${file.path} for scene '$sceneId'.",
        );
      }

      final meshes = <O2rCollisionMesh>[];
      for (final entry in entries) {
        if (meshes.length >= _maxMeshesPerScene) {
          break;
        }

        final parsed = _parseEntry(entry);
        if (parsed != null && parsed.triangleCount > 0) {
          meshes.add(parsed);
        }
      }

      if (meshes.isEmpty) {
        return O2rCollisionLoadResult(
          packagePath: packagePath,
          sceneId: sceneId,
          sceneTokens: sceneTokens,
          meshes: const [],
          scannedCollisionEntries: entries.length,
          message:
              "Found ${entries.length} collision header resource(s), but none could be decoded.",
        );
      }

      final triangleCount = meshes.fold<int>(
        0,
        (sum, mesh) => sum + mesh.triangleCount,
      );

      return O2rCollisionLoadResult(
        packagePath: packagePath,
        sceneId: sceneId,
        sceneTokens: sceneTokens,
        meshes: meshes,
        scannedCollisionEntries: entries.length,
        message:
            "Loaded ${meshes.length} collision mesh(es), $triangleCount triangle(s) from ${file.path}.",
      );
    } catch (error) {
      return O2rCollisionLoadResult(
        packagePath: packagePath,
        sceneId: sceneId,
        sceneTokens: sceneTokens,
        meshes: const [],
        scannedCollisionEntries: 0,
        message: "Failed to read $packagePath: $error",
      );
    }
  }

  List<String> buildSceneSearchTokens(String sceneId) {
    final normalized = sceneId.trim().toLowerCase();
    final tokens = <String>{};

    void addToken(String token) {
      final value = token.trim().toLowerCase();
      if (value.isNotEmpty) {
        tokens.add(value);
      }
    }

    addToken(normalized);
    addToken("${normalized}_scene");
    addToken(normalized.replaceAll("_", ""));

    for (final alias in _sceneAliases[normalized] ?? const <String>[]) {
      addToken(alias);
    }

    return tokens.toList(growable: false);
  }

  List<ArchiveFile> _findCandidateEntries(
    Archive archive,
    List<String> tokens,
  ) {
    final candidates = <ArchiveFile>[];

    for (final entry in archive.files) {
      if (!entry.isFile) {
        continue;
      }

      final lower = entry.name.toLowerCase().replaceAll("\\", "/");
      if (!lower.contains("collisionheader")) {
        continue;
      }

      if (tokens.isNotEmpty && !tokens.any(lower.contains)) {
        continue;
      }

      candidates.add(entry);
    }

    candidates.sort(
      (a, b) => _entryPriority(a.name).compareTo(_entryPriority(b.name)),
    );

    final unique = <ArchiveFile>[];
    final seenSceneKeys = <String>{};

    for (final candidate in candidates) {
      final key =
          _extractSceneKey(candidate.name.toLowerCase()) ??
          candidate.name.toLowerCase();
      if (!seenSceneKeys.add(key)) {
        continue;
      }
      unique.add(candidate);
    }

    return unique;
  }

  int _entryPriority(String path) {
    final lower = path.toLowerCase().replaceAll("\\", "/");
    var priority = 1000;

    if (lower.contains("/scenes/shared/")) {
      priority -= 200;
    }

    if (lower.contains("n64_ntsc_11") || lower.contains("gc_nmq_ntsc_u")) {
      priority -= 60;
    }

    if (lower.contains("_scenecollisionheader_")) {
      priority -= 40;
    }

    if (lower.contains("_room")) {
      priority += 20;
    }

    return priority;
  }

  O2rCollisionMesh? _parseEntry(ArchiveFile entry) {
    try {
      final bytes = entry.content;
      if (bytes.length < _resourceHeaderSize + 24) {
        return null;
      }

      final endiannessByte = bytes[0];
      final endian = endiannessByte == 1 ? Endian.big : Endian.little;
      final reader = _CollisionReader(
        bytes: bytes,
        offset: _resourceHeaderSize,
        endian: endian,
      );

      reader.readInt16();
      reader.readInt16();
      reader.readInt16();
      reader.readInt16();
      reader.readInt16();
      reader.readInt16();

      final vertexCount = reader.readInt32();
      if (vertexCount < 0 || vertexCount > _maxVertices) {
        return null;
      }

      final vertices = <O2rCollisionVertex>[];
      for (var i = 0; i < vertexCount; i++) {
        final x = reader.readInt16().toDouble();
        final y = reader.readInt16().toDouble();
        final z = reader.readInt16().toDouble();
        vertices.add(O2rCollisionVertex(x: x, y: y, z: z));
      }

      final polygonCount = reader.readUint32();
      if (polygonCount > _maxPolygons) {
        return null;
      }

      final triangleIndices = <int>[];
      for (var i = 0; i < polygonCount; i++) {
        reader.readUint16();
        final vtxA = reader.readUint16();
        final vtxB = reader.readUint16();
        final vtxC = reader.readUint16();
        reader.readInt16();
        reader.readInt16();
        reader.readInt16();
        reader.readInt16();

        final a = vtxA & _vertexIndexMask;
        final b = vtxB & _vertexIndexMask;
        final c = vtxC & _vertexIndexMask;

        if (a >= vertexCount || b >= vertexCount || c >= vertexCount) {
          continue;
        }

        if (a == b || b == c || a == c) {
          continue;
        }

        triangleIndices.add(a);
        triangleIndices.add(b);
        triangleIndices.add(c);
      }

      final surfaceTypeCount = reader.readUint32();
      reader.skipRepeated(count: surfaceTypeCount, bytesPerEntry: 8);

      final camDataCount = reader.readUint32();
      reader.skipRepeated(count: camDataCount, bytesPerEntry: 8);

      final camPosCount = reader.readInt32();
      if (camPosCount < 0) {
        return null;
      }
      reader.skipRepeated(count: camPosCount, bytesPerEntry: 6);

      final waterBoxCount = reader.readInt32();
      if (waterBoxCount < 0) {
        return null;
      }
      reader.skipRepeated(count: waterBoxCount, bytesPerEntry: 14);

      if (triangleIndices.isEmpty) {
        return null;
      }

      return O2rCollisionMesh(
        resourcePath: entry.name,
        sceneKey: _extractSceneKey(entry.name.toLowerCase()) ?? "scene",
        vertices: vertices,
        triangleIndices: triangleIndices,
      );
    } catch (_) {
      return null;
    }
  }

  String? _extractSceneKey(String lowerPath) {
    final normalized = lowerPath.replaceAll("\\", "/");
    final sceneMatch = RegExp(r"([a-z0-9_]+_scene)").firstMatch(normalized);
    return sceneMatch?.group(1);
  }
}

class _CollisionReader {
  _CollisionReader({
    required Uint8List bytes,
    required int offset,
    required Endian endian,
  }) : _bytes = bytes,
       _data = ByteData.sublistView(bytes),
       _offset = offset,
       _endian = endian;

  final Uint8List _bytes;
  final ByteData _data;
  final Endian _endian;
  int _offset;

  int readInt16() {
    _requireBytes(2);
    final value = _data.getInt16(_offset, _endian);
    _offset += 2;
    return value;
  }

  int readUint16() {
    _requireBytes(2);
    final value = _data.getUint16(_offset, _endian);
    _offset += 2;
    return value;
  }

  int readInt32() {
    _requireBytes(4);
    final value = _data.getInt32(_offset, _endian);
    _offset += 4;
    return value;
  }

  int readUint32() {
    _requireBytes(4);
    final value = _data.getUint32(_offset, _endian);
    _offset += 4;
    return value;
  }

  void skipRepeated({required int count, required int bytesPerEntry}) {
    if (count < 0 || bytesPerEntry < 0) {
      throw const FormatException("Invalid skip parameters.");
    }

    final total = count * bytesPerEntry;
    _requireBytes(total);
    _offset += total;
  }

  void _requireBytes(int count) {
    if (count < 0 || _offset + count > _bytes.length) {
      throw const FormatException("Unexpected end of collision data.");
    }
  }
}
