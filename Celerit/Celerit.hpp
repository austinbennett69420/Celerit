#ifndef CELERIT
//version number
#define CELERIT "0.0.1"

//include libs and things
#include "util.hpp"
#include "renderer.hpp"
#include "screen.hpp"
#include "input.hpp"
#include "CeleritObject.hpp"
#include "sprite.hpp"
#include "level.hpp"
#include "font.hpp"
#include "text_stream.hpp"
#include "UI.hpp"
#include "Particle.hpp"

//Initalize necessary SDL components and things
inline void CELERIT_INIT() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
}

//Clean up
inline void CELERIT_QUIT() {
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    Mix_Quit();
}
// a WHOPPING 2162 lines of code


#endif
