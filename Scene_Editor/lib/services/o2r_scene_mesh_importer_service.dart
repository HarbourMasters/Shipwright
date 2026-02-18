import "dart:io";
import "dart:math" as math;
import "dart:typed_data";

import "package:archive/archive.dart";

class O2rSceneVertex {
  const O2rSceneVertex({
    required this.x,
    required this.y,
    required this.z,
    required this.s,
    required this.t,
    required this.r,
    required this.g,
    required this.b,
    required this.a,
  });

  final double x;
  final double y;
  final double z;
  final int s;
  final int t;
  final int r;
  final int g;
  final int b;
  final int a;
}

class O2rSceneTexture {
  const O2rSceneTexture({
    required this.hash,
    required this.resourcePath,
    required this.width,
    required this.height,
    required this.rgbaBytes,
  });

  final int hash;
  final String resourcePath;
  final int width;
  final int height;
  final Uint8List rgbaBytes;
}

class O2rSceneMesh {
  const O2rSceneMesh({
    required this.resourcePath,
    required this.vertices,
    required this.triangleIndices,
    required this.textureHash,
  });

  final String resourcePath;
  final List<O2rSceneVertex> vertices;
  final List<int> triangleIndices;
  final int? textureHash;

  int get triangleCount => triangleIndices.length ~/ 3;
}

class O2rSceneMeshLoadResult {
  const O2rSceneMeshLoadResult({
    required this.packagePath,
    required this.sceneId,
    required this.sceneTokens,
    required this.meshes,
    required this.texturesByHash,
    required this.scannedDisplayLists,
    required this.message,
  });

  final String packagePath;
  final String sceneId;
  final List<String> sceneTokens;
  final List<O2rSceneMesh> meshes;
  final Map<int, O2rSceneTexture> texturesByHash;
  final int scannedDisplayLists;
  final String message;

  bool get hasGeometry => meshes.isNotEmpty;

  int get triangleCount =>
      meshes.fold<int>(0, (sum, mesh) => sum + mesh.triangleCount);

  int get textureCount => texturesByHash.length;
}

class O2rSceneMeshImporterService {
  static const int _resourceHeaderSize = 0x40;
  static const int _placeholderResourceId = 0xDEADBEEFDEADBEEF;

  static const int _resourceTypeArray = 0x4F415252; // OARR
  static const int _resourceTypeDisplayList = 0x4F444C54; // ODLT
  static const int _resourceTypeTexture = 0x4F544558; // OTEX

  static const int _arrayTypeVertex = 25;

  static const int _textureTypeRgba32 = 1;
  static const int _textureTypeRgba16 = 2;
  static const int _textureTypePalette4 = 3;
  static const int _textureTypePalette8 = 4;
  static const int _textureTypeI4 = 5;
  static const int _textureTypeI8 = 6;
  static const int _textureTypeIa4 = 7;
  static const int _textureTypeIa8 = 8;
  static const int _textureTypeIa16 = 9;

  static const int _opcodeSetTimgOtrHash = 0x20;
  static const int _opcodeVtxOtrFilePath = 0x24;
  static const int _opcodeSetTimgOtrFilePath = 0x25;
  static const int _opcodeTri1Otr = 0x26;
  static const int _opcodeDlOtrFilePath = 0x27;
  static const int _opcodeDlOtrHash = 0x31;
  static const int _opcodeVtxOtrHash = 0x32;
  static const int _opcodeTri1 = 0x05;
  static const int _opcodeTri2 = 0x06;
  static const int _opcodeEndDl = 0xDF;

  static const Set<int> _expandedCommandOpcodes = {
    0x20, // G_SETTIMG_OTR_HASH
    0x31, // G_DL_OTR_HASH
    0x32, // G_VTX_OTR_HASH
    0x33, // G_MARKER
    0x35, // G_BRANCH_Z_OTR
    0x36, // G_MTX_OTR
    0x42, // G_MOVEMEM_HASH
  };

  static const int _maxCommandCountPerDl = 50000;
  static const int _maxDlDepth = 20;
  static const int _maxRootDisplayLists = 180;
  static const int _maxTrianglesPerMesh = 220000;

  static const int _maxTrianglesForSanitizeStats = 8000;
  static const double _sanitizeEdgePercentile = 0.9;
  static const double _sanitizeEdgeMultiplier = 2.5;
  static const double _sanitizeMinAllowedEdge = 180.0;
  static const double _sanitizeMaxAllowedEdge = 16000.0;
  static const double _sanitizeMinTriangleArea2 = 0.25;
  static const double _sanitizeMaxAbsCoordinate = 38000.0;

  static const int _crc64Polynomial = 0x42F0E1EBA9EA3693;
  static const int _crc64Mask = 0xFFFFFFFFFFFFFFFF;

  static const Map<String, List<String>> _sceneAliases = {
    "kokiri_forest": ["spot04_scene", "spot04"],
    "links_house_interior": ["link_home_scene", "link_home"],
    "kakariko_village": ["spot01_scene", "spot01", "kakariko_scene"],
    "lost_woods": ["woods_scene", "woods"],
    "kokiri_parkour_room": ["parkour_room", "kokiri_parkour_room"],
  };

  Future<O2rSceneMeshLoadResult> loadSceneGeometry({
    required String packagePath,
    required String sceneId,
    bool sanitizeGeometry = true,
  }) async {
    final file = File(packagePath);
    final sceneTokens = buildSceneSearchTokens(sceneId);

    if (!file.existsSync()) {
      return O2rSceneMeshLoadResult(
        packagePath: packagePath,
        sceneId: sceneId,
        sceneTokens: sceneTokens,
        meshes: const [],
        texturesByHash: const <int, O2rSceneTexture>{},
        scannedDisplayLists: 0,
        message: "Package not found: $packagePath",
      );
    }

    try {
      final archiveBytes = await file.readAsBytes();
      final archive = ZipDecoder().decodeBytes(archiveBytes, verify: false);

      final indexedResources = _indexResources(archive);
      final rootDisplayLists = _findSceneDisplayListRoots(
        indexedResources.displayLists,
        sceneTokens,
      );

      if (rootDisplayLists.isEmpty) {
        return O2rSceneMeshLoadResult(
          packagePath: packagePath,
          sceneId: sceneId,
          sceneTokens: sceneTokens,
          meshes: const [],
          texturesByHash: const <int, O2rSceneTexture>{},
          scannedDisplayLists: 0,
          message:
              "No display lists found for scene '$sceneId' in ${file.path}.",
        );
      }

      final meshes = <O2rSceneMesh>[];
      final vertexCache = <int, List<O2rSceneVertex>?>{};

      for (final resource in rootDisplayLists) {
        final parser = _DisplayListParser(
          resourcesByHash: indexedResources.resourcesByHash,
          vertexCache: vertexCache,
          maxCommandCount: _maxCommandCountPerDl,
          maxDepth: _maxDlDepth,
          maxTriangles: _maxTrianglesPerMesh,
        );

        final parsedMeshes = parser.parse(resource.hash, resource.path);
        for (final mesh in parsedMeshes) {
          if (mesh.triangleCount > 0) {
            meshes.add(mesh);
          }
        }
      }

      if (meshes.isEmpty) {
        return O2rSceneMeshLoadResult(
          packagePath: packagePath,
          sceneId: sceneId,
          sceneTokens: sceneTokens,
          meshes: const [],
          texturesByHash: const <int, O2rSceneTexture>{},
          scannedDisplayLists: rootDisplayLists.length,
          message:
              "Found ${rootDisplayLists.length} display list resource(s), but none produced renderable triangles.",
        );
      }

      final sanitizedSummary = sanitizeGeometry
          ? _sanitizeMeshes(meshes)
          : _MeshSanitizeSummary(
              meshes: List<O2rSceneMesh>.unmodifiable(meshes),
              droppedTriangles: 0,
              droppedMeshes: 0,
            );

      if (sanitizedSummary.meshes.isEmpty) {
        return O2rSceneMeshLoadResult(
          packagePath: packagePath,
          sceneId: sceneId,
          sceneTokens: sceneTokens,
          meshes: const [],
          texturesByHash: const <int, O2rSceneTexture>{},
          scannedDisplayLists: rootDisplayLists.length,
          message: sanitizeGeometry
              ? "Found ${rootDisplayLists.length} display list resource(s), but sanitizer removed all triangles."
              : "Found ${rootDisplayLists.length} display list resource(s), but none produced renderable triangles.",
        );
      }

      final usedTextureHashes = sanitizedSummary.meshes
          .map((mesh) => mesh.textureHash)
          .whereType<int>()
          .toSet();
      final texturesByHash = <int, O2rSceneTexture>{};
      for (final hash in usedTextureHashes) {
        final texture = indexedResources.texturesByHash[hash];
        if (texture != null) {
          texturesByHash[hash] = texture;
        }
      }

      final totalTriangles = sanitizedSummary.meshes.fold<int>(
        0,
        (sum, mesh) => sum + mesh.triangleCount,
      );
      final sanitizeMessage = sanitizeGeometry
          ? " Sanitizer removed ${sanitizedSummary.droppedTriangles} triangle(s) and ${sanitizedSummary.droppedMeshes} mesh(es)."
          : "";

      return O2rSceneMeshLoadResult(
        packagePath: packagePath,
        sceneId: sceneId,
        sceneTokens: sceneTokens,
        meshes: sanitizedSummary.meshes,
        texturesByHash: Map<int, O2rSceneTexture>.unmodifiable(texturesByHash),
        scannedDisplayLists: rootDisplayLists.length,
        message:
            "Loaded ${sanitizedSummary.meshes.length} scene mesh(es), $totalTriangles triangle(s), ${texturesByHash.length} texture(s) from ${file.path}.$sanitizeMessage",
      );
    } catch (error) {
      return O2rSceneMeshLoadResult(
        packagePath: packagePath,
        sceneId: sceneId,
        sceneTokens: sceneTokens,
        meshes: const [],
        texturesByHash: const <int, O2rSceneTexture>{},
        scannedDisplayLists: 0,
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

  _IndexedResources _indexResources(Archive archive) {
    final resourcesByHash = <int, _ResourceEntry>{};
    final displayLists = <_ResourceEntry>[];
    final texturesByHash = <int, O2rSceneTexture>{};

    for (final file in archive.files) {
      if (!file.isFile) {
        continue;
      }

      final bytes = _archiveFileBytes(file);
      if (bytes.length < _resourceHeaderSize) {
        continue;
      }

      final header = _parseHeader(bytes);
      if (header == null) {
        continue;
      }

      final normalizedPath = file.name.replaceAll("\\", "/");
      final pathHash = _crc64PathHash(normalizedPath);
      final resource = _ResourceEntry(
        hash: pathHash,
        type: header.type,
        endian: header.endian,
        path: normalizedPath,
        bytes: bytes,
      );

      resourcesByHash[pathHash] = resource;

      if (header.id != 0 && header.id != _placeholderResourceId) {
        resourcesByHash.putIfAbsent(header.id, () => resource);
      }

      if (resource.type == _resourceTypeDisplayList) {
        displayLists.add(resource);
      } else if (resource.type == _resourceTypeTexture) {
        final decodedTexture = _parseTextureResource(resource);
        if (decodedTexture != null) {
          texturesByHash[pathHash] = decodedTexture;
          if (header.id != 0 && header.id != _placeholderResourceId) {
            texturesByHash.putIfAbsent(
              header.id,
              () => O2rSceneTexture(
                hash: header.id,
                resourcePath: decodedTexture.resourcePath,
                width: decodedTexture.width,
                height: decodedTexture.height,
                rgbaBytes: decodedTexture.rgbaBytes,
              ),
            );
          }
        }
      }
    }

    return _IndexedResources(
      resourcesByHash: resourcesByHash,
      displayLists: displayLists,
      texturesByHash: texturesByHash,
    );
  }

  O2rSceneTexture? _parseTextureResource(_ResourceEntry resource) {
    final payloadLength = resource.bytes.length - _resourceHeaderSize;
    if (payloadLength < 16) {
      return null;
    }

    final payload = ByteData.sublistView(resource.bytes, _resourceHeaderSize);
    final endian = resource.endian;

    final type = payload.getUint32(0, endian);
    final width = payload.getUint32(4, endian);
    final height = payload.getUint32(8, endian);

    if (width <= 0 || height <= 0 || width > 4096 || height > 4096) {
      return null;
    }

    var imageDataOffset = 16;
    var imageDataSize = payload.getUint32(12, endian);

    final isV0Valid = imageDataSize > 0 && imageDataSize <= payloadLength - 16;
    if (!isV0Valid) {
      if (payloadLength < 28) {
        return null;
      }
      imageDataOffset = 28;
      imageDataSize = payload.getUint32(24, endian);
      final isV1Valid =
          imageDataSize > 0 && imageDataSize <= payloadLength - 28;
      if (!isV1Valid) {
        return null;
      }
    }

    final imageDataStart = _resourceHeaderSize + imageDataOffset;
    final imageDataEnd = imageDataStart + imageDataSize;
    if (imageDataEnd > resource.bytes.length) {
      return null;
    }

    final imageData = Uint8List.sublistView(
      resource.bytes,
      imageDataStart,
      imageDataEnd,
    );

    final decoded = _decodeTextureRgba(
      type: type,
      width: width,
      height: height,
      imageData: imageData,
    );
    if (decoded == null) {
      return null;
    }

    return O2rSceneTexture(
      hash: resource.hash,
      resourcePath: resource.path,
      width: width,
      height: height,
      rgbaBytes: decoded,
    );
  }

  Uint8List? _decodeTextureRgba({
    required int type,
    required int width,
    required int height,
    required Uint8List imageData,
  }) {
    final pixelCount = width * height;
    if (pixelCount <= 0) {
      return null;
    }

    if (type == _textureTypePalette4 || type == _textureTypePalette8) {
      return null;
    }

    if (type == _textureTypeRgba32) {
      final requiredBytes = pixelCount * 4;
      if (imageData.length < requiredBytes) {
        return null;
      }
      return Uint8List.fromList(imageData.sublist(0, requiredBytes));
    }

    final rgba = Uint8List(pixelCount * 4);

    if (type == _textureTypeRgba16) {
      final requiredBytes = pixelCount * 2;
      if (imageData.length < requiredBytes) {
        return null;
      }

      for (var i = 0; i < pixelCount; i++) {
        final b1 = imageData[i * 2];
        final b2 = imageData[(i * 2) + 1];

        final r = ((b1 >> 3) & 0x1F) * 255 ~/ 31;
        final g = (((b1 & 0x07) << 2) | (b2 >> 6)) * 255 ~/ 31;
        final b = ((b2 >> 1) & 0x1F) * 255 ~/ 31;
        final a = (b2 & 1) == 1 ? 255 : 0;

        final out = i * 4;
        rgba[out] = r;
        rgba[out + 1] = g;
        rgba[out + 2] = b;
        rgba[out + 3] = a;
      }

      return rgba;
    }

    if (type == _textureTypeIa16) {
      final requiredBytes = pixelCount * 2;
      if (imageData.length < requiredBytes) {
        return null;
      }

      for (var i = 0; i < pixelCount; i++) {
        final intensity = imageData[i * 2];
        final alpha = imageData[(i * 2) + 1];
        final out = i * 4;
        rgba[out] = intensity;
        rgba[out + 1] = intensity;
        rgba[out + 2] = intensity;
        rgba[out + 3] = alpha;
      }

      return rgba;
    }

    if (type == _textureTypeIa8) {
      final requiredBytes = pixelCount;
      if (imageData.length < requiredBytes) {
        return null;
      }

      for (var i = 0; i < pixelCount; i++) {
        final ia = imageData[i];
        final intensity = ((ia >> 4) & 0x0F) * 255 ~/ 15;
        final alpha = (ia & 0x0F) * 255 ~/ 15;
        final out = i * 4;
        rgba[out] = intensity;
        rgba[out + 1] = intensity;
        rgba[out + 2] = intensity;
        rgba[out + 3] = alpha;
      }

      return rgba;
    }

    if (type == _textureTypeIa4) {
      final requiredBytes = (pixelCount + 1) ~/ 2;
      if (imageData.length < requiredBytes) {
        return null;
      }

      var outPixel = 0;
      for (var i = 0; i < requiredBytes && outPixel < pixelCount; i++) {
        final packed = imageData[i];
        final upper = (packed >> 4) & 0x0F;
        final lower = packed & 0x0F;

        void writeNibble(int nibble) {
          final intensity = ((nibble >> 1) & 0x07) * 255 ~/ 7;
          final alpha = (nibble & 1) == 1 ? 255 : 0;
          final out = outPixel * 4;
          rgba[out] = intensity;
          rgba[out + 1] = intensity;
          rgba[out + 2] = intensity;
          rgba[out + 3] = alpha;
          outPixel++;
        }

        writeNibble(upper);
        if (outPixel < pixelCount) {
          writeNibble(lower);
        }
      }

      return rgba;
    }

    if (type == _textureTypeI8) {
      final requiredBytes = pixelCount;
      if (imageData.length < requiredBytes) {
        return null;
      }

      for (var i = 0; i < pixelCount; i++) {
        final intensity = imageData[i];
        final out = i * 4;
        rgba[out] = intensity;
        rgba[out + 1] = intensity;
        rgba[out + 2] = intensity;
        rgba[out + 3] = intensity;
      }

      return rgba;
    }

    if (type == _textureTypeI4) {
      final requiredBytes = (pixelCount + 1) ~/ 2;
      if (imageData.length < requiredBytes) {
        return null;
      }

      var outPixel = 0;
      for (var i = 0; i < requiredBytes && outPixel < pixelCount; i++) {
        final packed = imageData[i];
        final upper = (packed >> 4) & 0x0F;
        final lower = packed & 0x0F;

        void writeNibble(int nibble) {
          final intensity = nibble * 255 ~/ 15;
          final out = outPixel * 4;
          rgba[out] = intensity;
          rgba[out + 1] = intensity;
          rgba[out + 2] = intensity;
          rgba[out + 3] = intensity;
          outPixel++;
        }

        writeNibble(upper);
        if (outPixel < pixelCount) {
          writeNibble(lower);
        }
      }

      return rgba;
    }

    return null;
  }

  List<_ResourceEntry> _findSceneDisplayListRoots(
    List<_ResourceEntry> displayLists,
    List<String> sceneTokens,
  ) {
    bool tokenMatch(String lower) {
      if (sceneTokens.isEmpty) {
        return true;
      }
      return sceneTokens.any(lower.contains);
    }

    final broadCandidates = displayLists
        .where((resource) {
          final lower = resource.path.toLowerCase();
          return lower.contains("dl") && tokenMatch(lower);
        })
        .toList(growable: false);

    if (broadCandidates.isEmpty) {
      return const <_ResourceEntry>[];
    }

    final strictCandidates = broadCandidates
        .where((resource) {
          final lower = resource.path.toLowerCase();
          if (!lower.contains("scenes/")) {
            return false;
          }
          if (lower.contains("objects/")) {
            return false;
          }
          if (lower.contains("set_") && lower.contains("dl_")) {
            return false;
          }
          return true;
        })
        .toList(growable: false);

    final chosen = strictCandidates.isNotEmpty
        ? strictCandidates
        : broadCandidates;

    final ranked = [...chosen]
      ..sort((a, b) {
        final pa = _displayListPriority(a.path.toLowerCase());
        final pb = _displayListPriority(b.path.toLowerCase());
        if (pa != pb) {
          return pa.compareTo(pb);
        }
        return a.path.compareTo(b.path);
      });

    final unique = <_ResourceEntry>[];
    final seen = <int>{};
    for (final resource in ranked) {
      if (!seen.add(resource.hash)) {
        continue;
      }
      unique.add(resource);
      if (unique.length >= _maxRootDisplayLists) {
        break;
      }
    }

    return unique;
  }

  int _displayListPriority(String lowerPath) {
    var score = 1000;

    if (lowerPath.contains("/scenes/shared/")) {
      score -= 300;
    }
    if (lowerPath.contains("_room_") && lowerPath.contains("dl_")) {
      score -= 240;
    }
    if (lowerPath.contains("set_") && lowerPath.contains("dl_")) {
      score -= 200;
    }
    if (lowerPath.contains("gspot") && lowerPath.contains("dl_")) {
      score -= 180;
    }
    if (lowerPath.contains("/objects/")) {
      score -= 70;
    }
    if (lowerPath.contains("collision")) {
      score += 200;
    }

    return score;
  }

  Uint8List _archiveFileBytes(ArchiveFile file) {
    final dynamic content = file.content;
    if (content is Uint8List) {
      return content;
    }
    if (content is List<int>) {
      return Uint8List.fromList(content);
    }
    if (content is String) {
      return Uint8List.fromList(content.codeUnits);
    }
    throw StateError("Unsupported archive entry content for '${file.name}'.");
  }

  _ResourceHeader? _parseHeader(Uint8List bytes) {
    if (bytes.length < _resourceHeaderSize) {
      return null;
    }

    final endiannessByte = bytes[0];
    final endian = endiannessByte == 1 ? Endian.big : Endian.little;
    final view = ByteData.sublistView(bytes, 0, _resourceHeaderSize);

    return _ResourceHeader(
      endian: endian,
      type: view.getUint32(4, endian),
      id: view.getUint64(12, endian),
    );
  }

  int _crc64PathHash(String path) {
    var crc = _crc64Mask;

    for (final codeUnit in path.codeUnits) {
      final byte = codeUnit & 0xFF;
      crc ^= (byte << 56);

      for (var i = 0; i < 8; i++) {
        if ((crc & 0x8000000000000000) != 0) {
          crc = ((crc << 1) ^ _crc64Polynomial) & _crc64Mask;
        } else {
          crc = (crc << 1) & _crc64Mask;
        }
      }
    }

    return crc & _crc64Mask;
  }

  _MeshSanitizeSummary _sanitizeMeshes(List<O2rSceneMesh> meshes) {
    final nextMeshes = <O2rSceneMesh>[];
    var droppedTriangles = 0;
    var droppedMeshes = 0;

    for (final mesh in meshes) {
      final result = _sanitizeMesh(mesh);
      droppedTriangles += result.droppedTriangles;
      if (result.mesh == null) {
        droppedMeshes++;
        continue;
      }
      nextMeshes.add(result.mesh!);
    }

    return _MeshSanitizeSummary(
      meshes: List<O2rSceneMesh>.unmodifiable(nextMeshes),
      droppedTriangles: droppedTriangles,
      droppedMeshes: droppedMeshes,
    );
  }

  _SingleMeshSanitizeResult _sanitizeMesh(O2rSceneMesh mesh) {
    final triangleCount = mesh.triangleCount;
    if (triangleCount <= 0 ||
        mesh.vertices.isEmpty ||
        mesh.triangleIndices.isEmpty) {
      return const _SingleMeshSanitizeResult(mesh: null, droppedTriangles: 0);
    }

    final maxAllowedEdge = _computeAdaptiveMaxEdge(mesh);
    final maxAllowedEdge2 = maxAllowedEdge * maxAllowedEdge;

    final keptIndices = <int>[];
    var droppedTriangles = 0;

    for (var tri = 0; tri < triangleCount; tri++) {
      final i = tri * 3;
      final aIndex = mesh.triangleIndices[i];
      final bIndex = mesh.triangleIndices[i + 1];
      final cIndex = mesh.triangleIndices[i + 2];

      if (!_isValidVertexIndex(mesh, aIndex) ||
          !_isValidVertexIndex(mesh, bIndex) ||
          !_isValidVertexIndex(mesh, cIndex)) {
        droppedTriangles++;
        continue;
      }

      if (aIndex == bIndex || bIndex == cIndex || aIndex == cIndex) {
        droppedTriangles++;
        continue;
      }

      final a = mesh.vertices[aIndex];
      final b = mesh.vertices[bIndex];
      final c = mesh.vertices[cIndex];

      if (!_isRenderableVertex(a) ||
          !_isRenderableVertex(b) ||
          !_isRenderableVertex(c)) {
        droppedTriangles++;
        continue;
      }

      final area2 = _triangleDoubleAreaSquared(a, b, c);
      if (!area2.isFinite || area2 < _sanitizeMinTriangleArea2) {
        droppedTriangles++;
        continue;
      }

      final edgeMax2 = _maxEdgeLengthSquared(a, b, c);
      if (!edgeMax2.isFinite || edgeMax2 > maxAllowedEdge2) {
        droppedTriangles++;
        continue;
      }

      keptIndices
        ..add(aIndex)
        ..add(bIndex)
        ..add(cIndex);
    }

    if (keptIndices.isEmpty) {
      return _SingleMeshSanitizeResult(
        mesh: null,
        droppedTriangles: triangleCount,
      );
    }

    if (droppedTriangles <= 0) {
      return _SingleMeshSanitizeResult(mesh: mesh, droppedTriangles: 0);
    }

    final remap = <int, int>{};
    final compactVertices = <O2rSceneVertex>[];
    final compactIndices = <int>[];

    for (final oldIndex in keptIndices) {
      final newIndex = remap.putIfAbsent(oldIndex, () {
        compactVertices.add(mesh.vertices[oldIndex]);
        return compactVertices.length - 1;
      });
      compactIndices.add(newIndex);
    }

    if (compactVertices.isEmpty || compactIndices.isEmpty) {
      return _SingleMeshSanitizeResult(
        mesh: null,
        droppedTriangles: triangleCount,
      );
    }

    return _SingleMeshSanitizeResult(
      mesh: O2rSceneMesh(
        resourcePath: mesh.resourcePath,
        vertices: List<O2rSceneVertex>.unmodifiable(compactVertices),
        triangleIndices: List<int>.unmodifiable(compactIndices),
        textureHash: mesh.textureHash,
      ),
      droppedTriangles: droppedTriangles,
    );
  }

  bool _isValidVertexIndex(O2rSceneMesh mesh, int index) {
    return index >= 0 && index < mesh.vertices.length;
  }

  bool _isRenderableVertex(O2rSceneVertex vertex) {
    if (!vertex.x.isFinite || !vertex.y.isFinite || !vertex.z.isFinite) {
      return false;
    }
    if (vertex.x.abs() > _sanitizeMaxAbsCoordinate ||
        vertex.y.abs() > _sanitizeMaxAbsCoordinate ||
        vertex.z.abs() > _sanitizeMaxAbsCoordinate) {
      return false;
    }
    return true;
  }

  double _computeAdaptiveMaxEdge(O2rSceneMesh mesh) {
    final edges = <double>[];
    final triangleCount = mesh.triangleCount;
    final sampleStep = math.max(
      1,
      triangleCount ~/ _maxTrianglesForSanitizeStats,
    );

    for (var tri = 0; tri < triangleCount; tri += sampleStep) {
      final i = tri * 3;
      final aIndex = mesh.triangleIndices[i];
      final bIndex = mesh.triangleIndices[i + 1];
      final cIndex = mesh.triangleIndices[i + 2];

      if (!_isValidVertexIndex(mesh, aIndex) ||
          !_isValidVertexIndex(mesh, bIndex) ||
          !_isValidVertexIndex(mesh, cIndex)) {
        continue;
      }

      final a = mesh.vertices[aIndex];
      final b = mesh.vertices[bIndex];
      final c = mesh.vertices[cIndex];

      if (!_isRenderableVertex(a) ||
          !_isRenderableVertex(b) ||
          !_isRenderableVertex(c)) {
        continue;
      }

      edges.add(_distance(a, b));
      edges.add(_distance(b, c));
      edges.add(_distance(c, a));
    }

    if (edges.isEmpty) {
      return _sanitizeMaxAllowedEdge;
    }

    edges.sort();
    final percentileIndex = ((edges.length - 1) * _sanitizeEdgePercentile)
        .round()
        .clamp(0, edges.length - 1);
    final percentileValue = edges[percentileIndex];
    if (!percentileValue.isFinite || percentileValue <= 0) {
      return _sanitizeMaxAllowedEdge;
    }

    final adaptive = percentileValue * _sanitizeEdgeMultiplier;
    if (!adaptive.isFinite || adaptive <= 0) {
      return _sanitizeMaxAllowedEdge;
    }

    return adaptive
        .clamp(_sanitizeMinAllowedEdge, _sanitizeMaxAllowedEdge)
        .toDouble();
  }

  double _distance(O2rSceneVertex a, O2rSceneVertex b) {
    final dx = a.x - b.x;
    final dy = a.y - b.y;
    final dz = a.z - b.z;
    return math.sqrt((dx * dx) + (dy * dy) + (dz * dz));
  }

  double _maxEdgeLengthSquared(
    O2rSceneVertex a,
    O2rSceneVertex b,
    O2rSceneVertex c,
  ) {
    final ab = _distanceSquared(a, b);
    final bc = _distanceSquared(b, c);
    final ca = _distanceSquared(c, a);
    return math.max(ab, math.max(bc, ca));
  }

  double _distanceSquared(O2rSceneVertex a, O2rSceneVertex b) {
    final dx = a.x - b.x;
    final dy = a.y - b.y;
    final dz = a.z - b.z;
    return (dx * dx) + (dy * dy) + (dz * dz);
  }

  double _triangleDoubleAreaSquared(
    O2rSceneVertex a,
    O2rSceneVertex b,
    O2rSceneVertex c,
  ) {
    final abx = b.x - a.x;
    final aby = b.y - a.y;
    final abz = b.z - a.z;

    final acx = c.x - a.x;
    final acy = c.y - a.y;
    final acz = c.z - a.z;

    final crossX = (aby * acz) - (abz * acy);
    final crossY = (abz * acx) - (abx * acz);
    final crossZ = (abx * acy) - (aby * acx);

    return (crossX * crossX) + (crossY * crossY) + (crossZ * crossZ);
  }
}

class _MeshSanitizeSummary {
  const _MeshSanitizeSummary({
    required this.meshes,
    required this.droppedTriangles,
    required this.droppedMeshes,
  });

  final List<O2rSceneMesh> meshes;
  final int droppedTriangles;
  final int droppedMeshes;
}

class _SingleMeshSanitizeResult {
  const _SingleMeshSanitizeResult({
    required this.mesh,
    required this.droppedTriangles,
  });

  final O2rSceneMesh? mesh;
  final int droppedTriangles;
}

class _DisplayListParser {
  _DisplayListParser({
    required this.resourcesByHash,
    required this.vertexCache,
    required this.maxCommandCount,
    required this.maxDepth,
    required this.maxTriangles,
  });

  final Map<int, _ResourceEntry> resourcesByHash;
  final Map<int, List<O2rSceneVertex>?> vertexCache;
  final int maxCommandCount;
  final int maxDepth;
  final int maxTriangles;

  List<O2rSceneMesh> parse(int rootDisplayListHash, String resourcePath) {
    final collector = _TriangleCollector(maxTriangles: maxTriangles);
    final state = _DisplayListState(
      resolveVertices: _resolveVertices,
      collector: collector,
    );

    final visiting = <int>{};
    _executeDisplayList(
      displayListHash: rootDisplayListHash,
      state: state,
      visiting: visiting,
      depth: 0,
    );

    if (!collector.hasTriangles) {
      return const <O2rSceneMesh>[];
    }

    return collector.buildMeshes(resourcePath);
  }

  void _executeDisplayList({
    required int displayListHash,
    required _DisplayListState state,
    required Set<int> visiting,
    required int depth,
  }) {
    if (depth > maxDepth || visiting.contains(displayListHash)) {
      return;
    }

    final resource = resourcesByHash[displayListHash];
    if (resource == null ||
        resource.type != O2rSceneMeshImporterService._resourceTypeDisplayList) {
      return;
    }

    visiting.add(displayListHash);

    final reader = _ByteReader(
      bytes: resource.bytes,
      offset: O2rSceneMeshImporterService._resourceHeaderSize,
      endian: resource.endian,
    );

    if (!reader.hasRemaining) {
      visiting.remove(displayListHash);
      return;
    }

    reader.readUint8(); // ucode
    while ((reader.position - O2rSceneMeshImporterService._resourceHeaderSize) %
                8 !=
            0 &&
        reader.hasRemaining) {
      reader.readUint8();
    }

    var commandCount = 0;
    while (reader.remaining >= 8 && commandCount < maxCommandCount) {
      commandCount++;

      final w0 = reader.readUint32();
      final w1 = reader.readUint32();
      final opcode = (w0 >> 24) & 0xFF;

      if (opcode == O2rSceneMeshImporterService._opcodeSetTimgOtrHash) {
        if (reader.remaining < 8) {
          break;
        }

        final hashHigh = reader.readUint32();
        final hashLow = reader.readUint32();
        final textureHash = (hashHigh << 32) | hashLow;
        state.setCurrentTexture(textureHash);
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeVtxOtrHash) {
        if (reader.remaining < 8) {
          break;
        }

        final vtxCount = (w0 >> 12) & 0xFF;
        final vtxBufferEnd = (w0 >> 1) & 0x7F;
        final vtxDest = vtxBufferEnd - vtxCount;
        final vtxDataOffset = w1;

        final hashHigh = reader.readUint32();
        final hashLow = reader.readUint32();
        final vertexHash = (hashHigh << 32) | hashLow;

        state.loadVertices(
          vertexHash: vertexHash,
          count: vtxCount,
          destination: vtxDest,
          byteOffset: vtxDataOffset,
        );
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeDlOtrHash) {
        if (reader.remaining < 8) {
          break;
        }

        final hashHigh = reader.readUint32();
        final hashLow = reader.readUint32();
        final targetHash = (hashHigh << 32) | hashLow;

        final isBranch = ((w0 >> 16) & 0x1) == 1;
        _executeDisplayList(
          displayListHash: targetHash,
          state: state,
          visiting: visiting,
          depth: depth + 1,
        );

        if (isBranch) {
          break;
        }
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeVtxOtrFilePath) {
        if (reader.remaining < 8) {
          break;
        }

        final argW0 = reader.readUint32();
        final argW1 = reader.readUint32();
        final vtxCount = argW0 & 0xFFFF;
        final vtxDest = (argW1 >> 16) & 0xFFFF;
        final vtxDataOffset = argW1 & 0xFFFF;

        state.loadVertices(
          vertexHash: 0,
          count: vtxCount,
          destination: vtxDest,
          byteOffset: vtxDataOffset,
        );
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeSetTimgOtrFilePath) {
        state.setCurrentTexture(null);
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeDlOtrFilePath) {
        final isBranch = ((w0 >> 16) & 0x1) == 1;
        if (isBranch) {
          break;
        }
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeTri1) {
        final a = ((w0 >> 16) & 0xFF) ~/ 2;
        final b = ((w0 >> 8) & 0xFF) ~/ 2;
        final c = (w0 & 0xFF) ~/ 2;
        state.emitTriangle(a, b, c);
        if (state.triangleLimitReached) {
          break;
        }
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeTri2) {
        final a = ((w0 >> 16) & 0xFF) ~/ 2;
        final b = ((w0 >> 8) & 0xFF) ~/ 2;
        final c = (w0 & 0xFF) ~/ 2;
        final d = ((w1 >> 16) & 0xFF) ~/ 2;
        final e = ((w1 >> 8) & 0xFF) ~/ 2;
        final f = (w1 & 0xFF) ~/ 2;
        state.emitTriangle(a, b, c);
        state.emitTriangle(d, e, f);
        if (state.triangleLimitReached) {
          break;
        }
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeTri1Otr) {
        final a = w0 & 0xFF;
        final b = (w1 >> 16) & 0xFF;
        final c = w1 & 0xFF;
        state.emitTriangle(a, b, c);
        if (state.triangleLimitReached) {
          break;
        }
        continue;
      }

      if (opcode == O2rSceneMeshImporterService._opcodeEndDl) {
        break;
      }

      if (O2rSceneMeshImporterService._expandedCommandOpcodes.contains(
        opcode,
      )) {
        if (reader.remaining < 8) {
          break;
        }
        reader.readUint32();
        reader.readUint32();
      }
    }

    visiting.remove(displayListHash);
  }

  List<O2rSceneVertex>? _resolveVertices(int vertexHash) {
    if (vertexHash == 0) {
      return null;
    }

    if (vertexCache.containsKey(vertexHash)) {
      return vertexCache[vertexHash];
    }

    final resource = resourcesByHash[vertexHash];
    if (resource == null ||
        resource.type != O2rSceneMeshImporterService._resourceTypeArray) {
      vertexCache[vertexHash] = null;
      return null;
    }

    try {
      final reader = _ByteReader(
        bytes: resource.bytes,
        offset: O2rSceneMeshImporterService._resourceHeaderSize,
        endian: resource.endian,
      );

      final arrayType = reader.readUint32();
      final count = reader.readUint32();
      if (arrayType != O2rSceneMeshImporterService._arrayTypeVertex ||
          count > 250000) {
        vertexCache[vertexHash] = null;
        return null;
      }

      final vertices = <O2rSceneVertex>[];
      for (var i = 0; i < count; i++) {
        final x = reader.readInt16().toDouble();
        final y = reader.readInt16().toDouble();
        final z = reader.readInt16().toDouble();
        reader.readUint16(); // flag
        final s = reader.readInt16();
        final t = reader.readInt16();
        final r = reader.readUint8();
        final g = reader.readUint8();
        final b = reader.readUint8();
        final a = reader.readUint8();
        vertices.add(
          O2rSceneVertex(x: x, y: y, z: z, s: s, t: t, r: r, g: g, b: b, a: a),
        );
      }

      vertexCache[vertexHash] = vertices;
      return vertices;
    } catch (_) {
      vertexCache[vertexHash] = null;
      return null;
    }
  }
}

class _DisplayListState {
  static const int _f3dex2VertexSlots = 64;
  _DisplayListState({
    required List<O2rSceneVertex>? Function(int) resolveVertices,
    required this.collector,
  }) : _resolveVertices = resolveVertices;

  final List<O2rSceneVertex>? Function(int) _resolveVertices;
  final _TriangleCollector collector;
  final List<O2rSceneVertex?> _loadedVertices = List<O2rSceneVertex?>.filled(
    _f3dex2VertexSlots,
    null,
    growable: false,
  );

  int? _currentTextureHash;

  bool get triangleLimitReached => collector.limitReached;

  void setCurrentTexture(int? textureHash) {
    _currentTextureHash = textureHash;
  }

  void loadVertices({
    required int vertexHash,
    required int count,
    required int destination,
    required int byteOffset,
  }) {
    if (count <= 0 || destination >= _loadedVertices.length) {
      return;
    }

    final vertices = _resolveVertices(vertexHash);
    if (vertices == null || vertices.isEmpty) {
      return;
    }

    final sourceStart = byteOffset ~/ 16;
    for (var i = 0; i < count; i++) {
      final sourceIndex = sourceStart + i;
      final destIndex = destination + i;

      if (sourceIndex < 0 ||
          sourceIndex >= vertices.length ||
          destIndex < 0 ||
          destIndex >= _loadedVertices.length) {
        continue;
      }

      _loadedVertices[destIndex] = vertices[sourceIndex];
    }
  }

  void emitTriangle(int a, int b, int c) {
    if (a < 0 ||
        b < 0 ||
        c < 0 ||
        a >= _loadedVertices.length ||
        b >= _loadedVertices.length ||
        c >= _loadedVertices.length) {
      return;
    }

    final va = _loadedVertices[a];
    final vb = _loadedVertices[b];
    final vc = _loadedVertices[c];
    if (va == null || vb == null || vc == null) {
      return;
    }

    if (a == b || b == c || a == c) {
      return;
    }

    collector.addTriangle(va, vb, vc, _currentTextureHash);
  }
}

class _TriangleCollector {
  _TriangleCollector({required this.maxTriangles});

  final int maxTriangles;
  final Map<int?, _TriangleMeshBuilder> _buckets =
      <int?, _TriangleMeshBuilder>{};

  bool limitReached = false;
  var _triangleCount = 0;

  bool get hasTriangles => _triangleCount > 0;

  void addTriangle(
    O2rSceneVertex a,
    O2rSceneVertex b,
    O2rSceneVertex c,
    int? textureHash,
  ) {
    if (limitReached) {
      return;
    }

    if (_triangleCount >= maxTriangles) {
      limitReached = true;
      return;
    }

    final bucket = _buckets.putIfAbsent(textureHash, _TriangleMeshBuilder.new);
    bucket.addTriangle(a, b, c);
    _triangleCount++;
  }

  List<O2rSceneMesh> buildMeshes(String resourcePath) {
    final meshes = <O2rSceneMesh>[];

    for (final entry in _buckets.entries) {
      final builder = entry.value;
      if (builder.indices.isEmpty) {
        continue;
      }

      meshes.add(
        O2rSceneMesh(
          resourcePath: resourcePath,
          vertices: builder.vertices,
          triangleIndices: builder.indices,
          textureHash: entry.key,
        ),
      );
    }

    return meshes;
  }
}

class _TriangleMeshBuilder {
  final List<O2rSceneVertex> vertices = <O2rSceneVertex>[];
  final List<int> indices = <int>[];

  void addTriangle(O2rSceneVertex a, O2rSceneVertex b, O2rSceneVertex c) {
    final base = vertices.length;
    vertices.add(a);
    vertices.add(b);
    vertices.add(c);

    indices.add(base);
    indices.add(base + 1);
    indices.add(base + 2);
  }
}

class _ByteReader {
  _ByteReader({
    required Uint8List bytes,
    required int offset,
    required Endian endian,
  }) : _data = ByteData.sublistView(bytes),
       _offset = offset,
       _endian = endian,
       _length = bytes.length;

  final ByteData _data;
  final Endian _endian;
  final int _length;
  int _offset;

  int get position => _offset;
  int get remaining => _length - _offset;
  bool get hasRemaining => _offset < _length;

  int readUint8() {
    _require(1);
    final value = _data.getUint8(_offset);
    _offset += 1;
    return value;
  }

  int readInt16() {
    _require(2);
    final value = _data.getInt16(_offset, _endian);
    _offset += 2;
    return value;
  }

  int readUint16() {
    _require(2);
    final value = _data.getUint16(_offset, _endian);
    _offset += 2;
    return value;
  }

  int readUint32() {
    _require(4);
    final value = _data.getUint32(_offset, _endian);
    _offset += 4;
    return value;
  }

  void _require(int length) {
    if (_offset + length > _length) {
      throw RangeError("Unexpected end of resource stream.");
    }
  }
}

class _IndexedResources {
  const _IndexedResources({
    required this.resourcesByHash,
    required this.displayLists,
    required this.texturesByHash,
  });

  final Map<int, _ResourceEntry> resourcesByHash;
  final List<_ResourceEntry> displayLists;
  final Map<int, O2rSceneTexture> texturesByHash;
}

class _ResourceHeader {
  const _ResourceHeader({
    required this.endian,
    required this.type,
    required this.id,
  });

  final Endian endian;
  final int type;
  final int id;
}

class _ResourceEntry {
  const _ResourceEntry({
    required this.hash,
    required this.type,
    required this.endian,
    required this.path,
    required this.bytes,
  });

  final int hash;
  final int type;
  final Endian endian;
  final String path;
  final Uint8List bytes;
}
