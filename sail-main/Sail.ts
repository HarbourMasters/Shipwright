import EventEmitter from "https://deno.land/x/eventemitter@1.2.4/mod.ts";
import { SohClient } from "./SohClient.ts";

export class Sail extends EventEmitter<{
  clientConnected: (client: SohClient) => void;
}> {
  private listener?: Deno.Listener;
  public clients: SohClient[] = [];
  public port = 43384;
  public debug = false;

  constructor({ port, debug }: { port?: number; debug?: boolean } = {}) {
    super();

    if (port) {
      this.port = port;
    }
    if (debug) {
      this.debug = debug;
    }
  }

  async start() {
    try {
      this.listener = Deno.listen({ port: this.port });

      this.log(`Server listening on port ${this.port}`);
      for await (const connection of this.listener) {
        try {
          const client = new SohClient(connection, this, { debug: this.debug });
          this.clients.push(client);
          this.emit("clientConnected", client);
        } catch (error) {
          this.log("Error connecting client:", error);
        }
      }
    } catch (error) {
      this.log("Error starting server:", error);
    }
  }

  removeClient(client: SohClient) {
    const index = this.clients.indexOf(client);
    this.clients.splice(index, 1);
  }

  // deno-lint-ignore no-explicit-any
  log(...data: any[]) {
    console.log("[Sail]:", ...data);
  }
}
