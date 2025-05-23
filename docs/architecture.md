# Twilight Troop Architecture

## System Architecture Diagram

```
+-----------------------------+
|        Main Game Loop       |
|        (main.c)             |
+-----------------------------+
            |
            v
+-----------------------------+
|       Game State            |
|      (game_state.h)         |
+-----------------------------+
            |
            v
+-----------+-----------+
|                       |
v                       v
+---------------+   +---------------+
| User Interface|   | Game Logic    |
+---------------+   +---------------+
| - ui_menu     |   | - game_logic  |
| - ui_battle   |   | - items       |
| - ui_placement|   | - save/load   |
| - ui_help     |   |               |
+-------+-------+   +-------+-------+
        |                   |
        |                   |
        v                   v
+-----------------------------+
|        Data Layer           |
| - items.json                |
| - unit_types.txt            |
| - army config files         |
| - savegame.dat              |
+-----------------------------+
```

## Module Dependency Diagram

```
                    +-------------+
                    | main.c      |
                    +------+------+
                           |
           +---------------+---------------+
           |               |               |
           v               v               v
 +-----------------+ +------------+ +--------------+
 | ui_menu.c/h     | | ui_battle  | | game_state.h |
 +-----------------+ +------------+ +--------------+
           |               |               |
           v               v               |
 +-----------------+ +------------+        |
 | ui_unit_select  | | ui_help    |        |
 +-----------------+ +------------+        |
           |                               |
           v                               v
 +-----------------+                +------------+
 | ui_placement    |<---------------| game_logic |
 +-----------------+                +------------+
                                           |
                                           v
                                    +------------+
                                    | items      |
                                    +------------+
                                           |
                                           v
                                    +------------+
                                    | save       |
                                    +------------+
```

## Data Flow Diagram

```
   +----------------+        +----------------+
   |  Configuration |        |    User Input  |
   |  Files (JSON)  |        |                |
   +-------+--------+        +--------+-------+
           |                          |
           v                          v
   +-------+------------------------+-+-------+
   |                                          |
   |             Game State                   |
   |                                          |
   +--+-------------------+------------------++
      |                   |                  |
      v                   v                  v
+-----+-------+  +--------+-------+  +-------+------+
| UI Rendering |  | Game Logic     |  | State Change |
|              |  | Processing     |  | Processing   |
+--------------+  +----------------+  +--------------+
      |                   |                  |
      +-------------------+------------------+
                          |
                          v
                 +--------+--------+
                 | Screen Display   |
                 | or Save File     |
                 +-----------------+
```

## Game States and Transitions

```
                   +-------------+
                   | MAIN MENU   +
                   +------+------+
                          |
           +-------------+--------------+
           |              |             |
           v              v             v
  +--------+----+  +------+-----+  +----+-------+
  | NEW GAME    |  | LOAD GAME   |  | HELP      |
  +-------------+  +------------+  +------------+
           |              |
           |              |
           v              |
  +--------+----+         |
  | UNIT         |        |
  | SELECTION    |        |
  +-------------+         |
           |              |
           v              |
  +--------+----+         |
  | UNIT         |        |
  | PLACEMENT    |        |
  +-------------+         |
           |              |
           v              |
  +--------+----+<--------+
  | BATTLE       |
  |              |
  +-------------+
           |
           v
  +--------+----+
  | GAME OVER    |
  |              |
  +-------------+
``` 