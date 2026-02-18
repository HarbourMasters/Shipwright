import "../data/oot_scene_catalog.dart";

String generateEntityId(String prefix) {
  return "${prefix}_${DateTime.now().microsecondsSinceEpoch}";
}

class Vec3 {
  const Vec3({required this.x, required this.y, required this.z});

  final double x;
  final double y;
  final double z;

  Map<String, dynamic> toJson() => {"x": x, "y": y, "z": z};
}

class SceneAssetRef {
  const SceneAssetRef({
    required this.id,
    required this.path,
    required this.type,
  });

  final String id;
  final String path;
  final String type;

  SceneAssetRef copyWith({String? id, String? path, String? type}) {
    return SceneAssetRef(
      id: id ?? this.id,
      path: path ?? this.path,
      type: type ?? this.type,
    );
  }

  Map<String, dynamic> toJson() => {"id": id, "path": path, "type": type};
}

class SceneDoorLink {
  const SceneDoorLink({
    required this.id,
    required this.name,
    required this.position,
    required this.targetSceneId,
    required this.targetSpawn,
    required this.oneWay,
  });

  final String id;
  final String name;
  final Vec3 position;
  final String targetSceneId;
  final int targetSpawn;
  final bool oneWay;

  SceneDoorLink copyWith({
    String? id,
    String? name,
    Vec3? position,
    String? targetSceneId,
    int? targetSpawn,
    bool? oneWay,
  }) {
    return SceneDoorLink(
      id: id ?? this.id,
      name: name ?? this.name,
      position: position ?? this.position,
      targetSceneId: targetSceneId ?? this.targetSceneId,
      targetSpawn: targetSpawn ?? this.targetSpawn,
      oneWay: oneWay ?? this.oneWay,
    );
  }

  Map<String, dynamic> toJson() => {
    "id": id,
    "name": name,
    "position": position.toJson(),
    "targetSceneId": targetSceneId,
    "targetSpawn": targetSpawn,
    "oneWay": oneWay,
  };
}

class CollisionBox {
  const CollisionBox({
    required this.id,
    required this.name,
    required this.min,
    required this.max,
    required this.climbable,
    required this.lethal,
  });

  final String id;
  final String name;
  final Vec3 min;
  final Vec3 max;
  final bool climbable;
  final bool lethal;

  CollisionBox copyWith({
    String? id,
    String? name,
    Vec3? min,
    Vec3? max,
    bool? climbable,
    bool? lethal,
  }) {
    return CollisionBox(
      id: id ?? this.id,
      name: name ?? this.name,
      min: min ?? this.min,
      max: max ?? this.max,
      climbable: climbable ?? this.climbable,
      lethal: lethal ?? this.lethal,
    );
  }

  Map<String, dynamic> toJson() => {
    "id": id,
    "name": name,
    "min": min.toJson(),
    "max": max.toJson(),
    "climbable": climbable,
    "lethal": lethal,
  };
}

class SceneHook {
  const SceneHook({
    required this.id,
    required this.name,
    required this.event,
    required this.action,
    required this.enabled,
  });

  final String id;
  final String name;
  final String event;
  final String action;
  final bool enabled;

  SceneHook copyWith({
    String? id,
    String? name,
    String? event,
    String? action,
    bool? enabled,
  }) {
    return SceneHook(
      id: id ?? this.id,
      name: name ?? this.name,
      event: event ?? this.event,
      action: action ?? this.action,
      enabled: enabled ?? this.enabled,
    );
  }

  Map<String, dynamic> toJson() => {
    "id": id,
    "name": name,
    "event": event,
    "action": action,
    "enabled": enabled,
  };
}

class ScenePatch {
  const ScenePatch({
    required this.sceneId,
    required this.sceneName,
    required this.description,
    required this.assets,
    required this.doors,
    required this.collisions,
    required this.hooks,
  });

  final String sceneId;
  final String sceneName;
  final String description;
  final List<SceneAssetRef> assets;
  final List<SceneDoorLink> doors;
  final List<CollisionBox> collisions;
  final List<SceneHook> hooks;

  bool get hasChanges =>
      doors.isNotEmpty || collisions.isNotEmpty || hooks.isNotEmpty;

  ScenePatch copyWith({
    String? sceneId,
    String? sceneName,
    String? description,
    List<SceneAssetRef>? assets,
    List<SceneDoorLink>? doors,
    List<CollisionBox>? collisions,
    List<SceneHook>? hooks,
  }) {
    return ScenePatch(
      sceneId: sceneId ?? this.sceneId,
      sceneName: sceneName ?? this.sceneName,
      description: description ?? this.description,
      assets: assets ?? this.assets,
      doors: doors ?? this.doors,
      collisions: collisions ?? this.collisions,
      hooks: hooks ?? this.hooks,
    );
  }

  Map<String, dynamic> toJson() => {
    "sceneId": sceneId,
    "sceneName": sceneName,
    "description": description,
    "assets": assets.map((asset) => asset.toJson()).toList(growable: false),
    "doors": doors.map((door) => door.toJson()).toList(growable: false),
    "collisions": collisions
        .map((collision) => collision.toJson())
        .toList(growable: false),
    "hooks": hooks.map((hook) => hook.toJson()).toList(growable: false),
  };

  factory ScenePatch.fromTemplate(SceneTemplate template) {
    final assets = <SceneAssetRef>[];
    for (var i = 0; i < template.defaultAssets.length; i++) {
      assets.add(
        SceneAssetRef(
          id: "asset_${template.id}_$i",
          path: template.defaultAssets[i],
          type: _inferAssetType(template.defaultAssets[i]),
        ),
      );
    }

    return ScenePatch(
      sceneId: template.id,
      sceneName: template.name,
      description: template.description,
      assets: assets,
      doors: const [],
      collisions: const [],
      hooks: const [],
    );
  }

  static String _inferAssetType(String path) {
    final lower = path.toLowerCase();
    if (lower.endsWith(".png") ||
        lower.endsWith(".jpg") ||
        lower.endsWith(".jpeg")) {
      return "texture";
    }
    if (lower.endsWith(".zobj") || lower.endsWith(".obj")) {
      return "object";
    }
    if (lower.endsWith(".zmap") || lower.endsWith(".scene")) {
      return "scene";
    }
    return "generic";
  }
}
