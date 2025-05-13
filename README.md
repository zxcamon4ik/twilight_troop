# Twilight Troop: Tactical Battle Game

![C Build](https://github.com/username/twilight_troop/actions/workflows/c-build.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A turn-based tactical battle game built with ncurses in C, featuring unit management, combat mechanics, and interactive gameplay.

![Tactical Battle Game](https://via.placeholder.com/800x400?text=Twilight+Troop+Screenshot)

## Overview

Twilight Troop is a tactical battle game where players strategically deploy different unit types with unique equipment combinations. The game features turn-based combat on a grid system using Manhattan distance for movement and attacks.

## Features

- **Unit Selection & Equipment System**: Choose from 5 unique unit types (Warrior, Archer, Mage, Scout, Tank) and equip them with various weapons and armor
- **Dynamic Placement Phase**: Place your units strategically on the battlefield before combat begins
- **Turn-based Tactical Combat**: Move and attack with your units on a grid using Manhattan distance mechanics
- **Save/Load System**: Save your game progress and continue later
- **Full-color ncurses UI**: Colorful interface with menus, grid representation, and animated combat feedback
- **In-game Help**: Comprehensive tutorial and help system

## Game Mechanics

### Unit Types
- **Warrior**: Balanced unit with medium attack and defense
- **Archer**: Ranged unit with high attack but low defense
- **Mage**: Powerful area-effect spellcaster with high damage radius but fragile defense
- **Scout**: Fast unit with extended movement range
- **Tank**: High HP and defense, but lower attack power

### Equipment System
Equipment items modify unit stats in the following ways:
- **Attack**: Increases damage dealt to opponents
- **Defense**: Reduces damage taken
- **Slots**: Each unit has 2 equipment slots maximum
- **Range**: Determines attack distance (Manhattan)
- **Radius**: Determines area-of-effect for attacks

## Installation

### Prerequisites
- GCC compiler
- ncurses library

### Linux/Unix
```bash
# Install ncurses if not already installed
sudo apt-get install libncurses5-dev  # Debian/Ubuntu
# OR
sudo yum install ncurses-devel        # CentOS/RHEL
# OR
sudo pacman -S ncurses                # Arch Linux

# Clone the repository
git clone https://github.com/username/twilight_troop.git
cd twilight_troop

# Build the game
make

# Run the game
make run
```

## Controls

- **Arrow Keys**: Move cursor/selected unit
- **Enter**: Select/Confirm
- **Escape**: Cancel/Back
- **H**: Show help screen
- **S**: Save game
- **L**: Load game
- **E**: End turn (during battle)
- **Q**: Quit game

## Game Flow

1. **Main Menu**: Choose to start a new game, load a saved game, view help, or exit
2. **Unit Selection**: Choose and equip your units for battle
3. **Unit Placement**: Position your units on the battlefield
4. **Battle Phase**: Take turns moving and attacking with your units
5. **Game Over**: Victory screen when one player defeats all enemy units

## Development

This project demonstrates skills in:
- C programming with data structures
- Game state management
- Turn-based game mechanics
- UI development with ncurses
- File I/O for save/load functionality

## Documentation

For more detailed documentation, see the [docs](docs/) directory.

## Contributing

Contributions are welcome! Please see our [Contributing Guidelines](CONTRIBUTING.md) for more information.

## License

[MIT License](LICENSE)

## Acknowledgments

- Thanks to all contributors and testers
- Inspired by classic tactical games like Final Fantasy Tactics and Advance Wars
