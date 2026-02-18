class EditableModFile {
  const EditableModFile({
    required this.id,
    required this.relativePath,
    required this.content,
    this.sourcePath,
  });

  final String id;
  final String relativePath;
  final String content;
  final String? sourcePath;

  EditableModFile copyWith({
    String? id,
    String? relativePath,
    String? content,
    String? sourcePath,
  }) {
    return EditableModFile(
      id: id ?? this.id,
      relativePath: relativePath ?? this.relativePath,
      content: content ?? this.content,
      sourcePath: sourcePath ?? this.sourcePath,
    );
  }

  Map<String, dynamic> toJson() {
    return {
      "id": id,
      "relativePath": relativePath,
      "sourcePath": sourcePath,
      "contentLength": content.length,
    };
  }
}

String generateEditableFileId() {
  return "file_${DateTime.now().microsecondsSinceEpoch}";
}
