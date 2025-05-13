#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game_state.h"

// Color pairs for the battle grid
#define COLOR_GRID_PLAIN 1
#define COLOR_GRID_FOREST 2
#define COLOR_GRID_MOUNTAIN 3
#define COLOR_PLAYER_1 4
#define COLOR_PLAYER_2 5
#define COLOR_HIGHLIGHT_MOVE 6
#define COLOR_HIGHLIGHT_ATTACK 7
#define COLOR_SELECTED 8
#define COLOR_UI_TEXT 9

// Cell dimensions
#define CELL_WIDTH 5
#define CELL_HEIGHT 3

// Battle UI dimensions
#define INFO_PANEL_WIDTH 30

// Function prototypes
void init_battle_ui();
void draw_grid(GameState* game_state, int cursor_x, int cursor_y);
void draw_cell(int y, int x, Cell* cell, int is_cursor);
void draw_unit(int y, int x, Unit* unit);
void draw_info_panel(GameState* game_state);
void draw_battle_ui(GameState* game_state, int cursor_x, int cursor_y);
void handle_battle_input(GameState* game_state, int* cursor_x, int* cursor_y);
void show_action_menu(GameState* game_state, Unit* unit);
void animate_attack(GameState* game_state, Unit* attacker, Unit* target);
void show_movement_preview(GameState* game_state, Unit* unit, int new_x, int new_y);

// Initialize the battle UI
void init_battle_ui() {
    // Initialize color pairs for the battle grid
    init_pair(COLOR_GRID_PLAIN, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_GRID_FOREST, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_GRID_MOUNTAIN, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_PLAYER_1, COLOR_BLACK, COLOR_BLUE);
    init_pair(COLOR_PLAYER_2, COLOR_BLACK, COLOR_RED);
    init_pair(COLOR_HIGHLIGHT_MOVE, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_HIGHLIGHT_ATTACK, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COLOR_SELECTED, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_UI_TEXT, COLOR_WHITE, COLOR_BLACK);
}

// Draw a single cell of the grid
void draw_cell(int y, int x, Cell* cell, int is_cursor) {
    int start_y = y * CELL_HEIGHT + 1;
    int start_x = x * CELL_WIDTH + 1;
    int color_pair;
    
    // Determine the color of the cell based on its content and highlight
    if (cell->unit) {
        if (cell->unit->player_id == PLAYER_1) {
            color_pair = COLOR_PLAYER_1;
        } else {
            color_pair = COLOR_PLAYER_2;
        }
    } else {
        switch (cell->highlight) {
            case 1: color_pair = COLOR_HIGHLIGHT_MOVE; break;
            case 2: color_pair = COLOR_HIGHLIGHT_ATTACK; break;
            default:
                switch (cell->terrain_type) {
                    case 0: color_pair = COLOR_GRID_PLAIN; break;
                    case 1: color_pair = COLOR_GRID_FOREST; break;
                    case 2: color_pair = COLOR_GRID_MOUNTAIN; break;
                    default: color_pair = COLOR_GRID_PLAIN;
                }
        }
    }
    
    // Draw the cell with the appropriate color
    attron(COLOR_PAIR(color_pair));
    
    // Draw cell border
    for (int i = 0; i < CELL_WIDTH; i++) {
        mvaddch(start_y, start_x + i, ' ');
        mvaddch(start_y + CELL_HEIGHT - 1, start_x + i, ' ');
    }
    
    for (int i = 0; i < CELL_HEIGHT; i++) {
        mvaddch(start_y + i, start_x, ' ');
        mvaddch(start_y + i, start_x + CELL_WIDTH - 1, ' ');
    }
    
    // Draw cell content
    if (cell->unit) {
        // Draw unit
        char unit_char = ' ';
        switch (cell->unit->type) {
            case UNIT_TYPE_WARRIOR: unit_char = 'W'; break;
            case UNIT_TYPE_ARCHER: unit_char = 'A'; break;
            case UNIT_TYPE_MAGE: unit_char = 'M'; break;
            case UNIT_TYPE_SCOUT: unit_char = 'S'; break;
            case UNIT_TYPE_TANK: unit_char = 'T'; break;
        }
        mvaddch(start_y + 1, start_x + 2, unit_char);
        
        // Draw HP
        char hp_str[3];
        sprintf(hp_str, "%d", cell->unit->current_hp);
        mvprintw(start_y + 2, start_x + 1, "%s", hp_str);
    }
    
    // If this is the cursor position, highlight it
    if (is_cursor) {
        attron(A_BOLD);
        for (int i = 0; i < CELL_WIDTH; i++) {
            mvaddch(start_y, start_x + i, ACS_HLINE);
            mvaddch(start_y + CELL_HEIGHT - 1, start_x + i, ACS_HLINE);
        }
        
        for (int i = 0; i < CELL_HEIGHT; i++) {
            mvaddch(start_y + i, start_x, ACS_VLINE);
            mvaddch(start_y + i, start_x + CELL_WIDTH - 1, ACS_VLINE);
        }
        
        // Draw corners
        mvaddch(start_y, start_x, ACS_ULCORNER);
        mvaddch(start_y, start_x + CELL_WIDTH - 1, ACS_URCORNER);
        mvaddch(start_y + CELL_HEIGHT - 1, start_x, ACS_LLCORNER);
        mvaddch(start_y + CELL_HEIGHT - 1, start_x + CELL_WIDTH - 1, ACS_LRCORNER);
        attroff(A_BOLD);
    }
    
    attroff(COLOR_PAIR(color_pair));
}

// Draw the entire grid
void draw_grid(GameState* game_state, int cursor_x, int cursor_y) {
    clear();
    
    // Draw grid coordinates
    attron(COLOR_PAIR(COLOR_UI_TEXT));
    for (int x = 0; x < GRID_WIDTH; x++) {
        mvprintw(0, x * CELL_WIDTH + 3, "%d", x);
    }
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        mvprintw(y * CELL_HEIGHT + 2, 0, "%d", y);
    }
    attroff(COLOR_PAIR(COLOR_UI_TEXT));
    
    // Draw each cell
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            draw_cell(y, x, &game_state->grid[y][x], (x == cursor_x && y == cursor_y));
        }
    }
}

// Draw the info panel
void draw_info_panel(GameState* game_state) {
    int panel_start_x = GRID_WIDTH * CELL_WIDTH + 3;
    int panel_start_y = 1;
    
    // Draw panel border
    attron(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
    mvaddch(panel_start_y, panel_start_x, ACS_ULCORNER);
    mvaddch(panel_start_y, panel_start_x + INFO_PANEL_WIDTH, ACS_URCORNER);
    mvaddch(panel_start_y + GRID_HEIGHT * CELL_HEIGHT, panel_start_x, ACS_LLCORNER);
    mvaddch(panel_start_y + GRID_HEIGHT * CELL_HEIGHT, panel_start_x + INFO_PANEL_WIDTH, ACS_LRCORNER);
    
    for (int i = 1; i < INFO_PANEL_WIDTH; i++) {
        mvaddch(panel_start_y, panel_start_x + i, ACS_HLINE);
        mvaddch(panel_start_y + GRID_HEIGHT * CELL_HEIGHT, panel_start_x + i, ACS_HLINE);
    }
    
    for (int i = 1; i < GRID_HEIGHT * CELL_HEIGHT; i++) {
        mvaddch(panel_start_y + i, panel_start_x, ACS_VLINE);
        mvaddch(panel_start_y + i, panel_start_x + INFO_PANEL_WIDTH, ACS_VLINE);
    }
    
    // Draw panel title
    mvprintw(panel_start_y + 1, panel_start_x + 2, "BATTLE INFO");
    mvaddch(panel_start_y + 2, panel_start_x, ACS_LTEE);
    for (int i = 1; i < INFO_PANEL_WIDTH; i++) {
        mvaddch(panel_start_y + 2, panel_start_x + i, ACS_HLINE);
    }
    mvaddch(panel_start_y + 2, panel_start_x + INFO_PANEL_WIDTH, ACS_RTEE);
    
    // Draw current player turn
    mvprintw(panel_start_y + 4, panel_start_x + 2, "TURN: Player %d", game_state->current_player + 1);
    
    // Draw selected unit info if any
    if (game_state->selected_unit) {
        Unit* unit = game_state->selected_unit;
        mvprintw(panel_start_y + 6, panel_start_x + 2, "SELECTED UNIT:");
        mvprintw(panel_start_y + 7, panel_start_x + 2, "%s (Player %d)", unit->name, unit->player_id + 1);
        mvprintw(panel_start_y + 8, panel_start_x + 2, "HP: %d/%d", unit->current_hp, unit->max_hp);
        mvprintw(panel_start_y + 9, panel_start_x + 2, "Move Range: %d", unit->move_range);
        mvprintw(panel_start_y + 10, panel_start_x + 2, "Attack Range: %d", unit->attack_range);
        
        // Display equipment
        mvprintw(panel_start_y + 12, panel_start_x + 2, "EQUIPMENT:");
        for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
            if (unit->equipment[i]) {
                const char* slot_name;
                switch (i) {
                    case EQUIP_WEAPON: slot_name = "Weapon"; break;
                    case EQUIP_ARMOR: slot_name = "Armor"; break;
                    case EQUIP_ACCESSORY: slot_name = "Accessory"; break;
                    default: slot_name = "Unknown";
                }
                mvprintw(panel_start_y + 13 + i, panel_start_x + 2, "%s: %s", slot_name, unit->equipment[i]->name);
            } else {
                const char* slot_name;
                switch (i) {
                    case EQUIP_WEAPON: slot_name = "Weapon"; break;
                    case EQUIP_ARMOR: slot_name = "Armor"; break;
                    case EQUIP_ACCESSORY: slot_name = "Accessory"; break;
                    default: slot_name = "Unknown";
                }
                mvprintw(panel_start_y + 13 + i, panel_start_x + 2, "%s: None", slot_name);
            }
        }
    }
    
    // Draw game status message
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 5, panel_start_x + 2, "%s", game_state->status_message);
    
    // Draw controls help
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 4, panel_start_x + 2, "Move: Arrow keys");
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 3, panel_start_x + 2, "Select: Enter, Back: Esc");
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 2, panel_start_x + 2, "Attack auto-ends turn!");
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 1, panel_start_x + 2, "Quit: Q, Save: S, Load: L");
    
    attroff(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
}

// Draw the complete battle UI
void draw_battle_ui(GameState* game_state, int cursor_x, int cursor_y) {
    draw_grid(game_state, cursor_x, cursor_y);
    draw_info_panel(game_state);
    refresh();
}

// Handle keyboard input during battle
void handle_battle_input(GameState* game_state, int* cursor_x, int* cursor_y) {
    int ch = getch();
    
    switch (ch) {
        case KEY_UP:
            if (*cursor_y > 0) (*cursor_y)--;
            break;
        case KEY_DOWN:
            if (*cursor_y < GRID_HEIGHT - 1) (*cursor_y)++;
            break;
        case KEY_LEFT:
            if (*cursor_x > 0) (*cursor_x)--;
            break;
        case KEY_RIGHT:
            if (*cursor_x < GRID_WIDTH - 1) (*cursor_x)++;
            break;
        case 10: // Enter key
            switch (game_state->turn_state) {
                case TURN_STATE_SELECT_UNIT:
                    // Select unit under cursor
                    if (game_state->grid[*cursor_y][*cursor_x].unit &&
                        game_state->grid[*cursor_y][*cursor_x].unit->player_id == game_state->current_player &&
                        game_state->grid[*cursor_y][*cursor_x].unit->is_active) {
                        
                        game_state->selected_unit = game_state->grid[*cursor_y][*cursor_x].unit;
                        game_state->turn_state = TURN_STATE_SELECT_MOVE;
                        clear_highlights(game_state);
                        highlight_movement_range(game_state, game_state->selected_unit);
                        
                        sprintf(game_state->status_message, "Select where to move");
                    }
                    break;
                    
                case TURN_STATE_SELECT_MOVE:
                    // Move selected unit to cursor position if valid
                    if (game_state->grid[*cursor_y][*cursor_x].highlight == 1) {
                        int prev_x = game_state->selected_unit->x;
                        int prev_y = game_state->selected_unit->y;
                        
                        if (move_unit(game_state, game_state->selected_unit, *cursor_x, *cursor_y)) {
                            game_state->turn_state = TURN_STATE_SELECT_ACTION;
                            clear_highlights(game_state);
                            highlight_attack_range(game_state, game_state->selected_unit);
                            
                            sprintf(game_state->status_message, "Select action");
                        }
                    }
                    break;
                    
                case TURN_STATE_SELECT_ACTION:
                    // Show action menu
                    show_action_menu(game_state, game_state->selected_unit);
                    break;
                    
                case TURN_STATE_SELECT_TARGET:
                    // Attack target under cursor if valid
                    if (game_state->grid[*cursor_y][*cursor_x].highlight == 2 &&
                        game_state->grid[*cursor_y][*cursor_x].unit &&
                        game_state->grid[*cursor_y][*cursor_x].unit->player_id != game_state->current_player) {
                        
                        Unit* target = game_state->grid[*cursor_y][*cursor_x].unit;
                        
                        // Make sure the target is still active
                        if (target && target->is_active) {
                            // Animate the attack
                            animate_attack(game_state, game_state->selected_unit, target);
                            
                            // Perform the attack
                            int damage = attack_unit(game_state, game_state->selected_unit, target);
                            if (damage > 0) {
                                // Attack was successful, show damage and automatically end turn
                                refresh();
                                napms(800); // Pause for 0.8 seconds to show damage message
                                
                                // Store the current player for the message
                                int prev_player = game_state->current_player;
                                
                                // End the turn automatically
                                end_turn(game_state);
                                
                                // Display message about turn ending
                                sprintf(game_state->status_message, "Turn ended. Player %d's turn now", 
                                       prev_player == PLAYER_1 ? 2 : 1);
                                
                                refresh();
                                napms(800); // Pause for 0.8 seconds to show turn switch message
                            } else {
                                sprintf(game_state->status_message, "Attack failed! Target out of range.");
                                game_state->selected_unit = NULL;
                                game_state->turn_state = TURN_STATE_SELECT_UNIT;
                                clear_highlights(game_state);
                            }
                        } else {
                            sprintf(game_state->status_message, "Invalid target!");
                            game_state->turn_state = TURN_STATE_SELECT_ACTION;
                            clear_highlights(game_state);
                            highlight_attack_range(game_state, game_state->selected_unit);
                        }
                    }
                    break;
            }
            break;
            
        case 27: // Escape key
            switch (game_state->turn_state) {
                case TURN_STATE_SELECT_UNIT:
                    // If in the unit selection state, go back to main menu
                    game_state->current_phase = PHASE_MAIN_MENU;
                    return; // Exit the battle phase
                
                case TURN_STATE_SELECT_MOVE:
                    game_state->selected_unit = NULL;
                    game_state->turn_state = TURN_STATE_SELECT_UNIT;
                    clear_highlights(game_state);
                    sprintf(game_state->status_message, "Select a unit");
                    break;
                    
                case TURN_STATE_SELECT_ACTION:
                    game_state->turn_state = TURN_STATE_SELECT_MOVE;
                    clear_highlights(game_state);
                    highlight_movement_range(game_state, game_state->selected_unit);
                    sprintf(game_state->status_message, "Select where to move");
                    break;
                    
                case TURN_STATE_SELECT_TARGET:
                    game_state->turn_state = TURN_STATE_SELECT_ACTION;
                    clear_highlights(game_state);
                    highlight_attack_range(game_state, game_state->selected_unit);
                    sprintf(game_state->status_message, "Select action");
                    break;
            }
            break;
            
        case 'q': // Quit game
        case 'Q':
            // Go back to main menu
            game_state->current_phase = PHASE_MAIN_MENU;
            return; // Exit the battle phase
            
        case 'e': // End turn
            if (game_state->turn_state == TURN_STATE_SELECT_UNIT) {
                end_turn(game_state);
                sprintf(game_state->status_message, "Player %d's turn", game_state->current_player + 1);
            }
            break;
            
        case 's': // Save game
            save_game(game_state, "savegame.dat");
            sprintf(game_state->status_message, "Game saved");
            break;
            
        case 'l': // Load game
            if (load_game(game_state, "savegame.dat")) {
                sprintf(game_state->status_message, "Game loaded");
                *cursor_x = 0;
                *cursor_y = 0;
            } else {
                sprintf(game_state->status_message, "Failed to load game");
            }
            break;
            
        case 'h': // Show help
            // TODO: Implement help screen
            break;
    }
}

// Show action menu for a unit
void show_action_menu(GameState* game_state, Unit* unit) {
    // Clear the existing status message area
    int panel_start_x = GRID_WIDTH * CELL_WIDTH + 3;
    int panel_start_y = 1 + GRID_HEIGHT * CELL_HEIGHT - 8; // Extended for more options
    
    attron(COLOR_PAIR(COLOR_UI_TEXT));
    
    // Show action options
    mvprintw(panel_start_y, panel_start_x + 2, "----- ACTIONS -----");
    mvprintw(panel_start_y + 1, panel_start_x + 2, "1. Attack (auto-ends turn)");
    mvprintw(panel_start_y + 2, panel_start_x + 2, "2. Skip (keep unit selected)");
    mvprintw(panel_start_y + 3, panel_start_x + 2, "3. End Turn");
    mvprintw(panel_start_y + 4, panel_start_x + 2, "ESC: Go Back");
    
    attroff(COLOR_PAIR(COLOR_UI_TEXT));
    
    // Wait for action selection
    int ch = getch();
    
    switch (ch) {
        case '1': // Attack
            game_state->turn_state = TURN_STATE_SELECT_TARGET;
            clear_highlights(game_state);
            highlight_attack_range(game_state, unit);
            sprintf(game_state->status_message, "Select target to attack");
            break;
            
        case '2': // Skip
            game_state->selected_unit = NULL;
            game_state->turn_state = TURN_STATE_SELECT_UNIT;
            clear_highlights(game_state);
            sprintf(game_state->status_message, "Select a unit");
            break;
            
        case '3': // End turn
            sprintf(game_state->status_message, "Turn ended. Player %d's turn now", 
                   (game_state->current_player == PLAYER_1) ? 2 : 1);
            end_turn(game_state);
            refresh();
            napms(1000); // Pause for 1 second to show turn switch message
            break;
            
        case 27: // ESC - Go back to move selection
            game_state->turn_state = TURN_STATE_SELECT_MOVE;
            clear_highlights(game_state);
            highlight_movement_range(game_state, game_state->selected_unit);
            sprintf(game_state->status_message, "Select where to move");
            break;
            
        default:
            // Stay in the same state if invalid key pressed
            break;
    }
}

// Animate an attack between units
void animate_attack(GameState* game_state, Unit* attacker, Unit* target) {
    // Check for NULL pointers
    if (!game_state || !attacker || !target) {
        return;
    }
    
    // Make sure the target has valid coordinates
    if (target->x < 0 || target->x >= GRID_WIDTH || target->y < 0 || target->y >= GRID_HEIGHT) {
        return;
    }
    
    // Flash the target cell a few times
    for (int i = 0; i < 3; i++) {
        // Highlight the target
        attron(COLOR_PAIR(COLOR_HIGHLIGHT_ATTACK) | A_BOLD);
        
        int start_y = target->y * CELL_HEIGHT + 1;
        int start_x = target->x * CELL_WIDTH + 1;
        
        for (int y = 0; y < CELL_HEIGHT; y++) {
            for (int x = 0; x < CELL_WIDTH; x++) {
                mvaddch(start_y + y, start_x + x, ' ');
            }
        }
        
        // Draw damage indication
        mvprintw(start_y + 1, start_x + 1, "HIT!");
        
        attroff(COLOR_PAIR(COLOR_HIGHLIGHT_ATTACK) | A_BOLD);
        
        refresh();
        napms(200); // Flash for 200ms
        
        // Redraw the normal cell - safely check if the cell is valid
        if (target->y >= 0 && target->y < GRID_HEIGHT && 
            target->x >= 0 && target->x < GRID_WIDTH) {
            draw_cell(target->y, target->x, &game_state->grid[target->y][target->x], 0);
        }
        refresh();
        napms(200); // Pause for 200ms
    }
}

// Run the battle phase
void run_battle(GameState* game_state) {
    // Check if terminal is large enough for battle UI
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Calculate minimum required size
    int min_width = GRID_WIDTH * CELL_WIDTH + INFO_PANEL_WIDTH + 5;
    int min_height = GRID_HEIGHT * CELL_HEIGHT + 2;
    
    if (max_y < min_height || max_x < min_width) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 1, "Terminal window too small for battle display!");
        mvprintw(3, 1, "Please resize your terminal to at least %dx%d characters", min_width, min_height);
        mvprintw(5, 1, "Press any key to return to main menu...");
        attroff(A_BOLD);
        refresh();
        getch();
        
        // Return to main menu
        game_state->current_phase = PHASE_MAIN_MENU;
        return;
    }
    
    // Initialize battle UI
    init_battle_ui();
    
    // Set up cursor position
    int cursor_x = GRID_WIDTH / 2;
    int cursor_y = GRID_HEIGHT / 2;
    
    // Initialize game state for battle
    game_state->current_phase = PHASE_BATTLE;
    game_state->turn_state = TURN_STATE_SELECT_UNIT;
    game_state->current_player = PLAYER_1;
    game_state->selected_unit = NULL;
    sprintf(game_state->status_message, "Player %d's turn", game_state->current_player + 1);
    
    // Main battle loop
    while (1) {
        draw_battle_ui(game_state, cursor_x, cursor_y);
        handle_battle_input(game_state, &cursor_x, &cursor_y);
        
        // Check for game over conditions
        int player1_units = 0, player2_units = 0;
        
        for (int i = 0; i < game_state->num_units[PLAYER_1]; i++) {
            if (game_state->player_units[PLAYER_1][i].is_active) {
                player1_units++;
            }
        }
        
        for (int i = 0; i < game_state->num_units[PLAYER_2]; i++) {
            if (game_state->player_units[PLAYER_2][i].is_active) {
                player2_units++;
            }
        }
        
        if (player1_units == 0 || player2_units == 0) {
            game_state->current_phase = PHASE_GAME_OVER;
            int winner = (player1_units > 0) ? PLAYER_1 : PLAYER_2;
            
            clear();
            attron(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
            mvprintw(LINES / 2, (COLS - 20) / 2, "GAME OVER");
            mvprintw(LINES / 2 + 1, (COLS - 30) / 2, "Player %d wins!", winner + 1);
            mvprintw(LINES / 2 + 3, (COLS - 30) / 2, "Press any key to continue...");
            attroff(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
            refresh();
            
            getch();
            break;
        }
    }
}
