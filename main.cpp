// main.cpp : Defines the entry point for the application.
//
#include <filesystem>
#include <iostream>
#include "game.h"
#include "gui/gui.h"


int main() {


    std::cout << "Working directory: "
        << std::filesystem::current_path()
        << '\n';

    GuiManager manager = GuiManager();
    manager.guiMainLoop();


    return 0;
}

