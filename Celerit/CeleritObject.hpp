#ifndef CELERIT_OBJECT
#define CELERIT_OBJECT

#include "util.hpp"
#include "sstream"

using std::stringstream;


//TODO: Make useful
class CObject {
    protected:
    /*
    The object class from which all other "game objects" (sprites, UI ect) will derive from
    */
    string obj_name;//for the to-string method

    public:
    CObject() {
        obj_name = "CObject";
    }

    string to_string() {
        stringstream ss;
        ss << "<" << this << ">" << "[" << obj_name << "]";
        return ss.str();
    }

    string get_obj_name() {
        return obj_name;
    }
};


#endif