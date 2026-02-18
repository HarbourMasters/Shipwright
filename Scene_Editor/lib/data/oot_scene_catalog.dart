class SceneTemplate {
  const SceneTemplate({
    required this.id,
    required this.name,
    required this.description,
    required this.defaultAssets,
  });

  final String id;
  final String name;
  final String description;
  final List<String> defaultAssets;
}

const List<SceneTemplate> ootSceneCatalog = [
  SceneTemplate(
    id: "kokiri_forest",
    name: "Kokiri Forest",
    description: "Main overworld scene around Link's house.",
    defaultAssets: [
      "scenes/kokiri_forest/scene.zmap",
      "objects/tree/object_wooden_door.zobj",
      "textures/kokiri/grass_01.png",
    ],
  ),
  SceneTemplate(
    id: "links_house_interior",
    name: "Link's House",
    description: "Interior room scene used for child start.",
    defaultAssets: [
      "scenes/links_house/interior_scene.zmap",
      "objects/interior/object_bed.zobj",
      "textures/interior/wood_floor.png",
    ],
  ),
  SceneTemplate(
    id: "kakariko_village",
    name: "Kakariko Village",
    description: "Village scene with multiple exits and actor spawn points.",
    defaultAssets: [
      "scenes/kakariko_village/scene.zmap",
      "objects/doors/object_village_gate.zobj",
      "textures/kakariko/wall_stone.png",
    ],
  ),
  SceneTemplate(
    id: "lost_woods",
    name: "Lost Woods",
    description: "Overworld connector between Kokiri and other regions.",
    defaultAssets: [
      "scenes/lost_woods/scene.zmap",
      "objects/forest/object_forest_sign.zobj",
      "textures/forest/moss_ground.png",
    ],
  ),
  SceneTemplate(
    id: "kokiri_parkour_room",
    name: "Kokiri Parkour Room (Custom)",
    description: "Suggested custom room linked from behind Link's house.",
    defaultAssets: [
      "custom/scenes/kokiri_parkour_room/scene.zmap",
      "custom/objects/parkour/platform_set.zobj",
      "custom/textures/parkour/wall_tiles.png",
    ],
  ),
];
