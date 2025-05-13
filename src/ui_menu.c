#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game_state.h"

// ASCII art for knight (left side decoration)
static const char* knight_art[] = {
    "  ,   A           {}",
    " / \\, | ,        .--.    ",
    "|    =|= >      /.--.\\   ",
    " \\ /` | `       |====|   ",
    "  `   |         |`::`|    ",
    "      |     .-;`\\..../`;-.",
    "     /\\\\/  /  |...::...|  \\",
    "     |:\\' |   /'''::'''\\   |",
    "      \\ /\\;-,/\\   ::   /\\--;",
    "      |\\ <` >  >._::_.<,<__>",
    "      | `\"`  /   ^^   \\|  |",
    "      |       |        |\\::/",
    "      |       |        |/|||",
    "      |       |___/\\___| '''",
    "      |        \\_ || _/     ",
    "      |        <_ >< _>     ",
    "      |        |  ||  |     ",
    "      |        |  ||  |     ",
    "      |       _\\.:||:./_    ",
    "      |      /____/\\____\\   "
};

// ASCII art for robot (right side decoration)
static const char* robot_art[] = {
    "                          ____      ",
    "                 []|    (______     ",
    "                 []|__ / snd   \\    ",
    "                 ||   \\________/    ",
    "                 ||      ___        ",
    "                 ||     /_  )__     ",
    "      __|\\/      ||   _/_ \\____)    ",
    ",----`     \\     ||  />=o)         ",
    "\\_____      \\    ||  \\]__\\        ",
    "      `--,_/U\\  B|\\__/===\\        ",
    "         |UUUU\\  ||_ _|_\\_ \\      ",
    "         |UUUUU\\_|[,`_|__|_)      ",
    "         |UUUUUU\\||__/_ __|       ",
    "         |UUUUUU/-(_\\_____/-------,",
    "         /UU/    |H\\__\\    HHHH|   \\\\",
    "         |UU/    |H\\  |HHHHHHH|    |\\\\\\",
    "         UU      |HH\\ \\HHHHHHH|    | \\\\\\",
    "         U       |<_\\,_\\HHHHHH|   /  \\\\\\\\",
    "          \\ (    |HHHHHHHHHHHHH   /  \\\\\\\\\\",
    "           \\ \\   |=============  /    \\\\\\\\\\",
    "              \\ |             | |         "
};

// ASCII art for the title
/* Updated ASCII title art */
static const char* title_art[] = {
    "     _            _ _ _       _     _    _                          ",
    "    | |          (_| (_)     | |   | |  | |                         ",
    "    | |___      ___| |_  __ _| |__ | |_ | |_ _ __ ___   ___  _ __   ",
    "    | __\\ \\ /\\ / | | | |/ _` | '_ \\| __|| __| '__/ _ \\ / _ \\| '_ \\  ",
    "    | |_ \\ V  V /| | | | (_| | | | | |_ | |_| | | (_) | (_) | |_) | ",
    "     \\__| \\_/\\_/ |_|_|_|\\__, |_| |_|\\__| \\__|_|  \\___/ \\___/| .__/  ",
    "                         __/ |       ______                 | |     ",
    "                        |___/       |______|                |_|     "
};

// Button structure
typedef struct {
    char* label;
    int y;
    int x;
    int width;
    int is_selected;
} Button;

// Menu structure
typedef struct {
    Button* buttons;
    int num_buttons;
    int selected_button;
    char* title;
} Menu;

// Function prototypes
void init_ui();
void draw_background();
void draw_title();
void create_start_menu();
void draw_button(Button* button);
void draw_menu(Menu* menu);
void handle_input(Menu* menu);
void start_existing_game();
void start_new_game();
void browse_items();
void cleanup_ui();

// Initialize ncurses for UI
void init_ui() {
    initscr();              // Initialize ncurses
    cbreak();               // Line buffering disabled
    noecho();               // Don't echo keypresses
    keypad(stdscr, TRUE);   // Enable arrow keys
    curs_set(0);            // Hide cursor
    
    // Initialize colors if terminal supports them
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);    // Default colors
        init_pair(2, COLOR_BLACK, COLOR_WHITE);    // Selected button
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);   // Knight color
        init_pair(4, COLOR_CYAN, COLOR_BLACK);     // Robot color
        init_pair(5, COLOR_GREEN, COLOR_BLACK);    // Title color
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);  // Highlight color
    }
    
    // Get the dimensions of the terminal window - now we'll accept any size
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Make sure terminal is at least minimally usable
    if (max_y < 10 || max_x < 40) {
        endwin();
        printf("Terminal window too small. Please resize to at least 40x10 characters.\n");
        exit(1);
    }
}

// Draw the ASCII art background
void draw_background() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Only draw decorations if there's enough space
    if (max_x >= 60 && max_y >= 20) {
        // Calculate starting positions
        int knight_start_x = 2;
        int knight_start_y = (max_y >= 24) ? max_y / 2 - 10 : 10;
        if (knight_start_y < 2) knight_start_y = 2;
        
        // Only draw the knight if there's room on the left
        if (max_x >= 60) {
            // Draw the knight (left side)
            attron(COLOR_PAIR(3));
            for (int i = 0; i < 20 && i + knight_start_y < max_y - 2; i++) {
                mvprintw(knight_start_y + i, knight_start_x, "%s", knight_art[i]);
            }
            attroff(COLOR_PAIR(3));
        }
        
        // Only draw the robot if there's room on the right
        if (max_x >= 80) {
            int robot_start_x = max_x - 45;
            int robot_start_y = knight_start_y;
            
            // Draw the robot (right side)
            attron(COLOR_PAIR(4));
            for (int i = 0; i < 21 && i + robot_start_y < max_y - 2; i++) {
                mvprintw(robot_start_y + i, robot_start_x, "%s", robot_art[i]);
            }
            attroff(COLOR_PAIR(4));
        }
    }
}

// Draw title at the top of the screen
void draw_title() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    attron(COLOR_PAIR(5) | A_BOLD);
    
    // For larger terminals, draw the ASCII art title
    if (max_x >= 50 && max_y >= 10) {
        int title_x = (max_x - 70) / 2; // Adjust width for the new title art
        int title_y = 2;
        
        // Draw all lines of the title art
        for (int i = 0; i < 8 && title_y + i < max_y - 6; i++) {
            mvprintw(title_y + i, title_x, "%s", title_art[i]);
        }
    } else {
        // For smaller terminals, just show a simple title
        mvprintw(1, (max_x - 14) / 2, "ROFLANDS GAME");
    }
    
    attroff(COLOR_PAIR(5) | A_BOLD);
}

// Draw single button
void draw_button(Button* button) {
    int color_pair = button->is_selected ? 2 : 1;
    attron(COLOR_PAIR(color_pair) | A_BOLD);
    
    // Draw button box
    for (int i = 0; i < button->width; i++) {
        mvaddch(button->y - 1, button->x + i, '-');
        mvaddch(button->y + 1, button->x + i, '-');
    }
    mvaddch(button->y - 1, button->x - 1, '+');
    mvaddch(button->y - 1, button->x + button->width, '+');
    mvaddch(button->y + 1, button->x - 1, '+');
    mvaddch(button->y + 1, button->x + button->width, '+');
    mvaddch(button->y, button->x - 1, '|');
    mvaddch(button->y, button->x + button->width, '|');
    
    // Draw button label
    mvprintw(button->y, button->x + (button->width - strlen(button->label)) / 2, "%s", button->label);
    
    attroff(COLOR_PAIR(color_pair) | A_BOLD);
}

// Draw menu with all buttons
void draw_menu(Menu* menu) {
    clear();
    draw_background();
    draw_title();
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Draw a border around the menu area if there's enough space
    if (max_x >= 60 && max_y >= 20) {
        int border_width = 30;
        int border_height = menu->num_buttons * 3 + 4;
        int border_start_x = max_x / 2 - border_width / 2;
        int border_start_y = (max_y >= 24) ? max_y / 2 - 2 : 10;
        
        // Draw the border
        attron(COLOR_PAIR(6) | A_BOLD);
        
        // Draw top and bottom border
        for (int i = 0; i < border_width; i++) {
            mvaddch(border_start_y, border_start_x + i, '-');
            mvaddch(border_start_y + border_height, border_start_x + i, '-');
        }
        
        // Draw left and right border
        for (int i = 0; i < border_height; i++) {
            mvaddch(border_start_y + i, border_start_x, '|');
            mvaddch(border_start_y + i, border_start_x + border_width, '|');
        }
        
        // Draw corners
        mvaddch(border_start_y, border_start_x, '+');
        mvaddch(border_start_y, border_start_x + border_width, '+');
        mvaddch(border_start_y + border_height, border_start_x, '+');
        mvaddch(border_start_y + border_height, border_start_x + border_width, '+');
        
        // Draw the menu title
        int title_x = max_x / 2 - strlen(menu->title) / 2;
        mvprintw(border_start_y + 1, title_x, "%s", menu->title);
        
        attroff(COLOR_PAIR(6) | A_BOLD);
    }
    
    // Draw all buttons
    for (int i = 0; i < menu->num_buttons; i++) {
        menu->buttons[i].is_selected = (i == menu->selected_button);
        draw_button(&menu->buttons[i]);
    }
    
    // Add instructions at the bottom
    attron(COLOR_PAIR(6));
    mvprintw(LINES - 2, (COLS - 70) / 2, "Use UP/DOWN arrow keys to navigate, ENTER to select, 'q' to quit");
    attroff(COLOR_PAIR(6));
    
    refresh();
}

// Handle keyboard input for menu navigation
void handle_input(Menu* menu) {
    int ch = getch();
    
    switch (ch) {
        case KEY_UP:
            menu->selected_button = (menu->selected_button > 0) ? 
                                    menu->selected_button - 1 : 
                                    menu->num_buttons - 1;
            break;
        case KEY_DOWN:
            menu->selected_button = (menu->selected_button < menu->num_buttons - 1) ? 
                                    menu->selected_button + 1 : 
                                    0;
            break;
        case 10: // Enter key
            if (menu->selected_button == 0) {
                start_existing_game();
            } else if (menu->selected_button == 1) {
                start_new_game();
            } else if (menu->selected_button == 2) {
                browse_items();
            } else if (menu->selected_button == 3) {
                cleanup_ui();
                exit(0);
            }
            break;
        case 'q':
        case 'Q':
            cleanup_ui();
            exit(0);
            break;
    }
}

// Placeholder function for starting an existing game
void start_existing_game() {
    clear();
    draw_background();
    mvprintw(LINES / 2, (COLS - 30) / 2, "Starting existing game...");
    refresh();
    napms(2000); // Wait 2 seconds for demonstration
    // TODO: Add actual game start code here
}

// Placeholder function for starting a new game
void start_new_game() {
    clear();
    draw_background();
    mvprintw(LINES / 2, (COLS - 30) / 2, "Starting new game...");
    refresh();
    napms(2000); // Wait 2 seconds for demonstration
    // TODO: Add actual new game code here
}

// Function to browse available items
void browse_items() {
    clear();
    draw_background();
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Adjust layout based on terminal size
    int start_row = (max_y >= 24) ? 9 : 2;
    int current_row = start_row;
    int items_per_page = max_y - ((max_y >= 24) ? 15 : 8);
    if (items_per_page < 1) items_per_page = 1;  // Show at least one item
    
    int current_page = 0;
    int total_pages = (NUMBER_OF_ITEMS + items_per_page - 1) / items_per_page; // Ceiling division
    
    // Adjust column positions based on screen width
    int name_col = 2;
    int att_col = (max_x >= 60) ? name_col + 12 : name_col + 8;
    int def_col = (max_x >= 60) ? att_col + 8 : att_col + 6;
    int slots_col, range_col, radius_col;
    
    // For very small screens, show fewer columns
    if (max_x < 50) {
        slots_col = def_col + 6;
        range_col = 0;  // Don't show these columns
        radius_col = 0; // Don't show these columns
    } else {
        slots_col = def_col + 8;
        range_col = slots_col + 8;
        radius_col = range_col + 8;
    }
    
    while (1) {
        clear();
        draw_background();
        
        current_row = start_row;
        
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(current_row++, (max_x - 15) / 2, "AVAILABLE ITEMS");
        attroff(COLOR_PAIR(5) | A_BOLD);
        
        current_row += 1;
        
        // Display column headers
        attron(A_BOLD | COLOR_PAIR(6));
        mvprintw(current_row, name_col, "Name");
        mvprintw(current_row, att_col, "Att");
        mvprintw(current_row, def_col, "Def");
        
        if (max_x >= 50) {
            mvprintw(current_row, slots_col, "Slot");
            if (range_col > 0) mvprintw(current_row, range_col, "Rng");
            if (radius_col > 0) mvprintw(current_row, radius_col, "Rad");
        }
        
        attroff(A_BOLD | COLOR_PAIR(6));
        
        current_row += 1;
        
        // Display items for the current page
        int start_idx = current_page * items_per_page;
        int end_idx = (start_idx + items_per_page < NUMBER_OF_ITEMS) ? start_idx + items_per_page : NUMBER_OF_ITEMS;
        
        for (int i = start_idx; i < end_idx; i++) {
            if (current_row >= max_y - 3) break; // Prevent drawing off-screen
            
            attron(COLOR_PAIR(1));
            mvprintw(current_row, name_col, "%-8.8s", items[i].name);
            mvprintw(current_row, att_col, "%d", items[i].att);
            mvprintw(current_row, def_col, "%d", items[i].def);
            
            if (max_x >= 50) {
                mvprintw(current_row, slots_col, "%d", items[i].slots);
                if (range_col > 0) mvprintw(current_row, range_col, "%d", items[i].range);
                if (radius_col > 0) mvprintw(current_row, radius_col, "%d", items[i].radius);
            }
            
            attroff(COLOR_PAIR(1));
            
            current_row++;
        }
        
        // Display page info and navigation help
        attron(COLOR_PAIR(6));
        mvprintw(max_y - 2, 2, "Page %d/%d (←→ navigate, q=quit)", current_page + 1, total_pages);
        attroff(COLOR_PAIR(6));
        
        refresh();
        
        int ch = getch();
        
        switch (ch) {
            case KEY_LEFT:
                current_page = (current_page > 0) ? current_page - 1 : total_pages - 1;
                break;
            case KEY_RIGHT:
                current_page = (current_page < total_pages - 1) ? current_page + 1 : 0;
                break;
            case 'q':
            case 'Q':
            case 27: // ESC key
                return;
        }
    }
}

// Clean up ncurses
void cleanup_ui() {
    endwin();
}

// Create and show the start menu
void create_start_menu() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Create menu structure with 4 buttons
    Menu menu;
    menu.title = "ROFLANDS BATTLE";
    menu.num_buttons = 4;
    menu.selected_button = 0;
    
    // Allocate memory for buttons
    menu.buttons = malloc(sizeof(Button) * menu.num_buttons);
    
    // Calculate button positions based on terminal size
    int btn_width = (max_x >= 40) ? 20 : 16;
    int start_y = (max_y >= 24) ? max_y / 2 : 12;
    int center_x = max_x / 2;
    
    // Adjust button spacing for smaller terminals
    int spacing = (max_y >= 20) ? 3 : 2;
    
    // Define buttons
    // Button 1: Start Game
    menu.buttons[0].label = "Start Game";
    menu.buttons[0].width = btn_width;
    menu.buttons[0].x = center_x - (btn_width / 2);
    menu.buttons[0].y = start_y;
    menu.buttons[0].is_selected = 1;
    
    // Button 2: New Game
    menu.buttons[1].label = "New Game";
    menu.buttons[1].width = btn_width;
    menu.buttons[1].x = center_x - (btn_width / 2);
    menu.buttons[1].y = start_y + spacing;
    menu.buttons[1].is_selected = 0;
    
    // Button 3: Browse Items
    menu.buttons[2].label = "Browse Items";
    menu.buttons[2].width = btn_width;
    menu.buttons[2].x = center_x - (btn_width / 2);
    menu.buttons[2].y = start_y + spacing * 2;
    menu.buttons[2].is_selected = 0;
    
    // Button 4: Exit
    menu.buttons[3].label = "Exit";
    menu.buttons[3].width = btn_width;
    menu.buttons[3].x = center_x - (btn_width / 2);
    menu.buttons[3].y = start_y + spacing * 3;
    menu.buttons[3].is_selected = 0;
    
    // Make sure buttons don't go off-screen
    for (int i = 0; i < menu.num_buttons; i++) {
        if (menu.buttons[i].y >= max_y - 3) {
            menu.buttons[i].y = max_y - 3 - ((menu.num_buttons - i - 1) * 2);
        }
    }
    
    // Main menu loop
    while (1) {
        draw_menu(&menu);
        handle_input(&menu);
    }
    
    // Free memory
    free(menu.buttons);
}

// Function to be called from main to show the starting menu
void show_start_menu() {
    init_ui();
    create_start_menu();
    cleanup_ui();
}