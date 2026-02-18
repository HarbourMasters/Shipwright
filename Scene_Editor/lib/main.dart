import "dart:io";
import "package:file_picker/file_picker.dart";
import "package:flutter/material.dart";

import "data/oot_scene_catalog.dart";
import "models/editable_mod_file.dart";
import "models/rom_metadata.dart";
import "models/scene_patch.dart";
import "models/toolchain_config.dart";
import "screens/mod_file_editor_screen.dart";
import "screens/scene_studio_3d_screen.dart";
import "services/fast64_bridge_service.dart";
import "services/mod_export_service.dart";
import "services/rom_loader_service.dart";
import "services/toolchain_config_service.dart";
import "widgets/scene_editor_panel.dart";

void main() {
  runApp(const SceneEditorApp());
}

class SceneEditorApp extends StatelessWidget {
  const SceneEditorApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "Scene Editor",
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        fontFamily: "Trebuchet MS",
        colorScheme: ColorScheme.fromSeed(
          seedColor: const Color(0xFF0E7A66),
          brightness: Brightness.light,
        ),
        useMaterial3: true,
        scaffoldBackgroundColor: const Color(0xFFF2ECE1),
        appBarTheme: const AppBarTheme(
          backgroundColor: Color(0xFF174451),
          foregroundColor: Colors.white,
        ),
        cardTheme: CardThemeData(
          elevation: 1.2,
          margin: EdgeInsets.zero,
          color: const Color(0xFFFFFBF5),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(14),
          ),
        ),
        inputDecorationTheme: InputDecorationTheme(
          filled: true,
          fillColor: const Color(0xFFFCF7EF),
          border: OutlineInputBorder(borderRadius: BorderRadius.circular(10)),
          enabledBorder: OutlineInputBorder(
            borderRadius: BorderRadius.circular(10),
            borderSide: const BorderSide(color: Color(0xFFCFBEA7)),
          ),
        ),
      ),
      home: const SceneEditorHome(),
    );
  }
}

class SceneEditorHome extends StatefulWidget {
  const SceneEditorHome({super.key});

  @override
  State<SceneEditorHome> createState() => _SceneEditorHomeState();
}

class _SceneEditorHomeState extends State<SceneEditorHome> {
  final RomLoaderService _romLoaderService = RomLoaderService();
  final ModExportService _modExportService = ModExportService();
  final ToolchainConfigService _toolchainConfigService =
      ToolchainConfigService();

  late final Fast64BridgeService _fast64BridgeService = Fast64BridgeService(
    toolchainConfigService: _toolchainConfigService,
  );

  late List<ScenePatch> _scenes;
  RomMetadata? _rom;
  ToolchainConfig? _toolchainConfig;
  ToolchainValidationResult? _toolchainValidation;
  Fast64ExportResult? _lastFast64Export;
  final List<EditableModFile> _editableFiles = [];
  String _status = "Load a ROM to start scene editing.";
  int _selectedSceneIndex = 0;
  bool _busy = false;

  @override
  void initState() {
    super.initState();
    _scenes = ootSceneCatalog
        .map(ScenePatch.fromTemplate)
        .toList(growable: true);
    _loadToolchainConfig();
  }

  Future<void> _loadToolchainConfig() async {
    final config = await _toolchainConfigService.load();
    if (!mounted) {
      return;
    }

    setState(() {
      _toolchainConfig = config;
    });
  }

  Future<void> _configureToolchain() async {
    final initial = _toolchainConfig ?? await _toolchainConfigService.load();
    if (!mounted) {
      return;
    }

    final blenderController = TextEditingController(
      text: initial.blenderExePath,
    );
    final fast64InstallController = TextEditingController(
      text: initial.fast64InstallPath,
    );
    final pinnedTagController = TextEditingController(
      text: initial.fast64PinnedTag,
    );
    final projectRootController = TextEditingController(
      text: initial.fast64ProjectRoot,
    );
    final pythonController = TextEditingController(
      text: initial.pythonPath ?? "",
    );

    final configured = await showDialog<ToolchainConfig>(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setLocalState) {
            Future<void> pickFile(
              TextEditingController controller, {
              required String title,
              List<String>? allowedExtensions,
            }) async {
              final result = await FilePicker.platform.pickFiles(
                dialogTitle: title,
                allowMultiple: false,
                type: allowedExtensions == null
                    ? FileType.any
                    : FileType.custom,
                allowedExtensions: allowedExtensions,
                withData: false,
              );
              if (result == null || result.files.isEmpty) {
                return;
              }

              final path = result.files.single.path;
              if (path == null || path.isEmpty) {
                return;
              }

              setLocalState(() {
                controller.text = path;
              });
            }

            Future<void> pickDir(
              TextEditingController controller, {
              required String title,
            }) async {
              final path = await FilePicker.platform.getDirectoryPath(
                dialogTitle: title,
              );
              if (path == null || path.isEmpty) {
                return;
              }
              setLocalState(() {
                controller.text = path;
              });
            }

            Widget buildPathField({
              required String label,
              required TextEditingController controller,
              required VoidCallback onPick,
            }) {
              return Row(
                children: [
                  Expanded(
                    child: TextField(
                      controller: controller,
                      decoration: InputDecoration(labelText: label),
                    ),
                  ),
                  const SizedBox(width: 8),
                  OutlinedButton(
                    onPressed: onPick,
                    child: const Text("Browse"),
                  ),
                ],
              );
            }

            return AlertDialog(
              title: const Text("Toolchain Settings"),
              content: SizedBox(
                width: 720,
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    buildPathField(
                      label: "Blender Executable",
                      controller: blenderController,
                      onPick: () => pickFile(
                        blenderController,
                        title: "Select blender executable",
                        allowedExtensions: const ["exe"],
                      ),
                    ),
                    const SizedBox(height: 8),
                    buildPathField(
                      label: "Fast64 Install Path",
                      controller: fast64InstallController,
                      onPick: () => pickDir(
                        fast64InstallController,
                        title: "Select Fast64 install directory",
                      ),
                    ),
                    const SizedBox(height: 8),
                    buildPathField(
                      label: "Fast64 Project Root",
                      controller: projectRootController,
                      onPick: () => pickDir(
                        projectRootController,
                        title: "Select Fast64 project root",
                      ),
                    ),
                    const SizedBox(height: 8),
                    TextField(
                      controller: pinnedTagController,
                      decoration: const InputDecoration(
                        labelText: "Fast64 Pinned Tag",
                      ),
                    ),
                    const SizedBox(height: 8),
                    buildPathField(
                      label: "Python Path (optional)",
                      controller: pythonController,
                      onPick: () => pickFile(
                        pythonController,
                        title: "Select python executable",
                        allowedExtensions: const ["exe"],
                      ),
                    ),
                  ],
                ),
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.of(context).pop(),
                  child: const Text("Cancel"),
                ),
                FilledButton(
                  onPressed: () {
                    Navigator.of(context).pop(
                      ToolchainConfig(
                        blenderExePath: blenderController.text.trim(),
                        fast64InstallPath: fast64InstallController.text.trim(),
                        fast64PinnedTag: pinnedTagController.text.trim(),
                        fast64ProjectRoot: projectRootController.text.trim(),
                        pythonPath: pythonController.text.trim().isEmpty
                            ? null
                            : pythonController.text.trim(),
                      ),
                    );
                  },
                  child: const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );

    blenderController.dispose();
    fast64InstallController.dispose();
    pinnedTagController.dispose();
    projectRootController.dispose();
    pythonController.dispose();

    if (!mounted || configured == null) {
      return;
    }

    await _toolchainConfigService.save(configured);
    final loaded = await _toolchainConfigService.load();

    if (!mounted) {
      return;
    }

    setState(() {
      _toolchainConfig = loaded;
      _status = "Toolchain config saved.";
    });
  }

  Future<void> _validateToolchain() async {
    setState(() {
      _busy = true;
      _status = "Validating Fast64 toolchain...";
    });

    try {
      final result = await _fast64BridgeService.validateToolchain();
      if (!mounted) {
        return;
      }

      setState(() {
        _toolchainValidation = result;
        if (result.success) {
          final blenderVersion = result.details["blenderVersion"];
          _status = blenderVersion == null
              ? "Toolchain validation succeeded."
              : "Toolchain validation succeeded ($blenderVersion).";
        } else {
          _status = "Toolchain validation failed: ${result.errors.join(" | ")}";
        }
      });
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Toolchain validation failed: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
        });
      }
    }
  }

  Future<void> _exportFast64Bundle() async {
    if (_scenes.isEmpty) {
      return;
    }

    setState(() {
      _busy = true;
      _status = "Exporting Fast64 scene bundle...";
    });

    try {
      final sceneId = _scenes[_selectedSceneIndex].sceneId;
      final result = await _fast64BridgeService.exportSceneBundle(sceneId);
      if (!mounted) {
        return;
      }

      setState(() {
        _lastFast64Export = result;
        if (result.success) {
          _status = "Fast64 bundle exported: ${result.bundlePath}";
        } else {
          _status = "Fast64 export failed: ${result.message}";
        }
      });
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Fast64 export failed: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
        });
      }
    }
  }

  Future<void> _copyFast64ArchiveToExample() async {
    final export = _lastFast64Export;
    if (export == null || export.bundlePath == null) {
      setState(() {
        _status = "No Fast64 bundle exported yet.";
      });
      return;
    }

    setState(() {
      _busy = true;
      _status = "Copying parkour_room.otr from Fast64 bundle...";
    });

    try {
      final sceneId = _scenes[_selectedSceneIndex].sceneId;
      final result = await _fast64BridgeService.buildModAssetArchive(
        sceneId,
        bundlePath: export.bundlePath,
      );

      if (!mounted) {
        return;
      }

      setState(() {
        _status = result.success
            ? "Asset archive ready: ${result.outputArchivePath}"
            : "Asset archive build failed: ${result.message}";
      });
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Asset archive build failed: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
        });
      }
    }
  }

  Future<void> _loadRom() async {
    setState(() {
      _busy = true;
      _status = "Loading ROM...";
    });

    try {
      final rom = await _romLoaderService.pickAndLoadRom();
      if (!mounted) {
        return;
      }

      if (rom == null) {
        setState(() {
          _status = "ROM selection canceled.";
        });
        return;
      }

      setState(() {
        _rom = rom;
        _status = rom.looksLikeUsa11
            ? "ROM loaded: looks compatible with OOT USA v1.1."
            : "ROM loaded: header differs from expected OOT USA v1.1.";
      });
    } catch (error) {
      setState(() {
        _status = "Failed to load ROM: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
        });
      }
    }
  }

  Future<void> _exportModZip() async {
    if (_rom == null) {
      setState(() {
        _status = "Load ROM before exporting a mod.";
      });
      return;
    }

    final modName = await _askText(
      title: "Mod Name",
      hint: "kokiri_parkour",
      initialValue: "kokiri_parkour",
    );

    if (!mounted || modName == null || modName.trim().isEmpty) {
      return;
    }

    final exportOptions = await _askExportOptions();
    if (!mounted || exportOptions == null) {
      return;
    }

    setState(() {
      _busy = true;
      _status = "Exporting mod ZIP...";
    });

    try {
      final result = await _modExportService.exportModZip(
        modName: modName.trim(),
        rom: _rom!,
        scenes: _scenes,
        editableFiles: _editableFiles,
        options: exportOptions,
      );

      if (!mounted) {
        return;
      }

      setState(() {
        _status = result.success
            ? "${result.message} Output: ${result.outputPath}"
            : result.message;
      });
    } catch (error) {
      if (!mounted) {
        return;
      }
      setState(() {
        _status = "Export failed: $error";
      });
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
        });
      }
    }
  }

  Future<void> _addCustomScene() async {
    final sceneId = await _askText(
      title: "Custom Scene ID",
      hint: "custom_parkour_room",
    );
    if (sceneId == null || sceneId.trim().isEmpty || !mounted) {
      return;
    }

    final sceneName = await _askText(
      title: "Custom Scene Name",
      hint: "Parkour Room",
    );
    if (sceneName == null || sceneName.trim().isEmpty || !mounted) {
      return;
    }

    final description = await _askText(
      title: "Description",
      hint: "New custom room connected to Kokiri.",
      initialValue: "",
    );
    if (!mounted) {
      return;
    }

    final newScene = ScenePatch(
      sceneId: sceneId.trim(),
      sceneName: sceneName.trim(),
      description: description?.trim() ?? "",
      assets: const [],
      doors: const [],
      collisions: const [],
      hooks: const [],
    );

    setState(() {
      _scenes.add(newScene);
      _selectedSceneIndex = _scenes.length - 1;
      _status = "Custom scene added: ${newScene.sceneName}.";
    });
  }

  Future<ModExportOptions?> _askExportOptions() async {
    var includeUnchangedScenes = false;
    var includeReadme = true;
    var includeValidationReport = true;
    var strictValidation = true;
    var includeLegacyScenePatches = true;
    var forceKokiriTemplate = true;

    final bundlePath = _lastFast64Export?.bundlePath;
    final bundleAssetCandidate = bundlePath == null
        ? null
        : "$bundlePath${Platform.pathSeparator}parkour_room.otr";
    final docsAssetCandidate =
        "docs${Platform.pathSeparator}examples${Platform.pathSeparator}external_mods${Platform.pathSeparator}kokiri_parkour${Platform.pathSeparator}assets${Platform.pathSeparator}parkour_room.otr";

    final assetController = TextEditingController(
      text: _firstExistingPath(<String?>[
        bundleAssetCandidate,
        docsAssetCandidate,
      ]),
    );
    final sceneManifestController = TextEditingController(
      text: _lastFast64Export?.sceneManifestPath ?? "",
    );
    final entryScriptController = TextEditingController(text: "");

    final options = await showDialog<ModExportOptions>(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setLocalState) {
            Future<void> pickFile(
              TextEditingController controller, {
              required String title,
              List<String>? allowedExtensions,
            }) async {
              final result = await FilePicker.platform.pickFiles(
                dialogTitle: title,
                allowMultiple: false,
                type: allowedExtensions == null
                    ? FileType.any
                    : FileType.custom,
                allowedExtensions: allowedExtensions,
                withData: false,
              );

              if (result == null || result.files.isEmpty) {
                return;
              }

              final path = result.files.single.path;
              if (path == null || path.isEmpty) {
                return;
              }

              setLocalState(() {
                controller.text = path;
              });
            }

            Widget pathField({
              required String label,
              required TextEditingController controller,
              required VoidCallback onPick,
            }) {
              return Row(
                children: [
                  Expanded(
                    child: TextField(
                      controller: controller,
                      decoration: InputDecoration(labelText: label),
                    ),
                  ),
                  const SizedBox(width: 8),
                  OutlinedButton(
                    onPressed: onPick,
                    child: const Text("Browse"),
                  ),
                ],
              );
            }

            return AlertDialog(
              title: const Text("Export Options"),
              content: SizedBox(
                width: 700,
                child: SingleChildScrollView(
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      CheckboxListTile(
                        value: includeUnchangedScenes,
                        contentPadding: EdgeInsets.zero,
                        title: const Text("Include unchanged scenes"),
                        subtitle: const Text(
                          "Exports all scene templates, even without changes.",
                        ),
                        onChanged: (value) {
                          setLocalState(() {
                            includeUnchangedScenes = value ?? false;
                          });
                        },
                      ),
                      CheckboxListTile(
                        value: includeReadme,
                        contentPadding: EdgeInsets.zero,
                        title: const Text("Include README_MOD.txt"),
                        subtitle: const Text(
                          "Adds install and package notes to ZIP.",
                        ),
                        onChanged: (value) {
                          setLocalState(() {
                            includeReadme = value ?? true;
                          });
                        },
                      ),
                      CheckboxListTile(
                        value: includeValidationReport,
                        contentPadding: EdgeInsets.zero,
                        title: const Text("Include validation report"),
                        subtitle: const Text(
                          "Writes reports/validation_report.json.",
                        ),
                        onChanged: (value) {
                          setLocalState(() {
                            includeValidationReport = value ?? true;
                          });
                        },
                      ),
                      CheckboxListTile(
                        value: strictValidation,
                        contentPadding: EdgeInsets.zero,
                        title: const Text("Strict validation"),
                        subtitle: const Text(
                          "Requires assets/parkour_room.otr in exported ZIP.",
                        ),
                        onChanged: (value) {
                          setLocalState(() {
                            strictValidation = value ?? true;
                          });
                        },
                      ),
                      CheckboxListTile(
                        value: includeLegacyScenePatches,
                        contentPadding: EdgeInsets.zero,
                        title: const Text("Include legacy scene patch files"),
                        subtitle: const Text(
                          "Keeps scenes/*.json and file_patches/* for fallback/debug.",
                        ),
                        onChanged: (value) {
                          setLocalState(() {
                            includeLegacyScenePatches = value ?? true;
                          });
                        },
                      ),
                      CheckboxListTile(
                        value: forceKokiriTemplate,
                        contentPadding: EdgeInsets.zero,
                        title: const Text(
                          "Use Kokiri Parkour trigger template",
                        ),
                        subtitle: const Text(
                          "Pre-fills 3 trigger flow Kokiri -> Parkour -> Kokiri/Kakariko.",
                        ),
                        onChanged: (value) {
                          setLocalState(() {
                            forceKokiriTemplate = value ?? true;
                          });
                        },
                      ),
                      const SizedBox(height: 8),
                      pathField(
                        label: "parkour_room.otr path",
                        controller: assetController,
                        onPick: () => pickFile(
                          assetController,
                          title: "Select parkour_room.otr",
                          allowedExtensions: const ["otr", "o2r"],
                        ),
                      ),
                      const SizedBox(height: 8),
                      pathField(
                        label: "scene_manifest.json path (optional)",
                        controller: sceneManifestController,
                        onPick: () => pickFile(
                          sceneManifestController,
                          title: "Select scene_manifest.json",
                          allowedExtensions: const ["json"],
                        ),
                      ),
                      const SizedBox(height: 8),
                      pathField(
                        label: "entry script override (optional)",
                        controller: entryScriptController,
                        onPick: () => pickFile(
                          entryScriptController,
                          title: "Select init script JSON",
                          allowedExtensions: const ["json"],
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.of(context).pop(),
                  child: const Text("Cancel"),
                ),
                FilledButton(
                  onPressed: () => Navigator.of(context).pop(
                    ModExportOptions(
                      includeUnchangedScenes: includeUnchangedScenes,
                      includeReadme: includeReadme,
                      includeValidationReport: includeValidationReport,
                      strictValidation: strictValidation,
                      includeLegacyScenePatches: includeLegacyScenePatches,
                      forceKokiriTemplate: forceKokiriTemplate,
                      parkourAssetArchivePath:
                          assetController.text.trim().isEmpty
                          ? null
                          : assetController.text.trim(),
                      sceneManifestPath:
                          sceneManifestController.text.trim().isEmpty
                          ? null
                          : sceneManifestController.text.trim(),
                      entryScriptPath: entryScriptController.text.trim().isEmpty
                          ? null
                          : entryScriptController.text.trim(),
                    ),
                  ),
                  child: const Text("Export"),
                ),
              ],
            );
          },
        );
      },
    );

    assetController.dispose();
    sceneManifestController.dispose();
    entryScriptController.dispose();

    return options;
  }

  String _firstExistingPath(List<String?> candidates) {
    for (final candidate in candidates) {
      if (candidate == null || candidate.trim().isEmpty) {
        continue;
      }

      final file = File(candidate);
      if (file.existsSync()) {
        return file.absolute.path;
      }
    }

    return "";
  }

  Future<String?> _askText({
    required String title,
    required String hint,
    String initialValue = "",
  }) async {
    final controller = TextEditingController(text: initialValue);

    final value = await showDialog<String>(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text(title),
          content: TextField(
            controller: controller,
            autofocus: true,
            decoration: InputDecoration(hintText: hint),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: const Text("Cancel"),
            ),
            FilledButton(
              onPressed: () => Navigator.of(context).pop(controller.text),
              child: const Text("OK"),
            ),
          ],
        );
      },
    );

    controller.dispose();
    return value;
  }

  void _onSceneChanged(ScenePatch updatedScene) {
    setState(() {
      _scenes[_selectedSceneIndex] = updatedScene;
      _status = "Scene updated: ${updatedScene.sceneName}.";
    });
  }

  Future<void> _open3DStudio() async {
    if (_scenes.isEmpty) {
      return;
    }

    if (_rom == null) {
      setState(() {
        _status = "Load ROM before opening 3D Studio.";
      });
      return;
    }

    final selected = _scenes[_selectedSceneIndex];
    final updated = await Navigator.of(context).push<ScenePatch>(
      MaterialPageRoute(
        builder: (context) => SceneStudio3DScreen(
          scene: selected,
          allSceneIds: _scenes
              .map((scene) => scene.sceneId)
              .toList(growable: false),
          romPath: _rom!.path,
        ),
      ),
    );

    if (!mounted || updated == null) {
      return;
    }

    _onSceneChanged(updated);
  }

  Future<void> _openFileEditor() async {
    final updatedFiles = await Navigator.of(context)
        .push<List<EditableModFile>>(
          MaterialPageRoute(
            builder: (context) => ModFileEditorScreen(
              initialFiles: _editableFiles,
              romPath: _rom?.path,
            ),
          ),
        );

    if (!mounted || updatedFiles == null) {
      return;
    }

    setState(() {
      _editableFiles
        ..clear()
        ..addAll(updatedFiles);
      _status =
          "File workspace updated: ${_editableFiles.length} editable files.";
    });
  }

  @override
  Widget build(BuildContext context) {
    final selectedScene = _scenes.isEmpty ? null : _scenes[_selectedSceneIndex];

    return Scaffold(
      appBar: AppBar(
        title: const Text("Scene Editor MVP"),
        bottom: PreferredSize(
          preferredSize: const Size.fromHeight(28),
          child: Align(
            alignment: Alignment.centerLeft,
            child: Padding(
              padding: const EdgeInsets.only(left: 16, bottom: 8),
              child: Text(
                "OOT External Mods Workspace",
                style: Theme.of(context).textTheme.labelLarge?.copyWith(
                  color: const Color(0xFFDDEEEF),
                ),
              ),
            ),
          ),
        ),
      ),
      body: DecoratedBox(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topCenter,
            end: Alignment.bottomCenter,
            colors: [Color(0xFFF6F1E7), Color(0xFFECE1CF)],
          ),
        ),
        child: Padding(
          padding: const EdgeInsets.all(12),
          child: LayoutBuilder(
            builder: (context, constraints) {
              if (constraints.maxWidth < 980) {
                return Column(
                  children: [
                    Expanded(flex: 5, child: _buildSidebar()),
                    const SizedBox(height: 12),
                    Expanded(flex: 6, child: _buildEditorPanel(selectedScene)),
                  ],
                );
              }

              return Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  SizedBox(width: 380, child: _buildSidebar()),
                  const SizedBox(width: 12),
                  Expanded(child: _buildEditorPanel(selectedScene)),
                ],
              );
            },
          ),
        ),
      ),
    );
  }

  Widget _buildSidebar() {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                Expanded(
                  child: FilledButton.icon(
                    onPressed: _busy ? null : _loadRom,
                    icon: const Icon(Icons.upload_file),
                    label: const Text("Load ROM"),
                  ),
                ),
                const SizedBox(width: 8),
                IconButton(
                  tooltip: "Add Custom Scene",
                  onPressed: _busy ? null : _addCustomScene,
                  icon: const Icon(Icons.add_box_outlined),
                ),
                IconButton(
                  tooltip: "Export Mod ZIP",
                  onPressed: _busy ? null : _exportModZip,
                  icon: const Icon(Icons.archive_outlined),
                ),
                IconButton(
                  tooltip: "Open 3D Studio",
                  onPressed: _busy ? null : _open3DStudio,
                  icon: const Icon(Icons.threed_rotation),
                ),
                IconButton(
                  tooltip: "Open File Editor",
                  onPressed: _busy ? null : _openFileEditor,
                  icon: const Icon(Icons.edit_document),
                ),
              ],
            ),
            const SizedBox(height: 8),
            Wrap(
              spacing: 8,
              runSpacing: 8,
              children: [
                OutlinedButton.icon(
                  onPressed: _busy ? null : _configureToolchain,
                  icon: const Icon(Icons.settings),
                  label: const Text("Toolchain"),
                ),
                OutlinedButton.icon(
                  onPressed: _busy ? null : _validateToolchain,
                  icon: const Icon(Icons.verified_outlined),
                  label: const Text("Validate Toolchain"),
                ),
                OutlinedButton.icon(
                  onPressed: _busy ? null : _exportFast64Bundle,
                  icon: const Icon(Icons.cloud_upload_outlined),
                  label: const Text("Export Fast64 Bundle"),
                ),
                OutlinedButton.icon(
                  onPressed: _busy ? null : _copyFast64ArchiveToExample,
                  icon: const Icon(Icons.copy_all_outlined),
                  label: const Text("Copy parkour_room.otr"),
                ),
              ],
            ),
            const SizedBox(height: 8),
            Container(
              width: double.infinity,
              padding: const EdgeInsets.all(10),
              decoration: BoxDecoration(
                color: Theme.of(context).colorScheme.surfaceContainerHighest,
                borderRadius: BorderRadius.circular(8),
              ),
              child: Text(_status),
            ),
            const SizedBox(height: 10),
            _buildRomCard(),
            const SizedBox(height: 10),
            _buildToolchainCard(),
            const SizedBox(height: 10),
            _buildFileWorkspaceCard(),
            const SizedBox(height: 10),
            Text("Scenes", style: Theme.of(context).textTheme.titleMedium),
            const SizedBox(height: 6),
            Expanded(
              child: ListView.builder(
                itemCount: _scenes.length,
                itemBuilder: (context, index) {
                  final scene = _scenes[index];
                  final selected = index == _selectedSceneIndex;
                  return Card(
                    elevation: selected ? 0 : null,
                    color: selected
                        ? Theme.of(context).colorScheme.primaryContainer
                        : null,
                    child: ListTile(
                      title: Text(scene.sceneName),
                      subtitle: Text(scene.sceneId),
                      trailing: scene.hasChanges
                          ? const Icon(Icons.edit_note, size: 18)
                          : const Icon(Icons.circle_outlined, size: 14),
                      selected: selected,
                      onTap: () {
                        setState(() {
                          _selectedSceneIndex = index;
                        });
                      },
                    ),
                  );
                },
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildToolchainCard() {
    final config = _toolchainConfig;
    final validation = _toolchainValidation;
    final export = _lastFast64Export;

    final configured = config != null && config.isConfigured;
    final validated = validation?.success == true;
    final configuredTag = config?.fast64PinnedTag ?? "unknown";

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                Icon(
                  configured ? Icons.build_circle : Icons.build_circle_outlined,
                  color: configured ? Colors.green : Colors.orange,
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: Text(
                    configured
                        ? "Toolchain configured ($configuredTag)"
                        : "Toolchain not configured",
                  ),
                ),
                if (validated)
                  const Icon(Icons.verified, color: Colors.green)
                else
                  const Icon(Icons.rule_folder_outlined),
              ],
            ),
            if (validation != null) ...[
              const SizedBox(height: 6),
              Text(
                validation.success
                    ? "Validation OK"
                    : "Validation errors: ${validation.errors.join(" | ")}",
                style: TextStyle(
                  color: validation.success ? Colors.green : Colors.red,
                ),
              ),
            ],
            if (export != null) ...[
              const SizedBox(height: 6),
              Text(
                export.success
                    ? "Last Fast64 bundle: ${export.bundlePath}"
                    : "Last Fast64 export failed: ${export.message}",
                maxLines: 2,
                overflow: TextOverflow.ellipsis,
              ),
            ],
          ],
        ),
      ),
    );
  }

  Widget _buildFileWorkspaceCard() {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Row(
          children: [
            const Icon(Icons.description_outlined),
            const SizedBox(width: 8),
            Expanded(
              child: Text(
                _editableFiles.isEmpty
                    ? "No editable files selected for mod export."
                    : "${_editableFiles.length} editable files will be included in ZIP.",
              ),
            ),
            TextButton(
              onPressed: _busy ? null : _openFileEditor,
              child: const Text("Manage"),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildRomCard() {
    if (_rom == null) {
      return const Card(
        child: Padding(
          padding: EdgeInsets.all(12),
          child: Text("No ROM loaded."),
        ),
      );
    }

    final rom = _rom!;
    final compatible = rom.looksLikeUsa11;
    final color = compatible ? Colors.green : Colors.orange;

    return Card(
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(rom.fileName, style: Theme.of(context).textTheme.titleSmall),
            const SizedBox(height: 6),
            Text("Title: ${rom.title}"),
            Text("Game code: ${rom.gameCode}"),
            Text("Version byte: ${rom.version}"),
            Text("Byte order: ${rom.byteOrderLabel}"),
            Text("Size: ${rom.sizeBytes} bytes"),
            Text("SHA1: ${rom.sha1}"),
            const SizedBox(height: 8),
            Row(
              children: [
                Icon(Icons.shield, color: color),
                const SizedBox(width: 6),
                Expanded(
                  child: Text(
                    compatible
                        ? "Header matches expected OOT USA 1.1 profile."
                        : "Header differs from expected OOT USA 1.1 profile.",
                    style: TextStyle(color: color),
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildEditorPanel(ScenePatch? scene) {
    if (scene == null) {
      return const Card(child: Center(child: Text("No scene selected.")));
    }

    return Card(
      child: SceneEditorPanel(
        key: ValueKey(scene.sceneId),
        scene: scene,
        allSceneIds: _scenes
            .map((item) => item.sceneId)
            .toList(growable: false),
        onChanged: _onSceneChanged,
      ),
    );
  }
}
