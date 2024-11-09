#ifndef SPRITE
#define SPRITE

#include "CeleritObject.hpp"
#include "renderer.hpp"
#include "level.hpp"
#include <unordered_set>

//Sprite class: contains basic functions for position, collision, and includes a renderer pointer
class sprite : public CObject {
    protected:
    renderer* rend;
    dvec2 position;
    rect collision;

    public:
    sprite(renderer& r) : CObject() {
        //creates a basic sprite
        obj_name = "sprite";
        rend = &r;
        position = {0, 0};
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    sprite(renderer& r, v2<T> pos) : CObject() {
        //creates a sprite with a position
        obj_name = "sprite";
        position = pos;
        rend = &r;
    }

    dvec2 get_pos() {
        //returns the position
        return position;
    }

    rect& get_rect() {
        //returns the collision rectangle
        return collision;
    }

    
    virtual bool move(dvec2 move_vec) {
        //moves the sprite (adds the move vector to the current position)
        position += move_vec;
        return true;
    }

    virtual bool set_pos(dvec2 pos) {
        //sets the position
        position = pos;
        return true;
    }

    virtual void draw() {/*override to add drawing funtionality*/};
    virtual void update() {/*override for updating your sprite*/};

    

    virtual bool isColliding(rect other) {
        return collide_rect(collision, other);
    }
};


//A simple sprite with a texture, useful for simple "props"
class prop : public sprite {
    protected:
    texture text;

    public:
    
    prop(renderer& r, const char* file) : sprite(r) {
        text = texture(r, file);
        position = {0, 0};
        obj_name = "prop";
        collision = text.get_rect();
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    prop(renderer& r, const char* file, v2<T> pos) : sprite(r, pos) {
        text = texture(r, file);
        position = {pos.x, pos.y};
        obj_name = "prop";
        collision = text.get_rect();
        collision.x = pos.x;
        collision.y = pos.y;
    }

    virtual void draw(level& l) {
        rend->blit_texture(text, dvec2{position.x - l.get_scroll().x, position.y - l.get_scroll().y}.convert_data<int>());
    }

    texture& get_texture() {
        return text;
    }
};



//class for storing sprites
class sprite_group {
    private:
    
    std::vector<sprite*> sprites;

    public:

    sprite_group() {}

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<sprite, T>>>
    T* create_sprite(T instance) {
        //raw pointers because new is too powerful for such cases
        T* Sprite = (T*)malloc(sizeof(T));
        memcpy(Sprite, &instance, sizeof(T));

        if (Sprite == nullptr) {
            cerr << "Error creating sprite instance!";
            exit(-1);
        }

        sprites.push_back(Sprite);
        return Sprite;
    }

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<sprite, T>>>
    void destroy_sprite(T** Sprite) {
        sprites.erase(std::find(sprites.begin(), sprites.end(), *Sprite));
        free(*Sprite);
        *Sprite = nullptr;
    }

    void draw() {
        for (sprite* s: sprites) {
            s->draw();
        }
    }

    void update() {
        for (sprite* s: sprites) {
            s->update();
        }
    }

    ~sprite_group() {
        for (sprite* sp: sprites) {
            if (sp) free(sp);
        }
    }

};


#endif