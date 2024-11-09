#ifndef FONT
#define FONT

#include "util.hpp"

/*
A wrapper for fonts, including some extra functionality such as "font::change_size(int new_size)"
aswell as font::load_system_font(string font_name, int ptsize)
*/
struct font {
    private:
    //the size of the font
    int f_size;
    //the actual SDL font
    TTF_Font* sdl_font = nullptr;
    //the name of the original file for resizing purposes
    string file_name;

    public:

    //default constructor initalizes nothing, SHOULD NOT BE CALLED!
    font() {
        sdl_font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial", 12);
        f_size = 12;
        this->file_name = "C:\\Windows\\Fonts\\Arial";
    }

    font(const string filename, int ptsize) {
        //creates a new font from a ttf font utilizing SDL_TTF library
        sdl_font = TTF_OpenFont(filename.c_str(), ptsize);
        f_size = ptsize;
        this->file_name = filename;
    }

    font(TTF_Font* fnt) {
        //create a font from an already loaded SDL font
        sdl_font = fnt;
    }

    font(const font&) = delete;
    font& operator=(const font&) = delete;

    font& operator = (font& other) {
        if (this != &other) {
            if (sdl_font != nullptr) TTF_CloseFont(sdl_font);
            
            //creates a new font from a ttf font utilizing SDL_TTF library
            sdl_font = TTF_OpenFont(other.file_name.c_str(), other.f_size);
            f_size = other.f_size;
            this->file_name = other.file_name;

        }

        return *this;
    }

    static font load_system_font(string font_name, int ptsize) {
        //static function that loads a font from the windows fonts folder so long as the font is of type .ttf
        return font("C:\\Windows\\Fonts\\" + font_name + ".ttf", ptsize);
    }

    void change_size(int new_size) {
        //changes the size of the font, should not be called every frame because it is quite slow
        TTF_CloseFont(sdl_font);
        sdl_font = TTF_OpenFont(file_name.c_str(), new_size);
        f_size = new_size;
    }

    int get_size() {
        //get the size of the font
        return f_size;
    }

    TTF_Font* get_sdl_font() {
        //returns the actual under-the-hood sdl_font
        return sdl_font;
    }

    ~font() {
        if (sdl_font != nullptr) TTF_CloseFont(sdl_font);
    }

};

#endif