# ASCII-RPG for Arduino UNO

A simple ASCII-based RPG for SSD1106 OLED displays, controlled with 4 push buttons.
Note that this game is still in development and there could be bugs.

## Hardware Setup
Plug the push buttons into the following Arduino pins:
- Up: 5  
- Down: 4  
- Left: 3  
- Right: 2  

## How to Play
- Move with the buttons (up/down/left/right).
- Move into enemies to attack them.
- When exiting the grid, a new one is generated. Level progress is increased by one.
- When level progress reaches 7, boss room is generated.
- Killing the boss completes the current stage.
- Skipping the boss increases difficulty by 1.
- Level progress can go up to 21. After that, the new grid will be always the boss room and the player is forced to fight.
- There are 3 stages total — beat the stage 3 boss to win.

## ASCII Symbols

### Player
- `P`: Player

### Natural Objects
- `B`: Boulder (impassable)
- `C`: Cactus (damages on contact, impassable)
- `H`: Chest (loot) *(not implemented)*
- `T`: Tree (impassable)
- `L`: Lava (instant death, impassable)
- `W`: Weapon (can be picked up)

### Enemies

#### Forest
- `E`: Elf – 5 HP, 10 DMG  
- `G`: Goblin – 10 HP, 5 DMG  
- `O`: Ogre – 35 HP, 10 DMG  

#### Desert
- `A`: Assassin – 10 HP, 15 DMG  
- `M`: Mummy – 15 HP, 10 DMG  
- `S`: Serpent – 50 HP, 20 DMG  

#### Volcanic
- `F`: Fiend – 20 HP, 20 DMG  
- `I`: Imp – 15 HP, 15 DMG  
- `D`: Dragon – 100 HP, 25 DMG  

## Items

### Consumables
- Health Pot: Fully restores HP

### Weapons (W)

#### Forest
- Stick (default): 5 DMG
- Sword (enemy drop): 8 DMG
- Enchanted Sword (boss drop at difficulty 3)

#### Desert
- Pointy Stick (found in starting area)
- Yoyo

#### Volcanic
- Sharp Stick (found in starting area)
- Molten Sword (5% drop chance, increases with difficulty)
- Excalibur (found after killing 30 enemies in volcanic area)



## Author
grzx3
