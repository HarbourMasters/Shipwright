enum N64ByteOrder { bigEndian, byteSwapped, littleEndian }

class RomMetadata {
  const RomMetadata({
    required this.path,
    required this.fileName,
    required this.sizeBytes,
    required this.sha1,
    required this.byteOrder,
    required this.title,
    required this.gameCode,
    required this.version,
  });

  final String path;
  final String fileName;
  final int sizeBytes;
  final String sha1;
  final N64ByteOrder byteOrder;
  final String title;
  final String gameCode;
  final int version;

  bool get looksLikeOot => title.toUpperCase().contains("ZELDA");
  bool get looksLikeNtscUsa => gameCode.endsWith("E");
  bool get looksLikeUsa11 => looksLikeOot && looksLikeNtscUsa && version == 1;

  String get byteOrderLabel {
    switch (byteOrder) {
      case N64ByteOrder.bigEndian:
        return "z64 (big-endian)";
      case N64ByteOrder.byteSwapped:
        return "n64 (byte-swapped)";
      case N64ByteOrder.littleEndian:
        return "v64 (little-endian)";
    }
  }
}
