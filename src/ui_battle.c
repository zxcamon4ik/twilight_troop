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
void draw_grid(GameState* g, int cx, int cy);
void draw_cell(int y, int x, Cell* cell, int is_cursor);
void draw_unit(int y, int x, Unit* unit);
void draw_info_panel(GameState* game_state);
void draw_battle_ui(GameState* g, int cx, int cy);
void handle_battle_input(GameState* game_state, int* cursor_x, int* cursor_y);
void show_action_menu(GameState* game_state, Unit* unit);
void animate_attack(GameState* game_state, Unit* attacker, Unit* target);
void show_movement_preview(GameState* game_state, Unit* unit, int new_x, int new_y);
void run_battle(GameState* g);

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
    int sy = y * CELL_HEIGHT + 1, sx = x * CELL_WIDTH + 1;
    
    static const int terrain_colors[] = {COLOR_GRID_PLAIN, COLOR_GRID_FOREST, COLOR_GRID_MOUNTAIN};
    int color = cell->unit ? 
                (cell->unit->player_id == PLAYER_1 ? COLOR_PLAYER_1 : COLOR_PLAYER_2) :
                cell->highlight == 1 ? COLOR_HIGHLIGHT_MOVE :
                cell->highlight == 2 ? COLOR_HIGHLIGHT_ATTACK :
                (cell->terrain_type < 3) ? terrain_colors[cell->terrain_type] : COLOR_GRID_PLAIN;
    
    attron(COLOR_PAIR(color));
    
    // Draw cell background
    for (int i = 0; i < CELL_HEIGHT; i++)
        for (int j = 0; j < CELL_WIDTH; j++)
            mvaddch(sy + i, sx + j, ' ');
    
    // Draw unit if present
    if (cell->unit) {
        static const char unit_chars[] = {'W', 'A', 'M', 'S', 'T'};
        mvaddch(sy + 1, sx + 2, (cell->unit->type < NUM_UNIT_TYPES) ? 
                unit_chars[cell->unit->type] : '?');
        mvprintw(sy + 2, sx + 1, "%d", cell->unit->current_hp);
    }
    
    // Draw cursor
    if (is_cursor) {
        attron(A_BOLD);
        mvhline(sy, sx, ACS_HLINE, CELL_WIDTH);
        mvhline(sy + CELL_HEIGHT - 1, sx, ACS_HLINE, CELL_WIDTH);
        mvvline(sy, sx, ACS_VLINE, CELL_HEIGHT);
        mvvline(sy, sx + CELL_WIDTH - 1, ACS_VLINE, CELL_HEIGHT);
        mvaddch(sy, sx, ACS_ULCORNER);
        mvaddch(sy, sx + CELL_WIDTH - 1, ACS_URCORNER);
        mvaddch(sy + CELL_HEIGHT - 1, sx, ACS_LLCORNER);
        mvaddch(sy + CELL_HEIGHT - 1, sx + CELL_WIDTH - 1, ACS_LRCORNER);
        attroff(A_BOLD);
    }
    
    attroff(COLOR_PAIR(color));
}

// Draw the entire grid
void draw_grid(GameState* g, int cx, int cy) {
    clear();
    
    // Draw coordinates
    attron(COLOR_PAIR(COLOR_UI_TEXT));
    for (int x=0; x<GRID_WIDTH; x++) mvprintw(0, x*CELL_WIDTH+3, "%d", x);
    for (int y=0; y<GRID_HEIGHT; y++) mvprintw(y*CELL_HEIGHT+2, 0, "%d", y);
    attroff(COLOR_PAIR(COLOR_UI_TEXT));
    
    // Draw cells
    for (int y=0; y<GRID_HEIGHT; y++)
        for (int x=0; x<GRID_WIDTH; x++)
            draw_cell(y, x, &g->grid[y][x], (x==cx && y==cy));
}

// Draw the info panel
void draw_info_panel(GameState* game_state) {
    int px = GRID_WIDTH * CELL_WIDTH + 3, py = 1, h = GRID_HEIGHT * CELL_HEIGHT;
    
    // Draw panel border
    attron(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
    // Draw corners
    mvaddch(py, px, ACS_ULCORNER); mvaddch(py, px + INFO_PANEL_WIDTH, ACS_URCORNER);
    mvaddch(py+h, px, ACS_LLCORNER); mvaddch(py+h, px + INFO_PANEL_WIDTH, ACS_LRCORNER);
    
    mvhline(py, px+1, ACS_HLINE, INFO_PANEL_WIDTH-1);
    mvhline(py+h, px+1, ACS_HLINE, INFO_PANEL_WIDTH-1);
    mvvline(py+1, px, ACS_VLINE, h-1);
    mvvline(py+1, px+INFO_PANEL_WIDTH, ACS_VLINE, h-1);
    
    // Draw title & divider
    mvprintw(py+1, px+2, "BATTLE INFO");
    mvaddch(py+2, px, ACS_LTEE);
    mvhline(py+2, px+1, ACS_HLINE, INFO_PANEL_WIDTH-1);
    mvaddch(py+2, px+INFO_PANEL_WIDTH, ACS_RTEE);
    
    // Draw player turn
    mvprintw(py+4, px+2, "TURN: Player %d", game_state->current_player + 1);
    
    // Draw selected unit info
    if (game_state->selected_unit) {
        int l = py+6;
        Unit* u = game_state->selected_unit;
        
        mvprintw(l++, px+2, "SELECTED UNIT:");
        mvprintw(l++, px+2, "%s (Player %d)", u->name, u->player_id+1);
        mvprintw(l++, px+2, "HP: %d/%d", u->current_hp, u->max_hp);
        mvprintw(l++, px+2, "Move: %d  Attack: %d", u->move_range, u->attack_range);
        
        // Equipment
        mvprintw(l+=2, px+2, "EQUIPMENT:");
        if (u->equipment[EQUIP_WEAPON])
            mvprintw(++l, px+2, "Weapon: %s", u->equipment[EQUIP_WEAPON]->name);
        if (u->equipment[EQUIP_ARMOR])
            mvprintw(++l, px+2, "Armor: %s", u->equipment[EQUIP_ARMOR]->name);
        if (u->equipment[EQUIP_ACCESSORY])
            mvprintw(++l, px+2, "Acc: %s", u->equipment[EQUIP_ACCESSORY]->name);
    }
    
    // Status message
    mvprintw(py+h-2, px+2, "%.40s", game_state->status_message);
    attroff(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
}

// Draw the complete battle UI
void draw_battle_ui(GameState* g, int cx, int cy) {
    draw_grid(g, cx, cy);
    draw_info_panel(g);
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
    int px = GRID_WIDTH * CELL_WIDTH + 3;
    int py = 1 + GRID_HEIGHT * CELL_HEIGHT - 8; // Extended for more options
    
    attron(COLOR_PAIR(COLOR_UI_TEXT));
    
    // Show action options
    mvprintw(py, px + 2, "----- ACTIONS -----");
    mvprintw(py + 1, px + 2, "1. Attack (auto-ends turn)");
    mvprintw(py + 2, px + 2, "2. Skip (keep unit selected)");
    mvprintw(py + 3, px + 2, "3. End Turn");
    mvprintw(py + 4, px + 2, "ESC: Go Back");
    
    attroff(COLOR_PAIR(COLOR_UI_TEXT));
    
    // Wait for action selection
    switch (getch()) {
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
    }
}

// Animate an attack between units
void animate_attack(GameState* game_state, Unit* attacker, Unit* target) {
    if (!game_state || !attacker || !target || 
        target->x < 0 || target->x >= GRID_WIDTH || 
        target->y < 0 || target->y >= GRID_HEIGHT)
        return;
    
    int sy = target->y * CELL_HEIGHT + 1, sx = target->x * CELL_WIDTH + 1;
    
    for (int i = 0; i < 3; i++) {
        attron(COLOR_PAIR(COLOR_HIGHLIGHT_ATTACK) | A_BOLD);
        
        // Flash cell
        for (int y = 0; y < CELL_HEIGHT; y++)
            for (int x = 0; x < CELL_WIDTH; x++)
                mvaddch(sy + y, sx + x, ' ');
        
        mvprintw(sy + 1, sx + 1, "HIT!");
        attroff(COLOR_PAIR(COLOR_HIGHLIGHT_ATTACK) | A_BOLD);
        refresh(); napms(200);
        
        // Redraw normal cell
        draw_cell(target->y, target->x, &game_state->grid[target->y][target->x], 0);
        refresh(); napms(200);
    }
}

// Run the battle phase
void run_battle(GameState* g) {
    // Check terminal size
    int my, mx, min_w = GRID_WIDTH*CELL_WIDTH+INFO_PANEL_WIDTH+5, min_h = GRID_HEIGHT*CELL_HEIGHT+2;
    getmaxyx(stdscr, my, mx);
    
    if (my < min_h || mx < min_w) {
        clear(); attron(A_BOLD);
        mvprintw(1, 1, "Terminal window too small for battle display!");
        mvprintw(3, 1, "Please resize to at least %dx%d characters", min_w, min_h);
        mvprintw(5, 1, "Press any key to return to main menu...");
        attroff(A_BOLD); refresh(); getch();
        g->current_phase = PHASE_MAIN_MENU;
        return;
    }
    
    // Initialize
    init_battle_ui();
    int cx = GRID_WIDTH/2, cy = GRID_HEIGHT/2;
    
    g->current_phase = PHASE_BATTLE;
    g->turn_state = TURN_STATE_SELECT_UNIT;
    g->current_player = PLAYER_1;
    g->selected_unit = NULL;
    sprintf(g->status_message, "Player %d's turn", g->current_player+1);
    
    // Main loop
    while (1) {
        draw_battle_ui(g, cx, cy);
        handle_battle_input(g, &cx, &cy);
        
        // Check for game over
        int alive[2] = {0};
        for (int p=0; p<NUM_PLAYERS; p++)
            for (int i=0; i<g->num_units[p]; i++)
                if (g->player_units[p][i].is_active) alive[p]++;
        
        if (!alive[0] || !alive[1]) {
            int w = alive[0] ? PLAYER_1 : PLAYER_2;
            clear(); attron(COLOR_PAIR(COLOR_UI_TEXT)|A_BOLD);
            mvprintw(LINES/2, (COLS-20)/2, "GAME OVER");
            mvprintw(LINES/2+1, (COLS-30)/2, "Player %d wins!", w+1);
            mvprintw(LINES/2+3, (COLS-30)/2, "Press any key to continue...");
            attroff(COLOR_PAIR(COLOR_UI_TEXT)|A_BOLD); refresh();
            g->current_phase = PHASE_GAME_OVER;
            getch(); break;
        }
    }
}
