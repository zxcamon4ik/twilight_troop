#!/bin/bash
set -e

# Set terminal type to support ncurses
export TERM=xterm-256color

# Execute the command passed to docker run
exec "$@"