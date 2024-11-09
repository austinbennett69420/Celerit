#ifndef SCREEN
#define SCREEN

#include "util.hpp"


/*
the screen class, essentially a wrapper for an SDL_Window
TODO: Make more useful
*/
class screen {
    private:
    SDL_Window* win;
    rect screen_rect;

    public:
    screen(int w, int h, int sdl_window_flags) {
        //create a window with width, w and height, h centered on screen
        win = SDL_CreateWindow("Celerit Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, sdl_window_flags);

        screen_rect = {0, 0, w, h};
    }

    screen(int sx, int sy, int w, int h, int sdl_window_flags) {
        //create a window positioned at sx, sy with width, w and height, h
        win = SDL_CreateWindow("Celerit Window", sx, sy, w, h, sdl_window_flags);
        screen_rect = {0, 0, w, h};
    }

    void set_title(string s) {
        //sets the title of the window
        SDL_SetWindowTitle(win, s.c_str());
    }

    SDL_Window* get_sdl_window() {
        //returns the internal SDL_Window
        return win;
    }

    rect get_screen_rect() {
        //returns the screen rect
        return screen_rect;
    }

    ~screen() {
        if (win) SDL_DestroyWindow(win);
    }
};

#endif