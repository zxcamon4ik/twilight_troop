# Makefile (top-level snippet)

myUID := $(shell id -u)

CC = gcc
CFLAGS = 
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
	TERM=xterm-256color ./$(EXEC)

# Docker commands
d-run: 
	@export myUID=${myUID} && \
	COMPOSE_DOCKER_CLI_BUILD=1 DOCKER_BUILDKIT=1 \
		docker compose build --build-arg SRC=$(SRC_DIR) && \
		docker compose up --build 

d-purge:
	@export myUID=${myUID} &&\
	COMPOSE_DOCKER_CLI_BUILD=1 DOCKER_BUILDKIT=1 \
		docker compose down --volumes --remove-orphans --rmi local --timeout 0 

# Shell into the Docker container
d-shell:
	@export myUID=${myUID} && \
	docker compose run --rm -it twilight_troop /bin/bash

# Help target
help:
	@echo "Twilight Troop Makefile"
	@echo ""
	@echo "Usage:"
	@echo "  make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all          Build the game (default target)"
	@echo "  clean        Remove build artifacts"
	@echo "  run          Run the game"
	@echo "  d-run        Build and run in Docker"
	@echo "  d-purge      Remove Docker containers, volumes, and images"
	@echo "  d-shell      Open a shell in the Docker container"
	@echo "  help         Show this help message"

.PHONY: all clean run d-run d-purge d-shell help 
