#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "game_state.h"

// Initialize the game state
void initialize_game_state(GameState* game_state) {
    // Set default values
    game_state->current_phase = PHASE_MAIN_MENU;
    game_state->turn_state = TURN_STATE_SELECT_UNIT;
    game_state->current_player = PLAYER_1;
    game_state->selected_unit = NULL;
    game_state->game_turn = 1;
    strcpy(game_state->status_message, "Game started");
    
    // Initialize grid
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            game_state->grid[y][x].unit = NULL;
            game_state->grid[y][x].terrain_type = 0; // Default to plain terrain
            game_state->grid[y][x].highlight = 0;
        }
    }
    
    // Initialize unit counts
    game_state->num_units[PLAYER_1] = 0;
    game_state->num_units[PLAYER_2] = 0;
}

// Place a unit on the grid
void place_unit(GameState* game_state, Unit* unit, int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return; // Invalid position
    }
    
    // If the unit is already on the grid, remove it first
    if (unit->x >= 0 && unit->y >= 0) {
        game_state->grid[unit->y][unit->x].unit = NULL;
    }
    
    // Place the unit at the new position
    game_state->grid[y][x].unit = unit;
    unit->x = x;
    unit->y = y;
}

// Remove a unit from the grid
void remove_unit(GameState* game_state, Unit* unit) {
    // Check for NULL pointers
    if (!game_state || !unit) {
        return;
    }
    
    // Check if the unit is actually on the grid
    if (unit->x >= 0 && unit->x < GRID_WIDTH && unit->y >= 0 && unit->y < GRID_HEIGHT) {
        // Only remove the unit from the grid if it's actually there
        if (game_state->grid[unit->y][unit->x].unit == unit) {
            game_state->grid[unit->y][unit->x].unit = NULL;
        }
        
        // Mark unit as inactive but don't reset coordinates yet
        unit->is_active = 0;
    }
}

// Calculate Manhattan distance between two points
int manhattan_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Move a unit to a new position
int move_unit(GameState* game_state, Unit* unit, int new_x, int new_y) {
    // Check if the new position is valid
    if (new_x < 0 || new_x >= GRID_WIDTH || new_y < 0 || new_y >= GRID_HEIGHT) {
        return 0; // Invalid position
    }
    
    // Check if the new position is already occupied
    if (game_state->grid[new_y][new_x].unit != NULL) {
        return 0; // Position already occupied
    }
    
    // Check if the move is within the unit's movement range
    int distance = manhattan_distance(unit->x, unit->y, new_x, new_y);
    if (distance > unit->move_range) {
        return 0; // Move is too far
    }
    
    // Move the unit
    place_unit(game_state, unit, new_x, new_y);
    return 1;
}

// Calculate the total attack power of a unit
int calculate_attack(Unit* unit) {
    int attack = 0;
    
    // Base attack based on unit type
    switch (unit->type) {
        case UNIT_TYPE_WARRIOR: attack = 5; break;
        case UNIT_TYPE_ARCHER: attack = 7; break;
        case UNIT_TYPE_MAGE: attack = 8; break;
        case UNIT_TYPE_SCOUT: attack = 4; break;
        case UNIT_TYPE_TANK: attack = 3; break;
    }
    
    // Add equipment bonuses
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
        if (unit->equipment[i]) {
            attack += unit->equipment[i]->att;
        }
    }
    
    return attack;
}

// Calculate the total defense of a unit
int calculate_defense(Unit* unit) {
    int defense = 0;
    
    // Base defense based on unit type
    switch (unit->type) {
        case UNIT_TYPE_WARRIOR: defense = 5; break;
        case UNIT_TYPE_ARCHER: defense = 3; break;
        case UNIT_TYPE_MAGE: defense = 2; break;
        case UNIT_TYPE_SCOUT: defense = 4; break;
        case UNIT_TYPE_TANK: defense = 8; break;
    }
    
    // Add equipment bonuses
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
        if (unit->equipment[i]) {
            defense += unit->equipment[i]->def;
        }
    }
    
    return defense;
}

// Attack another unit
int attack_unit(GameState* game_state, Unit* attacker, Unit* target) {
    // Validate pointers
    if (!game_state || !attacker || !target) {
        return 0; // Invalid parameters
    }
    
    // Check if target is active
    if (!target->is_active) {
        return 0; // Target already defeated
    }
    
    // Check if the target is in range
    int distance = manhattan_distance(attacker->x, attacker->y, target->x, target->y);
    if (distance > attacker->attack_range) {
        return 0; // Target is out of range
    }
    
    // Calculate damage
    int attack_power = calculate_attack(attacker);
    int defense = calculate_defense(target);
    
    // Apply a more balanced damage formula
    int damage = (attack_power * 2) / 3 - (defense / 2);
    
    // Ensure minimum damage of 1 and maximum of 25% of max HP
    if (damage < 1) {
        damage = 1;
    } else if (damage > target->max_hp / 4) {
        // Cap damage at 25% of max HP to prevent one-shot kills
        damage = target->max_hp / 4 + 1;
    }
    
    // Apply damage
    target->current_hp -= damage;
    
    // Display damage in status message - safely use name
    if (game_state->status_message) {
        snprintf(game_state->status_message, 100, "Dealt %d damage to %s", 
                damage, target->name ? target->name : "enemy");
    }
    
    // Check if target is defeated
    if (target->current_hp <= 0) {
        target->current_hp = 0;
        target->is_active = 0;
        
        // Remove unit from grid but keep in player_units array
        if (target->x >= 0 && target->y >= 0) {
            game_state->grid[target->y][target->x].unit = NULL;
            // Don't set x and y to -1 to preserve the last position for animations
        }
    }
    
    return damage; // Return the amount of damage dealt
}

// End the current player's turn
void end_turn(GameState* game_state) {
    // Switch to the other player
    game_state->current_player = (game_state->current_player == PLAYER_1) ? PLAYER_2 : PLAYER_1;
    
    // If we switched back to player 1, increment the game turn
    if (game_state->current_player == PLAYER_1) {
        game_state->game_turn++;
    }
    
    // Reset the turn state
    game_state->turn_state = TURN_STATE_SELECT_UNIT;
    game_state->selected_unit = NULL;
    
    // Clear highlights
    clear_highlights(game_state);
}

// Clear all highlights from the grid
void clear_highlights(GameState* game_state) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            game_state->grid[y][x].highlight = 0;
        }
    }
}

// Highlight cells within a unit's movement range
void highlight_movement_range(GameState* game_state, Unit* unit) {
    // Check for NULL pointers
    if (!game_state || !unit) {
        return;
    }
    
    // Check if unit position is valid
    if (unit->x < 0 || unit->x >= GRID_WIDTH || unit->y < 0 || unit->y >= GRID_HEIGHT) {
        return;
    }
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Skip cells that already have a unit
            if (game_state->grid[y][x].unit != NULL) {
                continue;
            }
            
            // Check if the cell is within the unit's movement range
            int distance = manhattan_distance(unit->x, unit->y, x, y);
            if (distance <= unit->move_range) {
                game_state->grid[y][x].highlight = 1;
            }
        }
    }
}

// Highlight cells within a unit's attack range
void highlight_attack_range(GameState* game_state, Unit* unit) {
    // Check for NULL pointers
    if (!game_state || !unit) {
        return;
    }
    
    // Check if unit position is valid
    if (unit->x < 0 || unit->x >= GRID_WIDTH || unit->y < 0 || unit->y >= GRID_HEIGHT) {
        return;
    }
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Skip cells that don't have an enemy unit
            if (game_state->grid[y][x].unit == NULL || 
                game_state->grid[y][x].unit->player_id == unit->player_id ||
                !game_state->grid[y][x].unit->is_active) {
                continue;
            }
            
            // Check if the cell is within the unit's attack range
            int distance = manhattan_distance(unit->x, unit->y, x, y);
            if (distance <= unit->attack_range) {
                game_state->grid[y][x].highlight = 2;
            }
        }
    }
}

// Save the game state to a file
void save_game(GameState* game_state, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return;
    }
    
    // Write game state
    fwrite(game_state, sizeof(GameState), 1, file);
    
    fclose(file);
}

// Load the game state from a file
int load_game(GameState* game_state, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }
    
    // Read game state
    size_t bytes_read = fread(game_state, sizeof(GameState), 1, file);
    
    fclose(file);
    
    return (bytes_read == 1);
} 