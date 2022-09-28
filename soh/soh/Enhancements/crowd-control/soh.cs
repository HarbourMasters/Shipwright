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

	private Dictionary<string, string> _enemyType = new Dictionary<string, string>()
	{
		{"wallmaster", ("Wallmaster")},
		{"arwing", ("Arwing")},
		{"darklink", ("Dark Link")},
		{"stalfos", ("Stalfos")},
		{"wolfos", ("Wolfos")},
		{"freezard", ("Freezard")},
		{"keese", ("Keese")},
		{"icekeese", ("Ice Keese")},
		{"firekeese", ("Fire Keese")},
		{"tektite", ("Tektite")},
		{"likelike", ("Like-Like")}
	};

	public override List<Effect> Effects
	{
		get
		{
			List<Effect> effects = new List<Effect>
			{
				new Effect("Add Heart Container", "add_heart_container"),
				new Effect("Remove Heart Container", "remove_heart_container"),
				new Effect("Damage Multiplier", "damage_multiplier", new[] { "damdefmulti" }) { Duration = 30 },
				new Effect("Defense Multiplier", "defense_multiplier", new[] { "damdefmulti" }) { Duration = 30 },
				new Effect("Freeze Link", "freeze"),
				new Effect("Giant Lonk", "giant_link") { Duration = 30 },
				new Effect("Empty Heart", "damage", new[] { "health20" }),
				new Effect("Fill Heart", "heal", new[] { "health20" }),
				new Effect("Kill Player", "kill"),
				new Effect("High Gravity", "high_gravity") { Duration = 30 },
				new Effect("Hover Boots", "hover_boots") { Duration = 30 },
				new Effect("Invisible Link", "invisible") { Duration = 30 },
				new Effect("No UI", "no_ui") { Duration = 60 },
				new Effect("Low Gravity", "low_gravity") { Duration = 30 },
				new Effect("Fill Magic", "fill_magic"),
				new Effect("Empty Magic", "empty_magic"),
				new Effect("Minish Link", "minish_link") { Duration = 30 },
				new Effect("No Z Button", "no_z") { Duration = 30 },
				new Effect("One-Hit KO", "ohko") { Duration = 30 },
				new Effect("Pacifist", "pacifist") { Duration = 15 },
				new Effect("Paper Link", "paper_link") { Duration = 30 },
				new Effect("Rainstorm", "rainstorm") { Duration = 30 },
				new Effect("Reverse Controls", "reverse") { Duration = 60 },
				new Effect("Give Rupees", "add_rupees", new[] { "rupees999" }),
				new Effect("Take Rupees", "remove_rupees", new[] { "rupees999" }),
				new Effect("Increase Speed", "increase_speed") { Duration = 30 },
				new Effect("Decrease Speed", "decrease_speed") { Duration = 30 },
				new Effect("Cucco Swarm", "cucco_storm"),
				new Effect("Knockback Link", "knockback", new[] { "knockbackstrength" }),
				new Effect("Burn Link", "burn"),
				new Effect("Electrocute Link", "electrocute"),
				new Effect("Iron Boots", "iron_boots") { Duration = 30 },
				new Effect("Give Deku Shield", "give_dekushield"),
				new Effect("Spawn Enemy", "spawn_enemy", ItemKind.Folder),
			};
			
			effects.AddRange(_enemyType.Select(t => new Effect($"Spawn {t.Value}", $"spawn_{t.Key}", "spawn_enemy")));

			return effects;
		}
	}

	//Slider ranges need to be defined
    public override List<ItemType> ItemTypes => new List<ItemType>
    {
        new ItemType("Rupees", "rupees999", ItemType.Subtype.Slider, "{\"min\":1,\"max\":999}"),
		new ItemType("Health", "health20", ItemType.Subtype.Slider, "{\"min\":1,\"max\":20}"),
		new ItemType("Damage/Defense Multiplier", "damdefmulti", ItemType.Subtype.Slider, "{\"min\":1,\"max\":10}"),
		new ItemType("Knockback Strength", "knockbackstrength", ItemType.Subtype.Slider, "{\"min\":1,\"max\":3}")
    };
	
}
