import "dart:typed_data";

import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/models/rom_metadata.dart";
import "package:scene_editor/services/n64_rom_parser.dart";

void main() {
  group("N64RomParser", () {
    test("detects and parses big-endian header", () {
      final data = _buildFakeRom(byteOrder: N64ByteOrder.bigEndian);
      final header = N64RomParser.parseHeader(data);

      expect(header.byteOrder, N64ByteOrder.bigEndian);
      expect(header.title, "THE LEGEND OF ZELDA");
      expect(header.gameCode, "CZLE");
      expect(header.version, 1);
    });

    test("normalizes byte-swapped header", () {
      final data = _buildFakeRom(byteOrder: N64ByteOrder.byteSwapped);
      final header = N64RomParser.parseHeader(data);

      expect(header.byteOrder, N64ByteOrder.byteSwapped);
      expect(header.title, "THE LEGEND OF ZELDA");
      expect(header.gameCode, "CZLE");
      expect(header.version, 1);
    });

    test("normalizes little-endian header", () {
      final data = _buildFakeRom(byteOrder: N64ByteOrder.littleEndian);
      final header = N64RomParser.parseHeader(data);

      expect(header.byteOrder, N64ByteOrder.littleEndian);
      expect(header.title, "THE LEGEND OF ZELDA");
      expect(header.gameCode, "CZLE");
      expect(header.version, 1);
    });
  });
}

Uint8List _buildFakeRom({required N64ByteOrder byteOrder}) {
  final bigEndian = Uint8List(0x60);
  bigEndian[0] = 0x80;
  bigEndian[1] = 0x37;
  bigEndian[2] = 0x12;
  bigEndian[3] = 0x40;

  final titleBytes = "THE LEGEND OF ZELDA".codeUnits;
  for (var i = 0; i < titleBytes.length; i++) {
    bigEndian[0x20 + i] = titleBytes[i];
  }
  bigEndian[0x3B] = "C".codeUnitAt(0);
  bigEndian[0x3C] = "Z".codeUnitAt(0);
  bigEndian[0x3D] = "L".codeUnitAt(0);
  bigEndian[0x3E] = "E".codeUnitAt(0);
  bigEndian[0x3F] = 1;

  switch (byteOrder) {
    case N64ByteOrder.bigEndian:
      return bigEndian;
    case N64ByteOrder.byteSwapped:
      final out = Uint8List.fromList(bigEndian);
      for (var i = 0; i + 1 < out.length; i += 2) {
        final tmp = out[i];
        out[i] = out[i + 1];
        out[i + 1] = tmp;
      }
      return out;
    case N64ByteOrder.littleEndian:
      final out = Uint8List.fromList(bigEndian);
      for (var i = 0; i + 3 < out.length; i += 4) {
        final a = out[i];
        final b = out[i + 1];
        out[i] = out[i + 3];
        out[i + 1] = out[i + 2];
        out[i + 2] = b;
        out[i + 3] = a;
      }
      return out;
  }
}
