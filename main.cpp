// main.cpp : Defines the entry point for the application.
//

#include "game.h"
#include "gui.h"


int main() {


    GuiManager manager = GuiManager();
    manager.guiMainLoop();


    return 0;
}

