using System;
using System.Collections.Generic;
using System.Linq;
using CrowdControl.Common;
using CrowdControl.Games.Packs;
using ConnectorType = CrowdControl.Common.ConnectorType;

public class SoH : SimpleTCPPack
{
    public override string Host { get; } = "127.0.0.1";

    public override ushort Port { get; } = 43384;

    public SoH(IPlayer player, Func<CrowdControlBlock, bool> responseHandler, Action<object> statusUpdateHandler) : base(player, responseHandler, statusUpdateHandler) { }

    public override Game Game { get; } = new Game(90, "Ship of Harkinian", "SoH", "PC", ConnectorType.SimpleTCPConnector);

	public override List<Effect> Effects => new List<Effect>
	{

        // Spawn Enemies and Objects
        new Effect("Spawn Enemies/Objects", "spawn_stuff", ItemKind.Folder),

        new Effect ("Cucco Storm", "spawn_cucco_storm", "spawn_stuff"),
        new Effect ("Lit Bomb", "spawn_lit_bomb", "spawn_stuff"),
        new Effect ("Explosion", "spawn_explosion", "spawn_stuff"),
        new Effect ("Arwing", "spawn_arwing", "spawn_stuff"),
        new Effect ("Dark Link", "spawn_darklink", "spawn_stuff"),
        new Effect ("Iron Knuckle", "spawn_iron_knuckle", "spawn_stuff"),
        new Effect ("Stalfos", "spawn_stalfos", "spawn_stuff"),
        new Effect ("Freezard", "spawn_freezard", "spawn_stuff"),
        new Effect ("Like-Like", "spawn_like_like", "spawn_stuff"),
        new Effect ("Gibdo", "spawn_gibdo", "spawn_stuff"),
        new Effect ("Flock of Keese (5x)", "spawn_keese", "spawn_stuff"),
        new Effect ("Ice Keese", "spawn_ice_keese", "spawn_stuff"),
        new Effect ("Fire Keese", "spawn_fire_keese", "spawn_stuff"),
        new Effect ("Wolfos", "spawn_wolfos", "spawn_stuff"),
        new Effect ("Wallmaster", "spawn_wallmaster", "spawn_stuff"),


        // Link Modifiers
        new Effect("Link Modifiers", "link_modifiers", ItemKind.Folder),

        new Effect("Take Half Damage", "take_half_damage", "link_modifiers") { Duration = 30 },
        new Effect("Take Double Damage", "take_double_damage", "link_modifiers") { Duration = 30 },
        new Effect("One-Hit KO", "one_hit_ko", "link_modifiers") { Duration = 30 },
        new Effect("Invincibility", "invincibility", "link_modifiers") { Duration = 15 },
        new Effect("Increase Speed", "increase_speed", "link_modifiers") { Duration = 30 },
        new Effect("Decrease Speed", "decrease_speed", "link_modifiers") { Duration = 30 },
        new Effect("Low Gravity", "low_gravity", "link_modifiers") { Duration = 30 },
        new Effect("High Gravity", "high_gravity", "link_modifiers") { Duration = 30 },
        new Effect("Force Iron Boots", "force_iron_boots", "link_modifiers") { Duration = 30 },
        new Effect("Force Hover Boots", "force_hover_boots", "link_modifiers") { Duration = 30 },
        new Effect("Slippery Floor", "slippery_floor", "link_modifiers") { Duration = 30 },
        new Effect("Disable Ledge Grabs", "no_ledge_grabs", "link_modifiers") { Duration = 30 },
        new Effect("Random Wind", "random_wind", "link_modifiers") { Duration = 30 },
        new Effect("Random Bonks When Rolling", "random_bonks", "link_modifiers") { Duration = 60 },


        // Hurt or Heal Link
        new Effect("Hurt/Heal Link", "hurtheal_link", ItemKind.Folder),

        new Effect("Empty Heart", "empty_heart", new[] { "health20" }, "hurtheal_link"),
        new Effect("Fill Heart", "fill_heart", new[] { "health20" }, "hurtheal_link"),
        new Effect("Knockback Link (Weak)", "knockback_link_weak", "hurtheal_link"),
        new Effect("Knockback Link (Strong)", "knockback_link_strong", "hurtheal_link"),
        new Effect("Knockback Link (Mega)", "knockback_link_mega", "hurtheal_link"),
        new Effect("Burn Link", "burn_link", "hurtheal_link"),
        new Effect("Freeze Link", "freeze_link", "hurtheal_link"),
        new Effect("Electrocute Link", "electrocute_link", "hurtheal_link"),
        new Effect("Kill Link", "kill_link", "hurtheal_link"),


        // Give Items and Consumables
        new Effect("Give Items/Consumables", "give_item", ItemKind.Folder),

        new Effect("Add Heart Container", "add_heart_container", "give_item"),
        new Effect("Fill Magic", "fill_magic", "give_item"),
        new Effect("Give Rupees", "add_rupees", new[] { "rupees999" }, "give_item"),
        new Effect("Give Deku Shield", "give_deku_shield", "give_item"),
        new Effect("Give Hylian Shield", "give_hylian_shield", "give_item"),
        new Effect("Refill Deku Sticks", "refill_sticks", new[] { "ammo30" }, "give_item"),
        new Effect("Refill Deku Nuts", "refill_nuts", new[] { "ammo30" }, "give_item"),
        new Effect("Refill Bombs", "refill_bombs", new[] { "ammo30" }, "give_item"),
        new Effect("Refill Slingshot Seeds", "refill_seeds", new[] { "ammo30" }, "give_item"),
        new Effect("Refill Arrows", "refill_arrows", new[] { "ammo30" }, "give_item"),
        new Effect("Refill Bombchus", "refill_bombchus", new[] { "ammo30" }, "give_item"),


        // Take Items and Consumables
        new Effect("Take Items/Consumables", "take_item", ItemKind.Folder),

        new Effect("Remove Heart Container", "remove_heart_container", "take_item"),
        new Effect("Empty Magic", "empty_magic", "take_item"),
        new Effect("Take Rupees", "remove_rupees", new[] { "rupees999" }, "take_item"),
        new Effect("Take Deku Shield", "take_deku_shield", "take_item"),
        new Effect("Take Hylian Shield", "take_hylian_shield", "take_item"),
        new Effect("Take Deku Sticks", "take_sticks", new[] { "ammo30" }, "take_item"),
        new Effect("Take Deku Nuts", "take_nuts", new[] { "ammo30" }, "take_item"),
        new Effect("Take Bombs", "take_bombs", new[] { "ammo30" }, "take_item"),
        new Effect("Take Slingshot Seeds", "take_seeds", new[] { "ammo30" }, "take_item"),
        new Effect("Take Arrows", "take_arrows", new[] { "ammo30" }, "take_item"),
        new Effect("Take Bombchus", "take_bombchus", new[] { "ammo30" }, "take_item"),


        // Link Size Modifiers
        new Effect("Change Link's Size", "link_size", ItemKind.Folder),

        new Effect("Giant Lonk", "giant_link", "link_size") { Duration = 30 },
        new Effect("Minish Link", "minish_link", "link_size") { Duration = 30 },
        new Effect("Paper Link", "paper_link", "link_size") { Duration = 30 },
        new Effect("Squished Link", "squished_link", "link_size") { Duration = 30 },
        new Effect("Invisible Link", "invisible_link", "link_size") { Duration = 30 },


        // Generic Effects
        new Effect("Generic Effects", "generic_effects", ItemKind.Folder),

        new Effect("Random Bomb Fuse Timer", "random_bomb_timer", "generic_effects") { Duration = 60 },
        new Effect("Set Time to Dawn", "set_time_to_dawn", "generic_effects"),
        new Effect("Set Time to Dusk", "set_time_to_dusk", "generic_effects"),


        // Visual Effects
        new Effect("Visual Effects", "visual_effects", ItemKind.Folder),

        new Effect("No UI", "no_ui", "visual_effects") { Duration = 60 },
        new Effect("Rainstorm", "rainstorm", "visual_effects") { Duration = 30 },
        new Effect("Debug Mode", "debug_mode", "visual_effects") { Duration = 30 },
        new Effect("Randomize Cosmetics", "random_cosmetics", "visual_effects") { Duration = 30 },


        // Controls
        new Effect("Controls", "controls", ItemKind.Folder),

        new Effect("No Z Button", "no_z_button", "controls") { Duration = 30 },
        new Effect("Reverse Controls", "reverse_controls", "controls") { Duration = 60 },
        new Effect("Pacifist Mode", "pacifist_mode", "controls") { Duration = 15 },
        new Effect("Press Random Buttons", "press_random_buttons", "controls") { Duration = 30 },
        new Effect("Clear C-Buttons", "clear_cbuttons", "controls"),
        new Effect("Clear D-pad", "clear_dpad", "controls"),


        // Teleport Player
        new Effect("Teleport Player", "teleport", ItemKind.Folder),

        new Effect("Link's House", "tp_links_house", "teleport"),
        new Effect("Minuet Destination", "tp_minuet", "teleport"),
        new Effect("Bolero Destination", "tp_bolero", "teleport"),
        new Effect("Serenade Destination", "tp_serenade", "teleport"),
        new Effect("Requiem Destination", "tp_requiem", "teleport"),
        new Effect("Nocturne Destination", "tp_nocturne", "teleport"),
        new Effect("Prelude Destination", "tp_prelude", "teleport"),


        // Tunic Color (Bidding War)
        new Effect("Tunic Color", "color_tunic", ItemKind.BidWar),

        new Effect("Red", "tunic_red", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Green", "tunic_green", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Blue", "tunic_blue", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Orange", "tunic_orange", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Yellow", "tunic_yellow", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Purple", "tunic_purple", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Pink", "tunic_pink", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Brown", "tunic_brown", ItemKind.BidWarValue, "color_tunic"),
        new Effect("Black", "tunic_black", ItemKind.BidWarValue, "color_tunic"),


        // Navi Color (Bidding War)
        new Effect("Navi Color", "color_navi", ItemKind.BidWar),

        new Effect("Red", "navi_red", ItemKind.BidWarValue, "color_navi"),
        new Effect("Green", "navi_green", ItemKind.BidWarValue, "color_navi"),
        new Effect("Blue", "navi_blue", ItemKind.BidWarValue, "color_navi"),
        new Effect("Orange", "navi_orange", ItemKind.BidWarValue, "color_navi"),
        new Effect("Yellow", "navi_yellow", ItemKind.BidWarValue, "color_navi"),
        new Effect("Purple", "navi_purple", ItemKind.BidWarValue, "color_navi"),
        new Effect("Pink", "navi_pink", ItemKind.BidWarValue, "color_navi"),
        new Effect("Brown", "navi_brown", ItemKind.BidWarValue, "color_navi"),
        new Effect("Black", "navi_black", ItemKind.BidWarValue, "color_navi"),


        // Link's Hair Color (Bidding War)
        new Effect("Link's Hair Color", "color_hair", ItemKind.BidWar),

        new Effect("Red", "hair_red", ItemKind.BidWarValue, "color_hair"),
        new Effect("Green", "hair_green", ItemKind.BidWarValue, "color_hair"),
        new Effect("Blue", "hair_blue", ItemKind.BidWarValue, "color_hair"),
        new Effect("Orange", "hair_orange", ItemKind.BidWarValue, "color_hair"),
        new Effect("Yellow", "hair_yellow", ItemKind.BidWarValue, "color_hair"),
        new Effect("Purple", "hair_purple", ItemKind.BidWarValue, "color_hair"),
        new Effect("Pink", "hair_pink", ItemKind.BidWarValue, "color_hair"),
        new Effect("Brown", "hair_brown", ItemKind.BidWarValue, "color_hair"),
        new Effect("Black", "hair_black", ItemKind.BidWarValue, "color_hair"),

    };

	//Slider ranges need to be defined
    public override List<ItemType> ItemTypes => new List<ItemType>
    {
        new ItemType("Rupees", "rupees999", ItemType.Subtype.Slider, "{\"min\":1,\"max\":999}"),
        new ItemType("Health", "health20", ItemType.Subtype.Slider, "{\"min\":1,\"max\":20}"),
        new ItemType("Ammo", "ammo30", ItemType.Subtype.Slider, "{\"min\":1,\"max\":30}"),
    };
	
}
