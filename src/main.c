#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <time.h>

// NEMENIT, inak vas kod neprejde testom !!!
#include "game_state.h"
#include "ui_menu.h"
#include "ui_unit_select.h"
#include "ui_placement.h"
#include "ui_battle.h"
#include "ui_help.h"

// This variable is now used to store the number of items, which is fixed at NUMBER_OF_ITEMS
int item_count = NUMBER_OF_ITEMS;

void remove_white_spaces(char *goida) {
    int j = 0;
    for (int i = 0; goida[i] != '\0'; i++) {
        if (!isspace(goida[i])) {
            goida[j++] = goida[i];
        }
    }
    goida[j] = '\0';
}

int check_is_num(char *value) {
    int value_len = strlen(value);
    for (int i = 0; i < value_len; i++) {
        if (!isdigit(value[i])) {
            return 1;
        }
    }
    return 0;
}

void print_item(const Item *item) {
    printf("Name: %s\n", item->name);
    printf("Attack: %d\n", item->att);
    printf("Defense: %d\n", item->def);
    printf("Slots: %d\n", item->slots);
    printf("Range: %d\n", item->range);
    printf("Radius: %d\n", item->radius);
}

// Function to print all available items
void print_items() {
    for (int i = 0; i < item_count; i++) {
        print_item(&items[i]);
        printf("\n");
    }
}

void print_item_arena(const char *label, const Item *item) {
    if (item) {
        printf("    %s: %s,%d,%d,%d,%d,%d\n", label,
               item->name,
               item->att,
               item->def,
               item->slots,
               item->range,
               item->radius);
    }
}

void print_arena(Unit *army0, Unit *army1) {
    printf("Army 1\n");
    for (int i = 0; i < MAX_ARMY; i++) {
        if (army0[i].name[0] == '\0') break;
        printf("    Unit: %d\n", i);
        printf("    Name: %s\n", army0[i].name);
        printf("    HP: %d\n", army0[i].current_hp);
        print_item_arena("Item 1", army0[i].equipment[EQUIP_WEAPON]);
        print_item_arena("Item 2", army0[i].equipment[EQUIP_ARMOR]);
		printf("\n");
	}

    printf("Army 2\n");
    for (int i = 0; i < MAX_ARMY; i++) {
        if (army1[i].name[0] == '\0') break;
        printf("    Unit: %d\n", i);
        printf("    Name: %s\n", army1[i].name);
        printf("    HP: %d\n", army1[i].current_hp);
        print_item_arena("Item 1", army1[i].equipment[EQUIP_WEAPON]);
        print_item_arena("Item 2", army1[i].equipment[EQUIP_ARMOR]);
		printf("\n");
	}
}

void load_army(Unit *army, char *armada) {
    FILE *armyf = fopen(armada, "r");

    if (armyf == NULL) {
        fprintf(stderr, "%s\n", ERR_FILE);
        exit(0);
    }

    char buffer[MAX_LINE + 1];

    if (!fgets(buffer, sizeof(buffer), armyf)) {
        fprintf(stderr, "%s\n", ERR_UNIT_COUNT);
        fclose(armyf);
        exit(0);
    }

    int quantity = atoi(buffer);
    if (!(1 <= quantity && quantity <= MAX_ARMY)) {
        fprintf(stderr, "%s\n", ERR_UNIT_COUNT);
        fclose(armyf);
        exit(0);
    }

    for (int i = 0; i < quantity; i++) {
        if (!fgets(buffer, sizeof(buffer), armyf)) {
            fprintf(stderr, "%s\n", ERR_ITEM_COUNT);
            fclose(armyf);
            exit(0);
        }

        char *name = NULL;
        char *item1 = NULL;
        char *item2 = NULL;

        int token_counter = 0;
        char *token = strtok(buffer, " \t\r\n");
        while (token != NULL) {
            if (token_counter == 0) name = token;
            else if (token_counter == 1) item1 = token;
            else if (token_counter == 2) item2 = token;
            token_counter++;
            token = strtok(NULL, " \t\r\n");
        }

        if (token_counter < 2 || token_counter > 3) {
            fprintf(stderr, "%s\n", ERR_ITEM_COUNT);
            fclose(armyf);
            exit(0);
        }

        strcpy(army[i].name, name);
        army[i].name[MAX_NAME] = '\0';

        const Item *found1 = NULL;
        const Item *found2 = NULL;

        for (int j = 0; j < item_count; j++) {
            if (strcmp(item1, items[j].name) == 0) found1 = &items[j];
            if (item2 && strcmp(item2, items[j].name) == 0) found2 = &items[j];
        }

        if (!found1 || (item2 && !found2)) {
            fprintf(stderr, "%s\n", ERR_WRONG_ITEM);
            fclose(armyf);
            exit(0);
        }

        army[i].equipment[EQUIP_WEAPON] = (Item*)found1;
        if (found2) {
            army[i].equipment[EQUIP_ARMOR] = (Item*)found2;
        } else {
            army[i].equipment[EQUIP_ARMOR] = NULL;
        }
        army[i].equipment[EQUIP_ACCESSORY] = NULL;

        int slots = 0;
        if (found1) slots += found1->slots;
        if (found2) slots += found2->slots;

        if (slots > 2) {
            fprintf(stderr, "%s\n", ERR_SLOTS);
            fclose(armyf);
            exit(0);
        }

        army[i].current_hp = 100;
        army[i].max_hp = 100;
        
        // Initialize other Unit properties
        army[i].type = UNIT_TYPE_WARRIOR; // Default type
        army[i].move_range = 3;           // Default move range
        army[i].attack_range = 1;         // Default attack range
        army[i].player_id = 0;            // Default to player 1
        army[i].is_active = 1;            // Unit is active
        army[i].x = -1;                   // Position will be set during placement
        army[i].y = -1;
    }

    fclose(armyf);
}

// Function prototypes
void game_loop();
void handle_main_menu(GameState* game_state);

int main() {
    // Seed the random number generator
    srand(time(NULL));
    
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // Hide cursor
    
    // Initialize colors if terminal supports them
    if (has_colors()) {
        start_color();
    }
    
    // Run the game loop
    game_loop();
    
    // Clean up ncurses
    endwin();
    
    return 0;
}

// Main game loop
void game_loop() {
    GameState game_state;
    initialize_game_state(&game_state);
    
    // Main game loop
    while (1) {
        switch (game_state.current_phase) {
            case PHASE_MAIN_MENU:
                handle_main_menu(&game_state);
                break;
                
            case PHASE_UNIT_SELECTION:
                if (!run_unit_selection(&game_state)) {
                    // If unit selection fails for some reason, go back to main menu
                    game_state.current_phase = PHASE_MAIN_MENU;
                }
                break;
                
            case PHASE_UNIT_PLACEMENT:
                if (!run_unit_placement(&game_state)) {
                    // If unit placement fails for some reason, go back to unit selection
                    game_state.current_phase = PHASE_UNIT_SELECTION;
                }
                break;
                
            case PHASE_BATTLE:
                run_battle(&game_state);
                break;
                
            case PHASE_GAME_OVER:
                // Show game over screen
                clear();
                attron(A_BOLD);
                mvprintw(LINES / 2, (COLS - 20) / 2, "GAME OVER");
                
                int player1_units = 0, player2_units = 0;
                
                for (int i = 0; i < game_state.num_units[PLAYER_1]; i++) {
                    if (game_state.player_units[PLAYER_1][i].is_active) {
                        player1_units++;
                    }
                }
                
                for (int i = 0; i < game_state.num_units[PLAYER_2]; i++) {
                    if (game_state.player_units[PLAYER_2][i].is_active) {
                        player2_units++;
                    }
                }
                
                int winner = (player1_units > 0) ? PLAYER_1 : PLAYER_2;
                mvprintw(LINES / 2 + 1, (COLS - 30) / 2, "Player %d wins!", winner + 1);
                
                mvprintw(LINES / 2 + 3, (COLS - 40) / 2, "Press any key to return to main menu");
                attroff(A_BOLD);
                
                refresh();
                getch();
                
                // Reset game state and go back to main menu
                initialize_game_state(&game_state);
                game_state.current_phase = PHASE_MAIN_MENU;
                break;
        }
    }
}

// Handle the main menu
void handle_main_menu(GameState* game_state) {
    typedef enum {
        MENU_OPTION_NEW_GAME,
        MENU_OPTION_LOAD_GAME,
        MENU_OPTION_HELP,
        MENU_OPTION_EXIT,
        NUM_MENU_OPTIONS
    } MenuOption;
    
    const char* menu_options[NUM_MENU_OPTIONS] = {
        "New Game",
        "Load Game",
        "Help",
        "Exit"
    };
    
    int selected_option = 0;
    
    while (1) {
        clear();
        
        // Draw title
        attron(A_BOLD);
        mvprintw(1, (COLS - 40) / 2, "TACTICAL BATTLE GAME");
        attroff(A_BOLD);
        
        // Draw menu options
        for (int i = 0; i < NUM_MENU_OPTIONS; i++) {
            if (i == selected_option) {
                attron(A_REVERSE);
            }
            
            mvprintw(5 + i * 2, (COLS - 20) / 2, "%s", menu_options[i]);
            
            if (i == selected_option) {
                attroff(A_REVERSE);
            }
        }
        
        // Draw controls
        mvprintw(LINES - 2, (COLS - 40) / 2, "UP/DOWN: Navigate, ENTER: Select");
        
        refresh();
        
        // Wait for user input
        int ch = getch();
        
        switch (ch) {
            case KEY_UP:
                selected_option = (selected_option > 0) ? selected_option - 1 : NUM_MENU_OPTIONS - 1;
                break;
                
            case KEY_DOWN:
                selected_option = (selected_option < NUM_MENU_OPTIONS - 1) ? selected_option + 1 : 0;
                break;
                
            case 10: // Enter key
                switch (selected_option) {
                    case MENU_OPTION_NEW_GAME:
                        // Start a new game
                        initialize_game_state(game_state);
                        game_state->current_phase = PHASE_UNIT_SELECTION;
                        return;
                        
                    case MENU_OPTION_LOAD_GAME:
                        // Try to load a saved game
                        if (load_game(game_state, "savegame.dat")) {
                            // Game loaded successfully
                            return;
                        } else {
                            // Failed to load game
                            mvprintw(LINES - 3, (COLS - 30) / 2, "Failed to load game!");
                            refresh();
                            napms(1500); // Show message for 1.5 seconds
                        }
                        break;
                        
                    case MENU_OPTION_HELP:
                        // Show help screen
                        run_help_screen();
                        break;
                        
                    case MENU_OPTION_EXIT:
                        // Exit the game
                        endwin();
                        exit(0);
                        break;
                }
                break;
        }
    }
}
