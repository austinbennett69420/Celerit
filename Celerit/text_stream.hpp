#ifndef TEXT_STREAM
#define TEXT_STREAM

#include "util.hpp"
#include "renderer.hpp"
#include "sstream"
#include <iomanip>
#include <map>

//Flags
namespace tstream {
    struct flag {
        
        uint32_t bin_flag;
        int value;
        const char* descripton;

        

        flag& operator()(int arg) {
            value = arg;
            return *this;
        }
    };

    static flag fNULL = {0, 0, "null flag"};//null flag
    static flag TRUNCATE = {1, -1, "TRUNCATION"};//truncates double values
}

/*
A class that makes drawing text exceptionally easy
*/
class text_stream {


    private:
    dvec2 pos;
    renderer* rend;
    font* Font;
    ivec2 current_offset = {0, 0};
    ivec2 buffer_offset = {0, 0};
    color Color;
    std::unordered_map<uint32_t, tstream::flag> flags = unordered_map<uint32_t, tstream::flag>();
    


    public:

    
    
    
    

    text_stream(renderer& Renderer, dvec2 Pos, font& f, color c) {
        //creates a text stream
        rend = &Renderer;
        this->pos = Pos;
        Font = &f;
        Color = c;

        

        for (int i = 1; i < 32; i = i*2) {
            flags[i] = tstream::fNULL;
        }

        flags[tstream::TRUNCATE.bin_flag] = tstream::TRUNCATE;
        flags[tstream::TRUNCATE.bin_flag].value = 1;
        
        
    }

    void change_color(color c) {
        //changes the color of the text
        Color = c;
    }

    text_stream& operator << (string s) {
        //renders a string of text
        int start = 0;
        ivec2 offset = {0, 0};

        if (s.length() > 0 && s[0] == '\n') {
            current_offset.y += buffer_offset.y;

        }

        for (int i = 0; i < s.length(); i++) {
            if (s[i] == '\n') {
                offset = rend->render_aatext(*Font, s.substr(start, i-start), (pos.convert_data<int>()+current_offset), Color);
                start = i+1;
                current_offset.y += offset.y + 10;
                current_offset.x = 0;
            }
        }
        offset = rend->render_aatext(*Font, s.substr(start), (pos.convert_data<int>()+current_offset), Color);
        current_offset.x += offset.x;
        
        buffer_offset = offset;
        return *this;
    }

    text_stream& operator << (std::stringstream ss) {
        //renders a string stream as text
        return (*this << ss.str());
    }

    text_stream& operator << (int i) {
        //renders an int as text
        return (*this << std::to_string(i));
    }

    text_stream& operator << (unsigned int i) {
        //renders a unsigned int as text
        return (*this << std::to_string(i));
    }

    text_stream& operator << (long long i) {
        //renders a long long as text
        return (*this << std::to_string(i));
    }

    text_stream& operator << (unsigned long long i) {
        //renders a unsigned long long as text
        return (*this << std::to_string(i));
    }

    text_stream& operator << (long i) {
        //renders a long as text
        return (*this << std::to_string(i));
    }


    text_stream& operator << (unsigned long i) {
        //renders a unsigned long as text
        return (*this << std::to_string(i));
    }



    text_stream& operator << (float i) {
        //renders a float as text, use the TS::TRUNCATE flag to truncate the value
        std::stringstream db;
        if (flags[tstream::TRUNCATE.bin_flag].bin_flag != 0 && flags[1].value != -1) {
            db << std::fixed << std::setprecision(flags[1].value) << i;
        }
        return (*this << db.str());
    }

    text_stream& operator << (double i) {
        //renders a double as text, use the TS::TRUNCATE flag to truncate the value
        std::stringstream db;
        if (flags[tstream::TRUNCATE.bin_flag].bin_flag != 0 && flags[1].value != -1) {
            db << std::fixed << std::setprecision(flags[1].value) << i;
        }
        return (*this << db.str());
    }

    text_stream& operator << (long double i) {
        //renders a double as text, use the TS::TRUNCATE flag to truncate the value
        std::stringstream db;
        if (flags[tstream::TRUNCATE.bin_flag].bin_flag != 0 && flags[1].value != -1) {
            db << std::fixed << std::setprecision(flags[1].value) << i;
        }
        return (*this << db.str());
    }

    text_stream& operator << (rect r) {
        //renders a rect as text
        return (*this << "{" << r.x << ", " << r.y << ", " << r.w << ", " << r.h << "}");
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    text_stream& operator << (v2<T> vec) {
        //renders a vector2 as text
        return (*this << "{" << vec.x << ", " << vec.y << "}");
    }

    text_stream& operator << (tstream::flag fl) {
        //adds a flag to the stream
        flags[fl.bin_flag] = fl;
        return *this;
    }


    void flush() {
        /*
        presents all text, resets the offsets and flags
        */
        current_offset = {0, 0};
        for (int i = 1; i < 32; i = i*2) {
            flags[i] = tstream::fNULL;
        }
    }

    

};



#endif
