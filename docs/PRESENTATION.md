# Twilight Troop: Project Presentation

## Project Overview

Twilight Troop is a tactical turn-based battle game developed in C using the ncurses library. The game features unit selection, strategic placement, and combat mechanics based on a grid system with Manhattan distance calculations.

## Architecture

The project follows a modular architecture with clear separation of concerns:

### Core Components

1. **Game State Management**
   - `game_state.h`: Central data structures and game state definitions
   - `main.c`: Main game loop and initialization

2. **User Interface**
   - `ui_menu.c/h`: Main menu and navigation
   - `ui_unit_select.c/h`: Unit selection and equipment interface
   - `ui_placement.c/h`: Unit placement on the battlefield
   - `ui_battle.c/h`: Battle visualization and interaction
   - `ui_help.c/h`: Help screens and tutorials

3. **Game Logic**
   - `game_logic.c`: Combat calculations and turn processing
   - `items.c`: Item and equipment handling

4. **Data Management**
   - `save.c`: Save/load game functionality
   - JSON and text configuration files

## Data Flow

1. Game initialization loads items from JSON configuration
2. Players select units and equipment
3. Players place units on the battlefield
4. Turn-based combat begins, with state updates after each action
5. Game concludes when one player's units are defeated

## Technical Highlights

### Data Structures

Key data structures include:
- `GameState`: Tracks overall game progression
- `Unit`: Represents battle units with stats and equipment
- `Item`: Equipment that modifies unit capabilities
- `Cell`: Grid elements that form the battlefield

### Algorithm Implementation

- Manhattan distance for movement and attack range calculation
- Grid-based pathfinding for unit movement
- Combat resolution with attack, defense, and equipment modifiers

### User Interface

- Full-color ncurses interface with:
  - Menus and selection screens
  - Grid-based battlefield visualization
  - Status displays and combat feedback
  - Help system

## Development Challenges

1. **State Management**: Maintaining consistent game state across different phases
2. **UI Development**: Creating an intuitive interface within terminal constraints
3. **Balance**: Ensuring fair and interesting gameplay through equipment and unit types

## Future Enhancements

1. **Campaign Mode**: Series of linked battles with progressive difficulty
2. **Additional Unit Types**: More variety in combat options
3. **Enhanced AI**: More sophisticated computer opponents
4. **Multiplayer**: Network play capabilities

## Demonstration

The game can be run with:

```bash
make run
```

Key demonstration points:
1. Main menu navigation
2. Unit selection and equipment
3. Battlefield placement
4. Combat mechanics
5. Special abilities and equipment effects
6. Save/load functionality

## Questions and Answers

Thank you for your attention to this presentation. I welcome any questions about the implementation or design decisions. 