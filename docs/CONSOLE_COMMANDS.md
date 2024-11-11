add_heart_container
Usage: add_heart_container <value>
Increases Link's health by a number of full hearts equal to the passed value. Will not allow for adding health past 20 hearts.

addammo
Usage: addammo <type> <amount>
Adds ammo for the specified item equal to the specified amount. Works for Deku Sticks, Deku Nuts, Deku Seeds, Bombs, Bombchus, Arrows, and Magic Beans (sticks, nuts, seeds, bombs, bombchus, arrows, beans). Will only accept a positive integer for the amount.

bItem

bind
Usage: bind <key> <command>
Binds a key to execute an arbitrary command, such as for healing link's health
#We could use an "unbind" command as well. Right now the best you can do is bind the key again and leave the command argument blank. This returns an error in the consol when the key is pressed

bind-toggle


boots
bottle

burn
Usage: burn
sets link on fire

clear
Usage: clear
Clears the console log as if you had hit the "Clear" button.

#MORE TESTING NEEDED
cosmetics
Usage: cosmetics <rest|randomise> [group name]
Randomise cosmetic settings, or reset to defaults.


cucco_storm
Usage: cucco_storm
Begins a cucco storm as if you had attacked a cucco several times.

damage
Usage: damage <value>
Damages Link for a number of hearts equal to the passes value. Only accepts positive values, and does not allow for taking fractional damage. To restore health, use the "heal" command.

defence_modifier
electorcute
empty_magic
entrance
file_select
fill_magic
freeze
fw
gen_rando
get
giant_link
give_item
give_shield

gravity
Usage: gravity <value>
Adjusts Link's fall speed. The default value is 1. Accepts decimal values, but not negative values.

heal
Usage: heal <value>
Heals Link a number of hearts equal to the passed value. Will not accept negative values, and cannot heal fractional hearts. To drain health, use the "damage" command.

help
Usage: help
Prints a list of console commands

invisible
item

kill
Usage: kill
Kills link

knockback
load_state
map
minish_link

no_ui
Usage: no_ui (1|0)
Toggles the UI off. Passing a 1 turns the UI off, and a 0 will restore it.

no_z
Usage: no_z (1|0)
Disables Z-targeting when passed a 1, and reenables it when passed a 0.

ohko
pacifist
paper_link
pos

quit
Usage: quit
Closes the game.

rainstorm
reload
remove_heart_container
reset
reverse_controls
rupee
save_state
set
set_slot
sfx
spawn
speed_modifier
takeammo
takeshield
update_rupees
void