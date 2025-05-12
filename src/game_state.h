/*
 *  Makra a struktury
 */

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdlib.h>
#include <string.h>

#define ITEMS_FILE "items.json"   // fixne dana cesta k JSON suboru s predmetmi ... NEMENIT !!!
#define MAX_LINE 1000       // maximalna dlzka riadku v TXT/JSON subore
#define MAX_FILENAME 500    // maximalna dlzka cesty k suboru
#define MAX_NAME 100        // maximalna dlzka nazvu/mena predmetu/jednotky
#define MIN_ARMY 1          // minimalny pocet bojovych jednotiek v armade
#define MAX_ARMY 5          // maximalny pocet bojovych jednotiek v armade
#define NUMBER_OF_ITEMS 10  // celkovy pocet predmetov v hre

// chyby vznikajuce pocas nacitania predmetov z JSON suboru
#define ERR_MISSING_ATTRIBUTE "ERR_MISSING_ATTRIBUTE"   // chybajuci atribut predmetu
#define ERR_MISSING_VALUE "ERR_MISSING_VALUE"           // chybajuca hodnota kluca
#define ERR_BAD_VALUE "ERR_BAD_VALUE"                   // zla hodnota kluca

// chyby vznikajuce pocas nacitania armady z TXT suboru
#define ERR_UNIT_COUNT "ERR_UNIT_COUNT"                 // nespravny pocet jednotiek
#define ERR_ITEM_COUNT "ERR_ITEM_COUNT"                 // nepovoleny pocet predmetov v inventari
#define ERR_WRONG_ITEM "ERR_WRONG_ITEM"                 // neznamy predmet v inventari
#define ERR_SLOTS "ERR_SLOTS"                           // pocet slotov prekroceny

// dalsie chyby
#define ERR_FILE "ERR_FILE"                             // subor sa neda otvorit (plati pre JSON aj TXT subor)
#define ERR_CMD "ERR_CMD"                               // zly pocet CMD argumentov

// Grid size for the battle arena
#define GRID_WIDTH 12
#define GRID_HEIGHT 12

// Maximum number of units per player
#define MAX_UNITS_PER_PLAYER 5

// Player identifiers
#define PLAYER_1 0
#define PLAYER_2 1
#define NUM_PLAYERS 2

// Item structure
typedef struct {
    char name[20];
    int att;
    int def;
    int slots;
    int range;
    int radius;
} Item;

// Unit types
typedef enum {
    UNIT_TYPE_WARRIOR,
    UNIT_TYPE_ARCHER,
    UNIT_TYPE_MAGE,
    UNIT_TYPE_SCOUT,
    UNIT_TYPE_TANK,
    NUM_UNIT_TYPES
} UnitType;

// Equipment slots
typedef enum {
    EQUIP_WEAPON,
    EQUIP_ARMOR,
    EQUIP_ACCESSORY,
    NUM_EQUIP_SLOTS
} EquipSlot;

// Unit structure
typedef struct {
    char name[20];
    UnitType type;
    int max_hp;
    int current_hp;
    int move_range;
    int attack_range;
    Item* equipment[NUM_EQUIP_SLOTS];
    int x;  // Position on grid
    int y;  // Position on grid
    int player_id;  // Which player owns this unit
    int is_active;  // Is this unit still alive
} Unit;

// Cell structure for the battle grid
typedef struct {
    Unit* unit;         // Unit on this cell, or NULL if empty
    int terrain_type;   // Type of terrain (0 = plain, 1 = forest, 2 = mountain, etc.)
    int highlight;      // For UI highlighting (0 = none, 1 = movement range, 2 = attack range)
} Cell;

// Game phases
typedef enum {
    PHASE_MAIN_MENU,
    PHASE_UNIT_SELECTION,
    PHASE_UNIT_PLACEMENT,
    PHASE_BATTLE,
    PHASE_GAME_OVER
} GamePhase;

// Turn states
typedef enum {
    TURN_STATE_SELECT_UNIT,
    TURN_STATE_SELECT_MOVE,
    TURN_STATE_SELECT_ACTION,
    TURN_STATE_SELECT_TARGET
} TurnState;

// Game state structure to track the overall game state
typedef struct {
    GamePhase current_phase;
    TurnState turn_state;
    int current_player;
    Cell grid[GRID_HEIGHT][GRID_WIDTH];
    Unit player_units[NUM_PLAYERS][MAX_UNITS_PER_PLAYER];
    int num_units[NUM_PLAYERS];
    Unit* selected_unit;
    int game_turn;
    char status_message[100];
} GameState;

// Array of predefined items
extern Item items[NUMBER_OF_ITEMS];

// Function prototypes
void initialize_game_state(GameState* game_state);
void place_unit(GameState* game_state, Unit* unit, int x, int y);
void remove_unit(GameState* game_state, Unit* unit);
int move_unit(GameState* game_state, Unit* unit, int new_x, int new_y);
int attack_unit(GameState* game_state, Unit* attacker, Unit* target);
void end_turn(GameState* game_state);
void save_game(GameState* game_state, const char* filename);
int load_game(GameState* game_state, const char* filename);
void clear_highlights(GameState* game_state);
void highlight_movement_range(GameState* game_state, Unit* unit);
void highlight_attack_range(GameState* game_state, Unit* unit);

#endif // GAME_STATE_H


