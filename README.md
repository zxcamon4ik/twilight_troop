# Twilight Troop

A turn-based tactical battle game built with ncurses in C, featuring unit management, combat mechanics, and interactive gameplay.

## Overview

Twilight Troop is a tactical battle game where players strategically deploy different unit types with unique equipment combinations. The game features turn-based combat on a grid system using Manhattan distance for movement and attacks.

## Key Features

- **Tactical Turn-Based Combat** - Grid-based movement with Manhattan distance calculations
- **5 Unique Unit Types** - Each with different strengths and abilities
- **Equipment System** - Customize units with weapons and armor
- **Interactive Placement** - Strategic unit positioning before battle
- **ncurses UI** - Full-color terminal interface with intuitive controls
- **Save/Load System** - Continue your battles later

## Game Mechanics

### Unit Types
- **Warrior** - Balanced unit with medium attack and defense
- **Archer** - Ranged unit with high attack but low defense
- **Mage** - Area-effect spellcaster with high damage radius
- **Scout** - Fast unit with extended movement range
- **Tank** - High HP and defense, lower attack power

### Equipment
Items modify your units' capabilities:
- **Attack** - Increases damage dealt
- **Defense** - Reduces damage taken
- **Range** - Determines attack distance
- **Radius** - Controls area-of-effect for attacks

## Installation

### Requirements
- GCC compiler
- ncurses library

### Build Instructions
```bash
# Install ncurses
sudo apt-get install libncurses5-dev  # Debian/Ubuntu
# OR
sudo pacman -S ncurses                # Arch Linux

# Build the game
make

# Run the game
make run
```

## Controls

- **Arrow Keys** - Move cursor/unit
- **Enter** - Select/Confirm
- **Escape** - Cancel/Back
- **H** - Help screen
- **S** - Save game
- **L** - Load game
- **E** - End turn
- **Q** - Quit game

## Project Structure

```
twilight_troop/
├── bin/                # Compiled binaries
├── data/               # Game data files
│   ├── army1.txt       # Sample army configuration
│   ├── army2.txt       # Sample army configuration
│   ├── items.json      # Equipment definitions
│   ├── items.txt       # Alternative item format
│   └── unit_types.txt  # Unit type definitions
├── docs/               # Documentation
├── obj/                # Object files
└── src/                # Source code
    ├── game_logic.c    # Core game mechanics
    ├── game_state.h    # Game state definitions
    ├── main.c          # Entry point
    ├── ui_*.c/h        # User interface components
    └── ...
```

## Documentation

For presentation materials and architectural diagrams, see the [docs](docs/) directory.

## Screenshots

*Screenshots coming soon*
