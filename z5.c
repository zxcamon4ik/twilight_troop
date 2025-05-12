#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// NEMENIT, inak vas kod neprejde testom !!!
#include "data.h"

ITEM *items = NULL;
int item_count = 0;

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

void print_item(ITEM *item) {
    printf("Name: %s\n", item->name);
    printf("Attack: %d\n", item->att);
    printf("Defense: %d\n", item->def);
    printf("Slots: %d\n", item->slots);
    printf("Range: %d\n", item->range);
    printf("Radius: %d\n", item->radius);
}

void assign_attr(ITEM *item, const char *key, const char *value) {
    if (strcmp(key, "name") == 0) {
        strcpy(item->name, value);
    } else if (strcmp(key, "att") == 0) {
        item->att = atoi(value);
    } else if (strcmp(key, "def") == 0) {
        item->def = atoi(value);
    } else if (strcmp(key, "slots") == 0) {
        item->slots = atoi(value);
    } else if (strcmp(key, "range") == 0) {
        item->range = atoi(value);
    } else if (strcmp(key, "radius") == 0) {
        item->radius = atoi(value);
    } else {
        fprintf(stderr, "%s\n", ERR_MISSING_ATTRIBUTE);
        free(items);
        exit(0);
    }
}

int load_items() {
    FILE *pysya = fopen(ITEMS_FILE, "r");
    if (pysya == NULL) {
        fprintf(stderr, "%s\n", ERR_FILE);
        exit(0);
    }

    char buffer[MAX_LINE + 1];

    while (fgets(buffer, sizeof(buffer), pysya)) {
        if (strchr(buffer, '{')) {
            items = realloc(items, (item_count + 1) * sizeof(ITEM));

            int found[6] = {0};
            for (int i = 0; i < 6; i++) {
                fgets(buffer, sizeof(buffer), pysya);

                if (!strchr(buffer, ':')) {
                    fprintf(stderr, "%s\n", ERR_MISSING_ATTRIBUTE);
                    fclose(pysya);
                    free(items);
                    exit(0);
                }

                char *key = strtok(buffer, " :\",\t\r\n{}");
                char *value = strtok(NULL, " :\",\t\r\n{}");

                if (value == NULL || key == NULL) {
                    fprintf(stderr, "%s\n", ERR_MISSING_VALUE);
                    fclose(pysya);
                    free(items);
                    exit(0);
                }

                remove_white_spaces(key);
                remove_white_spaces(value);

                if (strcmp(key, "name") != 0 && check_is_num(value)) {
                    fprintf(stderr, "%s\n", ERR_BAD_VALUE);
                    fclose(pysya);
                    free(items);
                    exit(0);
                }

                assign_attr(&items[item_count], key, value);
                if (strcmp(key, "name") == 0) found[0] = 1;
                if (strcmp(key, "att") == 0) found[1] = 1;
                if (strcmp(key, "def") == 0) found[2] = 1;
                if (strcmp(key, "slots") == 0) found[3] = 1;
                if (strcmp(key, "range") == 0) found[4] = 1;
                if (strcmp(key, "radius") == 0) found[5] = 1;
            }

            for (int i = 0; i < 6; i++) {
                if (!found[i]) {
                    fprintf(stderr, "%s\n", ERR_MISSING_ATTRIBUTE);
                    fclose(pysya);
                    free(items);
                    exit(0);
                }
            }

            item_count++;
        }
    }
    fclose(pysya);
    return item_count;
}

void print_items() {
    for (int i = 0; i < item_count; i++) {
        print_item(&items[i]);
        printf("\n");
    }
}

void print_item_arena(const char *label, const ITEM *item) {
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

void print_arena(UNIT *army0, UNIT *army1) {
    printf("Army 1\n");
    for (int i = 0; i < MAX_ARMY; i++) {
        if (army0[i].name[0] == '\0') break;
        printf("    Unit: %d\n", i);
        printf("    Name: %s\n", army0[i].name);
        printf("    HP: %d\n", army0[i].hp);
        print_item_arena("Item 1", army0[i].item1);
        print_item_arena("Item 2", army0[i].item2);
		printf("\n");
	}

    printf("Army 2\n");
    for (int i = 0; i < MAX_ARMY; i++) {
        if (army1[i].name[0] == '\0') break;
        printf("    Unit: %d\n", i);
        printf("    Name: %s\n", army1[i].name);
        printf("    HP: %d\n", army1[i].hp);
        print_item_arena("Item 1", army1[i].item1);
        print_item_arena("Item 2", army1[i].item2);
		printf("\n");
	}
}


void load_army(UNIT *army, char *armada) {
    FILE *armyf = fopen(armada, "r");

    if (armyf == NULL) {
        fprintf(stderr, "%s\n", ERR_FILE);
        free(items);
        exit(0);
    }

    char buffer[MAX_LINE + 1];

    if (!fgets(buffer, sizeof(buffer), armyf)) {
        fprintf(stderr, "%s\n", ERR_UNIT_COUNT);
        fclose(armyf);
        free(items);
        exit(0);
    }

    int quantity = atoi(buffer);
    if (!(1 <= quantity && quantity <= MAX_ARMY)) {
        fprintf(stderr, "%s\n", ERR_UNIT_COUNT);
        fclose(armyf);
        free(items);
        exit(0);
    }

    for (int i = 0; i < quantity; i++) {
        if (!fgets(buffer, sizeof(buffer), armyf)) {
            fprintf(stderr, "%s\n", ERR_ITEM_COUNT);
            fclose(armyf);
            free(items);
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
            free(items);
            exit(0);
        }

        strcpy(army[i].name, name);
        army[i].name[MAX_NAME] = '\0';

        const ITEM *found1 = NULL;
        const ITEM *found2 = NULL;

        for (int j = 0; j < item_count; j++) {
            if (strcmp(item1, items[j].name) == 0) found1 = &items[j];
            if (item2 && strcmp(item2, items[j].name) == 0) found2 = &items[j];
        }

        if (!found1 || (item2 && !found2)) {
            fprintf(stderr, "%s\n", ERR_WRONG_ITEM);
            fclose(armyf);
            free(items);
            exit(0);
        }

        army[i].item1 = found1;
        army[i].item2 = found2;

        int slots = 0;
        if (found1) slots += found1->slots;
        if (found2) slots += found2->slots;

        if (slots > 2) {
            fprintf(stderr, "%s\n", ERR_SLOTS);
            fclose(armyf);
            free(items);
            exit(0);
        }

        army[i].hp = 100;
    }

    fclose(armyf);
}



int main(const int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        fprintf(stderr, "%s\n", ERR_CMD);
        return 0;
    }

    load_items();

    if (argc == 1) {
        print_items();
        free(items);
        return 0;
    }

    UNIT army1[MAX_ARMY] = {0};
    UNIT army2[MAX_ARMY] = {0};

    load_army(army1, argv[1]);
    load_army(army2, argv[2]);
    print_arena(army1, army2);

    free(items);
    return 0;
}
