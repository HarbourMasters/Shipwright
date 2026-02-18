import "dart:io";

import "package:crypto/crypto.dart";
import "package:file_picker/file_picker.dart";

import "../models/rom_metadata.dart";
import "n64_rom_parser.dart";

class RomLoaderService {
  Future<RomMetadata?> pickAndLoadRom() async {
    final result = await FilePicker.platform.pickFiles(
      dialogTitle: "Select Ocarina of Time ROM",
      type: FileType.custom,
      allowedExtensions: const ["z64", "n64", "v64", "rom"],
      withData: false,
      allowMultiple: false,
    );

    if (result == null || result.files.isEmpty) {
      return null;
    }

    final path = result.files.single.path;
    if (path == null || path.isEmpty) {
      throw const FormatException("Invalid ROM path.");
    }

    final romFile = File(path);
    final bytes = await romFile.readAsBytes();
    final header = N64RomParser.parseHeader(bytes);
    final hash = sha1.convert(bytes).toString();
    final fileName = _extractFileName(path);

    return RomMetadata(
      path: path,
      fileName: fileName,
      sizeBytes: bytes.length,
      sha1: hash,
      byteOrder: header.byteOrder,
      title: header.title,
      gameCode: header.gameCode,
      version: header.version,
    );
  }

  String _extractFileName(String path) {
    final parts = path.split(RegExp(r"[\\/]"));
    if (parts.isEmpty) {
      return path;
    }
    return parts.last;
  }
}
