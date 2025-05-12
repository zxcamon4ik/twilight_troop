#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game_state.h"

// Function prototypes
void show_help_screen();

// Show the help screen
void show_help_screen() {
    clear();
    
    // Draw title
    attron(A_BOLD);
    mvprintw(1, (COLS - 10) / 2, "HELP MENU");
    attroff(A_BOLD);
    
    // Draw box around the help content
    int start_x = 5;
    int start_y = 3;
    int width = COLS - 10;
    int height = LINES - 6;
    
    // Draw horizontal borders
    for (int i = 0; i < width; i++) {
        mvaddch(start_y, start_x + i, ACS_HLINE);
        mvaddch(start_y + height, start_x + i, ACS_HLINE);
    }
    
    // Draw vertical borders
    for (int i = 0; i < height; i++) {
        mvaddch(start_y + i, start_x, ACS_VLINE);
        mvaddch(start_y + i, start_x + width, ACS_VLINE);
    }
    
    // Draw corners
    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y, start_x + width, ACS_URCORNER);
    mvaddch(start_y + height, start_x, ACS_LLCORNER);
    mvaddch(start_y + height, start_x + width, ACS_LRCORNER);
    
    // Draw help content
    int line = start_y + 2;
    int col = start_x + 2;
    
    mvprintw(line, col, "GAME CONTROLS:");
    line += 2;
    
    mvprintw(line++, col, "Arrow Keys: Move cursor");
    mvprintw(line++, col, "Enter: Select / Confirm");
    mvprintw(line++, col, "Escape: Cancel / Back");
    mvprintw(line++, col, "H: Show this help screen");
    mvprintw(line++, col, "S: Save game");
    mvprintw(line++, col, "L: Load game");
    mvprintw(line++, col, "E: End turn (during battle)");
    mvprintw(line++, col, "Q: Quit game");
    
    line += 2;
    mvprintw(line, col, "GAME RULES:");
    line += 2;
    
    mvprintw(line++, col, "1. Unit Selection:");
    mvprintw(line++, col + 3, "- Each player selects units and equips them with items");
    mvprintw(line++, col + 3, "- Different units have different abilities and stats");
    
    line++;
    mvprintw(line++, col, "2. Unit Placement:");
    mvprintw(line++, col + 3, "- Players place their units on their side of the battlefield");
    mvprintw(line++, col + 3, "- Player 1 uses the top rows, Player 2 uses the bottom rows");
    
    line++;
    mvprintw(line++, col, "3. Battle:");
    mvprintw(line++, col + 3, "- Players take turns moving and attacking with their units");
    mvprintw(line++, col + 3, "- Movement and attack ranges use Manhattan distance");
    mvprintw(line++, col + 3, "- Manhattan distance = |x1-x2| + |y1-y2|");
    mvprintw(line++, col + 3, "- The game ends when all units of one player are defeated");
    
    // Instructions to return
    attron(A_BOLD);
    mvprintw(LINES - 2, (COLS - 30) / 2, "Press any key to return");
    attroff(A_BOLD);
    
    refresh();
    getch(); // Wait for any key press
}

// Run the help screen
void run_help_screen() {
    show_help_screen();
} 