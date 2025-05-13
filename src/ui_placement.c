#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game_state.h"

// Color pairs for the placement screen
#define COLOR_GRID_PLAIN 1
#define COLOR_GRID_FOREST 2
#define COLOR_GRID_MOUNTAIN 3
#define COLOR_PLAYER_1 4
#define COLOR_PLAYER_2 5
#define COLOR_HIGHLIGHT 6
#define COLOR_UI_TEXT 7

// Cell dimensions for the placement grid
#define CELL_WIDTH 5
#define CELL_HEIGHT 3

// Function prototypes
void init_placement_ui();
void draw_placement_grid(GameState* game_state, int cursor_x, int cursor_y);
void draw_placement_cell(int y, int x, Cell* cell, int is_cursor);
void draw_placement_panel(GameState* game_state, int current_player, int current_unit);
void draw_placement_ui(GameState* game_state, int cursor_x, int cursor_y, int current_player, int current_unit);
void handle_placement_input(GameState* game_state, int* cursor_x, int* cursor_y, int* current_player, int* current_unit);
int run_unit_placement(GameState* game_state);

// Initialize the placement UI
void init_placement_ui() {
    // Initialize color pairs
    init_pair(COLOR_GRID_PLAIN, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_GRID_FOREST, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_GRID_MOUNTAIN, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_PLAYER_1, COLOR_BLACK, COLOR_BLUE);
    init_pair(COLOR_PLAYER_2, COLOR_BLACK, COLOR_RED);
    init_pair(COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COLOR_UI_TEXT, COLOR_WHITE, COLOR_BLACK);
}

// Draw a single cell in the placement grid
void draw_placement_cell(int y, int x, Cell* cell, int is_cursor) {
    int start_y = y * CELL_HEIGHT + 1;
    int start_x = x * CELL_WIDTH + 1;
    int color_pair;
    
    // Determine the color of the cell based on its content
    if (cell->unit) {
        if (cell->unit->player_id == PLAYER_1) {
            color_pair = COLOR_PLAYER_1;
        } else {
            color_pair = COLOR_PLAYER_2;
        }
    } else {
        switch (cell->terrain_type) {
            case 0: color_pair = COLOR_GRID_PLAIN; break;
            case 1: color_pair = COLOR_GRID_FOREST; break;
            case 2: color_pair = COLOR_GRID_MOUNTAIN; break;
            default: color_pair = COLOR_GRID_PLAIN;
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

// Draw the placement grid
void draw_placement_grid(GameState* game_state, int cursor_x, int cursor_y) {
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
    
    // Draw placement areas
    attron(COLOR_PAIR(COLOR_PLAYER_1) | A_BOLD);
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int start_y = y * CELL_HEIGHT + 1;
            int start_x = x * CELL_WIDTH + 1;
            
            mvaddch(start_y, start_x, '1');
        }
    }
    attroff(COLOR_PAIR(COLOR_PLAYER_1) | A_BOLD);
    
    attron(COLOR_PAIR(COLOR_PLAYER_2) | A_BOLD);
    for (int y = GRID_HEIGHT - 2; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int start_y = y * CELL_HEIGHT + 1;
            int start_x = x * CELL_WIDTH + 1;
            
            mvaddch(start_y, start_x, '2');
        }
    }
    attroff(COLOR_PAIR(COLOR_PLAYER_2) | A_BOLD);
    
    // Draw each cell
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            draw_placement_cell(y, x, &game_state->grid[y][x], (x == cursor_x && y == cursor_y));
        }
    }
}

// Draw the info panel for unit placement
void draw_placement_panel(GameState* game_state, int current_player, int current_unit) {
    int panel_start_x = GRID_WIDTH * CELL_WIDTH + 3;
    int panel_start_y = 1;
    
    // Draw panel border
    attron(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
    mvaddch(panel_start_y, panel_start_x, ACS_ULCORNER);
    mvaddch(panel_start_y, panel_start_x + 30, ACS_URCORNER);
    mvaddch(panel_start_y + GRID_HEIGHT * CELL_HEIGHT, panel_start_x, ACS_LLCORNER);
    mvaddch(panel_start_y + GRID_HEIGHT * CELL_HEIGHT, panel_start_x + 30, ACS_LRCORNER);
    
    for (int i = 1; i < 30; i++) {
        mvaddch(panel_start_y, panel_start_x + i, ACS_HLINE);
        mvaddch(panel_start_y + GRID_HEIGHT * CELL_HEIGHT, panel_start_x + i, ACS_HLINE);
    }
    
    for (int i = 1; i < GRID_HEIGHT * CELL_HEIGHT; i++) {
        mvaddch(panel_start_y + i, panel_start_x, ACS_VLINE);
        mvaddch(panel_start_y + i, panel_start_x + 30, ACS_VLINE);
    }
    
    // Draw panel title
    mvprintw(panel_start_y + 1, panel_start_x + 2, "UNIT PLACEMENT");
    mvaddch(panel_start_y + 2, panel_start_x, ACS_LTEE);
    for (int i = 1; i < 30; i++) {
        mvaddch(panel_start_y + 2, panel_start_x + i, ACS_HLINE);
    }
    mvaddch(panel_start_y + 2, panel_start_x + 30, ACS_RTEE);
    
    // Draw current player info
    mvprintw(panel_start_y + 4, panel_start_x + 2, "Player %d's Turn", current_player + 1);
    
    // Draw current unit info
    if (current_unit < game_state->num_units[current_player]) {
        Unit* unit = &game_state->player_units[current_player][current_unit];
        
        mvprintw(panel_start_y + 6, panel_start_x + 2, "Placing Unit %d:", current_unit + 1);
        mvprintw(panel_start_y + 7, panel_start_x + 2, "Name: %s", unit->name);
        
        const char* unit_type_names[NUM_UNIT_TYPES] = {
            "Warrior",
            "Archer",
            "Mage",
            "Scout",
            "Tank"
        };
        
        mvprintw(panel_start_y + 8, panel_start_x + 2, "Type: %s", unit_type_names[unit->type]);
        mvprintw(panel_start_y + 9, panel_start_x + 2, "HP: %d", unit->max_hp);
        mvprintw(panel_start_y + 10, panel_start_x + 2, "Move Range: %d", unit->move_range);
        mvprintw(panel_start_y + 11, panel_start_x + 2, "Attack Range: %d", unit->attack_range);
    }
    
    // Draw instructions
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 4, panel_start_x + 2, "Use arrow keys to move cursor");
    mvprintw(panel_start_y + GRID_HEIGHT * CELL_HEIGHT - 3, panel_start_x + 2, "Press ENTER to place unit");
    attroff(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
}

// Draw the complete placement UI
void draw_placement_ui(GameState* game_state, int cursor_x, int cursor_y, int current_player, int current_unit) {
    draw_placement_grid(game_state, cursor_x, cursor_y);
    draw_placement_panel(game_state, current_player, current_unit);
    
    // Draw title
    attron(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
    mvprintw(0, (COLS - 20) / 2, "UNIT PLACEMENT PHASE");
    attroff(COLOR_PAIR(COLOR_UI_TEXT) | A_BOLD);
    
    refresh();
}

// Handle input during unit placement
void handle_placement_input(GameState* game_state, int* cursor_x, int* cursor_y, int* current_player, int* current_unit) {
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
        case 10: // Enter key - place unit
            // Check if placement is valid
            if (game_state->grid[*cursor_y][*cursor_x].unit == NULL) {
                // For Player 1, placement is valid in top 2 rows
                if (*current_player == PLAYER_1 && *cursor_y < 2) {
                    Unit* unit = &game_state->player_units[*current_player][*current_unit];
                    place_unit(game_state, unit, *cursor_x, *cursor_y);
                    (*current_unit)++;
                    
                    // Check if all units for this player have been placed
                    if (*current_unit >= game_state->num_units[*current_player]) {
                        *current_player = PLAYER_2;
                        *current_unit = 0;
                    }
                }
                // For Player 2, placement is valid in bottom 2 rows
                else if (*current_player == PLAYER_2 && *cursor_y >= GRID_HEIGHT - 2) {
                    Unit* unit = &game_state->player_units[*current_player][*current_unit];
                    place_unit(game_state, unit, *cursor_x, *cursor_y);
                    (*current_unit)++;
                    
                    // Check if all units for this player have been placed
                    if (*current_unit >= game_state->num_units[*current_player]) {
                        // All units placed, start the battle
                        game_state->current_phase = PHASE_BATTLE;
                    }
                }
            }
            break;
    }
}

// Run the unit placement phase
int run_unit_placement(GameState* game_state) {
    // Check if terminal is large enough for placement UI
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Calculate minimum required size (similar to battle UI)
    int min_width = GRID_WIDTH * CELL_WIDTH + 5;
    int min_height = GRID_HEIGHT * CELL_HEIGHT + 2;
    
    if (max_y < min_height || max_x < min_width) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 1, "Terminal window too small for unit placement!");
        mvprintw(3, 1, "Please resize your terminal to at least %dx%d characters", min_width, min_height);
        mvprintw(5, 1, "Press any key to return to main menu...");
        attroff(A_BOLD);
        refresh();
        getch();
        
        // Return to main menu
        return 0;
    }
    
    int cursor_x = GRID_WIDTH / 2;
    int cursor_y = 0; // Start at top for Player 1
    int current_player = PLAYER_1;
    int current_unit = 0;
    
    init_placement_ui();
    
    // Initialize grid
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            game_state->grid[y][x].unit = NULL;
            game_state->grid[y][x].terrain_type = 0; // Default to plain terrain
            game_state->grid[y][x].highlight = 0;
        }
    }
    
    // Add some random terrain variations
    for (int i = 0; i < 10; i++) {
        int x = rand() % GRID_WIDTH;
        int y = 2 + rand() % (GRID_HEIGHT - 4); // Keep placement areas clear
        game_state->grid[y][x].terrain_type = 1; // Forest
    }
    
    for (int i = 0; i < 5; i++) {
        int x = rand() % GRID_WIDTH;
        int y = 2 + rand() % (GRID_HEIGHT - 4); // Keep placement areas clear
        game_state->grid[y][x].terrain_type = 2; // Mountain
    }
    
    game_state->current_phase = PHASE_UNIT_PLACEMENT;
    
    // Main placement loop
    while (game_state->current_phase == PHASE_UNIT_PLACEMENT) {
        draw_placement_ui(game_state, cursor_x, cursor_y, current_player, current_unit);
        handle_placement_input(game_state, &cursor_x, &cursor_y, &current_player, &current_unit);
        
        // When switching to Player 2, move cursor to the bottom
        if (current_player == PLAYER_2 && cursor_y < GRID_HEIGHT - 2) {
            cursor_y = GRID_HEIGHT - 1;
        }
    }
    
    return 1;
} 