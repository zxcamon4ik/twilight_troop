#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game_state.h"

// Color pairs for the unit selection menu
#define COLOR_TITLE 1
#define COLOR_NORMAL_TEXT 2
#define COLOR_SELECTED_UNIT 3
#define COLOR_SELECTED_ITEM 4
#define COLOR_HIGHLIGHT 5

// Unit selection menu structure
typedef struct {
    int selected_unit_type;
    int selected_item_idx[NUM_EQUIP_SLOTS];
    int current_slot;
    int current_player;
    int num_selected_units;
    Unit selected_units[MAX_UNITS_PER_PLAYER];
} UnitSelectionMenu;

// Function prototypes
void init_unit_select_ui();
void draw_unit_select_ui(UnitSelectionMenu* menu);
void handle_unit_select_input(UnitSelectionMenu* menu, GameState* game_state);
void draw_unit_types(UnitSelectionMenu* menu, int start_y, int start_x);
void draw_equipment_slots(UnitSelectionMenu* menu, int start_y, int start_x);
void draw_item_list(UnitSelectionMenu* menu, int start_y, int start_x);
void draw_selected_units(UnitSelectionMenu* menu, int start_y, int start_x);
void add_selected_unit(UnitSelectionMenu* menu);
void create_unit_from_selection(UnitSelectionMenu* menu, Unit* unit);
int run_unit_selection(GameState* game_state);

// Initialize the unit selection UI
void init_unit_select_ui() {
    // Initialize color pairs
    init_pair(COLOR_TITLE, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_NORMAL_TEXT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_SELECTED_UNIT, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_SELECTED_ITEM, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
}

// Draw the unit types list
void draw_unit_types(UnitSelectionMenu* menu, int start_y, int start_x) {
    attron(COLOR_PAIR(COLOR_NORMAL_TEXT) | A_BOLD);
    mvprintw(start_y, start_x, "UNIT TYPES:");
    attroff(A_BOLD);
    
    const char* unit_type_names[NUM_UNIT_TYPES] = {
        "Warrior",
        "Archer",
        "Mage",
        "Scout",
        "Tank"
    };
    
    for (int i = 0; i < NUM_UNIT_TYPES; i++) {
        if (i == menu->selected_unit_type) {
            attron(COLOR_PAIR(COLOR_SELECTED_UNIT));
        } else {
            attron(COLOR_PAIR(COLOR_NORMAL_TEXT));
        }
        
        mvprintw(start_y + i + 1, start_x, "%s", unit_type_names[i]);
        
        if (i == menu->selected_unit_type) {
            attroff(COLOR_PAIR(COLOR_SELECTED_UNIT));
        } else {
            attroff(COLOR_PAIR(COLOR_NORMAL_TEXT));
        }
    }
}

// Draw the equipment slots
void draw_equipment_slots(UnitSelectionMenu* menu, int start_y, int start_x) {
    attron(COLOR_PAIR(COLOR_NORMAL_TEXT) | A_BOLD);
    mvprintw(start_y, start_x, "EQUIPMENT:");
    attroff(A_BOLD);
    
    const char* slot_names[NUM_EQUIP_SLOTS] = {
        "Weapon",
        "Armor",
        "Accessory"
    };
    
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
        if (i == menu->current_slot) {
            attron(COLOR_PAIR(COLOR_SELECTED_ITEM));
        } else {
            attron(COLOR_PAIR(COLOR_NORMAL_TEXT));
        }
        
        mvprintw(start_y + i + 1, start_x, "%s:", slot_names[i]);
        
        // Display selected item for this slot
        if (menu->selected_item_idx[i] >= 0 && menu->selected_item_idx[i] < NUMBER_OF_ITEMS) {
            mvprintw(start_y + i + 1, start_x + 10, "%s", items[menu->selected_item_idx[i]].name);
        } else {
            mvprintw(start_y + i + 1, start_x + 10, "None");
        }
        
        if (i == menu->current_slot) {
            attroff(COLOR_PAIR(COLOR_SELECTED_ITEM));
        } else {
            attroff(COLOR_PAIR(COLOR_NORMAL_TEXT));
        }
    }
}

// Draw the item list
void draw_item_list(UnitSelectionMenu* menu, int start_y, int start_x) {
    attron(COLOR_PAIR(COLOR_NORMAL_TEXT) | A_BOLD);
    mvprintw(start_y, start_x, "AVAILABLE ITEMS:");
    attroff(A_BOLD);
    
    // Only show items for the current slot
    for (int i = 0; i < NUMBER_OF_ITEMS; i++) {
        if (i == menu->selected_item_idx[menu->current_slot]) {
            attron(COLOR_PAIR(COLOR_SELECTED_ITEM));
        } else {
            attron(COLOR_PAIR(COLOR_NORMAL_TEXT));
        }
        
        mvprintw(start_y + i + 1, start_x, "%-15s ATT:%-2d DEF:%-2d", 
                 items[i].name, items[i].att, items[i].def);
        
        if (i == menu->selected_item_idx[menu->current_slot]) {
            attroff(COLOR_PAIR(COLOR_SELECTED_ITEM));
        } else {
            attroff(COLOR_PAIR(COLOR_NORMAL_TEXT));
        }
    }
}

// Draw the list of selected units
void draw_selected_units(UnitSelectionMenu* menu, int start_y, int start_x) {
    attron(COLOR_PAIR(COLOR_NORMAL_TEXT) | A_BOLD);
    mvprintw(start_y, start_x, "SELECTED UNITS (Player %d): %d/%d", 
             menu->current_player + 1, menu->num_selected_units, MAX_UNITS_PER_PLAYER);
    attroff(A_BOLD);
    
    for (int i = 0; i < menu->num_selected_units; i++) {
        const char* unit_type_names[NUM_UNIT_TYPES] = {
            "Warrior",
            "Archer",
            "Mage",
            "Scout",
            "Tank"
        };
        
        attron(COLOR_PAIR(COLOR_NORMAL_TEXT));
        mvprintw(start_y + i + 1, start_x, "Unit %d: %s - %s", 
                 i + 1, menu->selected_units[i].name, unit_type_names[menu->selected_units[i].type]);
        attroff(COLOR_PAIR(COLOR_NORMAL_TEXT));
    }
}

// Draw the complete unit selection UI
void draw_unit_select_ui(UnitSelectionMenu* menu) {
    clear();
    
    // Draw title
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvprintw(1, (COLS - 20) / 2, "UNIT SELECTION MENU");
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    
    // Draw sections
    draw_unit_types(menu, 3, 2);
    draw_equipment_slots(menu, 3, 30);
    draw_item_list(menu, 3, 60);
    draw_selected_units(menu, 15, 2);
    
    // Draw controls help
    attron(COLOR_PAIR(COLOR_NORMAL_TEXT));
    mvprintw(LINES - 4, 2, "Controls:");
    mvprintw(LINES - 3, 2, "UP/DOWN: Navigate | TAB: Switch section | ENTER: Select");
    mvprintw(LINES - 2, 2, "A: Add unit | N: Next player | SPACE: Start game");
    attroff(COLOR_PAIR(COLOR_NORMAL_TEXT));
    
    refresh();
}

// Create a new unit based on current selection
void create_unit_from_selection(UnitSelectionMenu* menu, Unit* unit) {
    const char* unit_type_names[NUM_UNIT_TYPES] = {
        "Warrior",
        "Archer",
        "Mage",
        "Scout",
        "Tank"
    };
    
    // Set unit name based on type
    sprintf(unit->name, "%s_%d", unit_type_names[menu->selected_unit_type], menu->num_selected_units + 1);
    
    // Set unit type
    unit->type = menu->selected_unit_type;
    
    // Set unit stats based on type
    switch (unit->type) {
        case UNIT_TYPE_WARRIOR:
            unit->max_hp = 100;
            unit->move_range = 3;
            unit->attack_range = 1;
            break;
        case UNIT_TYPE_ARCHER:
            unit->max_hp = 70;
            unit->move_range = 3;
            unit->attack_range = 4;
            break;
        case UNIT_TYPE_MAGE:
            unit->max_hp = 60;
            unit->move_range = 2;
            unit->attack_range = 3;
            break;
        case UNIT_TYPE_SCOUT:
            unit->max_hp = 80;
            unit->move_range = 5;
            unit->attack_range = 1;
            break;
        case UNIT_TYPE_TANK:
            unit->max_hp = 150;
            unit->move_range = 2;
            unit->attack_range = 1;
            break;
    }
    
    // Set current HP
    unit->current_hp = unit->max_hp;
    
    // Set equipment
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
        if (menu->selected_item_idx[i] >= 0 && menu->selected_item_idx[i] < NUMBER_OF_ITEMS) {
            unit->equipment[i] = &items[menu->selected_item_idx[i]];
        } else {
            unit->equipment[i] = NULL;
        }
    }
    
    // Set player ID
    unit->player_id = menu->current_player;
    
    // Unit starts active
    unit->is_active = 1;
    
    // Position will be set during placement phase
    unit->x = -1;
    unit->y = -1;
}

// Add the current unit selection to the list
void add_selected_unit(UnitSelectionMenu* menu) {
    if (menu->num_selected_units < MAX_UNITS_PER_PLAYER) {
        create_unit_from_selection(menu, &menu->selected_units[menu->num_selected_units]);
        menu->num_selected_units++;
    }
}

// Handle input in the unit selection screen
void handle_unit_select_input(UnitSelectionMenu* menu, GameState* game_state) {
    int ch = getch();
    
    static int current_section = 0; // 0 = unit types, 1 = equipment slots, 2 = item list
    
    switch (ch) {
        case 9: // Tab key - switch section
            current_section = (current_section + 1) % 3;
            break;
            
        case KEY_UP:
            switch (current_section) {
                case 0: // Unit types
                    if (menu->selected_unit_type > 0) {
                        menu->selected_unit_type--;
                    }
                    break;
                case 1: // Equipment slots
                    if (menu->current_slot > 0) {
                        menu->current_slot--;
                    }
                    break;
                case 2: // Item list
                    if (menu->selected_item_idx[menu->current_slot] > 0) {
                        menu->selected_item_idx[menu->current_slot]--;
                    }
                    break;
            }
            break;
            
        case KEY_DOWN:
            switch (current_section) {
                case 0: // Unit types
                    if (menu->selected_unit_type < NUM_UNIT_TYPES - 1) {
                        menu->selected_unit_type++;
                    }
                    break;
                case 1: // Equipment slots
                    if (menu->current_slot < NUM_EQUIP_SLOTS - 1) {
                        menu->current_slot++;
                    }
                    break;
                case 2: // Item list
                    if (menu->selected_item_idx[menu->current_slot] < NUMBER_OF_ITEMS - 1) {
                        menu->selected_item_idx[menu->current_slot]++;
                    }
                    break;
            }
            break;
            
        case 'a': // Add unit
        case 'A':
            add_selected_unit(menu);
            break;
            
        case 'n': // Next player
        case 'N':
            if (menu->current_player == PLAYER_1 && menu->num_selected_units > 0) {
                // Copy selected units to game state
                for (int i = 0; i < menu->num_selected_units; i++) {
                    game_state->player_units[menu->current_player][i] = menu->selected_units[i];
                }
                game_state->num_units[menu->current_player] = menu->num_selected_units;
                
                // Switch to next player
                menu->current_player = PLAYER_2;
                menu->num_selected_units = 0;
                menu->selected_unit_type = 0;
                for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
                    menu->selected_item_idx[i] = -1;
                }
            }
            break;
            
        case ' ': // Space - start game
            if (menu->current_player == PLAYER_2 && menu->num_selected_units > 0) {
                // Copy selected units to game state
                for (int i = 0; i < menu->num_selected_units; i++) {
                    game_state->player_units[menu->current_player][i] = menu->selected_units[i];
                }
                game_state->num_units[menu->current_player] = menu->num_selected_units;
                
                // Go to unit placement phase
                game_state->current_phase = PHASE_UNIT_PLACEMENT;
                return;
            }
            break;
    }
}

// Run the unit selection phase
int run_unit_selection(GameState* game_state) {
    // Check if terminal is large enough for unit selection UI
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Unit selection needs less space than battle, but still needs a decent size
    int min_width = 70;  // Enough for unit and item lists side by side
    int min_height = 24; // Enough for headers, lists, and controls
    
    if (max_y < min_height || max_x < min_width) {
        clear();
        attron(A_BOLD);
        mvprintw(1, 1, "Terminal window too small for unit selection!");
        mvprintw(3, 1, "Please resize your terminal to at least %dx%d characters", min_width, min_height);
        mvprintw(5, 1, "Press any key to return to main menu...");
        attroff(A_BOLD);
        refresh();
        getch();
        
        // Return to main menu
        return 0;
    }
    
    UnitSelectionMenu menu;
    
    // Initialize menu
    menu.selected_unit_type = 0;
    menu.current_slot = 0;
    menu.current_player = PLAYER_1;
    menu.num_selected_units = 0;
    
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
        menu.selected_item_idx[i] = -1; // -1 means no item selected
    }
    
    init_unit_select_ui();
    game_state->current_phase = PHASE_UNIT_SELECTION;
    
    // Main unit selection loop
    while (game_state->current_phase == PHASE_UNIT_SELECTION) {
        draw_unit_select_ui(&menu);
        handle_unit_select_input(&menu, game_state);
    }
    
    return 1;
} 