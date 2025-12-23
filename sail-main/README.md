### SAMMI Sail

The SAMMI sail is meant to be used with the SAMMI bot, it simply redirects
traffic between your SAMMI bot and SoH. You can configure the sail with env
variables to point to your SAMMI's webhook endpoint with SAMMI_WEBHOOK_URL and
have SAMMI point to the Sail's endpoint at http://localhost:43383

## Custom Integrations

We have added new capabilities to the Sail library (`SohClient.ts`) to support advanced Twitch integrations.

### 1. Spawning Actors Relative to Player

You can spawn items or enemies relative to Link's current position and rotation. This is useful for "raining down" items or spawning enemies behind the player.

**Function:** `client.spawnActorRelative(actorId, params, xOffset, yOffset, zOffset, snapToFloor)`

**Parameters:**
*   `actorId` (number): The ID of the actor to spawn (e.g., `0x0010` for En_Bom).
*   `params` (number): The variable/params for the actor (e.g., `0` for normal bomb).
*   `xOffset` (number): Distance to the **Right** (+) or **Left** (-) of Link.
*   `yOffset` (number): Distance **Up** (+) or **Down** (-) from Link.
*   `zOffset` (number): Distance **Forward** (+) or **Backward** (-) from Link.
*   `snapToFloor` (boolean, default `true`): If true, the actor will snap to the ground below the spawn point. Set to `false` to spawn in the air (e.g., for falling items).

**Example:**
```typescript
// Spawn a Redead 100 units behind Link
// Redead Actor ID = 66 (0x42), Params = 0
client.spawnActorRelative(66, 0, 0, 0, -100);

// Rain down a lit bomb 200 units above Link
// Bomb Actor ID = 16 (0x10), Params = 0
// snapToFloor = false so it falls
client.spawnActorRelative(16, 0, 0, 200, 0, false);
```

### 2. Displaying In-Game Messages

You can display custom text boxes in-game, similar to Navi or NPC dialogue.

**Function:** `client.displayMessageBox(text, style, position)`

**Parameters:**
*   `text` (string): The message to display. Supports standard game control codes (e.g., `^` for new box).
*   `style` (number, default `2`): The background style of the text box.
    *   `0`: Black (Cinematic)
    *   `1`: Wooden (Signpost)
    *   `2`: Blue (Navi/Default)
    *   `3`: Ocarina
    *   `4`: None (Bottom)
    *   `5`: None (No Shadow)
*   `position` (number, default `2`): The vertical position of the box.
    *   `0`: Top
    *   `1`: Middle
    *   `2`: Bottom

**Example:**
```typescript
// Display a message from a Twitch viewer
const user = "ZeldaFan123";
const message = "Use the hookshot!";
client.displayMessageBox(`[${user}] says:^${message}`, 2, 2);
```

### 3. Triggering Events (Cucco Storm)

To trigger external events based on game occurrences (like a Cucco Storm), use the existing `actorInit` hook.

**Example:**
```typescript
// Listen for the "Attack Cucco" swarm actor (ID 324 / 0x144)
client.on("actorInit", (event) => {
  if (event.actorId === 324) {
    console.log("CUCCO STORM STARTED!");
    // Trigger OBS source or play sound here
  }
});
```
