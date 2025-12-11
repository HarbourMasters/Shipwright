import { Sail } from "../Sail.ts";
import { SohClient } from "../SohClient.ts";
import { TwitchClient } from "../TwitchClient.ts";
import { TwitchEventSub } from "../TwitchEventSub.ts";
import { nanoid } from "https://deno.land/x/nanoid@v3.0.0/nanoid.ts";

// --- CONFIGURATION ---
const TWITCH_CHANNEL = "lycheelilith";
const OAUTH_TOKEN = "61bxsk935zl4jft7z8z4140pdrdkkw";
const SAVE_FILE = "./sail_save.json"; 

// --- REWARD UUIDS ---
// These are the IDs !setup will generate. 
// If you run !setup again, these will update in your console.
const REWARD_IDS = {
  ARROWS: "2bfb863a-b6ed-4f93-8e25-67c512258997",
  BOMBS: "0a50422f-d52b-4a6c-85ac-7a6c2a33bad0",
  NUTS: "f2d7f817-0f63-4512-835f-ece87c8223db",
  SEEDS: "031ae222-dea3-4e1c-8db4-085d76392c12",
  STICKS: "ae0603d1-6639-41e0-8814-da7482a4bf0e",
  MAGIC: "ca2c9d2a-cb6a-4ddf-bd85-f160942c2e6e",
  
  HEARTS: "6bd72300-9fbf-4451-bd18-79165d6b2804", 
  FAIRY: "b14b4ee8-94d5-4d8c-a4bf-20c545cb52d4", 

  WALLMASTER: "35d048e0-2ce7-45f3-b739-0728c7c64be0",
  BOMB_TRAP: "708b5fdd-0978-48d4-a2e4-5e5e78422117",
  MYSTERY: "3dcb54bb-a50f-4ddb-a196-8773c3b5cb24",
  CUCCO: "1929e409-bf66-45e8-b1af-8a79ac0a9136",

  GREEN_RUPEES: "8642627c-2726-49fd-ab87-3ec9b552e259",
  BLUE_RUPEES: "3d532c46-1b8d-4b38-8863-17f74ea1f212",
  RED_RUPEE: "ae8fd0a7-6cfb-4a33-b03c-d159a4e6a66c",
  PURPLE_RUPEE: "80e989bb-a602-439b-94ef-2bde192703e7",
  GOLD_RUPEE: "40e7ae5b-7723-47d2-876f-9381c047e732",
};

// --- AUTOMATION SETUP LIST ---
// Updated titles to reflect single items
const AUTO_REWARDS = [
  { key: "ARROWS", title: "Give Arrows", cost: 100 },
  { key: "BOMBS", title: "Give Bombs", cost: 150 },
  { key: "NUTS", title: "Give Deku Nuts", cost: 50 },
  { key: "SEEDS", title: "Give Deku Seeds", cost: 50 },
  { key: "STICKS", title: "Give Deku Stick", cost: 50 },
  { key: "MAGIC", title: "Give Magic Jar", cost: 200 },
  { key: "FAIRY", title: "Give Fairy", cost: 200 },
  { key: "WALLMASTER", title: "Summon Wallmaster", cost: 666 },
  { key: "BOMB_TRAP", title: "Bomb Trap!", cost: 300 },
  { key: "CUCCO", title: "Cucco Storm", cost: 500 },
  { key: "MYSTERY", title: "Mystery Item", cost: 100 },
  { key: "GREEN_RUPEES", title: "Green Rupee Spam", cost: 50 },
  { key: "BLUE_RUPEES", title: "Blue Rupee Drop", cost: 100 },
  { key: "RED_RUPEE", title: "Red Rupee", cost: 200 },
  { key: "PURPLE_RUPEE", title: "Purple Rupee", cost: 500 },
  { key: "GOLD_RUPEE", title: "Gold Rupee", cost: 1000 },
];

// --- INITIALIZATION ---
const sail = new Sail({ port: 43384, debug: true });
const chatClient = new TwitchClient({ channel: TWITCH_CHANNEL });
const eventSub = new TwitchEventSub(OAUTH_TOKEN, { debug: true });
let sohClient: SohClient | undefined;

// --- PERSISTENCE ---
const state = { unlockedFeatures: new Set<string>(), skulltulaCount: 0 };

async function loadSave() {
  try {
    const data = await Deno.readTextFile(SAVE_FILE);
    const json = JSON.parse(data);
    if (Array.isArray(json)) {
      json.forEach((item: string) => state.unlockedFeatures.add(item));
    } else if (json.unlockedFeatures) {
      json.unlockedFeatures.forEach((item: string) => state.unlockedFeatures.add(item));
      state.skulltulaCount = json.skulltulaCount || 0;
    }
    console.log(`[Save] Loaded: ${state.unlockedFeatures.size} Unlocks | ${state.skulltulaCount} Tokens`);
  } catch (e) { console.log("[Save] No existing save file."); }
}

async function saveSave() {
  const data = JSON.stringify({ unlockedFeatures: [...state.unlockedFeatures], skulltulaCount: state.skulltulaCount }, null, 2);
  await Deno.writeTextFile(SAVE_FILE, data);
}

// --- SAFE REWARD UPDATER ---
async function safeUpdateReward(id: string, name: string) {
  if (id === "REPLACE_ME") {
    console.log(`[Config] Skipped enabling '${name}' - ID not set in config yet.`);
    return;
  }
  await eventSub.updateRewardStatus(id, true);
  console.log(`✨ New Reward Unlocked: ${name}!`);
}

// Helpers
function randomRange(min: number, max: number) { return Math.floor(Math.random() * (max - min + 1)) + min; }
const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));
const mysteryItems = [[21, 11], [21, 5], [21, 12], [21, 13], [21, 15], [21, 2], [21, 1], [21, 3]];

// --- 1. CHAT COMMANDS ---
chatClient.on("chat", async (message, user) => {
  if (message.match(/^!rupee$/i) && !onCooldown("rupee", 1, user)) {
    sohClient?.sendPacket({ id: nanoid(), type: "effect", effect: { type: "apply", name: "ModifyRupees", parameters: [1] } });
  }
  if (message.match(/^!rupoor$/i) && !onCooldown("rupoor", 1, user)) {
    sohClient?.sendPacket({ id: nanoid(), type: "effect", effect: { type: "apply", name: "ModifyRupees", parameters: [-1] } });
  }
  if (message.match(/^!tokens$/i) && !onCooldown("tokens", 10, user)) {
     console.log(`🕸️ Link has collected ${state.skulltulaCount} Gold Skulltula Tokens!`);
  }

  // --- SETUP COMMAND (ADMIN) ---
  if (message.match(/^!setup$/i) && user.toLowerCase() === TWITCH_CHANNEL.toLowerCase()) {
    console.log("\n--- STARTING AUTOMATIC REWARD CREATION ---");
    let outputLog = "\n// NEW IDS GENERATED:\n";
    for (const reward of AUTO_REWARDS) {
      const newReward = await eventSub.createReward(reward.title, reward.cost);
      if (newReward) {
        outputLog += `  ${reward.key}: "${newReward.id}", // ${reward.title}\n`;
        console.log(`✅ Created '${reward.title}'`);
      } else {
        outputLog += `  ${reward.key}: "ERROR",\n`;
      }
      await sleep(500);
    }
    console.log(outputLog);
    console.log("\n------------------------------------------");
  }
});

// --- 2. REDEMPTIONS ---
eventSub.on("redeem", async (event) => {
  const { rewardId, userName } = event;
  console.log(`[EventSub] Redeem: ${event.rewardTitle} (ID: ${rewardId}) by ${userName}`);

  switch (rewardId) {
    case REWARD_IDS.ARROWS: sohClient?.spawnActorAtPlayer(21, 5); break; // 1 Arrow Bundle
    case REWARD_IDS.BOMBS: sohClient?.spawnActorAtPlayer(21, 4); break; // 1 Bomb Drop
    case REWARD_IDS.NUTS: sohClient?.spawnActorAtPlayer(21, 12); break; // 1 Nut Drop
    case REWARD_IDS.SEEDS: sohClient?.spawnActorAtPlayer(21, 16); break; // 1 Seed Drop
    case REWARD_IDS.STICKS: sohClient?.spawnActorAtPlayer(21, 13); break; // 1 Stick Drop
    case REWARD_IDS.MAGIC: sohClient?.spawnActorAtPlayer(21, 15); break;
    
    case REWARD_IDS.HEARTS: for(let i=0; i<3; i++) { await sleep(30); sohClient?.spawnActorAtPlayer(21, 3); } break; // 3 Hearts
    case REWARD_IDS.FAIRY: sohClient?.spawnActorAtPlayer(24, 4); break;
    
    case REWARD_IDS.WALLMASTER: sohClient?.spawnActorCommand(17, 0); break;
    case REWARD_IDS.BOMB_TRAP: sohClient?.spawnActorAtPlayer(16, 0); break;
    case REWARD_IDS.MYSTERY: sohClient?.spawnActorAtPlayer(mysteryItems[randomRange(0, mysteryItems.length - 1)][0], mysteryItems[randomRange(0, mysteryItems.length - 1)][1]); break;
    case REWARD_IDS.CUCCO: sohClient?.spawnActorAtPlayer(25, 0); break;

    case REWARD_IDS.GREEN_RUPEES: for(let i=0; i<5; i++) { await sleep(30); sohClient?.spawnActorAtPlayer(21, 0); } break; // 5 Green
    case REWARD_IDS.BLUE_RUPEES: for(let i=0; i<2; i++) { await sleep(30); sohClient?.spawnActorAtPlayer(21, 1); } break; // 2 Blue
    case REWARD_IDS.RED_RUPEE: sohClient?.spawnActorAtPlayer(21, 2); break;
    case REWARD_IDS.PURPLE_RUPEE: sohClient?.spawnActorAtPlayer(21, 20); break;
    case REWARD_IDS.GOLD_RUPEE: sohClient?.spawnActorAtPlayer(21, 19); break;
    
    default: console.log(`[EventSub] Unmapped Reward: ${event.rewardTitle} | ID: ${rewardId}`);
  }
});

// --- 3. GAME LOGIC ---
sail.on("clientConnected", (client) => {
  sohClient = client;

  client.on("sceneFlagSet", (e) => console.log(`[SPY - SCENE] S:${e.sceneNum} FT:${e.flagType} F:${e.flag}`));
  client.on("flagSet", (e) => console.log(`[SPY - GLOBAL] FT:${e.flagType} F:${e.flag}`));

  client.on("itemReceive", (event) => {
    // Skulltula Token (91)
    if (event.getItemId === 91) {
       state.skulltulaCount++;
       saveSave();
       if ([10, 20, 30, 40, 50, 100].includes(state.skulltulaCount)) {
          console.log(`🕷️ Milestone! Link has collected ${state.skulltulaCount} Tokens!`);
       }
    }
    // Smart Unlocks
    if (event.getItemId === 4 && !state.unlockedFeatures.has("ARROWS")) {
        safeUpdateReward(REWARD_IDS.ARROWS, "Arrows"); state.unlockedFeatures.add("ARROWS"); saveSave(); 
    }
    if (event.getItemId === 15 && !state.unlockedFeatures.has("BOTTLE")) {
        safeUpdateReward(REWARD_IDS.CUCCO, "Cucco Storm"); state.unlockedFeatures.add("BOTTLE"); saveSave();
    }
    if (event.getItemId === 102 && !state.unlockedFeatures.has("WALLMASTER")) {
        safeUpdateReward(REWARD_IDS.WALLMASTER, "Wallmaster"); state.unlockedFeatures.add("WALLMASTER"); saveSave();
    }
    if (event.getItemId === 7 && !state.unlockedFeatures.has("STICKS")) {
        safeUpdateReward(REWARD_IDS.STICKS, "Deku Sticks"); state.unlockedFeatures.add("STICKS"); saveSave();
    }
    if (event.getItemId === 2 && !state.unlockedFeatures.has("NUTS")) {
        safeUpdateReward(REWARD_IDS.NUTS, "Deku Nuts"); state.unlockedFeatures.add("NUTS"); saveSave();
    }
    if (event.getItemId === 5 && !state.unlockedFeatures.has("SEEDS")) {
        safeUpdateReward(REWARD_IDS.SEEDS, "Deku Seeds"); state.unlockedFeatures.add("SEEDS"); saveSave();
    }
    if ((event.getItemId >= 50 && event.getItemId <= 52) && !state.unlockedFeatures.has("BOMBS")) {
        safeUpdateReward(REWARD_IDS.BOMBS, "Bombs"); safeUpdateReward(REWARD_IDS.BOMB_TRAP, "Bomb Trap");
        state.unlockedFeatures.add("BOMBS"); saveSave();
    }
  });

  // Progression Flags
  client.on("flagSet", (event) => {
    if (event.flag === 0xC0 && !state.unlockedFeatures.has("WALLMASTER")) {
         safeUpdateReward(REWARD_IDS.WALLMASTER, "Wallmaster"); state.unlockedFeatures.add("WALLMASTER"); saveSave();
    }
  });
  client.on("disconnected", () => sohClient = undefined);
});

const cooldownMap: Record<string, boolean> = {};
function onCooldown(command: string, sec: number, user: string) {
  if (cooldownMap[command]) return true;
  cooldownMap[command] = true;
  setTimeout(() => cooldownMap[command] = false, sec * 1000);
  return false;
}

(async () => {
  try {
    await loadSave(); 
    console.log("1. Connecting Chat..."); await chatClient.connect(); 
    console.log("2. Connecting EventSub..."); await eventSub.connect();   
    console.log(`3. Authenticated as ${eventSub.username}. Connecting to Game...`);
    // Connect chat as the user found in EventSub
    await chatClient.connect(eventSub.username, OAUTH_TOKEN);
    console.log("4. Starting Sail..."); await sail.start();         
  } catch (error) {
    console.error("Startup Error:", error);
    console.log("Press Enter to close...");
    const buf = new Uint8Array(1024);
    await Deno.stdin.read(buf);
    Deno.exit(1);
  }
})();