# Makefile (top-level snippet)

CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DOC_DIR = docs

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# Executable name
EXEC = $(BIN_DIR)/roflands_battle

# Default target - build the game
all: $(EXEC)

# Rule to create directories
$(OBJ_DIR) $(BIN_DIR) $(DOC_DIR):
	mkdir -p $@

# Rule to build the executable
$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Rule to compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DOC_DIR)

# Run the game
run: $(EXEC)
	./$(EXEC)

# Generate documentation using Doxygen (if installed)
docs: | $(DOC_DIR)
	@command -v doxygen >/dev/null 2>&1 || { echo "Doxygen not installed. Please install it to generate documentation."; exit 1; }
	@if [ ! -f Doxyfile ]; then \
		doxygen -g Doxyfile; \
		sed -i 's/PROJECT_NAME.*=.*/PROJECT_NAME = "Twilight Troop"/' Doxyfile; \
		sed -i 's/OUTPUT_DIRECTORY.*=.*/OUTPUT_DIRECTORY = docs/' Doxyfile; \
		sed -i 's/EXTRACT_ALL.*=.*/EXTRACT_ALL = YES/' Doxyfile; \
		sed -i 's/RECURSIVE.*=.*/RECURSIVE = YES/' Doxyfile; \
	fi
	doxygen Doxyfile

# Generate presentation materials
presentation: | $(DOC_DIR)
	@mkdir -p $(DOC_DIR)/presentation
	@cp README.md $(DOC_DIR)/presentation/
	@echo "Generating code structure overview..."
	@find $(SRC_DIR) -type f -name "*.c" -o -name "*.h" | sort > $(DOC_DIR)/presentation/file_list.txt
	@echo "Generating item list..."
	@cat data/items.json > $(DOC_DIR)/presentation/items.json
	@echo "Presentation materials generated in $(DOC_DIR)/presentation/"

.PHONY: all clean run docs presentation
