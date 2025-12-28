#!/bin/bash
set -e  # stop on first error

echo "Generating Heron Linux Makefiles..."

# go to repo root (assumes script is in scripts/)
cd "$(dirname "$0")/.."

# clean old build files
echo "Cleaning old build files..."
./vendor/premake/premake5 clean

# generate Makefiles
echo "Generating Makefiles..."
./vendor/premake/premake5 gmake2

echo "Done generating Makefiles."