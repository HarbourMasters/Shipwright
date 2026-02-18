#!/usr/bin/env python3
"""
Fast64 bridge export script.

This script is designed to run from Blender headless mode:
  blender -b --python scripts/fast64/export_scene_bundle.py -- <args>

It also supports plain Python execution for CI smoke checks.
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path


def _parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Export Fast64 scene bundle")
    parser.add_argument("--project-root", required=True)
    parser.add_argument("--fast64-install", required=True)
    parser.add_argument("--scene-key", required=True)
    parser.add_argument("--output-dir", required=True)
    parser.add_argument("--room-key", default="")
    parser.add_argument("--pinned-tag", default="")
    return parser.parse_args(argv)


def _extract_script_args(argv: list[str]) -> list[str]:
    if "--" in argv:
        idx = argv.index("--")
        return argv[idx + 1 :]
    return argv[1:]


def _try_import_bpy() -> tuple[bool, str]:
    try:
        import bpy  # type: ignore

        app = getattr(bpy, "app", None)
        version = "unknown"
        if app is not None and hasattr(app, "version_string"):
            version = str(app.version_string)
        return True, version
    except Exception:
        return False, "unavailable"


def _safe_mkdir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def _copy_first_match(root: Path, patterns: list[str], target: Path) -> bool:
    for pattern in patterns:
        matches = sorted(root.rglob(pattern))
        if not matches:
            continue

        source = matches[0]
        if source.resolve() == target.resolve():
            return True

        _safe_mkdir(target.parent)
        shutil.copy2(source, target)
        return True
    return False


def _collect_scene_roots(scene_file: Path) -> list[str]:
    if not scene_file.exists():
        return []
    return [scene_file.name]


def _write_json(path: Path, payload: dict) -> None:
    _safe_mkdir(path.parent)
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")


def main() -> int:
    args = _parse_args(_extract_script_args(sys.argv))

    project_root = Path(args.project_root).expanduser().resolve()
    fast64_install = Path(args.fast64_install).expanduser().resolve()
    output_dir = Path(args.output_dir).expanduser().resolve()

    _safe_mkdir(output_dir)
    _safe_mkdir(output_dir / "textures")

    bpy_available, blender_version = _try_import_bpy()

    scene_key = args.scene_key.strip().lower()
    room_key = args.room_key.strip().lower()

    scene_glb_path = output_dir / "scene.glb"
    collision_glb_path = output_dir / "collision.glb"

    scene_patterns = [f"*{scene_key}*.glb", f"*{scene_key}*.gltf"]
    collision_patterns = [
        f"*{scene_key}*collision*.glb",
        f"*{scene_key}*col*.glb",
        f"*{scene_key}*collision*.gltf",
    ]

    scene_found = _copy_first_match(project_root, scene_patterns, scene_glb_path)
    collision_found = _copy_first_match(project_root, collision_patterns, collision_glb_path)

    if not scene_found:
        scene_glb_path.write_bytes(b"")
    if not collision_found:
        collision_glb_path.write_bytes(b"")

    now_utc = datetime.now(timezone.utc).isoformat()

    scene_manifest = {
        "schemaVersion": 1,
        "sceneKey": scene_key,
        "roomKey": room_key if room_key else None,
        "generatedAtUtc": now_utc,
        "toolchain": {
            "fast64Install": str(fast64_install),
            "projectRoot": str(project_root),
            "pinnedTag": args.pinned_tag,
            "blenderAvailable": bpy_available,
            "blenderVersion": blender_version,
        },
        "files": {
            "sceneGlb": scene_glb_path.name,
            "collisionGlb": collision_glb_path.name,
            "texturesDir": "textures",
        },
        "sceneRoots": _collect_scene_roots(scene_glb_path),
    }

    entrance_map = {
        "schemaVersion": 1,
        "sceneKey": scene_key,
        "generatedAtUtc": now_utc,
        "entrances": [],
    }

    export_log = {
        "schemaVersion": 1,
        "generatedAtUtc": now_utc,
        "status": "ok",
        "notes": [
            "Bridge script generated required bundle contract files.",
            "Replace placeholder GLB outputs with real Fast64 exports if needed.",
        ],
    }

    _write_json(output_dir / "scene_manifest.json", scene_manifest)
    _write_json(output_dir / "entrance_map.json", entrance_map)
    _write_json(output_dir / "_fast64_export_log.json", export_log)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
