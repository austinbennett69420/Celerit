#include "input.hpp"
#include <optional>
#ifndef UI
#define UI "Available UI features: progress bars, images, canvases"

#include <memory>
#include <vector>

#include "renderer.hpp"
#include "CeleritObject.hpp"



/*
A base class for all UI elements, contains functions for transform aswell as interface functions
*/
class CUIElement : public CObject {
    protected:
    dvec2 scaling = {1, 1};
    double rotation_angle = 0.0;
    dvec2 scr_pos = {0, 0};
    dvec2 relative_center = {0, 0};

    public:
    
    CUIElement() {
        obj_name = "CUIElement";
    }

    virtual void scale(double scalar) {
        scaling.x *= scalar;
        scaling.y *= scalar;

        relative_center.x *= scalar;
        relative_center.y *= scalar;
    }

    virtual void scale(dvec2 scalar) {
        scaling.x *= scalar.x;
        scaling.y *= scalar.y;

        relative_center.x *= scalar.x;
        relative_center.y *= scalar.y;
    }

    virtual void set_scale(dvec2 scale) {
        
        relative_center.x *= scale.x / scaling.x;
        relative_center.y *= scale.y / scaling.y;
        
        scaling = scale;

        
    }


    virtual void set_pos(dvec2 vec) {

        relative_center += (vec - scr_pos);

        scr_pos = vec;
    }

    virtual void move(dvec2 movement) {
        relative_center += movement;
        scr_pos += movement;
    }

    virtual void rotate(arcdegrees angle) {
        rotation_angle += angle;
        rotation_angle = rotation_clamp(rotation_angle, 0.0, 360.0);
    }

    virtual void set_rotation(arcdegrees angle) {
        rotation_angle = angle;
        rotation_angle = rotation_clamp(rotation_angle, 0.0, 360.0);
    }

    transform<double> get_transfrom() {
        return {scr_pos, scaling, rotation_angle};
    }

    virtual void draw() {};

    virtual void set_relative_center(dvec2 new_center) {
        relative_center = new_center;
    };
};





/*
A Progress bar, if overlay mode is set, then it will render the filled texture from the back,
otherwise it will start rendering from the front
*/
class progress_bar : public CUIElement {
    protected:

    float progress = 0.0;
    texture filled;
    texture empty;
    renderer* rend;
    rect self_rect;
    bool overlay_mode = false;

    

    dvec2 last_scaling = scaling;

    public:

    progress_bar(renderer& r, 
    texture empty_texture, 
    texture filled_texture, 
    dvec2 scr_pos
    ) : CUIElement() {
        filled = filled_texture;
        empty = empty_texture;
        this->scr_pos = scr_pos;
        rend = &r;
        self_rect = {static_cast<int>(scr_pos.x), static_cast<int>(scr_pos.y), empty_texture.get_rect().w, empty_texture.get_rect().h};


        obj_name = "Progress Bar";
        relative_center = ivec2{self_rect.w / 2, self_rect.h / 2}.convert_data<double>();
        
    }

    progress_bar(renderer& r,
    texture empty_texture,
    texture filled_texture,
    dvec2 scr_pos,
    float starting_percent
    ) : CUIElement() {
        starting_percent = clamp(starting_percent, 0.0F, 1.0F);
        rend = &r;
        progress = starting_percent;
        filled = filled_texture;
        empty = empty_texture;
        this->scr_pos = scr_pos;
        self_rect = {static_cast<int>(scr_pos.x), static_cast<int>(scr_pos.y), empty_texture.get_rect().w, empty_texture.get_rect().h};


        obj_name = "Progress Bar";
        
    }

    progress_bar(
        renderer& r,
        color empty_color,
        color filled_color,
        dvec2 scr_pos,
        int width,
        int height
    ) : CUIElement() {
        this->scr_pos = scr_pos;
        rend = &r;
        self_rect = {static_cast<int>(scr_pos.x), static_cast<int>(scr_pos.y), width, height};
        
        SDL_Surface* emp = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_Surface* fil = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        SDL_FillRect(emp, NULL, SDL_MapRGBA(emp->format, empty_color.r, empty_color.g, empty_color.b, empty_color.a));
        SDL_FillRect(fil, NULL, SDL_MapRGBA(emp->format, filled_color.r, filled_color.g, filled_color.b, filled_color.a));

        empty = texture(SDL_CreateTextureFromSurface(rend->get_sdl_renderer(), emp));
        filled = texture(SDL_CreateTextureFromSurface(rend->get_sdl_renderer(), fil));

        SDL_FreeSurface(emp);
        SDL_FreeSurface(fil);


        obj_name = "Progress Bar";

    }

    progress_bar(
        renderer& r,
        color empty_color,
        color filled_color,
        dvec2 scr_pos,
        int width,
        int height,
        float starting_percent
    ) : CUIElement() {
        this->scr_pos = scr_pos;
        rend = &r;
        self_rect = {static_cast<int>(scr_pos.x), static_cast<int>(scr_pos.y), width, height};
        

        SDL_Surface* emp = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_Surface* fil = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        SDL_FillRect(emp, NULL, SDL_MapRGBA(emp->format, empty_color.r, empty_color.g, empty_color.b, empty_color.a));
        SDL_FillRect(fil, NULL, SDL_MapRGBA(emp->format, filled_color.r, filled_color.g, filled_color.b, filled_color.a));

        empty = texture(SDL_CreateTextureFromSurface(rend->get_sdl_renderer(), emp));
        filled = texture(SDL_CreateTextureFromSurface(rend->get_sdl_renderer(), fil));

        SDL_FreeSurface(emp);
        SDL_FreeSurface(fil);


        starting_percent = clamp(starting_percent, 0.0F, 1.0F);
        progress = starting_percent;


        obj_name = "Progress Bar";
    }

    void set_overlay_mode(bool b) {
        overlay_mode = true;
    }

    void increase_percent(float amount) {
        //amount should be between 0-1
        amount = clamp(amount, -1.0F, 1.0F);
        progress = clamp(progress+amount, 0.0F, 1.0F);
    }

    void set_percent(float x) {
        progress = clamp(x, 0.0F, 1.0F);
    }

    float get_percent() {
        return progress;
    }

    void move(dvec2 vec) override {
        CUIElement::move(vec);
        self_rect.x = scr_pos.x;
        self_rect.y = scr_pos.y;
    }

    void set_pos(dvec2 vec) override {
        CUIElement::set_pos(vec);
        self_rect.x = vec.x;
        self_rect.y = vec.y;
    }

    void draw() override {
        
            
        rend->blit_texture(empty, {self_rect.x, self_rect.y, static_cast<int>(self_rect.w * scaling.x), static_cast<int>(self_rect.h*scaling.y)},
        rotation_angle, ivec2{static_cast<int>((self_rect.w*scaling.x)/2), static_cast<int>((self_rect.h*scaling.y)/2)}.convert_data<double>());
        
        rect filled_rect = filled.get_rect();
        if (!overlay_mode) {
            filled_rect.x = filled_rect.w - (filled_rect.w*progress);
            filled_rect.w = (filled_rect.w*progress);
        } else {
            filled_rect.w = (filled_rect.w*progress);
        }
        rend->blit_texture(filled, filled_rect, {self_rect.x, self_rect.y, static_cast<int>(filled_rect.w*scaling.x), 
        static_cast<int>(scaling.y*filled_rect.h)}, 
        rotation_angle, ivec2{static_cast<int>((self_rect.w*scaling.x)/2), static_cast<int>((self_rect.h*scaling.y)/2)}.convert_data<double>());

        
    }
};







class image : public CUIElement {
    protected:
    renderer* r;
    texture t;

    public:
    
    image(renderer& rend, string texture_file, dvec2 scrpos) : CUIElement() {
        r = &rend;
        this->scr_pos = scrpos;
        t = texture(rend, texture_file);
    }

    image(renderer& rend, texture& text, dvec2 scrpos) : CUIElement() {
        r = &rend;
        this->scr_pos = scrpos;
        t = text;
    }

    void draw() override {
        r->blit_texture(t, {static_cast<int>(scr_pos.x), static_cast<int>(scr_pos.y), 
        static_cast<int>(scaling.x*t.get_rect().w), static_cast<int>(scaling.y*t.get_rect().h)}, rotation_angle,
        {(scaling.x*t.get_rect().w)/2.0, (scaling.y*t.get_rect().h)/2.0});
    }
};











/*
fT the return type of the buttons function

*/
class button : public CUIElement {
    protected:
    quad button_quad;
    rect unrotated;
    bool active = true;
    texture unpressed_texture;
    texture pressed_texture;
    bool is_pressed = false;
    renderer* rend;
    
    

    public:

    

    button(renderer& r, texture unpressed, texture pressed, dvec2 scr_pos) {
        rend = &r;
        unpressed_texture = unpressed;
        pressed_texture = pressed;
        button_quad = quad::rtoq(unpressed_texture.get_rect());
        unrotated = unpressed_texture.get_rect();
        this->scr_pos = scr_pos;
        button_quad.move(scr_pos);

        obj_name = "Button";
        relative_center = ivec2{unrotated.w /2, unrotated.h/2}.convert_data<double>();
    }

    button(renderer& r, color unpressed, color pressed, rect rectangle) {
        rend = &r;

        SDL_Surface* unpre = SDL_CreateRGBSurface(0, rectangle.w, rectangle.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_Surface* press = SDL_CreateRGBSurface(0, rectangle.w, rectangle.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        SDL_FillRect(unpre, NULL, SDL_MapRGBA(unpre->format, unpressed.r, unpressed.g, unpressed.b, unpressed.a));
        SDL_FillRect(press, NULL, SDL_MapRGBA(press->format, pressed.r, pressed.g, pressed.b, pressed.a));

        unpressed_texture = texture(SDL_CreateTextureFromSurface(rend->get_sdl_renderer(), unpre));
        pressed_texture = texture(SDL_CreateTextureFromSurface(rend->get_sdl_renderer(), press));

        SDL_FreeSurface(unpre);
        SDL_FreeSurface(press);

        scr_pos = ivec2{rectangle.x, rectangle.y}.convert_data<double>();

        button_quad = quad::rtoq(unpressed_texture.get_rect());
        unrotated = unpressed_texture.get_rect();
        button_quad.move(scr_pos);


        obj_name = "Button";
        relative_center = ivec2{unrotated.w /2, unrotated.h/2}.convert_data<double>();
    }

    
    

    bool is_clicked(input& i) {
        if (i.get_mouse().left && button_quad.is_in(i.get_mouse_pos().convert_data<double>())) {
            is_pressed = true;
            return true;
        } else {
            is_pressed = false;
            return false;
        }
    }

    
    bool update(bool is_being_pressed) {
        //returns whether the buttons state has changed or not
        if (is_being_pressed != is_pressed) {
            is_pressed = is_being_pressed;
            return true;
        }
        return false;
    }

    void press() {
        is_pressed = true;
    }


    bool get_pressed() {
        return is_pressed;
    }

    bool get_active() {
        return active;
    }

    void set_active(bool b) {
        active = b;
    }


    void scale(double scalar) override {
        CUIElement::scale(scalar);

        button_quad.scale(scalar, scr_pos);
    }

    void scale(dvec2 scalar) override {
        CUIElement::scale(scalar);
        
        button_quad.scale(scalar, scr_pos);
    }

    void set_scale(dvec2 scale) override {

        button_quad.scale(dvec2{scaling.x / scale.x, scaling.y / scale.y}, scr_pos);

        CUIElement::set_scale(scale);


    }

    void set_pos(dvec2 vec) override {
        button_quad.move(vec-scr_pos);

        CUIElement::set_pos(vec);

        
    }

    void move(dvec2 movement) override {
        CUIElement::move(movement);
        button_quad.move(movement);
    }

    void rotate(arcdegrees angle) override {
        rotation_angle += angle;
        button_quad.rotate(angle);
    }

    void set_rotation(arcdegrees angle) override {
        button_quad.rotate(angle - rotation_angle);
        
        rotation_angle = angle;
        
    }

    quad get_quad() {
        return button_quad;
    }

    void draw() override {
        rect r = rect{static_cast<int>(scr_pos.x), static_cast<int>(scr_pos.y), static_cast<int>(unrotated.w*scaling.x), 
            static_cast<int>(unrotated.h*scaling.y)};
        
        if (is_pressed) {
            rend->blit_texture(pressed_texture, r, 
            rotation_angle, relative_center);
        } else {
            rend->blit_texture(unpressed_texture, r, rotation_angle, relative_center);
        }
        rend->draw_rect(r, WHITE, 1);
    }
};


class canvas : public CUIElement {
    std::vector<CUIElement*> elements;

    public:
    canvas(dvec2 pos) : CUIElement() {
        obj_name = "Canvas";
        scr_pos = pos;
    }


    template<typename T, typename = std::enable_if_t<std::is_base_of_v<CUIElement, T>>>
    T* create_UI_element(T instance) {
        
        T* UIobj = (T*)malloc(sizeof(T));
        memcpy(UIobj, &instance, sizeof(T));
        
        
        if (UIobj == nullptr) {
            cerr << "Error creating UI object!";
            exit(-1);
        }


        elements.push_back(UIobj);

        dvec2 rel_pos = elements.back()->get_transfrom().position;
        elements.back()->set_pos(scr_pos + rel_pos);

        return UIobj;

    }

    void scale(double scalar) override {
        CUIElement::scale(scalar);
        for (CUIElement* elm: elements) {
            elm->scale(scalar);
        }
    }

    void scale(dvec2 scalar) override {
        CUIElement::scale(scalar);
        for (CUIElement* elm: elements) {
            elm->scale(scalar);
        }
    }

    void set_scale(dvec2 scale) override {

        for (CUIElement* elm: elements) {
            elm->scale(scaling - scale);
        }

        CUIElement::set_scale(scale);
    }

    void move(dvec2 movement) override {
        CUIElement::move(movement);
        for (CUIElement* elm: elements) {
            elm->move(movement);
        }
    }

    void set_pos(dvec2 vec) override {
        
        for (CUIElement* elm: elements) {
            elm->move(vec-scr_pos);
        }
        CUIElement::set_pos(vec);
    }

    void rotate(arcdegrees angle) override {
        CUIElement::rotate(angle);
        for (CUIElement* elm: elements) {
            elm->rotate(angle);
        }
    }

    void set_rotation(arcdegrees angle) override {
        
        for (CUIElement* elm: elements) {
            elm->rotate(angle - rotation_angle);
        }
        CUIElement::rotate(angle);
    }
    


    void draw() override {
        for (CUIElement* elm: elements) {
            elm->draw();
        }
    }



    ~canvas() {
        for (CUIElement* elem: elements) {
            free(elem);
        }
    }

};


#endif