#ifndef UI_MENU_H
#define UI_MENU_H

// Function to display the starting menu with options
void show_start_menu();

// Function to initialize UI components
void init_ui();

// Function to clean up UI components
void cleanup_ui();

// Game state functions for menu actions
void start_existing_game();
void start_new_game();
void browse_items();

// Drawing functions
void draw_background();
void draw_title();

#endif // UI_MENU_H 