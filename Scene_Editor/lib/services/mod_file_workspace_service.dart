import "dart:convert";
import "dart:io";

import "package:file_picker/file_picker.dart";

import "../models/editable_mod_file.dart";

class ModFileWorkspaceService {
  static const Set<String> _editableExtensions = {
    ".xml",
    ".json",
    ".txt",
    ".cfg",
    ".ini",
    ".md",
    ".csv",
    ".lua",
  };

  static const int _maxEditableBytes = 1024 * 1024;

  Future<List<EditableModFile>> pickFiles() async {
    final result = await FilePicker.platform.pickFiles(
      dialogTitle: "Select files to edit",
      allowMultiple: true,
      withData: false,
      type: FileType.custom,
      allowedExtensions: _editableExtensions
          .map((item) => item.replaceAll(".", ""))
          .toList(growable: false),
    );

    if (result == null || result.files.isEmpty) {
      return const [];
    }

    final imported = <EditableModFile>[];
    for (final picked in result.files) {
      final path = picked.path;
      if (path == null || path.isEmpty) {
        continue;
      }

      final file = File(path);
      final editable = await _readFileAsEditable(
        file,
        sanitizeRelativePath(picked.name),
      );
      if (editable != null) {
        imported.add(editable);
      }
    }

    return imported;
  }

  Future<List<EditableModFile>> pickDirectoryAndImport({
    int maxFiles = 400,
  }) async {
    final directoryPath = await FilePicker.platform.getDirectoryPath(
      dialogTitle: "Select folder with editable files",
    );
    if (directoryPath == null || directoryPath.isEmpty) {
      return const [];
    }

    return importFromDirectory(directoryPath, maxFiles: maxFiles);
  }

  Future<List<EditableModFile>> importFromDirectory(
    String rootPath, {
    int maxFiles = 400,
  }) async {
    final root = Directory(rootPath);
    if (!root.existsSync()) {
      return const [];
    }

    final files = <EditableModFile>[];
    await for (final entity in root.list(recursive: true, followLinks: false)) {
      if (entity is! File) {
        continue;
      }

      if (files.length >= maxFiles) {
        break;
      }

      final extension = _getExtension(entity.path);
      if (!_editableExtensions.contains(extension)) {
        continue;
      }

      final relativePath = _toRelativePath(root.path, entity.path);
      final editable = await _readFileAsEditable(entity, relativePath);
      if (editable != null) {
        files.add(editable);
      }
    }

    return files;
  }

  Future<EditableModFile?> _readFileAsEditable(
    File file,
    String relativePath,
  ) async {
    try {
      final bytes = await file.readAsBytes();
      if (bytes.length > _maxEditableBytes) {
        return null;
      }

      final content = utf8.decode(bytes, allowMalformed: true);
      return EditableModFile(
        id: generateEditableFileId(),
        relativePath: sanitizeRelativePath(relativePath),
        content: content,
        sourcePath: file.path,
      );
    } catch (_) {
      return null;
    }
  }

  String sanitizeRelativePath(String path) {
    var normalized = path.replaceAll("\\", "/").trim();
    normalized = normalized.replaceAll(RegExp(r"^/+"), "");

    final segments = normalized
        .split("/")
        .where(
          (segment) => segment.isNotEmpty && segment != "." && segment != "..",
        )
        .toList(growable: false);

    if (segments.isEmpty) {
      return "unnamed.txt";
    }

    return segments.join("/");
  }

  String _toRelativePath(String rootPath, String fullPath) {
    final normalizedRoot = rootPath.replaceAll("\\", "/");
    final normalizedFull = fullPath.replaceAll("\\", "/");

    if (normalizedFull.toLowerCase().startsWith(normalizedRoot.toLowerCase())) {
      var relative = normalizedFull.substring(normalizedRoot.length);
      relative = relative.replaceAll(RegExp(r"^/+"), "");
      return sanitizeRelativePath(relative);
    }

    final parts = normalizedFull.split("/");
    return sanitizeRelativePath(parts.isEmpty ? normalizedFull : parts.last);
  }

  String _getExtension(String path) {
    final normalized = path.replaceAll("\\", "/");
    final dotIndex = normalized.lastIndexOf(".");
    if (dotIndex < 0) {
      return "";
    }
    return normalized.substring(dotIndex).toLowerCase();
  }
}
