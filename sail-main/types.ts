export type EffectName =
  | "SetSceneFlag"
  | "UnsetSceneFlag"
  | "SetFlag"
  | "UnsetFlag"
  | "ModifyHeartContainers"
  | "FillMagic"
  | "EmptyMagic"
  | "ModifyRupees"
  | "NoUI"
  | "ModifyGravity"
  | "ModifyHealth"
  | "SetPlayerHealth"
  | "FreezePlayer"
  | "BurnPlayer"
  | "ElectrocutePlayer"
  | "KnockbackPlayer"
  | "ModifyLinkSize"
  | "InvisibleLink"
  | "PacifistMode"
  | "DisableZTargeting"
  | "WeatherRainstorm"
  | "ReverseControls"
  | "ForceEquipBoots"
  | "ModifyRunSpeedModifier"
  | "OneHitKO"
  | "ModifyDefenseModifier"
  | "GiveOrTakeShield"
  | "TeleportPlayer"
  | "ClearAssignedButtons"
  | "SetTimeOfDay"
  | "SetCollisionViewer"
  | "SetCosmeticsColor"
  | "RandomizeCosmetics"
  | "PressButton"
  | "PressRandomButton"
  | "AddOrTakeAmmo"
  | "RandomBombFuseTimer"
  | "DisableLedgeGrabs"
  | "RandomWind"
  | "RandomBonks"
  | "PlayerInvincibility"
  | "SlipperyFloor"
  | "SpawnActorRelative"
  | "DisplayMessageBox"
  | "SpawnActor"; // Keep SpawnActor for Method 1

export interface CommandEffect {
  type: "command";
  command: string;
}

export interface ApplyEffect {
  type: "apply";
  name: EffectName;
  parameters?: (number | string)[];
  text?: string;
}

export interface RemoveEffect {
  type: "remove";
  name: EffectName;
}

export type Effect = CommandEffect | ApplyEffect | RemoveEffect;

export interface EffectPacket {
  id: string;
  type: "effect";
  effect: Effect;
}

export interface CommandPacket {
  id: string;
  type: "command";
  command: string;
}

export type ResultStatus = "success" | "failure" | "try_again" | "timeout";

export interface ResultPacket {
  id: string;
  type: "result";
  status: ResultStatus;
}

export interface OnTransitionEndHook {
  type: "OnTransitionEnd";
  sceneNum: number;
}

export interface OnLoadGameHook {
  type: "OnLoadGame";
  fileNum: number;
}

export interface OnExitGameHook {
  type: "OnExitGame";
  fileNum: number;
}

export interface OnItemReceiveHook {
  type: "OnItemReceive";
  tableId: number;
  getItemId: number;
}

export interface OnEnemyDefeatHook {
  type: "OnEnemyDefeat";
  actorId: number;
  params: number;
}

export interface OnActorInitHook {
  type: "OnActorInit";
  actorId: number;
  params: number;
}

export interface OnFlagSetHook {
  type: "OnFlagSet";
  flagType: number;
  flag: number;
}

export interface OnFlagUnsetHook {
  type: "OnFlagUnset";
  flagType: number;
  flag: number;
}

export interface OnSceneFlagSetHook {
  type: "OnSceneFlagSet";
  flagType: number;
  flag: number;
  sceneNum: number;
}

export interface OnSceneFlagUnsetHook {
  type: "OnSceneFlagUnset";
  flagType: number;
  flag: number;
  sceneNum: number;
}

export type Hook =
  | OnTransitionEndHook
  | OnLoadGameHook
  | OnExitGameHook
  | OnItemReceiveHook
  | OnEnemyDefeatHook
  | OnActorInitHook
  | OnFlagSetHook
  | OnFlagUnsetHook
  | OnSceneFlagSetHook
  | OnSceneFlagUnsetHook;

export interface HookPacket {
  id: string;
  type: "hook";
  hook: Hook;
}

export type IncomingPacket = ResultPacket | HookPacket;
export type OutgoingPacket = EffectPacket | CommandPacket;
export type Packet = IncomingPacket | OutgoingPacket;