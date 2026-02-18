import "package:flutter_test/flutter_test.dart";
import "package:scene_editor/models/scene_patch.dart";

void main() {
  test("scene patch reports changes and serializes fields", () {
    final patch = ScenePatch(
      sceneId: "kokiri_forest",
      sceneName: "Kokiri Forest",
      description: "Test patch",
      assets: const [
        SceneAssetRef(
          id: "a1",
          path: "textures/kokiri/grass_01.png",
          type: "texture",
        ),
      ],
      doors: const [
        SceneDoorLink(
          id: "d1",
          name: "Behind House Door",
          position: Vec3(x: 10, y: 0, z: -12),
          targetSceneId: "kokiri_parkour_room",
          targetSpawn: 0,
          oneWay: false,
        ),
      ],
      collisions: const [],
      hooks: const [],
    );

    expect(patch.hasChanges, isTrue);

    final json = patch.toJson();
    expect(json["sceneId"], "kokiri_forest");
    expect((json["assets"] as List).length, 1);
    expect((json["doors"] as List).length, 1);
  });
}
