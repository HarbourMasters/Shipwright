import "dart:io";

import "package:flutter/material.dart";

import "../models/editable_mod_file.dart";
import "../services/mod_file_workspace_service.dart";

class ModFileEditorScreen extends StatefulWidget {
  const ModFileEditorScreen({
    super.key,
    required this.initialFiles,
    this.romPath,
  });

  final List<EditableModFile> initialFiles;
  final String? romPath;

  @override
  State<ModFileEditorScreen> createState() => _ModFileEditorScreenState();
}

class _ModFileEditorScreenState extends State<ModFileEditorScreen> {
  final ModFileWorkspaceService _workspaceService = ModFileWorkspaceService();
  final TextEditingController _relativePathController = TextEditingController();
  final TextEditingController _contentController = TextEditingController();

  late List<EditableModFile> _files;
  int _selectedIndex = -1;
  bool _busy = false;
  bool _updatingControllers = false;
  String _status = "Import XML/JSON/TXT files and edit before exporting ZIP.";

  EditableModFile? get _selectedFile {
    if (_selectedIndex < 0 || _selectedIndex >= _files.length) {
      return null;
    }
    return _files[_selectedIndex];
  }

  @override
  void initState() {
    super.initState();
    _files = widget.initialFiles
        .map(
          (item) => EditableModFile(
            id: item.id,
            relativePath: item.relativePath,
            content: item.content,
            sourcePath: item.sourcePath,
          ),
        )
        .toList(growable: true);

    if (_files.isNotEmpty) {
      _selectedIndex = 0;
    }

    _relativePathController.addListener(_onEditorChanged);
    _contentController.addListener(_onEditorChanged);

    _syncEditorFromSelection();
  }

  @override
  void dispose() {
    _relativePathController.dispose();
    _contentController.dispose();
    super.dispose();
  }

  void _onEditorChanged() {
    if (_updatingControllers) {
      return;
    }

    final selected = _selectedFile;
    if (selected == null) {
      return;
    }

    final updated = selected.copyWith(
      relativePath: _workspaceService.sanitizeRelativePath(
        _relativePathController.text,
      ),
      content: _contentController.text,
    );

    setState(() {
      _files[_selectedIndex] = updated;
    });
  }

  void _syncEditorFromSelection() {
    final selected = _selectedFile;
    _updatingControllers = true;
    if (selected == null) {
      _relativePathController.text = "";
      _contentController.text = "";
    } else {
      _relativePathController.text = selected.relativePath;
      _contentController.text = selected.content;
    }
    _updatingControllers = false;
  }

  Future<void> _importFiles() async {
    await _runImport(
      actionLabel: "Importing selected files...",
      importer: () => _workspaceService.pickFiles(),
    );
  }

  Future<void> _importFolder() async {
    await _runImport(
      actionLabel: "Importing files from folder...",
      importer: () => _workspaceService.pickDirectoryAndImport(maxFiles: 600),
    );
  }

  Future<void> _importFromRomFolder() async {
    final romPath = widget.romPath;
    if (romPath == null || romPath.isEmpty) {
      setState(() {
        _status = "Load ROM first to import from ROM folder.";
      });
      return;
    }

    final folderPath = Directory(romPath).parent.path;
    await _runImport(
      actionLabel: "Importing files from ROM folder...",
      importer: () =>
          _workspaceService.importFromDirectory(folderPath, maxFiles: 600),
    );
  }

  Future<void> _runImport({
    required String actionLabel,
    required Future<List<EditableModFile>> Function() importer,
  }) async {
    setState(() {
      _busy = true;
      _status = actionLabel;
    });

    try {
      final imported = await importer();
      if (!mounted) {
        return;
      }

      if (imported.isEmpty) {
        setState(() {
          _status = "No editable files imported.";
        });
        return;
      }

      final firstImportedPath = _mergeImportedFiles(imported);
      setState(() {
        _status = "Imported ${imported.length} editable files.";
      });

      if (firstImportedPath != null) {
        final index = _files.indexWhere(
          (item) => item.relativePath == firstImportedPath,
        );
        if (index >= 0) {
          setState(() {
            _selectedIndex = index;
          });
          _syncEditorFromSelection();
        }
      }
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Import failed: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
        });
      }
    }
  }

  String? _mergeImportedFiles(List<EditableModFile> imported) {
    var firstAddedPath = "";

    for (final file in imported) {
      var path = _workspaceService.sanitizeRelativePath(file.relativePath);
      path = _nextUniquePath(path);

      final updated = file.copyWith(relativePath: path);
      _files.add(updated);

      if (firstAddedPath.isEmpty) {
        firstAddedPath = path;
      }
    }

    return firstAddedPath.isEmpty ? null : firstAddedPath;
  }

  String _nextUniquePath(String relativePath) {
    final existing = _files
        .map((item) => item.relativePath.toLowerCase())
        .toSet();
    if (!existing.contains(relativePath.toLowerCase())) {
      return relativePath;
    }

    final slashIndex = relativePath.lastIndexOf("/");
    final directory = slashIndex >= 0
        ? relativePath.substring(0, slashIndex + 1)
        : "";
    final fileName = slashIndex >= 0
        ? relativePath.substring(slashIndex + 1)
        : relativePath;

    final dotIndex = fileName.lastIndexOf(".");
    final base = dotIndex > 0 ? fileName.substring(0, dotIndex) : fileName;
    final extension = dotIndex > 0 ? fileName.substring(dotIndex) : "";

    var counter = 1;
    while (true) {
      final candidate = "$directory${base}_$counter$extension";
      if (!existing.contains(candidate.toLowerCase())) {
        return candidate;
      }
      counter++;
    }
  }

  void _removeSelected() {
    if (_selectedIndex < 0 || _selectedIndex >= _files.length) {
      return;
    }

    setState(() {
      _files.removeAt(_selectedIndex);
      if (_files.isEmpty) {
        _selectedIndex = -1;
      } else {
        _selectedIndex = _selectedIndex.clamp(0, _files.length - 1);
      }
      _status = "File removed from workspace.";
    });

    _syncEditorFromSelection();
  }

  void _saveAndClose() {
    Navigator.of(context).pop(_files);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Mod File Editor"),
        actions: [
          TextButton.icon(
            onPressed: _busy ? null : () => Navigator.of(context).pop(),
            icon: const Icon(Icons.close),
            label: const Text("Cancel"),
          ),
          const SizedBox(width: 8),
          FilledButton.icon(
            onPressed: _busy ? null : _saveAndClose,
            icon: const Icon(Icons.save_outlined),
            label: const Text("Apply"),
          ),
          const SizedBox(width: 12),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(12),
        child: Row(
          children: [
            SizedBox(
              width: 360,
              child: Card(
                child: Padding(
                  padding: const EdgeInsets.all(10),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Row(
                        children: [
                          Expanded(
                            child: FilledButton.icon(
                              onPressed: _busy ? null : _importFiles,
                              icon: const Icon(Icons.note_add_outlined),
                              label: const Text("Import Files"),
                            ),
                          ),
                          const SizedBox(width: 6),
                          IconButton(
                            tooltip: "Import folder",
                            onPressed: _busy ? null : _importFolder,
                            icon: const Icon(Icons.folder_open),
                          ),
                          IconButton(
                            tooltip: "Import from ROM folder",
                            onPressed: _busy ? null : _importFromRomFolder,
                            icon: const Icon(Icons.sd_storage_outlined),
                          ),
                        ],
                      ),
                      const SizedBox(height: 8),
                      Container(
                        width: double.infinity,
                        padding: const EdgeInsets.all(8),
                        decoration: BoxDecoration(
                          color: Theme.of(
                            context,
                          ).colorScheme.surfaceContainerHighest,
                          borderRadius: BorderRadius.circular(8),
                        ),
                        child: Text(_status),
                      ),
                      const SizedBox(height: 8),
                      Row(
                        children: [
                          Text(
                            "Workspace Files (${_files.length})",
                            style: Theme.of(context).textTheme.titleMedium,
                          ),
                          const Spacer(),
                          IconButton(
                            tooltip: "Remove selected",
                            onPressed: _busy || _selectedFile == null
                                ? null
                                : _removeSelected,
                            icon: const Icon(Icons.delete_outline),
                          ),
                        ],
                      ),
                      const SizedBox(height: 4),
                      Expanded(
                        child: _files.isEmpty
                            ? const Center(child: Text("No files imported."))
                            : ListView.builder(
                                itemCount: _files.length,
                                itemBuilder: (context, index) {
                                  final file = _files[index];
                                  final selected = index == _selectedIndex;
                                  return Card(
                                    color: selected
                                        ? Theme.of(
                                            context,
                                          ).colorScheme.primaryContainer
                                        : null,
                                    child: ListTile(
                                      dense: true,
                                      title: Text(
                                        file.relativePath,
                                        maxLines: 1,
                                        overflow: TextOverflow.ellipsis,
                                      ),
                                      subtitle: Text(
                                        file.sourcePath ?? "workspace",
                                        maxLines: 1,
                                        overflow: TextOverflow.ellipsis,
                                      ),
                                      selected: selected,
                                      onTap: _busy
                                          ? null
                                          : () {
                                              setState(() {
                                                _selectedIndex = index;
                                              });
                                              _syncEditorFromSelection();
                                            },
                                    ),
                                  );
                                },
                              ),
                      ),
                    ],
                  ),
                ),
              ),
            ),
            const SizedBox(width: 12),
            Expanded(
              child: Card(
                child: Padding(
                  padding: const EdgeInsets.all(10),
                  child: _selectedFile == null
                      ? const Center(
                          child: Text("Select or import a file to edit."),
                        )
                      : Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            TextField(
                              controller: _relativePathController,
                              enabled: !_busy,
                              decoration: const InputDecoration(
                                labelText: "Archive relative path",
                                hintText: "assets/custom/file.xml",
                                border: OutlineInputBorder(),
                                isDense: true,
                              ),
                            ),
                            const SizedBox(height: 8),
                            Expanded(
                              child: TextField(
                                controller: _contentController,
                                enabled: !_busy,
                                expands: true,
                                maxLines: null,
                                minLines: null,
                                textAlignVertical: TextAlignVertical.top,
                                style: const TextStyle(
                                  fontFamily: "Consolas",
                                  fontSize: 13,
                                ),
                                decoration: const InputDecoration(
                                  labelText: "File content",
                                  alignLabelWithHint: true,
                                  border: OutlineInputBorder(),
                                ),
                              ),
                            ),
                          ],
                        ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
