#ifndef INPUT
#define INPUT

#include "util.hpp"
#include <functional>

using std::function, std::vector;

//Union for storing the buttons in the mouse
union mouse_buttons {
    Uint32 mask;
    //buttons here
    struct {
        bool left : 1;      // 1st bit (0x01)
        bool middle : 1;    // 2nd bit (0x02)
        bool right : 1;     // 3rd bit (0x04)
        bool x1 : 1;        // 4th bit (0x08)
        bool x2 : 1;        // 5th bit (0x10)
        Uint32 reserved : 27; // Unused bits (fill out to 32 bits)
    };
};

/*
a class for managing input
*/
class input {
    private:
    //a map of all the keyboard keys
    //all maps are accesed through the SDLK enums
    unordered_map<int, bool> keyboard;
    //mapped functions for when a button is pressed
    unordered_map<int, vector<function<void()>>> button_down_binds;
    //mapped fuctios for when a button is released
    unordered_map<int, vector<function<void()>>> button_up_binds;
    //mouse things
    mouse_buttons mouse;
    ivec2 mouse_pos = {-1, -1};

    public:
    input() {
        //initalize the mouse
        mouse.mask = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        mouse.reserved = 0;
    }

    void bind_keydown(int sdl_keycode, const function<void()> func) {
        //bind a function that returns nothing (but you can use lamda refrences to get around this) to a keyboard key being pressed
        
        //if we dont find the button function vector, we make one
        if (button_down_binds.find(sdl_keycode) == button_down_binds.end()) {
            button_down_binds[sdl_keycode] = vector<function<void()>>();
        }
        //add the function the the SDL keycode
        button_down_binds[sdl_keycode].push_back(func);
    }
    void bind_keyup(int sdl_keycode, const function<void()> func) {
        //bind a function that returns nothing (but you can use lamda refrences to get around this) to a keyboard key being released 

        //the comments would be the same here as up there in input::bind_keydown
        if (button_up_binds.find(sdl_keycode) == button_up_binds.end()) {
            button_up_binds[sdl_keycode] = vector<function<void()>>();
        }
        button_up_binds[sdl_keycode].push_back(func);
    }

    void update(SDL_Event& e) {
        //should be called every frame with an SDL_Event passed to it
        

        //update the mask
        mouse.mask = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        if (e.type == SDL_KEYDOWN) {
            //check and update keypresses
            keyboard[e.key.keysym.sym] = true;
            if (button_down_binds.find(e.key.keysym.sym) != button_down_binds.end()) {
                for (function<void()> i: button_down_binds[e.key.keysym.sym]) {
                    //run all functions in the mapped keybinds
                    i();
                }
            }
        } else if (e.type == SDL_KEYUP) {
            //same thing for key releases
            keyboard[e.key.keysym.sym] = false;
            if (button_up_binds.find(e.key.keysym.sym) != button_up_binds.end()) {
                for (function<void()> i: button_up_binds[e.key.keysym.sym]) {
                    i();
                }
            }
        }
    }



    const unordered_map<int, bool>& get_keyboard() {
        //get the keyboard state, specifically, get the internal map of SDLK enums to bools (a bool meaning that the key is pressed)
        return keyboard;
    }

    mouse_buttons get_mouse() {
        //returns the internal mouse buttons union
        return mouse;
    }

    ivec2 get_mouse_pos() {
        //returns the mouse buttons position
        return mouse_pos;
    }




};



#endif