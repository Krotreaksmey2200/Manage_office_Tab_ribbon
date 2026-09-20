#!/bin/bash
# Script to launch Office Extension & Ribbon Manager

APP_PATH="$(cd "$(dirname "$0")" && pwd)/build/OfficeExtensionManager.app"

if [ -d "$APP_PATH" ]; then
    echo "Opening Office Extension & Ribbon Manager..."
    open "$APP_PATH"
else
    echo "App not found. Building now..."
    cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qtbase -B build
    cmake --build build
    open "$APP_PATH"
fi
