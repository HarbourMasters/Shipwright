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

    public override ISimpleTCPPack.QuantityFormat QuantityFormat => ISimpleTCPPack.QuantityFormat.ParameterAndField;

    public ShipOfHarkinian(UserRecord player, Func<CrowdControlBlock, bool> responseHandler, Action<object> statusUpdateHandler) : base(player, responseHandler, statusUpdateHandler) { }

    public override Game Game { get; } = new("Ship of Harkinian", "ShipOfHarkinian", "PC", ConnectorType.SimpleTCPServerConnector);

    public override EffectList Effects => new List<Effect>
    {

        // Spawn Enemies and Objects
        new("Cucco Storm", "spawn_cucco_storm") { Category = "Spawn Enemies/Objects", Price = 35, Description = "Spawns an angry cucco that summons his friends to attack the poor streamer." },
        new("Lit Bomb", "spawn_lit_bomb") { Category = "Spawn Enemies/Objects", Price = 15, Description = "Spawns a bomb that is already lit." },
        new("Explosion", "spawn_explosion") { Category = "Spawn Enemies/Objects", Price = 20, Description = "Turns Link into a fireworks display by spawning an explosion on him." },
        new("Arwing", "spawn_arwing") { Category = "Spawn Enemies/Objects", Price = 25, Description = "FOX, GET THAT THING OFF ME!" },
        new("Dark Link", "spawn_darklink") { Category = "Spawn Enemies/Objects", Price = 50, Description = "*insert 2 Spiderman pointing at each other*. Only one Dark Link can exist at a time." },
        new("Iron Knuckle", "spawn_iron_knuckle") { Category = "Spawn Enemies/Objects", Price = 50, Description = "monkaS" },
        new("Stalfos", "spawn_stalfos") { Category = "Spawn Enemies/Objects", Price = 35, Description = "They're actually pretty dangerous when they aren't on a boat." },
        new("Freezard", "spawn_freezard") { Category = "Spawn Enemies/Objects", Price = 20, Description = "Cold Pun." },
        new("Like-Like", "spawn_like_like") { Category = "Spawn Enemies/Objects", Price = 30, Description = "Do you like Like-Likes? Or do you like-like Like-Likes? Try pairing it with a Dark Link :)" },
        new("Gibdo", "spawn_gibdo") { Category = "Spawn Enemies/Objects", Price = 25, Description = "*SCREAM*" },
        new("Flock of Keese (5x)", "spawn_keese") { Category = "Spawn Enemies/Objects", Price = 30, Description = "Release the bees! I mean Keese." },
        new("Ice Keese", "spawn_ice_keese") { Category = "Spawn Enemies/Objects", Price = 15, Description = "These things are seriously annoying." },
        new("Fire Keese", "spawn_fire_keese") { Category = "Spawn Enemies/Objects", Price = 15, Description = "Do not bring close to Deku Shields. Or do, we're not your parents." },
        new("Wolfos", "spawn_wolfos") { Category = "Spawn Enemies/Objects", Price = 15, Description = "Awooooooo" },
        new("Wallmaster", "spawn_wallmaster") { Category = "Spawn Enemies/Objects", Price = 30, Description = "Need a hand? Try it together with electrocuting Link." },


        // Link Modifiers
        new("Take Half Damage", "take_half_damage") { Category = "Link Modifiers", Duration = 30, Price = 20, Description = "Halves the damage Link takes. Won't save the streamer if they're bad at the game though." },
        new("Take Double Damage", "take_double_damage") { Category = "Link Modifiers", Duration = 30, Price = 30, Description = "Doubles the damage Link takes. Turns OoT into Dark Souls.. kinda." },
        new("One-Hit KO", "one_hit_ko") { Category = "Link Modifiers", Duration = 30, Price = 150, Description = "Everything that takes health away kills Link. EVERYTHING." },
        new("Invincibility", "invincibility") { Category = "Link Modifiers", Duration = 15, Price = 25, Description = "Turn Link into a super hero, but his only superpower is not dieing." },
        new("Increase Speed", "increase_speed") { Category = "Link Modifiers", Duration = 30, Price = 20, Description = "Give Link the zoomies!" },
        new("Decrease Speed", "decrease_speed") { Category = "Link Modifiers", Duration = 30, Price = 25, Description = "Instead of a slow clap, how about a slow walk?" },
        new("Low Gravity", "low_gravity") { Category = "Link Modifiers", Duration = 30, Price = 25, Description = "I beliiiiieve I can flyyyyy." },
        new("High Gravity", "high_gravity") { Category = "Link Modifiers", Duration = 30, Price = 20, Description = "Initiate 50 times earth gravity training simulator (it's really only twice or so)." },
        new("Force Iron Boots", "force_iron_boots") { Category = "Link Modifiers", Duration = 30, Price = 30, Description = "WARNING: Danger of drowning." },
        new("Force Hover Boots", "force_hover_boots") { Category = "Link Modifiers", Duration = 30, Price = 25, Description = "Force Link to hover their way to victory for a while." },
        new("Slippery Floor", "slippery_floor") { Category = "Link Modifiers", Duration = 30, Price = 15, Description = "The ground is made of ice. Not really, but it sure feels like it." },
        new("Disable Ledge Grabs", "no_ledge_grabs") { Category = "Link Modifiers", Duration = 30, Price = 15, Description = "Jump! Pull up-- Oh, woops." },
        new("Random Wind", "random_wind") { Category = "Link Modifiers", Duration = 30, Price = 30, Description = "A wind blows, which changes to a random direction every 5 seconds." },
        new("Random Bonks When Rolling", "random_bonks") { Category = "Link Modifiers", Duration = 60, Price = 15, Description = "Why do I keep bonking while there's nothing here!?" },


        // Hurt or Heal Link
        new("Empty Heart", "empty_heart") { Category = "Hurt/Heal Link", Quantity = 20, Price = 10, Description = "Damage Link for however many hearts you choose." },
        new("Fill Heart", "fill_heart") { Category = "Hurt/Heal Link", Quantity = 20, Price = 5, Description = "Heal Link for however many hearts you choose." },
        new("Knockback Link (Weak)", "knockback_link_weak") { Category = "Hurt/Heal Link", Price = 15, Description = "Pull the rug under Link's feet." },
        new("Knockback Link (Strong)", "knockback_link_strong") { Category = "Hurt/Heal Link", Price = 30, Description = "Pull the 2 rugs under Link's feet." },
        new("Knockback Link (Mega)", "knockback_link_mega") { Category = "Hurt/Heal Link", Price = 45, Description = "Pull the maximum number of rugs under Link's feet." },
        new("Burn Link", "burn_link") { Category = "Hurt/Heal Link", Price = 30, Description = "Burn, baby, burn. Deku Shields don't like this one." },
        new("Freeze Link", "freeze_link") { Category = "Hurt/Heal Link", Price = 20, Description = "FREEZE! Don't move!" },
        new("Electrocute Link", "electrocute_link") { Category = "Hurt/Heal Link", Price = 20, Description = "Ganon used Thundershock!" },
        new("Kill Link", "kill_link") { Category = "Hurt/Heal Link", Price = 150, Description = "Rest in RIP." },


        // Give Items and Consumables
        new("Add Heart Container", "add_heart_container") { Category = "Give Items/Consumables", Price = 25, Description = "The limit is 20 heart containers." },
        new("Fill Magic", "fill_magic") { Category = "Give Items/Consumables", Price = 20, Description = "Fills the entire magic bar. Abra-kadabra." },
        new("Give Rupees", "add_rupees") { Category = "Give Items/Consumables", Quantity = 999, Price = 1, Description = "Turn your CC coins into a different type of currency. Magic Armor not included." },
        new("Give Deku Shield", "give_deku_shield") { Category = "Give Items/Consumables", Price = 20, Description = "Grants the player a Deku Shield and equips it if they don't have a shield equipped yet." },
        new("Give Hylian Shield", "give_hylian_shield") { Category = "Give Items/Consumables", Price = 20, Description = "Grants the player a Hylian Shield and equips it if they don't have a shield equipped yet." },
        new("Refill Deku Sticks", "refill_sticks") { Category = "Give Items/Consumables", Quantity = 30, Price = 3, Description = "Only works when the player already had Deku Sticks before." },
        new("Refill Deku Nuts", "refill_nuts") { Category = "Give Items/Consumables", Quantity = 30, Price = 3, Description = "Only works when the player already had Deku Nuts before." },
        new("Refill Bombs", "refill_bombs") { Category = "Give Items/Consumables", Quantity = 30, Price = 5, Description = "Only works when the player already has a bomb bag." },
        new("Refill Slingshot Seeds", "refill_seeds") { Category = "Give Items/Consumables", Quantity = 30, Price = 1, Description = "Only works when the player already has a Slingshot." },
        new("Refill Arrows", "refill_arrows") { Category = "Give Items/Consumables", Price = 1, Description = "Only works when the player already has a bow." },
        new("Refill Bombchus", "refill_bombchus") { Category = "Give Items/Consumables", Quantity = 30, Price = 5, Description = "Only works when the player already had Bombchus before." },


        // Take Items and Consumables
        new("Remove Heart Container", "remove_heart_container") { Category = "Take Items/Consumables", Price = 35, Description = "The limit is 1 heart. But you wouldn't go that far, would you?" },
        new("Empty Magic", "empty_magic") { Category = "Take Items/Consumables", Price = 25, Description = "Empties the entire magic bar. They didn't need it anyway." },
        new("Take Rupees", "remove_rupees") { Category = "Take Items/Consumables", Quantity = 999, Price = 2, Description = "They say money doesn't make you happy, so you might as well take it away." },
        new("Take Deku Shield", "take_deku_shield") { Category = "Take Items/Consumables", Price = 30, Description = "Take away the Deku Shield if the player has one. Shielding is for cowards anyway." },
        new("Take Hylian Shield", "take_hylian_shield") { Category = "Take Items/Consumables", Price = 30, Description = "Take away the Hylian Shield if the player has one. Shielding is for cowards anyway." },
        new("Take Deku Sticks", "take_sticks") { Category = "Take Items/Consumables", Quantity = 30, Price = 5, Description = "Take away from breakable Master Sticks." },
        new("Take Deku Nuts", "take_nuts") { Category = "Take Items/Consumables", Quantity = 30, Price = 5, Description = "You want to take these away? That's nuts." },
        new("Take Bombs", "take_bombs") { Category = "Take Items/Consumables", Quantity = 30, Price = 10, Description = "Good luck with that boulder!" },
        new("Take Slingshot Seeds", "take_seeds") { Category = "Take Items/Consumables", Quantity = 30, Price = 2, Description = "Who even uses the Slingshot?" },
        new("Take Arrows", "take_arrows") { Category = "Take Items/Consumables", Quantity = 30, Price = 2, Description = "*ploink*" },
        new("Take Bombchus", "take_bombchus") { Category = "Take Items/Consumables", Quantity = 30, Price = 10, Description = "Hopefully they still have some regular bombs." },


        // Link Size Modifiers
        new("Giant Lonk", "giant_link") { Category = "Change Link's Size", Duration = 30, Price = 20, Description = "Big Lonk equals big sword." },
        new("Minish Link", "minish_link") { Category = "Change Link's Size", Duration = 30, Price = 20, Description = "They say size doesn't matter." },
        new("Paper Link", "paper_link") { Category = "Change Link's Size", Duration = 30, Price = 20, Description = "We might not have a Zelda variant of Paper Mario, but this gets pretty close." },
        new("Squished Link", "squished_link") { Category = "Change Link's Size", Duration = 30, Price = 20, Description = "Drop an anvil on Link, but the anvil is invisible." },
        new("Invisible Link", "invisible_link") { Category = "Change Link's Size", Duration = 30, Price = 20, Description = "Now you see me, now you don't." },


        // Generic Effects
        new("Random Bomb Fuse Timer", "random_bomb_timer") { Category = "Generic Effects", Duration = 60, Price = 5, Description = "Sets the timer for new bombs to either super short, super long or something inbetween (does not work for bombchus)." },
        new("Set Time to Dawn", "set_time_to_dawn") { Category = "Generic Effects", Price = 25, Description = "Sets the time to early day. Might require an area reload to take effect." },
        new("Set Time to Dusk", "set_time_to_dusk") { Category = "Generic Effects", Price = 25, Description = "Sets the time to early night. Might require an area reload to take effect." },


        // Visual Effects
        new("No UI", "no_ui") { Category = "Visual Effects", Duration = 60, Price = 20, Description = "No need to see ammo counts. The cinematic experience." },
        new("Rainstorm", "rainstorm") { Category = "Visual Effects", Duration = 30, Price = 5, Description = "Summon a rainstorm for a sad moment." },
        new("Debug Mode", "debug_mode") { Category = "Visual Effects", Duration = 30, Price = 20, Description = "if (debug_mode) { ShowCollision(); }" },
        new("Randomize Cosmetics", "random_cosmetics") { Category = "Visual Effects", Price = 30, Description = "Randomize most cosmetics options. Cosmetics changed by bidding wars are unaffected." },


        // Controls
        new("No Z Button", "no_z_button") { Category = "Controls", Duration = 30, Price = 20, Description = "No sidehops, no backflips, no jump attacks, no camera adjustments." },
        new("Reverse Controls", "reverse_controls") { Category = "Controls", Duration = 60, Price = 25, Description = "Just hold the controller upside down." },
        new("Pacifist Mode", "pacifist_mode") { Category = "Controls", Duration = 15, Price = 30, Description = "Disable the B button, C buttons and D-pad buttons for the duration." },
        new("Press Random Buttons", "press_random_buttons") { Category = "Controls", Duration = 30, Price = 25, Description = "Press random buttons on the controller for the duration. Maybe it'll help (spoiler: probably not)." },
        new("Clear C-Buttons", "clear_cbuttons") { Category = "Controls", Price = 10, Description = "Clear the assigned items from the C-buttons." },
        new("Clear D-pad", "clear_dpad") { Category = "Controls", Price = 10, Description = "Clear the assigned items from the D-pad." },


        // Teleport Player
        new("Link's House", "tp_links_house") { Category = "Teleport Player", Price = 100, Description = "Teleport the player to Link's House. Welcome Home!" },
        new("Minuet Destination", "tp_minuet") { Category = "Teleport Player", Price = 100, Description = "Teleport the player to Sacred Forest Meadow." },
        new("Bolero Destination", "tp_bolero") { Category = "Teleport Player", Price = 120, Description = "Teleport the player to Death Mountain Crater." },
        new("Serenade Destination", "tp_serenade") { Category = "Teleport Player", Price = 100, Description = "Teleport the player to Lake Hylia." },
        new("Requiem Destination", "tp_requiem") { Category = "Teleport Player", Price = 100, Description = "Teleport the player to Desert Colossus." },
        new("Nocturne Destination", "tp_nocturne") { Category = "Teleport Player", Price = 100, Description = "Teleport the player to the Raveyard." },
        new("Prelude Destination", "tp_prelude") { Category = "Teleport Player", Price = 100, Description = "Teleport the player to the Temple of Time." },


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
