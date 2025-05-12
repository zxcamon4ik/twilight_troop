# Roflands Tactical Battle Game

A turn-based tactical battle game built with ncurses in C.

## Features

- **Unit Selection & Load-out**: Choose different unit types and equipment
- **Interactive Placement**: Place your units on the battlefield before the battle begins
- **Turn-based Combat**: Move and attack with your units on a grid using Manhattan distance
- **Save/Load System**: Save and restore your game progress
- **In-game Help**: Access game instructions anytime with the help screen
- **Full-screen ncurses UI**: Colorful interface with menus, grid, and animated feedback

## How to Build

To build the game, make sure you have the required libraries installed:

```
sudo apt-get install libncurses5-dev
```

Then compile the game:

```
make
```

## How to Run

After building, run the game with:

```
make run
```

Or directly:

```
./bin/roflands_battle
```

## Controls

- **Arrow Keys**: Move cursor
- **Enter**: Select/Confirm
- **Escape**: Cancel/Back
- **H**: Show help screen
- **S**: Save game
- **L**: Load game
- **E**: End turn (during battle)
- **Q**: Quit game

## Game Rules

1. **Unit Selection**:
   - Each player selects units and equips them with items
   - Different units have different abilities and stats

2. **Unit Placement**:
   - Players place their units on their side of the battlefield
   - Player 1 uses the top rows, Player 2 uses the bottom rows

3. **Battle**:
   - Players take turns moving and attacking with their units
   - Movement and attack ranges use Manhattan distance (|x1-x2| + |y1-y2|)
   - The game ends when all units of one player are defeated

## Unit Types

- **Warrior**: Balanced unit with medium attack and defense
- **Archer**: Ranged unit with high attack but low defense
- **Mage**: Powerful ranged attacker with area effects but fragile
- **Scout**: Fast unit with high movement range
- **Tank**: High HP and defense, but lower attack power
