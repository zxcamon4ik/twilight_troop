#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "game_state.h"

// Function prototypes
void show_help_screen();

// Show the help screen
void show_help_screen() {
    clear();
    attron(A_BOLD); mvprintw(1, (COLS-10)/2, "HELP MENU"); attroff(A_BOLD);
    
    int x=5, y=3, w=COLS-10, h=LINES-6;
    
    box(stdscr, 0, 0);
    mvhline(y, x, ACS_HLINE, w); mvhline(y+h, x, ACS_HLINE, w);
    mvvline(y, x, ACS_VLINE, h); mvvline(y, x+w, ACS_VLINE, h);
    mvaddch(y, x, ACS_ULCORNER); mvaddch(y, x+w, ACS_URCORNER);
    mvaddch(y+h, x, ACS_LLCORNER); mvaddch(y+h, x+w, ACS_LRCORNER);
    
    int line=y+2, col=x+2;
    mvprintw(line, col, "GAME CONTROLS:"); line+=2;
    
    const char* controls[] = {
        "Arrow Keys: Move cursor", "Enter: Select / Confirm",
        "Escape: Cancel / Back", "H: Show this help screen",
        "S: Save game", "L: Load game", 
        "E: End turn (during battle)", "Q: Quit game"
    };
    
    for (int i=0; i<8; i++) mvprintw(line++, col, "%s", controls[i]);
    
    line+=2; mvprintw(line, col, "GAME RULES:"); line+=2;
    
    mvprintw(line++, col, "1. Unit Selection:");
    mvprintw(line++, col+3, "- Each player selects units and equips them with items");
    mvprintw(line++, col+3, "- Different units have different abilities and stats");
    
    line++;
    mvprintw(line++, col, "2. Unit Placement:");
    mvprintw(line++, col+3, "- Players place their units on their side of the battlefield");
    mvprintw(line++, col+3, "- Player 1 uses the top rows, Player 2 uses the bottom rows");
    
    line++;
    mvprintw(line++, col, "3. Battle:");
    mvprintw(line++, col+3, "- Players take turns moving and attacking with their units");
    mvprintw(line++, col+3, "- Movement and attack ranges use Manhattan distance");
    mvprintw(line++, col+3, "- Manhattan distance = |x1-x2| + |y1-y2|");
    mvprintw(line++, col+3, "- The game ends when all units of one player are defeated");
    
    attron(A_BOLD); mvprintw(LINES-2, (COLS-30)/2, "Press any key to return"); attroff(A_BOLD);
    refresh(); getch();
}

// Run the help screen
void run_help_screen() {
    show_help_screen();
} 