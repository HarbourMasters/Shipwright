# ExternalModManager Reference (Hooks, Behaviors e Funcoes Data-Driven)

Este documento lista o que esta **disponivel hoje** no `ExternalModManager` (codigo atual), com foco em hooks, behavior graph e pipeline data-driven.

Arquivos de referencia no codigo:
- `soh/soh/Enhancements/external-mods/ExternalModManager.cpp`
- `soh/soh/Enhancements/external-mods/ExternalModTypes.h`
- `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp`

## 1. Capabilities suportadas no manifest

Capabilities reconhecidas atualmente:
- `hooks.extended.v1`
- `items.data.v2`
- `actors.vm.v1`
- `behaviors.graph.v1`
- `actors.generic.v1`
- `items.catalog.v1`
- `scenes.bundle.v1`
- `render.filter_override.v1`
- `statuses.catalog.v1`
- `combat.damage.v1`
- `combat.targeting.v1`
- `combat.projectiles.v1`
- `combat.aoe.v1`
- `movement.profiles.v1`
- `world.queries.v1`
- `items.use_profiles.v1`
- `patches.vanilla_items.v1`

Quando a capability e declarada no `mod.json`, o path correspondente vira obrigatorio:
- `hookDefinitions` para `hooks.extended.v1`
- `actorDefinitions` para `actors.vm.v1` ou `actors.generic.v1`
- `behaviorDefinitions` para `behaviors.graph.v1`
- `sceneDefinitions` para `scenes.bundle.v1`
- `statusDefinitions` para `statuses.catalog.v1`
- `damageDefinitions` para `combat.damage.v1`
- `targetingDefinitions` para `combat.targeting.v1`
- `projectileDefinitions` para `combat.projectiles.v1`
- `aoeDefinitions` para `combat.aoe.v1`
- `movementDefinitions` para `movement.profiles.v1`
- `itemUseProfiles` para `items.use_profiles.v1`
- `vanillaItemPatches` para `patches.vanilla_items.v1`

Observacao: `patches.vanilla_items.v1` hoje passa no parse/validacao, mas o runtime ainda loga warning de "not yet implemented" para execucao de patch.

## 2. Hooks estendidos (`hooks/hooks.json`)

Formato aceito:
- array direto de subscriptions, ou objeto com `subscriptions: []`

Cada subscription:
- `id` (obrigatorio, unico)
- `hook` (obrigatorio)
- `dispatch` (opcional): `actions` (default) ou `wasmExport`
- `actions` (obrigatorio quando `dispatch=actions`)
- `wasmExport` (obrigatorio quando `dispatch=wasmExport`)
- `cooldownFrames` (opcional, inteiro >= 0)
- `filters` (opcional)

### 2.1 Hooks disponiveis

Nomes aceitos (case-insensitive):
- `onLoadGame`
- `onExitGame`
- `onSceneInit`
- `afterSceneCommands`
- `onTransitionEnd`
- `onFlagSet`
- `onFlagUnset`
- `onSceneFlagSet`
- `onSceneFlagUnset`
- `onPlayerUpdate`
- `onPlayerUseItem`
- `onPlayerHealthChange`
- `onItemReceive`
- `onActorInit`
- `onActorSpawn`
- `onActorUpdate`
- `onActorKill`
- `onActorDestroy`
- `onEnemyDefeat`
- `onBossDefeat`
- `onPlayDestroy`
- `onGameFrameUpdate`

### 2.2 Filtros de hook disponiveis

Campos aceitos em `filters`:
- `scene`
- `actorId`
- `category`
- `itemId`
- `flagType`
- `flagId`
- `healthDeltaRange: [min, max]`

### 2.3 Args entregues para `dispatch=wasmExport`

Ordem fixa do vetor de argumentos:
1. `scene`
2. `actorId`
3. `actorCategory`
4. `itemId`
5. `flagType`
6. `flagId`
7. `healthDelta`

## 3. Entry script (`entryScript`, geralmente `scripts/init.json`)

Blocos aceitos hoje:
- `onGameLoaded: [actions]`
- `onSceneInit: [{ scene, actions[] }]`
- `onFrameTriggers` (ou alias `triggers`):
  - `{ id?, scene, bounds: { min:[x,y,z], max:[x,y,z] }, cooldownFrames?, actions[] }`
- `onInput` (api v2):
  - `{ id?, binding|bindingId, trigger?, cooldownFrames?, actions[] }`
- `behaviorRuntime`:
  - `maxStepsPerActorPerFrame` (clamp 1..1024)
  - `maxStepsPerModPerFrame` (clamp 1..50000)

## 4. Behavior graph (`behaviors/behaviors.json`)

Formato aceito:
- array direto de behaviors, ou objeto com `behaviors: []`

Cada behavior:
- `id` (obrigatorio)
- `events` (objeto, obrigatorio)

Cada evento aceita 2 formas:
1. Lista direta de actions:
- `"onItemUsed": [ {action...}, {action...} ]`

2. Lista de regras:
- `"onItemUsed": [ { conditions:[...], chance?, actions:[...] }, ... ]`

### 4.1 Eventos disparados atualmente pelo runtime

Eventos consumidos pelo runtime:
- `onspawn`
- `ondestroy`
- `onupdate`
- `onrandomtick`
- `onroomenter`
- `ontimeofdaychanged`
- `onswitchflagchanged`
- `onplayernear`
- `onplayerfar`
- `ontimer`
- `oninteract`
- `onsignal`
- `onsceneenter`
- `onitemused`
- `onitemgranted`
- `onitemequipped`
- `oncooldownready`
- `manual` (via action `callBehavior`)

Aliases aplicados automaticamente:
- `onspawn` <-> `oninit`
- `ondestroy` <-> `ondespawn`
- `onitemgranted` <-> `onitemequipped`

### 4.2 Condicoes de behavior disponiveis

Tipos suportados atualmente:
- `isChild`
- `isAdult`
- `isDay`
- `isNight`
- `randomChance`
- `hasItem`
- `hasStatus`
- `statusRemaining`
- `distanceToPlayer`
- `sceneIs`
- `roomIs`
- `hasSwitchFlag`
- `switchIsOn`
- `var`
- `varEquals`
- `varCompare`

Campos de condicao suportados:
- `condition` (ou `type`)
- `scope`
- `key`
- `op`
- `value` (string|number|boolean)
- `number` (number)

Scopes usados por condicoes/actions de variavel:
- `actor`
- `scene`
- `mod_save` (mesmo bucket de scene)
- `global_mod`

## 5. Acoes reconhecidas pelo parser

A lista abaixo e a lista real de `action` aceita hoje.

Acoes gerais:
- `showNotification`
- `teleportToEntrance`
- `loadModScene`
- `pressButton`
- `showEquippedItemGet` (alias: `showEquippedItemPickup`)
- `spawnSmoke`
- `spawnKusa`
- `lanternLight`
- `igniteFrontTarget`
- `freezeFrontTarget`
- `spawnActor`
- `despawnActor`
- `setActorState`
- `moveActorToPathNode`
- `openDialog`
- `setSwitchFlag`
- `clearSwitchFlag`
- `setEventChkInf`
- `clearEventChkInf`
- `setInfTable`
- `clearInfTable`
- `giveRupees`
- `takeRupees`
- `grantModItem`
- `revokeModItem`
- `setVar`
- `addVar`
- `clampVar`
- `emitSignal`
- `callBehavior`
- `invokeWasm`

Acoes data-driven (catalog-centric):
- `applyStatus`
- `clearStatus`
- `clearAllStatuses`
- `useItemProfile`
- `dealDamage`
- `spawnProjectile`
- `spawnAoE`
- `applyMovementProfile`
- `applyImpulse`
- `getGroundInfo`
- `raycast`
- `raycastAll`

## 6. Funcoes data-driven: parametros aceitos hoje

### 6.1 `applyStatus`
Campos aceitos:
- `status` (obrigatorio, string)
- `target` (opcional): `frontTarget|self|player|actorHandle`
- `actorHandle` ou `handle` (obrigatorio quando `target=actorHandle`)
- `itemId` ou `requiresItemId` (opcional)
- `durationFrames` (opcional)
- `tickFrames` (opcional)
- `damagePerTick` (opcional)
- `shakeFrames` (opcional)
- `range` (opcional)
- `intensity` (opcional)

Status token built-in aceitos em `status`:
- `core:burning`, `core:fire`, `fire`
- `core:freeze`, `core:frozen`, `freeze`
- `core:stun`, `stun`
- `core:poison`, `poison`
- `core:blind`, `blind`
- `core:speed`, `speed`
- `core:slow`, `slow`
- `core:high_jump`, `highjump`, `high_jump`
- `core:strength`, `strength`
- `core:weakness`, `weakness`
- qualquer `namespace:id` (vira `Custom`)

### 6.2 `clearStatus`
Campos aceitos:
- `status` (opcional; se ausente, limpa por tipo resolvido)
- `target` (opcional)
- `actorHandle` ou `handle` (opcional)

### 6.3 `clearAllStatuses`
Campos aceitos:
- `target` (opcional)
- `actorHandle` ou `handle` (opcional)

### 6.4 `useItemProfile`
Campos aceitos (um deles obrigatorio):
- `profile`
- `useProfile`
- `profileId`

### 6.5 `dealDamage`
Campos aceitos:
- `profile` ou `damageProfileId` (obrigatorio)
- `target` (opcional)
- `actorHandle` ou `handle` (opcional)

### 6.6 `spawnProjectile`
Campos aceitos:
- `projectile` ou `profile` ou `projectileProfileId` (obrigatorio)

### 6.7 `spawnAoE`
Campos aceitos:
- `aoe` ou `profile` ou `aoeProfileId` (obrigatorio)

### 6.8 `applyMovementProfile`
Campos aceitos:
- `movement` ou `profile` ou `movementProfileId` (obrigatorio)
- `durationFrames` (opcional)

### 6.9 `applyImpulse`
Campos aceitos:
- `mode` (opcional; atualmente parser aceita, runtime ainda usa impulso padrao)
- `strength` (opcional)

### 6.10 `getGroundInfo`
Sem parametros.

Saida hoje em `globalBlackboard`:
- `__groundY`
- `__grounded`

### 6.11 `raycast` e `raycastAll`
Campos aceitos:
- `range` (opcional)

Saida hoje em `globalBlackboard`:
- `__raycastHit` (`1` ou `0`)
- `__raycastActorId` (id do ator ou `-1`)

Observacao: atualmente `raycast` e `raycastAll` compartilham a mesma resolucao frontal simplificada.

## 7. Catalogos data-driven suportados (schema atual)

Todos exigem `schemaVersion: 1`.

### 7.1 `statuses/statuses.json` (`statuses.catalog.v1`)
Raiz:
- `{ schemaVersion, statuses: [] }`

Campos por status:
- `id` (namespaced)
- `displayName?`
- `baseStatus?`
- `durationFrames?`
- `tickFrames?`
- `damagePerTick?`
- `intensity?`
- `shakeFrames?`
- `stacking.mode?`, `stacking.maxStacks?`
- `effects.onApply[]?`, `effects.onTick[]?`, `effects.onExpire[]?`

### 7.2 `combat/damage_profiles.json` (`combat.damage.v1`)
Raiz:
- `{ schemaVersion, profiles: [] }`

Campos por profile:
- `id` (namespaced)
- `amount?`
- `type?`
- `iframes.frames?`

### 7.3 `combat/targeting_profiles.json` (`combat.targeting.v1`)
Raiz:
- `{ schemaVersion, profiles: [] }`

Campos por profile:
- `id` (namespaced)
- `mode` (obrigatorio): `frontTarget|lockedOnTarget|raycast|cone|sphere|self|player`
- `range?`, `radius?`, `angle?`
- `filters.stopOnWall?`, `filters.includeProps?`, `filters.includeEnemies?`

### 7.4 `items/use_profiles.json` (`items.use_profiles.v1`)
Raiz:
- `{ schemaVersion, useProfiles: [] }`

Campos por useProfile:
- `id` (namespaced)
- `targetingProfile` (obrigatorio)
- `cooldownFrames?`
- `effects[]` (obrigatorio)

Acoes de effect atualmente executadas:
- `dealDamage`
- `applyStatus`
- `spawnProjectile`
- `spawnAoE`
- `applyMovementProfile`

### 7.5 `combat/projectiles.json` (`combat.projectiles.v1`)
Raiz:
- `{ schemaVersion, projectiles: [] }`

Campos por projectile:
- `id` (namespaced)
- `shape?`, `speed?`, `gravityScale?`, `lifetimeFrames?`, `radius?`
- `pierce.maxHits?`
- `collision.stopOnWall?`
- `onHit.damageProfile?`
- `onHit.applyStatuses[]?`

### 7.6 `combat/aoe_profiles.json` (`combat.aoe.v1`)
Raiz:
- `{ schemaVersion, aoe: [] }`

Campos por aoe:
- `id` (namespaced)
- `shape?`, `range?`, `radius?`, `angle?`
- `durationFrames?`, `tickFrames?`
- `effects.onEnter[]?`, `effects.onTick[]?`, `effects.onExit[]?`

### 7.7 `movement/movement_profiles.json` (`movement.profiles.v1`)
Raiz:
- `{ schemaVersion, profiles: [] }`

Campos por profile:
- `id` (namespaced)
- `durationFrames?`
- `speedMultiplier?`
- `accelMultiplier?`
- `gravityScale?`

### 7.8 `patches/vanilla_items.patch.json` (`patches.vanilla_items.v1`)
- Arquivo e parse basico aceitos.
- Execucao de patch em runtime ainda nao implementada.

## 8. Bridge legado e compatibilidade ativa

Mantido no runtime:
- `freezeFrontTarget` segue disponivel e entra no pipeline de status.
- `igniteFrontTarget` segue como acao legacy dedicada.
- `freezeOnMeleeHit` em `items.params` segue funcionando (bridge legacy).

## 9. Limites e observacoes de runtime atuais

- Budget default de behavior por runtime:
  - `behaviorMaxStepsPerActorPerFrame = 64`
  - `behaviorMaxStepsPerModPerFrame = 5000`
- Budget default de hooks por frame:
  - `maxHookCallsPerFrame = 256`
- `WasmRuntime` atual esta em modo MVP:
  - valida modulo/header/budgets
  - `InvokeExport` nao executa host API rica ainda
  - sem imports data-driven expostos no runtime atual

## 10. Chaves internas uteis no blackboard (runtime)

Chaves que o runtime escreve automaticamente e podem ser lidas por condicoes/acoes de behavior:
- `__groundY`, `__grounded` (de `getGroundInfo`)
- `__raycastHit`, `__raycastActorId` (de `raycast`/`raycastAll`)
- `__switchFlag`, `__switchValue` (mudanca de switch no frame)
- `__lastSignal` (ultimo sinal processado em `onsignal`)

---

Se quiser, o proximo passo e eu gerar uma segunda versao deste doc em formato de "quick reference" (1 pagina) com tabela unica: `evento -> condicoes -> actions -> arquivos/capabilities`.
