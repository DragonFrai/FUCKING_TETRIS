#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "app.cpp"

using namespace std::chrono_literals;

//Screen dimension constants

int main( int argc, char* args[] )
{
    auto app = Tetris_initApplication();

    bool doLoop = true;
    while (doLoop) {
        std::this_thread::sleep_for(20ms);
        doLoop = app.tick();
    }

    Tetris_closeApplication(&app);

    return 0;
}