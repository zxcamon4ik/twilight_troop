#!/bin/bash
set -e

echo "Building Twilight Troop..."
make

echo "Running Twilight Troop..."
# Instead of directly running the game, which requires a proper terminal,
# we'll just display a success message
echo "Build successful! The game is ready to run."
echo "To run the game interactively, use: docker exec -it twilight_troop-twilight_troop-1 /bin/bash"
echo "Then inside the container: TERM=xterm-256color ./bin/roflands_battle"

# Keep container running for debugging
tail -f /dev/null