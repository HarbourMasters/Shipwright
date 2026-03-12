/**
 * Ship of Harkinian - PartyKit WebSocket Server
 *
 * This PartyKit server replaces the original Anchor TCP server for web-based
 * multiplayer. It acts as a message relay, forwarding JSON packets between
 * connected clients, similar to how the original anchor.hm64.org server works.
 *
 * Deploy with: npx partykit deploy
 * Or run locally: npx partykit dev
 */

import type { Party, PartyKitServer, Connection } from "partykit/server";

interface ClientInfo {
  clientId: number;
  name: string;
  teamId: string;
}

export default {
  async onConnect(connection: Connection, room: Party) {
    console.log(`[Anchor] Client connected: ${connection.id}`);

    // Send the connection its assigned client ID
    const clientId = hashConnectionId(connection.id);
    connection.send(JSON.stringify({
      type: "WELCOME",
      clientId: clientId,
      roomId: room.id,
    }));

    // Notify all other clients
    for (const conn of room.getConnections()) {
      if (conn.id !== connection.id) {
        conn.send(JSON.stringify({
          type: "CLIENT_CONNECTED",
          clientId: clientId,
        }));
      }
    }
  },

  async onMessage(message: string | ArrayBuffer, sender: Connection, room: Party) {
    if (typeof message !== "string") return;

    try {
      const packet = JSON.parse(message);

      // Add sender info if not present
      if (packet.clientId == null) {
        packet.clientId = hashConnectionId(sender.id);
      }

      const serialized = JSON.stringify(packet);

      // Check if this is a targeted message
      if (packet.targetClientId) {
        // Send to specific client
        for (const conn of room.getConnections()) {
          if (hashConnectionId(conn.id) === packet.targetClientId) {
            conn.send(serialized);
            break;
          }
        }
      } else {
        // Broadcast to all other clients (relay mode)
        for (const conn of room.getConnections()) {
          if (conn.id !== sender.id) {
            conn.send(serialized);
          }
        }
      }
    } catch (e) {
      console.error(`[Anchor] Failed to parse message from ${sender.id}:`, e);
    }
  },

  async onClose(connection: Connection, room: Party) {
    const clientId = hashConnectionId(connection.id);
    console.log(`[Anchor] Client disconnected: ${connection.id} (clientId: ${clientId})`);

    // Notify remaining clients
    for (const conn of room.getConnections()) {
      conn.send(JSON.stringify({
        type: "CLIENT_DISCONNECTED",
        clientId: clientId,
      }));
    }
  },

  async onError(connection: Connection, error: Error, room: Party) {
    console.error(`[Anchor] Error for ${connection.id}:`, error.message);
  },
} satisfies PartyKitServer;

/**
 * Generate a stable numeric client ID from a connection ID string.
 * This ensures the same connection always gets the same clientId.
 */
function hashConnectionId(id: string): number {
  let hash = 0;
  for (let i = 0; i < id.length; i++) {
    const char = id.charCodeAt(i);
    hash = ((hash << 5) - hash) + char;
    hash = hash & 0x7FFFFFFF; // Keep positive 31-bit integer
  }
  return hash;
}
