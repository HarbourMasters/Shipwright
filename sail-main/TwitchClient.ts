import * as TwitchIrc from "https://deno.land/x/twitch_irc@0.10.2/mod.ts";
import { Privmsg } from "https://deno.land/x/twitch_irc@0.10.2/lib/message/privmsg.ts";
import EventEmitter from "https://deno.land/x/eventemitter@1.2.4/mod.ts";

export class TwitchClient extends EventEmitter<{
  chat: (message: string, user: string, raw: Privmsg) => void;
  bits: (bits: number, message: string, user: string, raw: Privmsg) => void;
  redeem: (reward: string, message: string, user: string, raw: Privmsg) => void;
  raw: (raw: Privmsg) => void;
}> {
  public ircClient: any;
  public channel: string;
  public debug = false;

  constructor({ channel, debug }: { channel: string; debug?: boolean }) {
    super();
    if (debug) this.debug = debug;
    this.channel = channel;
  }

  async connect(username?: string, token?: string) {
    if (username && token) {
        console.log(`[TwitchClient] Authenticating as ${username}...`);
        this.ircClient = new TwitchIrc.Client({
            channels: [`#${this.channel}`],
            userName: username,
            password: `oauth:${token}`
        });
    } else {
        console.log(`[TwitchClient] Connecting Anonymously (Read Only)...`);
        this.ircClient = new TwitchIrc.Client();
    }

    this.ircClient.on("privmsg", (e: any) => this.handleMessage(e));
    this.ircClient.on("usernotice", (e: any) => this.handleMessage(e as any));

    // Wait for the connection to complete
    await new Promise<void>((resolve) => {
      this.ircClient.on("open", async () => {
        // If anonymous, we must join manually. Authenticated handles it in config.
        if (!username) await this.ircClient.join(`#${this.channel}`);
        this.log(`Connected to chat for ${this.channel}`);
        resolve();
      });
      
      this.ircClient.on("error", (err: any) => {
         console.error("[TwitchClient] Connection Error:", err);
      });
    });
  }

  handleMessage(event: any) {
    this.emit("raw", event);
    
    const userDisplayName = event.user?.displayName || event.tags?.displayName || "Unknown";
    const messageContent = event.message || "";

    if (event.raw?.tags?.customRewardId) {
      this.emit("redeem", event.raw.tags.customRewardId, messageContent, userDisplayName, event);
    } else if (event.raw?.tags?.bits) {
      this.emit("bits", parseInt(event.raw.tags.bits, 10), messageContent, userDisplayName, event);
    } else {
      if (event.kind === "PRIVMSG" || (!event.kind && messageContent)) {
        this.emit("chat", messageContent, userDisplayName, event);
      }
    }
  }

  log(...data: any[]) {
    console.log("[TwitchClient]:", ...data);
  }
}