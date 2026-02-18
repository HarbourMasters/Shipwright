import "dart:convert";
import "dart:typed_data";

import "../models/rom_metadata.dart";

class N64RomHeader {
  const N64RomHeader({
    required this.byteOrder,
    required this.title,
    required this.gameCode,
    required this.version,
  });

  final N64ByteOrder byteOrder;
  final String title;
  final String gameCode;
  final int version;
}

class N64RomParser {
  static N64ByteOrder detectByteOrder(Uint8List data) {
    if (data.length < 4) {
      throw const FormatException("ROM is too small to contain an N64 header.");
    }

    final b0 = data[0];
    final b1 = data[1];
    final b2 = data[2];
    final b3 = data[3];

    if (b0 == 0x80 && b1 == 0x37 && b2 == 0x12 && b3 == 0x40) {
      return N64ByteOrder.bigEndian;
    }
    if (b0 == 0x37 && b1 == 0x80 && b2 == 0x40 && b3 == 0x12) {
      return N64ByteOrder.byteSwapped;
    }
    if (b0 == 0x40 && b1 == 0x12 && b2 == 0x37 && b3 == 0x80) {
      return N64ByteOrder.littleEndian;
    }

    throw const FormatException("Unknown ROM byte order.");
  }

  static Uint8List normalizeToBigEndian(Uint8List data) {
    final byteOrder = detectByteOrder(data);
    final output = Uint8List.fromList(data);

    switch (byteOrder) {
      case N64ByteOrder.bigEndian:
        return output;
      case N64ByteOrder.byteSwapped:
        for (var i = 0; i + 1 < output.length; i += 2) {
          final tmp = output[i];
          output[i] = output[i + 1];
          output[i + 1] = tmp;
        }
        return output;
      case N64ByteOrder.littleEndian:
        for (var i = 0; i + 3 < output.length; i += 4) {
          final a = output[i];
          final b = output[i + 1];
          output[i] = output[i + 3];
          output[i + 1] = output[i + 2];
          output[i + 2] = b;
          output[i + 3] = a;
        }
        return output;
    }
  }

  static N64RomHeader parseHeader(Uint8List romBytes) {
    if (romBytes.length < 0x40) {
      throw const FormatException(
        "ROM is too small. Expected at least 0x40 bytes.",
      );
    }

    final byteOrder = detectByteOrder(romBytes);
    final header = normalizeToBigEndian(romBytes.sublist(0, 0x40));
    final title = _readAscii(header, 0x20, 20).trim();
    final gameCode = _readAscii(header, 0x3B, 4).trim();
    final version = header[0x3F];

    return N64RomHeader(
      byteOrder: byteOrder,
      title: title,
      gameCode: gameCode,
      version: version,
    );
  }

  static String _readAscii(Uint8List bytes, int start, int length) {
    final slice = bytes.sublist(start, start + length);
    final sanitized = slice
        .map((byte) {
          if (byte >= 32 && byte <= 126) {
            return byte;
          }
          return 32;
        })
        .toList(growable: false);
    return latin1.decode(sanitized);
  }
}
