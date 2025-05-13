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

void remove_white_spaces(char *s){int j=0;for(int i=0;s[i];i++)if(!isspace(s[i]))s[j++]=s[i];s[j]=0;}
int check_is_num(char *v){for(int i=0;v[i];i++)if(!isdigit(v[i]))return 1;return 0;}

void print_item(const Item *i) {
    printf("Name: %s\nAttack: %d\nDefense: %d\nSlots: %d\nRange: %d\nRadius: %d\n", 
           i->name, i->att, i->def, i->slots, i->range, i->radius);
}

// Function to print all available items
void print_items() {
    for (int i = 0; i < item_count; i++) {
        print_item(&items[i]);
        printf("\n");
    }
}

void print_item_arena(const char *l, const Item *i) {
    if (i) printf("    %s: %s,%d,%d,%d,%d,%d\n", l, i->name, i->att, i->def, i->slots, i->range, i->radius);
}

void print_arena(Unit *a0, Unit *a1) {
    printf("Army 1\n");
    for (int i = 0; i < MAX_ARMY && a0[i].name[0]; i++) {
        printf("    Unit: %d\n    Name: %s\n    HP: %d\n", i, a0[i].name, a0[i].current_hp);
        print_item_arena("Item 1", a0[i].equipment[EQUIP_WEAPON]);
        print_item_arena("Item 2", a0[i].equipment[EQUIP_ARMOR]);
        printf("\n");
    }

    printf("Army 2\n");
    for (int i = 0; i < MAX_ARMY && a1[i].name[0]; i++) {
        printf("    Unit: %d\n    Name: %s\n    HP: %d\n", i, a1[i].name, a1[i].current_hp);
        print_item_arena("Item 1", a1[i].equipment[EQUIP_WEAPON]);
        print_item_arena("Item 2", a1[i].equipment[EQUIP_ARMOR]);
        printf("\n");
    }
}

void load_army(Unit *army, char *armada) {
    FILE *f = fopen(armada, "r");
    if (!f) {fprintf(stderr, "%s\n", ERR_FILE); exit(0);}

    char buf[MAX_LINE + 1];
    if (!fgets(buf, sizeof(buf), f)) {fprintf(stderr, "%s\n", ERR_UNIT_COUNT); fclose(f); exit(0);}

    int qty = atoi(buf);
    if (qty < 1 || qty > MAX_ARMY) {fprintf(stderr, "%s\n", ERR_UNIT_COUNT); fclose(f); exit(0);}

    for (int i = 0; i < qty; i++) {
        if (!fgets(buf, sizeof(buf), f)) {fprintf(stderr, "%s\n", ERR_ITEM_COUNT); fclose(f); exit(0);}

        char *name = NULL, *item1 = NULL, *item2 = NULL;
        int tc = 0;
        char *token = strtok(buf, " \t\r\n");
        
        while (token) {
            if (tc == 0) name = token;
            else if (tc == 1) item1 = token;
            else if (tc == 2) item2 = token;
            token = strtok(NULL, " \t\r\n");
            tc++;
        }

        if (tc < 2 || tc > 3) {fprintf(stderr, "%s\n", ERR_ITEM_COUNT); fclose(f); exit(0);}

        strcpy(army[i].name, name);
        army[i].name[MAX_NAME] = '\0';

        const Item *found1 = NULL, *found2 = NULL;
        for (int j = 0; j < item_count; j++) {
            if (strcmp(item1, items[j].name) == 0) found1 = &items[j];
            if (item2 && strcmp(item2, items[j].name) == 0) found2 = &items[j];
        }

        if (!found1 || (item2 && !found2)) {fprintf(stderr, "%s\n", ERR_WRONG_ITEM); fclose(f); exit(0);}

        army[i].equipment[EQUIP_WEAPON] = (Item*)found1;
        army[i].equipment[EQUIP_ARMOR] = found2 ? (Item*)found2 : NULL;
        army[i].equipment[EQUIP_ACCESSORY] = NULL;

        int slots = (found1 ? found1->slots : 0) + (found2 ? found2->slots : 0);
        if (slots > 2) {fprintf(stderr, "%s\n", ERR_SLOTS); fclose(f); exit(0);}

        // Initialize unit properties
        army[i].current_hp = army[i].max_hp = 100;
        army[i].type = UNIT_TYPE_WARRIOR;
        army[i].move_range = 3;
        army[i].attack_range = 1;
        army[i].player_id = 0;
        army[i].is_active = 1;
        army[i].x = army[i].y = -1;
    }
    fclose(f);
}

// Function prototypes
void game_loop();
void handle_main_menu(GameState* game_state);

int main() {
    srand(time(NULL));
    
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) start_color();
    
    game_loop();
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
                if (!run_unit_selection(&game_state))
                    game_state.current_phase = PHASE_MAIN_MENU;
                break;
                
            case PHASE_UNIT_PLACEMENT:
                if (!run_unit_placement(&game_state))
                    game_state.current_phase = PHASE_UNIT_SELECTION;
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
