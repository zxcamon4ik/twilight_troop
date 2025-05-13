#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "game_state.h"

// Initialize the game state
void initialize_game_state(GameState* g) {
    g->current_phase = PHASE_MAIN_MENU;
    g->turn_state = TURN_STATE_SELECT_UNIT;
    g->current_player = PLAYER_1;
    g->selected_unit = NULL;
    g->game_turn = 1;
    strcpy(g->status_message, "Game started");
    
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++) {
            g->grid[y][x].unit = NULL;
            g->grid[y][x].terrain_type = 0;
            g->grid[y][x].highlight = 0;
        }
    
    g->num_units[PLAYER_1] = g->num_units[PLAYER_2] = 0;
}

// Place a unit on the grid
void place_unit(GameState* g, Unit* u, int x, int y) {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return;
    if (u->x >= 0 && u->y >= 0) g->grid[u->y][u->x].unit = NULL;
    g->grid[y][x].unit = u;
    u->x = x;
    u->y = y;
}

// Remove a unit from the grid
void remove_unit(GameState* g, Unit* u) {
    if (!g || !u) return;
    if (u->x >= 0 && u->x < GRID_WIDTH && u->y >= 0 && u->y < GRID_HEIGHT)
        if (g->grid[u->y][u->x].unit == u)
            g->grid[u->y][u->x].unit = NULL;
    u->is_active = 0;
}

// Calculate Manhattan distance between two points
int manhattan_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Move a unit to a new position
int move_unit(GameState* g, Unit* u, int nx, int ny) {
    if (nx < 0 || nx >= GRID_WIDTH || ny < 0 || ny >= GRID_HEIGHT ||
        g->grid[ny][nx].unit != NULL ||
        manhattan_distance(u->x, u->y, nx, ny) > u->move_range)
        return 0;
    place_unit(g, u, nx, ny);
    return 1;
}

// Calculate the total attack power of a unit
int calculate_attack(Unit* u) {
    static const int base_atk[] = {5, 7, 8, 4, 3}; // By unit type
    int atk = (u->type < NUM_UNIT_TYPES) ? base_atk[u->type] : 0;
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++)
        if (u->equipment[i]) atk += u->equipment[i]->att;
    return atk;
}

// Calculate the total defense of a unit
int calculate_defense(Unit* u) {
    static const int base_def[] = {5, 3, 2, 4, 8}; // By unit type
    int def = (u->type < NUM_UNIT_TYPES) ? base_def[u->type] : 0;
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++)
        if (u->equipment[i]) def += u->equipment[i]->def;
    return def;
}

// Attack another unit
int attack_unit(GameState* g, Unit* atk, Unit* tgt) {
    if (!g || !atk || !tgt || !tgt->is_active ||
        manhattan_distance(atk->x, atk->y, tgt->x, tgt->y) > atk->attack_range)
        return 0;
    
    int dmg = (calculate_attack(atk) * 2) / 3 - (calculate_defense(tgt) / 2);
    dmg = (dmg < 1) ? 1 : (dmg > tgt->max_hp / 4) ? tgt->max_hp / 4 + 1 : dmg;
    
    tgt->current_hp -= dmg;
    if (g->status_message)
        snprintf(g->status_message, 100, "Dealt %d damage to %s", dmg, tgt->name ? tgt->name : "enemy");
    
    if (tgt->current_hp <= 0) {
        tgt->current_hp = 0;
        tgt->is_active = 0;
        if (tgt->x >= 0 && tgt->y >= 0)
            g->grid[tgt->y][tgt->x].unit = NULL;
    }
    
    return dmg;
}

// End the current player's turn
void end_turn(GameState* g) {
    g->current_player = 1 - g->current_player;
    if (g->current_player == PLAYER_1) g->game_turn++;
    g->turn_state = TURN_STATE_SELECT_UNIT;
    g->selected_unit = NULL;
    clear_highlights(g);
}

// Clear all highlights from the grid
void clear_highlights(GameState* g) {
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            g->grid[y][x].highlight = 0;
}

// Highlight cells within a unit's movement range
void highlight_movement_range(GameState* g, Unit* u) {
    if (!g || !u || u->x < 0 || u->x >= GRID_WIDTH || u->y < 0 || u->y >= GRID_HEIGHT) return;
    
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (!g->grid[y][x].unit && manhattan_distance(u->x, u->y, x, y) <= u->move_range)
                g->grid[y][x].highlight = 1;
}

// Highlight cells within a unit's attack range
void highlight_attack_range(GameState* g, Unit* u) {
    if (!g || !u || u->x < 0 || u->x >= GRID_WIDTH || u->y < 0 || u->y >= GRID_HEIGHT) return;
    
    for (int y = 0; y < GRID_HEIGHT; y++)
        for (int x = 0; x < GRID_WIDTH; x++)
            if (g->grid[y][x].unit && g->grid[y][x].unit->player_id != u->player_id &&
                g->grid[y][x].unit->is_active && manhattan_distance(u->x, u->y, x, y) <= u->attack_range)
                g->grid[y][x].highlight = 2;
}

// Save the game state to a file
void save_game(GameState* g, const char* f) {
    FILE* file = fopen(f, "wb");
    if (!file) return;
    fwrite(g, sizeof(GameState), 1, file);
    fclose(file);
}

// Load the game state from a file
int load_game(GameState* g, const char* f) {
    FILE* file = fopen(f, "rb");
    if (!file) return 0;
    size_t r = fread(g, sizeof(GameState), 1, file);
    fclose(file);
    return (r == 1);
} 