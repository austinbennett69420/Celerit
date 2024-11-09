#ifndef LEVEL
#define LEVEL

#include "CeleritObject.hpp"
#include "renderer.hpp"
#include <vector>
#include "map"

using std::vector;
using std::unordered_map;



/*
A class that defines behaviors and other things, essentially creates a playground where gameplay can be easily built upon,
sadly, it is recommended to derive a class from this class rather than using it raw
a level will store all the collision and such and can easily check for collisions using level::is_colliding(rect)
*/
class level : public CObject {
    protected:
    //store our renderer, our current scrolling, and all the collision in our level
    renderer* rend;
    dvec2 scroll_vec;
    vector<rect*> collision_rects;
    

    

    public:

    level(renderer& r) : CObject() {
        //creates a level with no scrolling
        obj_name = "level";
        rend = &r;
        scroll_vec = {0, 0};
    }

    level(renderer& r, dvec2 scrolling) : CObject() {
        //creates a level with set scrolling
        obj_name = "level";
        rend = &r;
        scroll_vec = scrolling;
    }

    
    void focus_scroll(dvec2 pos) {
        //focuses the scroll on a specific point
        int w, h;

        //get the width and height of the screen
        w = rend->get_screen_rect().w;
        h = rend->get_screen_rect().h;

        //to focus, we simply subtract half of the screen size from the position
        //for example: to focus on 0, 0, we need to make an offset such that anything at 0, 0 will appear on the middle of the screen
        // if the screen size is 800, 600 then:
        //0 - 400 = -400
        //0 - 300 = -300
        //then when we apply the offset
        //thing.x - scroll.x (0 - -400) = 400
        //thing.y - scroll.y (0 - -300) = 300
        scroll_vec = {pos.x-((float)w/2.0), pos.y-((float)h/2.0)};
    }

    void scroll(dvec2 scroll_vector) {
        //scrolls the level by a certain amount defined by the vector 2
        scroll_vec.x += scroll_vector.x;
        scroll_vec.y += scroll_vector.y;
    }

    dvec2 get_scroll() {
        //returns the scroll of the level
        return scroll_vec;
    }

    dvec2& get_scroll_refrence() {
        //DO NOT MESS WITH THE REFRENCE, only use for things like particle emmiters
        return scroll_vec;
    }

    void add_collision(rect* r) {
        /*
        add a pointer to a rect for collision in the level
        requires a pointer because:
        1. Makes sure arbitrary collision isnt added
        2. When checking collision we want to make sure we dont check against the same rect
        so we can easily do so by checking if the memory addreses of the 2 rects are the same
        3. so that you know what youre adding to the level rather than taking a refrence and getting the pointer that way
        */
        collision_rects.push_back(r);
    }

    const vector<rect*>& get_collision() {
        //returns an internal vector to all the collision in the level
        return collision_rects;
    }


    bool is_colliding(rect& collision_rect) {
        //checks if <collision_rect> is colliding with any collision in the level
        for (rect* r: collision_rects) {
            if (collide_rect(*r, collision_rect) && r != &collision_rect) {
                return true;
            }
        }
        return false;
    }


    

    virtual void draw() {/*override this function to add custom drawing capabilities*/};

};



#endif