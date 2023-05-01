using System;
using System.Collections.Generic;
using CrowdControl.Common;
using ConnectorType = CrowdControl.Common.ConnectorType;

namespace CrowdControl.Games.Packs;

public class ShipOfHarkinian : SimpleTCPPack
{
    public override string Host => "127.0.0.1";

    public override ushort Port => 43384;

    public override ISimpleTCPPack.MessageFormat MessageFormat => ISimpleTCPPack.MessageFormat.CrowdControlLegacy;

    public ShipOfHarkinian(UserRecord player, Func<CrowdControlBlock, bool> responseHandler, Action<object> statusUpdateHandler) : base(player, responseHandler, statusUpdateHandler) { }

    public override Game Game { get; } = new("Ship of Harkinian", "ShipOfHarkinian", "PC", ConnectorType.SimpleTCPServerConnector);

    public override EffectList Effects => new List<Effect>
    {

        // Spawn Enemies and Objects
        new("Cucco Storm", "spawn_cucco_storm") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Lit Bomb", "spawn_lit_bomb") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Explosion", "spawn_explosion") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Arwing", "spawn_arwing") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Dark Link", "spawn_darklink") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Iron Knuckle", "spawn_iron_knuckle") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Stalfos", "spawn_stalfos") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Freezard", "spawn_freezard") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Like-Like", "spawn_like_like") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Gibdo", "spawn_gibdo") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Flock of Keese (5x)", "spawn_keese") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Ice Keese", "spawn_ice_keese") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Fire Keese", "spawn_fire_keese") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Wolfos", "spawn_wolfos") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },
        new("Wallmaster", "spawn_wallmaster") { Category = "Spawn Enemies/Objects", Price = 0, Description = "" },


        // Link Modifiers
        new("Take Half Damage", "take_half_damage") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Take Double Damage", "take_double_damage") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("One-Hit KO", "one_hit_ko") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Invincibility", "invincibility") { Category = "Link Modifiers", Duration = 15, Price = 0, Description = "" },
        new("Increase Speed", "increase_speed") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Decrease Speed", "decrease_speed") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Low Gravity", "low_gravity") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("High Gravity", "high_gravity") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Force Iron Boots", "force_iron_boots") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Force Hover Boots", "force_hover_boots") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Slippery Floor", "slippery_floor") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Disable Ledge Grabs", "no_ledge_grabs") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Random Wind", "random_wind") { Category = "Link Modifiers", Duration = 30, Price = 0, Description = "" },
        new("Random Bonks When Rolling", "random_bonks") { Category = "Link Modifiers", Duration = 60, Price = 0, Description = "" },


        // Hurt or Heal Link
        new("Empty Heart", "empty_heart") { Category = "Hurt/Heal Link", Quantity = 20, Price = 0, Description = "" },
        new("Fill Heart", "fill_heart") { Category = "Hurt/Heal Link", Quantity = 20, Price = 0, Description = "" },
        new("Knockback Link (Weak)", "knockback_link_weak") { Category = "Hurt/Heal Link", Price = 0, Description = "" },
        new("Knockback Link (Strong)", "knockback_link_strong") { Category = "Hurt/Heal Link", Price = 0, Description = "" },
        new("Knockback Link (Mega)", "knockback_link_mega") { Category = "Hurt/Heal Link", Price = 0, Description = "" },
        new("Burn Link", "burn_link") { Category = "Hurt/Heal Link", Price = 0, Description = "" },
        new("Freeze Link", "freeze_link") { Category = "Hurt/Heal Link", Price = 0, Description = "" },
        new("Electrocute Link", "electrocute_link") { Category = "Hurt/Heal Link", Price = 0, Description = "" },
        new("Kill Link", "kill_link") { Category = "Hurt/Heal Link", Price = 0, Description = "" },


        // Give Items and Consumables
        new("Add Heart Container", "add_heart_container") { Category = "Give Items/Consumables", Price = 0, Description = "" },
        new("Fill Magic", "fill_magic") { Category = "Give Items/Consumables", Price = 0, Description = "" },
        new("Give Rupees", "add_rupees") { Category = "Give Items/Consumables", Quantity = 999, Price = 0, Description = "" },
        new("Give Deku Shield", "give_deku_shield") { Category = "Give Items/Consumables", Price = 0, Description = "" },
        new("Give Hylian Shield", "give_hylian_shield") { Category = "Give Items/Consumables", Price = 0, Description = "" },
        new("Refill Deku Sticks", "refill_sticks") { Category = "Give Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Refill Deku Nuts", "refill_nuts") { Category = "Give Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Refill Bombs", "refill_bombs") { Category = "Give Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Refill Slingshot Seeds", "refill_seeds") { Category = "Give Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Refill Arrows", "refill_arrows") { Category = "Give Items/Consumables", Price = 0, Description = "" },
        new("Refill Bombchus", "refill_bombchus") { Category = "Give Items/Consumables", Quantity = 30, Price = 0, Description = "" },


        // Take Items and Consumables
        new("Remove Heart Container", "remove_heart_container") { Category = "Take Items/Consumables", Price = 0, Description = "" },
        new("Empty Magic", "empty_magic") { Category = "Take Items/Consumables", Price = 0, Description = "" },
        new("Take Rupees", "remove_rupees") { Category = "Take Items/Consumables", Quantity = 999, Price = 0, Description = "" },
        new("Take Deku Shield", "take_deku_shield") { Category = "Take Items/Consumables", Price = 0, Description = "" },
        new("Take Hylian Shield", "take_hylian_shield") { Category = "Take Items/Consumables", Price = 0, Description = "" },
        new("Take Deku Sticks", "take_sticks") { Category = "Take Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Take Deku Nuts", "take_nuts") { Category = "Take Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Take Bombs", "take_bombs") { Category = "Take Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Take Slingshot Seeds", "take_seeds") { Category = "Take Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Take Arrows", "take_arrows") { Category = "Take Items/Consumables", Quantity = 30, Price = 0, Description = "" },
        new("Take Bombchus", "take_bombchus") { Category = "Take Items/Consumables", Quantity = 30, Price = 0, Description = "" },


        // Link Size Modifiers
        new("Giant Lonk", "giant_link") { Category = "Change Link's Size", Duration = 30, Price = 0, Description = "" },
        new("Minish Link", "minish_link") { Category = "Change Link's Size", Duration = 30, Price = 0, Description = "" },
        new("Paper Link", "paper_link") { Category = "Change Link's Size", Duration = 30, Price = 0, Description = "" },
        new("Squished Link", "squished_link") { Category = "Change Link's Size", Duration = 30, Price = 0, Description = "" },
        new("Invisible Link", "invisible_link") { Category = "Change Link's Size", Duration = 30, Price = 0, Description = "" },


        // Generic Effects
        new("Random Bomb Fuse Timer", "random_bomb_timer") { Category = "Generic Effects", Duration = 60, Price = 0, Description = "" },
        new("Set Time to Dawn", "set_time_to_dawn") { Category = "Generic Effects", Price = 0, Description = "" },
        new("Set Time to Dusk", "set_time_to_dusk") { Category = "Generic Effects", Price = 0, Description = "" },


        // Visual Effects
        new("No UI", "no_ui") { Category = "Visual Effects", Duration = 60, Price = 0, Description = "" },
        new("Rainstorm", "rainstorm") { Category = "Visual Effects", Duration = 30, Price = 0, Description = "" },
        new("Debug Mode", "debug_mode") { Category = "Visual Effects", Duration = 30, Price = 0, Description = "" },
        new("Randomize Cosmetics", "random_cosmetics") { Category = "Visual Effects", Duration = 30, Price = 0, Description = "" },


        // Controls
        new("No Z Button", "no_z_button") { Category = "Controls", Duration = 30, Price = 0, Description = "" },
        new("Reverse Controls", "reverse_controls") { Category = "Controls", Duration = 60, Price = 0, Description = "" },
        new("Pacifist Mode", "pacifist_mode") { Category = "Controls", Duration = 15, Price = 0, Description = "" },
        new("Press Random Buttons", "press_random_buttons") { Category = "Controls", Duration = 30, Price = 0, Description = "" },
        new("Clear C-Buttons", "clear_cbuttons") { Category = "Controls", Price = 0, Description = "" },
        new("Clear D-pad", "clear_dpad") { Category = "Controls", Price = 0, Description = "" },


        // Teleport Player
        new("Link's House", "tp_links_house") { Category = "Teleport Player", Price = 0, Description = "" },
        new("Minuet Destination", "tp_minuet") { Category = "Teleport Player", Price = 0, Description = "" },
        new("Bolero Destination", "tp_bolero") { Category = "Teleport Player", Price = 0, Description = "" },
        new("Serenade Destination", "tp_serenade") { Category = "Teleport Player", Price = 0, Description = "" },
        new("Requiem Destination", "tp_requiem") { Category = "Teleport Player", Price = 0, Description = "" },
        new("Nocturne Destination", "tp_nocturne") { Category = "Teleport Player", Price = 0, Description = "" },
        new("Prelude Destination", "tp_prelude") { Category = "Teleport Player", Price = 0, Description = "" },


        // Tunic Color (Bidding War)
        new("Tunic Color", "tunic", ItemKind.BidWar)
        {
            Parameters = new ParameterDef("Color", "color_tunic_param",
                new("Red", "red"),
                new("Green", "green"),
                new("Blue", "blue"),
                new("Orange", "orange"),
                new("Yellow", "yellow"),
                new("Purple", "purple"),
                new("Pink", "pink"),
                new("Brown", "brown"),
                new("Black", "black"))
        },


        // Navi Color (Bidding War)
        new("Navi Color", "navi", ItemKind.BidWar)
        {
            Parameters = new ParameterDef("Color", "color_navi_param",
                new("Red", "red"),
                new("Green", "green"),
                new("Blue", "blue"),
                new("Orange", "orange"),
                new("Yellow", "yellow"),
                new("Purple", "purple"),
                new("Pink", "pink"),
                new("Brown", "brown"),
                new("Black", "black"))
        },


        // Link's Hair Color (Bidding War)
        new("Link's Hair Color", "hair", ItemKind.BidWar)
        {
            Parameters = new ParameterDef("Color", "color_hair_param",
                new("Red", "red"),
                new("Green", "green"),
                new("Blue", "blue"),
                new("Orange", "orange"),
                new("Yellow", "yellow"),
                new("Purple", "purple"),
                new("Pink", "pink"),
                new("Brown", "brown"),
                new("Black", "black"))
        }
    };
}
