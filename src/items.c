/*
 *  Definicie
 */

#include "game_state.h"

// Array of predefined items
Item items[NUMBER_OF_ITEMS] = {
    // Weapons
    {"Sword", 5, 0, 1, 1, 0},
    {"Bow", 4, 0, 1, 4, 0},
    {"Staff", 3, 2, 1, 2, 1},
    {"Dagger", 3, 0, 1, 1, 0},
    {"Axe", 6, 0, 2, 1, 0},
    
    // Armor
    {"Leather", 0, 3, 1, 0, 0},
    {"Chainmail", 0, 5, 2, 0, 0},
    {"Plate", 0, 7, 3, 0, 0},
    {"Robe", 1, 2, 1, 0, 0},
    {"Shield", 0, 4, 1, 0, 0}
}; 