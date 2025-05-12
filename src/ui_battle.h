#ifndef UI_BATTLE_H
#define UI_BATTLE_H

#include "game_state.h"

// Initialize the battle UI
void init_battle_ui();

// Draw the entire battle UI
void draw_battle_ui(GameState* game_state, int cursor_x, int cursor_y);

// Handle input during battle
void handle_battle_input(GameState* game_state, int* cursor_x, int* cursor_y);

// Run the battle phase of the game
void run_battle(GameState* game_state);

#endif // UI_BATTLE_H 