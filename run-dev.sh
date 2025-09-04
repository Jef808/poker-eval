#!/usr/bin/bash

SOURCES="gui.cpp"

BUILD_COMMAND="cmake --build build --target gui"

EXECUTABLE_PATH="./build/gui"

APP_PID=0

kill_app() {
    if [ $APP_PID -ne 0 ]; then
        if kill -0 $APP_PID 2>/dev/null; then
            echo "--> Stopping application (PID: $APP_PID)..."
            kill $APP_PID
            wait $APP_PID 2>/dev/null
        fi
    fi
    APP_PID=0
}

initial_build_and_run() {
    echo "--> Performing initial build..."
    build_output=$(eval $BUILD_COMMAND 2>&1)
    local build_status=$?

    if [ $build_status -eq 0 ]; then
        echo "--> Build succeeded."
        "$EXECUTABLE_PATH" &
        APP_PID=$!
        echo "--> Application started (PID: $APP_PID)."
    else
        echo "--> Build failed. See details below:"
        echo "$build_output"
        exit 1
    fi
}

# Ensure the app is killed when the script exits (e.g., via Ctrl+C)
trap kill_app EXIT

initial_build_and_run

# Main watch loop
echo "--> Watching for changes in files: $SOURCES"
fswatch -o "$SOURCES" | while read -r event; do
    echo "--> Change detected: $event"
    echo "--> Attempting to rebuild..."

    build_output=$(eval "$BUILD_COMMAND" 2>&1)

    if [ $? -eq 0 ]; then
        echo "--> Rebuild succeeded."
        kill_app
        "$EXECUTABLE_PATH" &
        APP_PID=$!
        echo "--> Application restarted (PID: $APP_PID)."
    else
        echo "--> Rebuild failed. See details below:"
        echo "$build_output"
    fi
done
