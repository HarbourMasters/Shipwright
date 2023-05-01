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

    public override Game Game { get; } = new("Ship of Harkinian", "SoH", "PC", ConnectorType.SimpleTCPServerConnector);

    public override EffectList Effects => new List<Effect>
    {

        // Spawn Enemies and Objects
        new("Cucco Storm", "spawn_cucco_storm") { Category = "Spawn Enemies/Objects" },
        new("Lit Bomb", "spawn_lit_bomb") { Category = "Spawn Enemies/Objects" },
        new("Explosion", "spawn_explosion") { Category = "Spawn Enemies/Objects" },
        new("Arwing", "spawn_arwing") { Category = "Spawn Enemies/Objects" },
        new("Dark Link", "spawn_darklink") { Category = "Spawn Enemies/Objects" },
        new("Iron Knuckle", "spawn_iron_knuckle") { Category = "Spawn Enemies/Objects" },
        new("Stalfos", "spawn_stalfos") { Category = "Spawn Enemies/Objects" },
        new("Freezard", "spawn_freezard") { Category = "Spawn Enemies/Objects" },
        new("Like-Like", "spawn_like_like") { Category = "Spawn Enemies/Objects" },
        new("Gibdo", "spawn_gibdo") { Category = "Spawn Enemies/Objects" },
        new("Flock of Keese (5x)", "spawn_keese") { Category = "Spawn Enemies/Objects" },
        new("Ice Keese", "spawn_ice_keese") { Category = "Spawn Enemies/Objects" },
        new("Fire Keese", "spawn_fire_keese") { Category = "Spawn Enemies/Objects" },
        new("Wolfos", "spawn_wolfos") { Category = "Spawn Enemies/Objects" },
        new("Wallmaster", "spawn_wallmaster") { Category = "Spawn Enemies/Objects" },


        // Link Modifiers
        new("Take Half Damage", "take_half_damage") { Category = "Link Modifiers", Duration = 30 },
        new("Take Double Damage", "take_double_damage") { Category = "Link Modifiers", Duration = 30 },
        new("One-Hit KO", "one_hit_ko") { Category = "Link Modifiers", Duration = 30 },
        new("Invincibility", "invincibility") { Category = "Link Modifiers", Duration = 15 },
        new("Increase Speed", "increase_speed") { Category = "Link Modifiers", Duration = 30 },
        new("Decrease Speed", "decrease_speed") { Category = "Link Modifiers", Duration = 30 },
        new("Low Gravity", "low_gravity") { Category = "Link Modifiers", Duration = 30 },
        new("High Gravity", "high_gravity") { Category = "Link Modifiers", Duration = 30 },
        new("Force Iron Boots", "force_iron_boots") { Category = "Link Modifiers", Duration = 30 },
        new("Force Hover Boots", "force_hover_boots") { Category = "Link Modifiers", Duration = 30 },
        new("Slippery Floor", "slippery_floor") { Category = "Link Modifiers", Duration = 30 },
        new("Disable Ledge Grabs", "no_ledge_grabs") { Category = "Link Modifiers", Duration = 30 },
        new("Random Wind", "random_wind") { Category = "Link Modifiers", Duration = 30 },
        new("Random Bonks When Rolling", "random_bonks") { Category = "Link Modifiers", Duration = 60 },


        // Hurt or Heal Link
        new("Empty Heart", "empty_heart") { Category = "Hurt/Heal Link", Quantity = 20 },
        new("Fill Heart", "fill_heart") { Category = "Hurt/Heal Link", Quantity = 20 },
        new("Knockback Link (Weak)", "knockback_link_weak") { Category = "Hurt/Heal Link" },
        new("Knockback Link (Strong)", "knockback_link_strong") { Category = "Hurt/Heal Link" },
        new("Knockback Link (Mega)", "knockback_link_mega") { Category = "Hurt/Heal Link" },
        new("Burn Link", "burn_link") { Category = "Hurt/Heal Link" },
        new("Freeze Link", "freeze_link") { Category = "Hurt/Heal Link" },
        new("Electrocute Link", "electrocute_link") { Category = "Hurt/Heal Link" },
        new("Kill Link", "kill_link") { Category = "Hurt/Heal Link" },


        // Give Items and Consumables
        new("Add Heart Container", "add_heart_container") { Category = "Give Items/Consumables" },
        new("Fill Magic", "fill_magic") { Category = "Give Items/Consumables" },
        new("Give Rupees", "add_rupees") { Category = "Give Items/Consumables", Quantity = 999 },
        new("Give Deku Shield", "give_deku_shield") { Category = "Give Items/Consumables" },
        new("Give Hylian Shield", "give_hylian_shield") { Category = "Give Items/Consumables" },
        new("Refill Deku Sticks", "refill_sticks") { Category = "Give Items/Consumables", Quantity = 30 },
        new("Refill Deku Nuts", "refill_nuts") { Category = "Give Items/Consumables", Quantity = 30 },
        new("Refill Bombs", "refill_bombs") { Category = "Give Items/Consumables", Quantity = 30 },
        new("Refill Slingshot Seeds", "refill_seeds") { Category = "Give Items/Consumables", Quantity = 30 },
        new("Refill Arrows", "refill_arrows") { Category = "Give Items/Consumables" },
        new("Refill Bombchus", "refill_bombchus") { Category = "Give Items/Consumables", Quantity = 30 },


        // Take Items and Consumables
        new("Remove Heart Container", "remove_heart_container") { Category = "Take Items/Consumables" },
        new("Empty Magic", "empty_magic") { Category = "Take Items/Consumables" },
        new("Take Rupees", "remove_rupees") { Category = "Take Items/Consumables", Quantity = 999 },
        new("Take Deku Shield", "take_deku_shield") { Category = "Take Items/Consumables" },
        new("Take Hylian Shield", "take_hylian_shield") { Category = "Take Items/Consumables" },
        new("Take Deku Sticks", "take_sticks") { Category = "Take Items/Consumables", Quantity = 30 },
        new("Take Deku Nuts", "take_nuts") { Category = "Take Items/Consumables", Quantity = 30 },
        new("Take Bombs", "take_bombs") { Category = "Take Items/Consumables", Quantity = 30 },
        new("Take Slingshot Seeds", "take_seeds") { Category = "Take Items/Consumables", Quantity = 30 },
        new("Take Arrows", "take_arrows") { Category = "Take Items/Consumables", Quantity = 30 },
        new("Take Bombchus", "take_bombchus") { Category = "Take Items/Consumables", Quantity = 30 },


        // Link Size Modifiers
        new("Giant Lonk", "giant_link") { Category = "Change Link's Size", Duration = 30 },
        new("Minish Link", "minish_link") { Category = "Change Link's Size", Duration = 30 },
        new("Paper Link", "paper_link") { Category = "Change Link's Size", Duration = 30 },
        new("Squished Link", "squished_link") { Category = "Change Link's Size", Duration = 30 },
        new("Invisible Link", "invisible_link") { Category = "Change Link's Size", Duration = 30 },


        // Generic Effects
        new("Random Bomb Fuse Timer", "random_bomb_timer") { Category = "Generic Effects", Duration = 60 },
        new("Set Time to Dawn", "set_time_to_dawn") { Category = "Generic Effects", },
        new("Set Time to Dusk", "set_time_to_dusk") { Category = "Generic Effects", },


        // Visual Effects
        new("No UI", "no_ui") { Category = "Visual Effects", Duration = 60 },
        new("Rainstorm", "rainstorm") { Category = "Visual Effects", Duration = 30 },
        new("Debug Mode", "debug_mode") { Category = "Visual Effects", Duration = 30 },
        new("Randomize Cosmetics", "random_cosmetics") { Category = "Visual Effects", Duration = 30 },


        // Controls
        new("No Z Button", "no_z_button") { Category = "Controls", Duration = 30 },
        new("Reverse Controls", "reverse_controls") { Category = "Controls", Duration = 60 },
        new("Pacifist Mode", "pacifist_mode") { Category = "Controls", Duration = 15 },
        new("Press Random Buttons", "press_random_buttons") { Category = "Controls", Duration = 30 },
        new("Clear C-Buttons", "clear_cbuttons") { Category = "Controls" },
        new("Clear D-pad", "clear_dpad") { Category = "Controls" },


        // Teleport Player
        new("Link's House", "tp_links_house") { Category = "Teleport Player" },
        new("Minuet Destination", "tp_minuet") { Category = "Teleport Player" },
        new("Bolero Destination", "tp_bolero") { Category = "Teleport Player" },
        new("Serenade Destination", "tp_serenade") { Category = "Teleport Player" },
        new("Requiem Destination", "tp_requiem") { Category = "Teleport Player" },
        new("Nocturne Destination", "tp_nocturne") { Category = "Teleport Player" },
        new("Prelude Destination", "tp_prelude") { Category = "Teleport Player" },


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
