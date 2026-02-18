import "package:flutter/material.dart";

import "../models/scene_patch.dart";

class SceneEditorPanel extends StatefulWidget {
  const SceneEditorPanel({
    super.key,
    required this.scene,
    required this.allSceneIds,
    required this.onChanged,
  });

  final ScenePatch scene;
  final List<String> allSceneIds;
  final ValueChanged<ScenePatch> onChanged;

  @override
  State<SceneEditorPanel> createState() => _SceneEditorPanelState();
}

class _SceneEditorPanelState extends State<SceneEditorPanel> {
  @override
  Widget build(BuildContext context) {
    final scene = widget.scene;
    return Padding(
      padding: const EdgeInsets.all(12),
      child: SingleChildScrollView(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              scene.sceneName,
              style: Theme.of(context).textTheme.headlineSmall,
            ),
            const SizedBox(height: 6),
            Text(scene.sceneId, style: Theme.of(context).textTheme.bodySmall),
            const SizedBox(height: 6),
            Text(scene.description),
            const SizedBox(height: 12),
            _buildDoorSection(scene),
            const SizedBox(height: 10),
            _buildCollisionSection(scene),
            const SizedBox(height: 10),
            _buildHookSection(scene),
            const SizedBox(height: 10),
            _buildAssetSection(scene),
          ],
        ),
      ),
    );
  }

  Widget _buildDoorSection(ScenePatch scene) {
    return _SectionCard(
      title: "Doors / Scene Links",
      subtitle: "Add entrances between scenes.",
      onAdd: () async {
        final created = await _showDoorDialog();
        if (created == null) {
          return;
        }
        widget.onChanged(scene.copyWith(doors: [...scene.doors, created]));
      },
      child: _EntityList(
        items: scene.doors
            .asMap()
            .entries
            .map(
              (entry) => _EntityRowData(
                title: "${entry.value.name} -> ${entry.value.targetSceneId}",
                subtitle:
                    "pos(${entry.value.position.x}, ${entry.value.position.y}, ${entry.value.position.z}), spawn ${entry.value.targetSpawn}, oneWay=${entry.value.oneWay}",
                onEdit: () async {
                  final updated = await _showDoorDialog(initial: entry.value);
                  if (updated == null) {
                    return;
                  }
                  final next = [...scene.doors];
                  next[entry.key] = updated;
                  widget.onChanged(scene.copyWith(doors: next));
                },
                onDelete: () {
                  final next = [...scene.doors]..removeAt(entry.key);
                  widget.onChanged(scene.copyWith(doors: next));
                },
              ),
            )
            .toList(growable: false),
      ),
    );
  }

  Widget _buildCollisionSection(ScenePatch scene) {
    return _SectionCard(
      title: "Collision",
      subtitle: "Define additional collision volumes.",
      onAdd: () async {
        final created = await _showCollisionDialog();
        if (created == null) {
          return;
        }
        widget.onChanged(
          scene.copyWith(collisions: [...scene.collisions, created]),
        );
      },
      child: _EntityList(
        items: scene.collisions
            .asMap()
            .entries
            .map(
              (entry) => _EntityRowData(
                title: entry.value.name,
                subtitle:
                    "min(${entry.value.min.x}, ${entry.value.min.y}, ${entry.value.min.z}) max(${entry.value.max.x}, ${entry.value.max.y}, ${entry.value.max.z}) climbable=${entry.value.climbable} lethal=${entry.value.lethal}",
                onEdit: () async {
                  final updated = await _showCollisionDialog(
                    initial: entry.value,
                  );
                  if (updated == null) {
                    return;
                  }
                  final next = [...scene.collisions];
                  next[entry.key] = updated;
                  widget.onChanged(scene.copyWith(collisions: next));
                },
                onDelete: () {
                  final next = [...scene.collisions]..removeAt(entry.key);
                  widget.onChanged(scene.copyWith(collisions: next));
                },
              ),
            )
            .toList(growable: false),
      ),
    );
  }

  Widget _buildHookSection(ScenePatch scene) {
    return _SectionCard(
      title: "Hooks",
      subtitle: "Bind scene events to actions.",
      onAdd: () async {
        final created = await _showHookDialog();
        if (created == null) {
          return;
        }
        widget.onChanged(scene.copyWith(hooks: [...scene.hooks, created]));
      },
      child: _EntityList(
        items: scene.hooks
            .asMap()
            .entries
            .map(
              (entry) => _EntityRowData(
                title: entry.value.name,
                subtitle:
                    "${entry.value.event} -> ${entry.value.action} (enabled=${entry.value.enabled})",
                onEdit: () async {
                  final updated = await _showHookDialog(initial: entry.value);
                  if (updated == null) {
                    return;
                  }
                  final next = [...scene.hooks];
                  next[entry.key] = updated;
                  widget.onChanged(scene.copyWith(hooks: next));
                },
                onDelete: () {
                  final next = [...scene.hooks]..removeAt(entry.key);
                  widget.onChanged(scene.copyWith(hooks: next));
                },
              ),
            )
            .toList(growable: false),
      ),
    );
  }

  Widget _buildAssetSection(ScenePatch scene) {
    return _SectionCard(
      title: "Assets",
      subtitle: "Map texture/object/scene assets used by this scene.",
      onAdd: () async {
        final created = await _showAssetDialog();
        if (created == null) {
          return;
        }
        widget.onChanged(scene.copyWith(assets: [...scene.assets, created]));
      },
      child: _EntityList(
        items: scene.assets
            .asMap()
            .entries
            .map(
              (entry) => _EntityRowData(
                title: entry.value.path,
                subtitle: "type=${entry.value.type}",
                onEdit: () async {
                  final updated = await _showAssetDialog(initial: entry.value);
                  if (updated == null) {
                    return;
                  }
                  final next = [...scene.assets];
                  next[entry.key] = updated;
                  widget.onChanged(scene.copyWith(assets: next));
                },
                onDelete: () {
                  final next = [...scene.assets]..removeAt(entry.key);
                  widget.onChanged(scene.copyWith(assets: next));
                },
              ),
            )
            .toList(growable: false),
      ),
    );
  }

  Future<SceneDoorLink?> _showDoorDialog({SceneDoorLink? initial}) async {
    final nameController = TextEditingController(
      text: initial?.name ?? "New Door",
    );
    final xController = TextEditingController(
      text: initial?.position.x.toString() ?? "0",
    );
    final yController = TextEditingController(
      text: initial?.position.y.toString() ?? "0",
    );
    final zController = TextEditingController(
      text: initial?.position.z.toString() ?? "0",
    );
    final targetController = TextEditingController(
      text:
          initial?.targetSceneId ??
          (widget.allSceneIds.isEmpty ? "" : widget.allSceneIds.first),
    );
    final spawnController = TextEditingController(
      text: initial?.targetSpawn.toString() ?? "0",
    );
    var oneWay = initial?.oneWay ?? false;

    final result = await showDialog<SceneDoorLink>(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setLocalState) {
            return AlertDialog(
              title: Text(initial == null ? "Add Door Link" : "Edit Door Link"),
              content: SingleChildScrollView(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    _dialogField(nameController, "Name"),
                    _dialogField(xController, "X"),
                    _dialogField(yController, "Y"),
                    _dialogField(zController, "Z"),
                    _dialogField(targetController, "Target Scene ID"),
                    _dialogField(spawnController, "Target Spawn"),
                    CheckboxListTile(
                      contentPadding: EdgeInsets.zero,
                      value: oneWay,
                      title: const Text("One way"),
                      onChanged: (value) {
                        setLocalState(() {
                          oneWay = value ?? false;
                        });
                      },
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
                    final next = SceneDoorLink(
                      id: initial?.id ?? generateEntityId("door"),
                      name: nameController.text.trim().isEmpty
                          ? "Door"
                          : nameController.text.trim(),
                      position: Vec3(
                        x: _parseDouble(xController.text),
                        y: _parseDouble(yController.text),
                        z: _parseDouble(zController.text),
                      ),
                      targetSceneId: targetController.text.trim(),
                      targetSpawn: _parseInt(spawnController.text),
                      oneWay: oneWay,
                    );
                    Navigator.of(context).pop(next);
                  },
                  child: const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );

    nameController.dispose();
    xController.dispose();
    yController.dispose();
    zController.dispose();
    targetController.dispose();
    spawnController.dispose();
    return result;
  }

  Future<CollisionBox?> _showCollisionDialog({CollisionBox? initial}) async {
    final nameController = TextEditingController(
      text: initial?.name ?? "New Collision",
    );
    final minXController = TextEditingController(
      text: initial?.min.x.toString() ?? "-100",
    );
    final minYController = TextEditingController(
      text: initial?.min.y.toString() ?? "0",
    );
    final minZController = TextEditingController(
      text: initial?.min.z.toString() ?? "-100",
    );
    final maxXController = TextEditingController(
      text: initial?.max.x.toString() ?? "100",
    );
    final maxYController = TextEditingController(
      text: initial?.max.y.toString() ?? "200",
    );
    final maxZController = TextEditingController(
      text: initial?.max.z.toString() ?? "100",
    );
    var climbable = initial?.climbable ?? false;
    var lethal = initial?.lethal ?? false;

    final result = await showDialog<CollisionBox>(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setLocalState) {
            return AlertDialog(
              title: Text(
                initial == null ? "Add Collision Box" : "Edit Collision Box",
              ),
              content: SingleChildScrollView(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    _dialogField(nameController, "Name"),
                    _dialogField(minXController, "Min X"),
                    _dialogField(minYController, "Min Y"),
                    _dialogField(minZController, "Min Z"),
                    _dialogField(maxXController, "Max X"),
                    _dialogField(maxYController, "Max Y"),
                    _dialogField(maxZController, "Max Z"),
                    CheckboxListTile(
                      contentPadding: EdgeInsets.zero,
                      value: climbable,
                      title: const Text("Climbable"),
                      onChanged: (value) {
                        setLocalState(() {
                          climbable = value ?? false;
                        });
                      },
                    ),
                    CheckboxListTile(
                      contentPadding: EdgeInsets.zero,
                      value: lethal,
                      title: const Text("Lethal"),
                      onChanged: (value) {
                        setLocalState(() {
                          lethal = value ?? false;
                        });
                      },
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
                    final next = CollisionBox(
                      id: initial?.id ?? generateEntityId("collision"),
                      name: nameController.text.trim().isEmpty
                          ? "Collision"
                          : nameController.text.trim(),
                      min: Vec3(
                        x: _parseDouble(minXController.text),
                        y: _parseDouble(minYController.text),
                        z: _parseDouble(minZController.text),
                      ),
                      max: Vec3(
                        x: _parseDouble(maxXController.text),
                        y: _parseDouble(maxYController.text),
                        z: _parseDouble(maxZController.text),
                      ),
                      climbable: climbable,
                      lethal: lethal,
                    );
                    Navigator.of(context).pop(next);
                  },
                  child: const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );

    nameController.dispose();
    minXController.dispose();
    minYController.dispose();
    minZController.dispose();
    maxXController.dispose();
    maxYController.dispose();
    maxZController.dispose();
    return result;
  }

  Future<SceneHook?> _showHookDialog({SceneHook? initial}) async {
    final nameController = TextEditingController(
      text: initial?.name ?? "On Enter Trigger",
    );
    final eventController = TextEditingController(
      text: initial?.event ?? "on_player_enter_area",
    );
    final actionController = TextEditingController(
      text: initial?.action ?? "warp:kakariko_village:0",
    );
    var enabled = initial?.enabled ?? true;

    final result = await showDialog<SceneHook>(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setLocalState) {
            return AlertDialog(
              title: Text(initial == null ? "Add Hook" : "Edit Hook"),
              content: SingleChildScrollView(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    _dialogField(nameController, "Name"),
                    _dialogField(eventController, "Event"),
                    _dialogField(actionController, "Action"),
                    CheckboxListTile(
                      contentPadding: EdgeInsets.zero,
                      value: enabled,
                      title: const Text("Enabled"),
                      onChanged: (value) {
                        setLocalState(() {
                          enabled = value ?? true;
                        });
                      },
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
                    final next = SceneHook(
                      id: initial?.id ?? generateEntityId("hook"),
                      name: nameController.text.trim().isEmpty
                          ? "Hook"
                          : nameController.text.trim(),
                      event: eventController.text.trim(),
                      action: actionController.text.trim(),
                      enabled: enabled,
                    );
                    Navigator.of(context).pop(next);
                  },
                  child: const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );

    nameController.dispose();
    eventController.dispose();
    actionController.dispose();
    return result;
  }

  Future<SceneAssetRef?> _showAssetDialog({SceneAssetRef? initial}) async {
    final pathController = TextEditingController(
      text: initial?.path ?? "custom/assets/path",
    );
    final typeController = TextEditingController(
      text: initial?.type ?? "generic",
    );

    final result = await showDialog<SceneAssetRef>(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text(initial == null ? "Add Asset" : "Edit Asset"),
          content: SingleChildScrollView(
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                _dialogField(pathController, "Asset Path"),
                _dialogField(
                  typeController,
                  "Asset Type (texture/object/scene/generic)",
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
                final next = SceneAssetRef(
                  id: initial?.id ?? generateEntityId("asset"),
                  path: pathController.text.trim(),
                  type: typeController.text.trim().isEmpty
                      ? "generic"
                      : typeController.text.trim(),
                );
                Navigator.of(context).pop(next);
              },
              child: const Text("Save"),
            ),
          ],
        );
      },
    );

    pathController.dispose();
    typeController.dispose();
    return result;
  }

  Widget _dialogField(TextEditingController controller, String label) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 8),
      child: TextField(
        controller: controller,
        decoration: InputDecoration(
          border: const OutlineInputBorder(),
          labelText: label,
          isDense: true,
        ),
      ),
    );
  }

  double _parseDouble(String input) {
    return double.tryParse(input.trim()) ?? 0;
  }

  int _parseInt(String input) {
    return int.tryParse(input.trim()) ?? 0;
  }
}

class _SectionCard extends StatelessWidget {
  const _SectionCard({
    required this.title,
    required this.subtitle,
    required this.onAdd,
    required this.child,
  });

  final String title;
  final String subtitle;
  final VoidCallback onAdd;
  final Widget child;

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(10),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Row(
              children: [
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        title,
                        style: Theme.of(context).textTheme.titleMedium,
                      ),
                      Text(
                        subtitle,
                        style: Theme.of(context).textTheme.bodySmall,
                      ),
                    ],
                  ),
                ),
                IconButton(
                  tooltip: "Add",
                  onPressed: onAdd,
                  icon: const Icon(Icons.add_circle_outline),
                ),
              ],
            ),
            const SizedBox(height: 6),
            child,
          ],
        ),
      ),
    );
  }
}

class _EntityRowData {
  const _EntityRowData({
    required this.title,
    required this.subtitle,
    required this.onEdit,
    required this.onDelete,
  });

  final String title;
  final String subtitle;
  final VoidCallback onEdit;
  final VoidCallback onDelete;
}

class _EntityList extends StatelessWidget {
  const _EntityList({required this.items});

  final List<_EntityRowData> items;

  @override
  Widget build(BuildContext context) {
    if (items.isEmpty) {
      return const Text("No entries yet.");
    }

    return Column(
      children: items
          .map(
            (item) => Card(
              child: ListTile(
                title: Text(item.title),
                subtitle: Text(item.subtitle),
                trailing: Row(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    IconButton(
                      tooltip: "Edit",
                      onPressed: item.onEdit,
                      icon: const Icon(Icons.edit_outlined),
                    ),
                    IconButton(
                      tooltip: "Delete",
                      onPressed: item.onDelete,
                      icon: const Icon(Icons.delete_outline),
                    ),
                  ],
                ),
              ),
            ),
          )
          .toList(growable: false),
    );
  }
}
