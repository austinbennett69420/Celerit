#ifndef RENDERER
#define RENDERER

#include "screen.hpp"
#include "util.hpp"
#include "font.hpp"




//A wrapper for an SDL_Renderer, adds more functionality, and creates an easy interface for the rest of the engine
class renderer {
    protected:
    //the internal SDL_Renderer and the screen rectangle
    SDL_Renderer* rend;
    rect screen_rect;


    
    
    //internal function that wraps the SDL_SetRenderDrawColor function for the engine color type
    static inline void SetColor(SDL_Renderer* r, color c) {
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    }

    public:


    //nested texture class, wraps a SDL_Texture and provides added functionality
    //nested so that the texture and renderer can refrence eachother
    class texture {
        private:
        //stores the textures rect and the actual texture itself
        SDL_Texture* text;
        rect texture_rect = {0, 0, 0, 0};
        //storing the rect itself is easier and much faster than manually querying the texture every single time we need it

        public:

        texture() {}//creates a null texture


        texture(renderer& r, string file) {
            //loads a texture from an image file
            text = IMG_LoadTexture((&r)->get_sdl_renderer(), file.c_str());
            SDL_QueryTexture(text, NULL, NULL, &texture_rect.w, &texture_rect.h);
        }

        texture(SDL_Texture* t) {
            //converts an SDL_Texture to a texture
            text = t;
            SDL_QueryTexture(text, NULL, NULL, &texture_rect.w, &texture_rect.h);
        }

        texture(renderer& r, int w, int h) {
            texture_rect.w = w;
            texture_rect.h = h;
            text = SDL_CreateTexture(r.get_sdl_renderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);
            SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
        }

        bool canBeTargeted() {
            int access;
            SDL_QueryTexture(text, nullptr, &access, nullptr, nullptr);

            return access == SDL_TEXTUREACCESS_TARGET;
        }

        SDL_Texture* get_sdl_texture() const {
            //returns the internal SDL_Texture
            if (text == nullptr) {
                cerr << "Error: Texture<" << this << ">" << "has not been constructed or has been destroyed\n";
                exit(-1);
            }
            return text;
        }

        rect get_rect() const {
            //returns the rect of the texture
            if (text == nullptr) {
                cerr << "Error: Texture<" << this << ">" << "has not been constructed or has been destroyed\n";
                exit(-1);
            }
            return texture_rect;
        }

        void scale(int scalar) {
            //scales the texture by an integer scaling
            if (text == nullptr) {
                cerr << "Error: Texture<" << this << ">" << "has not been constructed or has been destroyed\n";
                exit(-1);
            }
            texture_rect.w *= scalar;
            texture_rect.h *= scalar;
        }

        void scale(ivec2 scalar) {
            //scales the texture by a vector2
            if (text == nullptr) {
                cerr << "Error: Texture<" << this << ">" << "has not been constructed or has been destroyed\n";
                exit(-1);
            }
            texture_rect.w *= scalar.x;
            texture_rect.h *= scalar.y;
        }

        void destroy_texture() {
            //destroy the texture manually, since there is no destructor for this class, you should probably call this when youre done with youre texture
            if (text == nullptr) {
                cerr << "Error: Texture<" << this << ">" << "has not been constructed or has been destroyed\n";
                exit(-1);
            }
            SDL_DestroyTexture(text);
            text = nullptr;//no use after frees here
            texture_rect = {0, 0, 0, 0};
        }
    };



    renderer(screen s) {
        //creates a hardware renderer with alpha blending
        rend = SDL_CreateRenderer(s.get_sdl_window(), -1, SDL_RENDERER_ACCELERATED);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        screen_rect = s.get_screen_rect();
    }

    void set_render_target(texture& t) {
        SDL_SetRenderTarget(rend, t.get_sdl_texture());
    }

    void reset_target() {
        SDL_SetRenderTarget(rend, nullptr);
    }

    SDL_Renderer* get_sdl_renderer() {
        //returns the internal SDL_Renderer
        return rend;
    }

    rect get_screen_rect() {
        //returns the screen rect
        return screen_rect;
    }

    texture create_texture(string file) {
        //creates a texture
        return texture(*this, file);
    }

    void update() {
        //presents the render
        SDL_RenderPresent(rend);
    }

    void fill(color c) {
        //fills the screen with the color
        SetColor(rend, c);
        SDL_RenderClear(rend);
    }

    void draw_point(color c, int x, int y) {
        SetColor(rend, c);
        SDL_RenderDrawPoint(rend, x, y);
    }

    void draw_point(color c, ivec2 pos) {
        SetColor(rend, c);
        SDL_RenderDrawPoint(rend, pos.x, pos.y);
    }

    void draw_line(int x1, int y1, int x2, int y2, color c, int width = 1, bool aaliasing = false) {
        //draws a line from a to b with a specified width and very basic anti-aliasing if you enable it
        rect r = {x1, y1, x2-x1, y2-y1};
        if (collide_rect(r, screen_rect)) {
            SetColor(rend, c);
            int offset = 0;
            
            for (; offset < width - (width != 1 && aaliasing); offset++) {
                SDL_RenderDrawLine(rend, x1+offset, y1, x2+offset, y2);
                SDL_RenderDrawLine(rend, x1-offset, y1, x2-offset, y2);
            }
            if (aaliasing) {
                c = {c.r, c.g, c.b, static_cast<uint8_t>(c.a/4)};
                SetColor(rend, c);
                SDL_RenderDrawLine(rend, x1+offset, y1, x2+offset, y2);
                SDL_RenderDrawLine(rend, x1-offset, y1, x2-offset, y2);
            }
            
        }
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void draw_line(v2<T> p1, v2<T> p2, color c, int width = 1, bool aaliasing = false) {
        //draws a line from a to b with a specified width and very basic anti-aliasing if you enable it
        int x1 = static_cast<int>(p1.x);
        int x2 = static_cast<int>(p2.x);
        int y1 = static_cast<int>(p1.y);
        int y2 = static_cast<int>(p2.y);
        rect r = {x1, y1, x2-x1, y2-y1};
        if (collide_rect(r, screen_rect)) {
            SetColor(rend, c);
            int offset = 0;
            
            for (; offset < width - (width != 1 && aaliasing); offset++) {
                SDL_RenderDrawLine(rend, x1+offset, y1, x2+offset, y2);
                SDL_RenderDrawLine(rend, x1-offset, y1, x2-offset, y2);
            }
            if (aaliasing) {
                c = {c.r, c.g, c.b, static_cast<uint8_t>(c.a/4)};
                SetColor(rend, c);
                SDL_RenderDrawLine(rend, x1+offset, y1, x2+offset, y2);
                SDL_RenderDrawLine(rend, x1-offset, y1, x2-offset, y2);
            }
            
        }
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void draw_line(line<T> ln, color c, int width = 1, bool aaliasing = false) {
        //draws a line from a to b with a specified width and very basic anti-aliasing if you enable it
        
        int x1 = static_cast<int>(ln.p1.x);
        int x2 = static_cast<int>(ln.p2.x);
        int y1 = static_cast<int>(ln.p1.y);
        int y2 = static_cast<int>(ln.p2.y);
        rect r = {x1, y1, x2-x1, y2-y1};
        if (collide_rect(r, screen_rect)) {
            SetColor(rend, c);
            int offset = 0;
            
            for (; offset < width - (width != 1 && aaliasing); offset++) {
                SDL_RenderDrawLine(rend, x1+offset, y1, x2+offset, y2);
                SDL_RenderDrawLine(rend, x1-offset, y1, x2-offset, y2);
            }
            if (aaliasing) {
                c = {c.r, c.g, c.b, static_cast<uint8_t>(c.a/4)};
                SetColor(rend, c);
                SDL_RenderDrawLine(rend, x1+offset, y1, x2+offset, y2);
                SDL_RenderDrawLine(rend, x1-offset, y1, x2-offset, y2);
            }
            
        }
    }

    void draw_rect(int x1, int y1, int w, int h, color c, int width = 0) {
        //draws a rectangle on screen with a specified width
        rect r = {x1, y1, w, h};
        //transform rect such that w and h are positive
        if (w < 0) {
            x1 = x1+w;
            w = -w;
        }
        if (h < 0) {
            x1 = x1+h;
            w = -w;
        }
        if (collide_rect(r, screen_rect)) {
            SetColor(rend, c);
            
            
            if (width <= 0) {
                SDL_RenderFillRect(rend, &r);
            } else {
                for (int _ = 0; _ < width; _++) {
                    SDL_RenderDrawRect(rend, &r);
                    r.x += 1;
                    r.y += 1;
                    r.w -= 2;
                    r.h -= 2;
                }
            }
            
        }
    }

    void draw_rect(rect r, color c, int width = 0) {
        //draws a rect on screen with a specified width
        if (collide_rect(r, screen_rect)) {
            SetColor(rend, c);
            
            
            if (width <= 0) {
                SDL_RenderFillRect(rend, &r);
            } else {
                for (int _ = 0; _ < width; _++) {
                    SDL_RenderDrawRect(rend, &r);
                    r.x += 1;
                    r.y += 1;
                    r.w -= 2;
                    r.h -= 2;
                }
            }
            
        }
    }

    void draw_quad(quad q, color c, int width) {
        //draws a quad on screen
        for (int i = 0; i < 4; i++) {
            draw_line(q[i], q[(i+1)%4], c, width);
        }
    }


    void blit_texture(texture& t,  rect source, rect dest, double angle = 0.0, dvec2 center = {0, 0}, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        //draws a texture with a source rect (where from the texture) and a destination rect (where to render) and with rotation and flip around a relative center
        SDL_Point p = center;
        if (collide_rect(dest, screen_rect)) SDL_RenderCopyEx(rend, t.get_sdl_texture(), &source, &dest, angle, &p, flip);
    }

    void blit_texture(texture& t, rect dest, double angle = 0.0, dvec2 center = {0, 0}, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        //blits a texture with a destination rect (where to render) and with rotation and flip around a relative center
        SDL_Point p = center;
        if (collide_rect(dest, screen_rect)) {
            rect r = t.get_rect();
            SDL_RenderCopyEx(rend, t.get_sdl_texture(), &r, &dest, angle, &p, flip);
        }
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void blit_texture(texture& t, v2<T> vec, double angle = 0.0, dvec2 center = {0, 0}, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        //blits a texture at the vector position and with rotation and flip around a relative center
        SDL_Point p = center;
        rect src = t.get_rect();
        rect r = {static_cast<int>(round(vec.x)), static_cast<int>(round(vec.y)), src.w, src.h};
        if (collide_rect(r, screen_rect)) SDL_RenderCopyEx(rend, t.get_sdl_texture(), &src, &r, angle, &p, flip);
    }

    void blit_texture(texture& t, int x, int y, double angle = 0.0, dvec2 center = {0, 0}, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        //blits a texture at position {X, Y} and with rotation and flip around a relative center
        SDL_Point p = center;
        rect src = t.get_rect();
        rect r = {x, y, src.w, src.h};
        if (collide_rect(r, screen_rect)) SDL_RenderCopyEx(rend, t.get_sdl_texture(), &src, &r, angle, &p, flip);
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    ivec2 render_text(font& fnt, string text, v2<T> pos, color fg, color bg = {0, 0, 0, 0}) {
        /*
        draws text onto the screen using the specified font and fg and background colors
        returns the width and height of the text
        */

        SDL_Surface* rendered_text;

        if (bg.a > 0) {
            rendered_text = TTF_RenderText(fnt.get_sdl_font(), text.c_str(), fg, bg);
        } else {
            rendered_text = TTF_RenderText_Solid(fnt.get_sdl_font(), text.c_str(), fg);
        }
        SDL_Texture* t = SDL_CreateTextureFromSurface(rend, rendered_text);


        rect text_rect = {0, 0, 0, 0};
        SDL_QueryTexture(t, nullptr, nullptr, &text_rect.w, &text_rect.h);
        rect dest = text_rect;
        dest.x = pos.x;
        dest.y = pos.y;
        if (collide_rect(dest, screen_rect)) SDL_RenderCopy(rend, t, &text_rect, &dest);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(rendered_text);

        return {text_rect.w, text_rect.h};
    }


    void draw_circle(int centerX, int centerY, int radius, color c, bool filled = false) {
        //draws a circle centered at X, Y with a radius and can be filled or not filled


        // Check if the circle is within the screen rectangle
        
        rect boundingBox = { centerX - radius, centerY - radius, 2 * radius, 2 * radius };
        if (!collide_rect(boundingBox, screen_rect)) {
            return; // Circle is out of bounds
        }

        SetColor(rend, c); // Set the drawing color
        

        if (filled) {
            // Draw a filled circle
            for (int y = -radius; y <= radius; ++y) {
                int xLimit = static_cast<int>(sqrt(radius * radius - y * y)); // Calculate the x limit
                SDL_RenderDrawLine(rend, centerX - xLimit, centerY + y, centerX + xLimit, centerY + y); // Draw horizontal line
            }
        } else {
            // Draw an outline circle using Midpoint Circle Algorithm
        
            int x = radius;
            int y = 0;
            int decisionOver2 = 1 - x; // Decision variable

            while (y <= x) {
                // Draw the eight octants of the circle
                SDL_RenderDrawPoint(rend, centerX + x, centerY + y);
                SDL_RenderDrawPoint(rend, centerX + y, centerY + x);
                SDL_RenderDrawPoint(rend, centerX - x, centerY + y);
                SDL_RenderDrawPoint(rend, centerX - y, centerY + x);
                SDL_RenderDrawPoint(rend, centerX + x, centerY - y);
                SDL_RenderDrawPoint(rend, centerX + y, centerY - x);
                SDL_RenderDrawPoint(rend, centerX - x, centerY - y);
                SDL_RenderDrawPoint(rend, centerX - y, centerY - x);

                y++;

                // Update the decision variable
                if (decisionOver2 <= 0) {
                    decisionOver2 += 2 * y + 1; // Midpoint is inside or on the perimeter
                } else {
                    x--;
                    decisionOver2 += 2 * y - 2 * x + 1; // Midpoint is outside the perimeter
                }
            }
        }
    }

    
    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    ivec2 render_aatext(font& fnt, string text, v2<T> pos, color fg) {
        /*
        draws text with anti aliasing
        returns the width and height of the text
        */

        SDL_Surface* rendered_text;

        
        rendered_text = TTF_RenderText_Blended(fnt.get_sdl_font(), text.c_str(), fg);
        
        SDL_Texture* t = SDL_CreateTextureFromSurface(rend, rendered_text);


        rect text_rect = {0, 0, 0, 0};
        SDL_QueryTexture(t, nullptr, nullptr, &text_rect.w, &text_rect.h);
        rect dest = text_rect;
        dest.x = pos.x;
        dest.y = pos.y;
        if (collide_rect(dest, screen_rect)) SDL_RenderCopy(rend, t, &text_rect, &dest);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(rendered_text);

        return {text_rect.w, text_rect.h};
    }

    ~renderer() {
        SDL_DestroyRenderer(rend);
    }

};



//type stuff
using texture = renderer::texture;



#endif