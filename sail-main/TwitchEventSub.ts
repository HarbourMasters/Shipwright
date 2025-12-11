import EventEmitter from "https://deno.land/x/eventemitter@1.2.4/mod.ts";

export class TwitchEventSub extends EventEmitter<{
  redeem: (event: any) => void;
  connected: () => void;
}> {
  private ws: WebSocket | null = null;
  private sessionId: string = "";
  private token: string;
  private clientId: string = "";
  private broadcasterId: string = "";
  public username: string = ""; 
  public debug: boolean = false;

  constructor(token: string, options?: { debug?: boolean }) {
    super();
    this.token = token.trim().startsWith("oauth:") ? token.trim().slice(6) : token.trim();
    if (options?.debug) this.debug = options.debug;
  }

  async connect() {
    try {
      await this.validateToken();
      const url = "wss://eventsub.wss.twitch.tv/ws";
      this.ws = new WebSocket(url);

      this.ws.onopen = () => { if (this.debug) console.log("[EventSub] WebSocket Connected"); };
      this.ws.onmessage = (msg) => { this.handleMessage(JSON.parse(msg.data)); };
      this.ws.onerror = (err) => { if (this.debug) console.log("[EventSub] WebSocket Warning/Error."); };
      this.ws.onclose = () => { if (this.debug) console.log("[EventSub] WebSocket Closed."); };

    } catch (e) {
      throw new Error(`EventSub Connection Failed: ${e instanceof Error ? e.message : e}`);
    }
  }

  async createReward(title: string, cost: number) {
    if (!this.broadcasterId) return null;
    console.log(`[EventSub] Creating reward: ${title}...`);
    const url = `https://api.twitch.tv/helix/channel_points/custom_rewards?broadcaster_id=${this.broadcasterId}`;
    const res = await fetch(url, {
      method: "POST",
      headers: {
        "Client-ID": this.clientId,
        "Authorization": `Bearer ${this.token}`,
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        title: title,
        cost: cost,
        is_enabled: false,
        is_user_input_required: false
      }),
    });
    if (!res.ok) {
      console.error(`[EventSub] Failed to create reward ${title}:`, await res.text());
      return null;
    }
    const data = await res.json();
    return data.data[0]; 
  }

  async updateRewardStatus(rewardId: string, isEnabled: boolean) {
    if (this.debug) console.log(`[EventSub] Setting reward ${rewardId} to ${isEnabled ? "ENABLED" : "DISABLED"}...`);
    if (!this.broadcasterId) return;

    const url = `https://api.twitch.tv/helix/channel_points/custom_rewards?broadcaster_id=${this.broadcasterId}&id=${rewardId}`;
    const res = await fetch(url, {
      method: "PATCH",
      headers: {
        "Client-ID": this.clientId,
        "Authorization": `Bearer ${this.token}`,
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ is_enabled: isEnabled }),
    });

    if (!res.ok) {
      const err = await res.text();
      if (!err.includes("Not Found")) console.error(`[EventSub] Update Failed:`, err);
    } else {
      if (this.debug) console.log(`[EventSub] Reward ${rewardId} updated!`);
    }
  }

  private async validateToken() {
    const res = await fetch("https://id.twitch.tv/oauth2/validate", {
      headers: { "Authorization": `OAuth ${this.token}` },
    });
    if (!res.ok) throw new Error(`Token validation failed: ${res.status}`);
    const data = await res.json();
    this.clientId = data.client_id;
    this.broadcasterId = data.user_id;
    this.username = data.login; 
    if (this.debug) console.log(`[EventSub] Authenticated as user: ${this.username}`);
  }

  private handleMessage(data: any) {
    if (data.metadata.message_type === "session_welcome") {
      this.sessionId = data.payload.session.id;
      this.subscribeToRedeems();
      this.emit("connected");
    } else if (data.metadata.message_type === "notification") {
      if (data.metadata.subscription_type === "channel.channel_points_custom_reward_redemption.add") {
        const r = data.payload.event;
        this.emit("redeem", { rewardId: r.reward.id, rewardTitle: r.reward.title, userInput: r.user_input, userName: r.user_name });
      }
    }
  }

  private async subscribeToRedeems() {
    await fetch("https://api.twitch.tv/helix/eventsub/subscriptions", {
      method: "POST",
      headers: {
        "Client-ID": this.clientId,
        "Authorization": `Bearer ${this.token}`,
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        type: "channel.channel_points_custom_reward_redemption.add",
        version: "1",
        condition: { broadcaster_user_id: this.broadcasterId },
        transport: { method: "websocket", session_id: this.sessionId },
      }),
    });
    if (this.debug) console.log("[EventSub] Subscribed to Redemptions!");
  }
}