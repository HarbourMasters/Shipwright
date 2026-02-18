enum EditorEntityType { staticObject, portal, collision, npc }

class EditorEntity {
  const EditorEntity({
    required this.id,
    required this.name,
    required this.type,
    required this.positionX,
    required this.positionY,
    required this.positionZ,
    required this.rotationX,
    required this.rotationY,
    required this.rotationZ,
    required this.scaleX,
    required this.scaleY,
    required this.scaleZ,
    this.modelPath,
    this.texturePath,
    this.targetSceneId,
    this.targetSpawn,
    this.npcId,
  });

  final String id;
  final String name;
  final EditorEntityType type;

  final double positionX;
  final double positionY;
  final double positionZ;

  final double rotationX;
  final double rotationY;
  final double rotationZ;

  final double scaleX;
  final double scaleY;
  final double scaleZ;

  final String? modelPath;
  final String? texturePath;
  final String? targetSceneId;
  final int? targetSpawn;
  final String? npcId;

  EditorEntity copyWith({
    String? id,
    String? name,
    EditorEntityType? type,
    double? positionX,
    double? positionY,
    double? positionZ,
    double? rotationX,
    double? rotationY,
    double? rotationZ,
    double? scaleX,
    double? scaleY,
    double? scaleZ,
    String? modelPath,
    String? texturePath,
    String? targetSceneId,
    int? targetSpawn,
    String? npcId,
  }) {
    return EditorEntity(
      id: id ?? this.id,
      name: name ?? this.name,
      type: type ?? this.type,
      positionX: positionX ?? this.positionX,
      positionY: positionY ?? this.positionY,
      positionZ: positionZ ?? this.positionZ,
      rotationX: rotationX ?? this.rotationX,
      rotationY: rotationY ?? this.rotationY,
      rotationZ: rotationZ ?? this.rotationZ,
      scaleX: scaleX ?? this.scaleX,
      scaleY: scaleY ?? this.scaleY,
      scaleZ: scaleZ ?? this.scaleZ,
      modelPath: modelPath ?? this.modelPath,
      texturePath: texturePath ?? this.texturePath,
      targetSceneId: targetSceneId ?? this.targetSceneId,
      targetSpawn: targetSpawn ?? this.targetSpawn,
      npcId: npcId ?? this.npcId,
    );
  }
}
