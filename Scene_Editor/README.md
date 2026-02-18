# Scene Editor MVP

Editor em Flutter para orquestrar criacao de mod externo ZIP para Ocarina of Time (SoH).

## Features

- Carregar ROM (`.z64`, `.n64`, `.v64`, `.rom`).
- Studio 3D com indexacao de assets e preview de cena por `sceneId`.
- Configuracao persistente de toolchain Fast64 em:
  - `%APPDATA%/scene_editor/toolchain_config.json`
- Validacao de toolchain (`Blender`, instalacao Fast64, projeto, tag pinada).
- Bridge Fast64 para exportar bundle de cena (`scene_manifest.json`, `entrance_map.json`, `scene.glb`, `collision.glb`).
- Export de mod ZIP no layout runtime:
  - `mod.json`
  - `scripts/init.json`
  - `assets/parkour_room.otr` (quando fornecido)
  - `reports/validation_report.json`
  - `README_MOD.txt`
- Fluxo de template Kokiri Parkour (3 triggers padrao).

## Rodar

```powershell
cd Scene_Editor
flutter pub get
flutter run -d windows
```

## Testes

```powershell
cd Scene_Editor
flutter analyze
flutter test
```

## Build Windows

```powershell
cd Scene_Editor
flutter build windows --release
```

Saida:

`Scene_Editor\build\windows\x64\runner\Release\scene_editor.exe`
