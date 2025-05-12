# Makefile (top-level snippet)

CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# Executable name
EXEC = $(BIN_DIR)/roflands_battle

# Default target - build the game
all: $(EXEC)

# Rule to create directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Rule to build the executable
$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Rule to compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the game
run: $(EXEC)
	./$(EXEC)

.PHONY: all clean run
