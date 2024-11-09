#ifndef UTIL
#define UTIL

//this file includes includes that every file will use basically
#include <cmath>
#include <cstdint>
#include "sstream"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <cfloat>
#include <chrono>
#include <cstdlib>


using namespace std::chrono;

typedef double arcdegrees;
typedef double radians;
typedef long double seconds_t;
typedef long double milliseconds_t;
typedef long double nanoseconds_t;

const double nulldub = std::numeric_limits<double>::infinity();
const double nullint = std::numeric_limits<int>::infinity();

constexpr double RADIAN_CONVERSION = M_PI / 180.0;
constexpr double DEGREE_CONVERSION = 180.0 / M_PI;

inline seconds_t getUTCTime() {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    now = time_point_cast<nanoseconds>(now);

    
    nanoseconds now_ns = now.time_since_epoch();
    unsigned long long ns = duration_cast<nanoseconds>(now_ns).count();

    return static_cast<long double>(ns) / 1000000000.0;//convert to seconds
}

inline milliseconds_t getUTCMilliTime() {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    now = time_point_cast<nanoseconds>(now);

    
    nanoseconds now_ns = now.time_since_epoch();
    unsigned long long ns = duration_cast<nanoseconds>(now_ns).count();

    return static_cast<long double>(ns) / 1000000.0;//convert to milliseconds
}

inline nanoseconds_t getUTCNanoTime() {
    high_resolution_clock::time_point now = high_resolution_clock::now();
    now = time_point_cast<nanoseconds>(now);

    
    nanoseconds now_ns = now.time_since_epoch();
    unsigned long long ns = duration_cast<nanoseconds>(now_ns).count();

    return static_cast<long double>(ns);
}




//a SDL_Rect that can be outputeed to a stream
struct rect : public SDL_Rect {

    friend std::ostream& operator <<(std::ostream& os, rect& self) {
        os << "{" << self.x << ", " << self.y << ", " << self.w << ", " << self.h << "}";
        return os;
    }

};




using std::string, std::cerr, std::cout, std::unordered_map;

inline bool collide_rect(rect r1, rect r2) {
    // Check if one rectangle is to the left or right of the other
    if (r1.x + r1.w < r2.x || r2.x + r2.w < r1.x) {
        return false;
    }
    
    // Check if one rectangle is above or below the other
    if (r1.y + r1.h < r2.y || r2.y + r2.h < r1.y) {
        return false;
    }
    
    // Otherwise, the rectangles are colliding
    return true;
}

inline double rsqrt(double number) {
    //shamelessly stolen from quake
	long i;
	double x2, y;
	const double threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return y;
}


//the color struct for storing color information
struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 255;

    friend std::ostream& operator << (std::ostream& os, color& self) {
        os << "{" << static_cast<int>(self.r) << ", " << static_cast<int>(self.g) << ", " << static_cast<int>(self.b) << ", " << static_cast<int>(self.a) << "}";
        return os;
    }

    operator SDL_Color() {
        return {r, g, b, a};
    }
};

//some colors
const color RED = {255, 0, 0};
const color ORANGE = {255, 128, 0};
const color YELLOW = {255, 255, 0};
const color GREEN = {0, 255, 0};
const color AQUA = {0, 255, 255};
const color BLUE = {0, 0, 255};
const color PURPLE = {255, 0, 255};
const color WHITE = {255, 255, 255};
const color BLACK = {0, 0, 0};
const color EMPTY = {0, 0, 0, 0};


//the v2 or vector-2 type, stores 2 numbers, an X and a Y
template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct v2 {


    public:
    

    T x;
    T y;


    

    T get_distance() {
        //returns the distance of the vector
        return sqrt((x*x)+(y*y));
    }

    T get_distance2() {
        //returns the squared distance of the vector
        return (x*x) + (y*y);
    }

    template<typename number_t = int, typename = typename std::enable_if<std::is_arithmetic<number_t>::value, number_t>::type>
    static T cast(number_t n) {
        return static_cast<T>(n);
    }


    v2 normalize() {
        //set the vectors distance to 1 without changing its overall angle/trajectory
        double invroot = rsqrt((x*x)+(y*y));
        return {x*invroot, y*invroot};
    }

    arcdegrees get_horizantal_angle() {
        //returns the angle of the resulting line from 0, 0 to this vector
        double slope = static_cast<double>(y) / static_cast<double>(x);
        return std::atan(slope) * DEGREE_CONVERSION;
    }

    v2 get_rotated(arcdegrees angle, v2<double> origin = {0, 0}) {
        radians r_angle = angle * RADIAN_CONVERSION;
        T nx = cast(
            x*cos(r_angle) - y*sin(r_angle)
        );
        T ny = cast(
            x*sin(r_angle) + y*cos(r_angle)
        );

        return {nx, ny};
    }

    //operators
    inline bool operator ==(v2 other) {
        //returns if the other vectors x and y are equal to each other
        return other.x == x && other.y == y;
    }
    inline bool operator !=(v2 other) {
        //returns the opposite as the previous
        return !(*this == other);
    }

    //adds the components of 2 vectors and returns the result
    v2 operator +(v2 other) {
        return {x+other.x, y+other.y};
    }

    //subtracts the components of 2 vectors and returns the result
    v2 operator -(v2 other) {
        return {x-other.x, y-other.y};
    }

    //adds the others components to this
    void operator +=(v2 other) {
        x += other.x;
        y += other.y;
    }

    //subtracts the others components from this
    void operator -=(v2 other) {
        x -= other.x;
        y -= other.y;
    }

    v2 operator *(v2& other) {
        //returns the vectors values multplied by the others, use v2::get_distance2() for the dot product
        return {x * other.x, y * other.y};
    }

    void operator *=(v2& other) {
        //returns the vectors values multplied by the others, use v2::get_distance2() for the dot product
        x = x*other.x;
        y = y*other.y;
    }


    v2 get_opposite() {
        //returns a vector pointing opposite to this
        return {-x, -y};
    }
    



    //so that this can be used as a SDL_Point
    operator SDL_Point() {
        return {static_cast<int>(std::round(x)), static_cast<int>(round(y))};
        
    }

    //for accesibility or code style
    T operator [](int index) {
        if (index > 0 || index > 1) {
            cerr << "Cannot access member of index " << index << "\nvalid indexes are { 0, 1 }";
        }
        if (index == 0) return x;
        if (index == 1) return y;
    }

    //for accesibility or code style
    T operator [](char index) {
        if (index < 97) index -= 32;
        if (index != 'x' && index != 'y') {
            cerr << "Cannot access member " << index << "\nvalid indexes are { 'x', 'y' } and their uppercase counterparts";
        }
        if (index == 'x') return x;
        if (index == 'y') return y;
    }

    //prints the vector
    friend std::ostream& operator << (std::ostream& os, v2 self) {
        os << "{" << self.x << ", " << self.y << "}";
        return os;
    }

    //returns the vector as a string
    string to_string() {
        std::stringstream ss;
        ss << "{" << x << ", " << y << "}";
        return ss.str();
    }

    //converts the data type of this vector to a new data type and returns the resultant vector
    template<typename nT, typename = typename std::enable_if<std::is_arithmetic<nT>::value, nT>::type>
    v2<nT> convert_data() {
        return v2<nT>{static_cast<nT>(x), static_cast<nT>(y)};
    }
    
};

template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
v2<T> make_vec2(arcdegrees angle, T length) {
    
    //creates a v2 of length **length** at the angle **angle**
    v2<T> ret;
    radians r_angle = angle * RADIAN_CONVERSION;
    ret.x = length * cos(r_angle);
    ret.y = length * cos(r_angle);

    return ret;
}

//a struct for storing transform
template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct transform {
    v2<T> position;
    v2<T> scale;
    double rotation_angle;
};

//some typedefs for v2
typedef v2<int> ivec2;
typedef v2<long> lvec2;
typedef v2<double> dvec2;
typedef v2<float> fvec2;


//function for clamping a value to be in the range [min, max]
template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
T clamp(T x, T min, T max) {
    return x < min ? min:(x > max ? max:x);
}

/*
function for clamping a number between min-max in a mod style

example: rotation_clamp(9, 2, 6) will return 5 because we can think of it like rotating the number x
since rotating can only really be between 0-359 when we add 3 to 359 instead of getting 362, we get 2
and when we apply this logic to 9, 2, 6, 9 is like adding 3 to 6 so we instead add 3 to 2
*/
template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
T rotation_clamp(T x, T min, T max) {
    T over = (x - min) - (max-min);
    return over > 0 ? over:x;
}


//a structure for representing a line
template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct line {
    v2<T> p1;
    v2<T> p2;
};



typedef line<int> iline;
typedef line<long> Lline;
typedef line<double> dline;
typedef line<float> fline;

//raycasts a line into a virtual space of more lines and returns the number of intersections
inline int ray_cast(dline l1, const std::vector<dline>& lines) {
    // Returns the number of intersections of line l1 with the lines in the vector

    bool l1_vertical = (l1.p1.x == l1.p2.x);
    double m1 = 0, b1 = 0; // m1 and b1 must be declared outside of conditional blocks
    
    if (!l1_vertical) {
        m1 = (l1.p2.y - l1.p1.y) / (l1.p2.x - l1.p1.x); // Calculate slope
        b1 = l1.p1.y - m1 * l1.p1.x;                    // Calculate y-intercept
    }

    double m, b, x, y;
    int intersections = 0;

    for (const dline& l : lines) {
        bool l_vertical = (l.p1.x == l.p2.x);

        if (!l_vertical && !l1_vertical) {
            // Both lines are not vertical
            m = (l.p2.y - l.p1.y) / (l.p2.x - l.p1.x);
            b = l.p1.y - m * l.p1.x;

            // Check for parallel lines
            if (m1 == m) {
                // Parallel lines, they intersect if they have the same y-intercept
                if (b1 == b) {
                    intersections += (fmax(l1.p1.x, l1.p2.x) >= fmin(l.p1.x, l.p2.x) && fmin(l1.p1.x, l1.p2.x) <= fmax(l.p1.x, l.p2.x));
                }
                continue;
            }

            // Find intersection point
            x = (b - b1) / (m1 - m);
            y = m1 * x + b1;

            // Check if the intersection point lies within both line segments
            if ((x >= fmin(l.p1.x, l.p2.x) && x <= fmax(l.p1.x, l.p2.x)) &&
                (x >= fmin(l1.p1.x, l1.p2.x) && x <= fmax(l1.p1.x, l1.p2.x))) {
                intersections++;
            }
        } else if (!l_vertical) {
            // l1 is vertical, l is not
            x = l1.p1.x;
            y = m * x + b;

            // Check if the intersection lies within both line segments
            if ((y >= fmin(l1.p1.y, l1.p2.y) && y <= fmax(l1.p1.y, l1.p2.y)) &&
                (x >= fmin(l.p1.x, l.p2.x) && x <= fmax(l.p1.x, l.p2.x))) {
                intersections++;
            }
        } else if (!l1_vertical) {
            // l is vertical, l1 is not
            x = l.p1.x;
            y = m1 * x + b1;

            // Check if the intersection lies within both line segments
            if ((y >= fmin(l.p1.y, l.p2.y) && y <= fmax(l.p1.y, l.p2.y)) &&
                (x >= fmin(l1.p1.x, l1.p2.x) && x <= fmax(l1.p1.x, l1.p2.x))) {
                intersections++;
            }
        } else {
            // Both lines are vertical
            if (l1.p1.x == l.p1.x) {
                // Check if the vertical lines overlap in the y-range
                if (fmax(l1.p1.y, l1.p2.y) >= fmin(l.p1.y, l.p2.y) && fmin(l1.p1.y, l1.p2.y) <= fmax(l.p1.y, l.p2.y)) {
                    intersections++;
                }
            }
        }
    }

    return intersections;
}

//a quad, similar to a rect, except it has 4 points rather than x, y, w, h
//contains functions for transform, can be used as a regular rect in this way, however collision checking can be up to 10 times more costly on average
//although this is a difference of maybe 1 or 2 microseconds on a relatively fast CPU
struct quad {
    dvec2 v1;
    dvec2 v2;
    dvec2 v3;
    dvec2 v4;


    static quad rtoq(rect r) {
        return {
            ivec2{r.x, r.y}.convert_data<double>(),
            ivec2{r.x+r.w, r.y}.convert_data<double>(),
            ivec2{r.x+r.w, r.y+r.h}.convert_data<double>(),
            ivec2{r.x, r.y + r.h}.convert_data<double>()
        };
    }

    dvec2& operator[] (int index){
        if (index == 0) {
            return v1;
        } else if (index == 1) {
            return v2;
        } else if (index == 2) {
            return v3;
        } else if (index == 3) {
            return v4;
        }
        cerr << "Out of range 0-3";
        exit(-1);
    }

    dvec2 get_center() {
        return dvec2{
            (((v1.x + v2.x + v3.x) / 3) + ((v1.x + v3.x + v4.x) / 3)) / 2,
            (((v1.y + v2.y + v3.y) / 3) + ((v1.y + v3.y + v4.y) / 3)) / 2,
        };
    }

    
    bool is_in(dvec2 point) {
        dline lines[4];
        for (int i = 0; i < 4; i++) {
            lines[i] = {(*this)[i], (*this)[(i+1)%4]};
        }
        dline l = {point, dvec2{DBL_MAX, point.y}};
        return ray_cast(l, std::vector<dline>(lines, lines+4)) % 2 == 1;
    }



    void rotate(arcdegrees angle) {
        double rad = angle * (M_PI / 180);

        dvec2 center = {
            (((v1.x + v2.x + v3.x) / 3) + ((v1.x + v3.x + v4.x) / 3)) / 2,
            (((v1.y + v2.y + v3.y) / 3) + ((v1.y + v3.y + v4.y) / 3)) / 2,
        };

        double sa = sin(rad);
        double ca = cos(rad);

        for (int i = 0; i < 4; i++) {
            dvec2& p = (*this)[i];
            dvec2 np = {
                p.x - center.x,
                p.y - center.y
            };

            p.x = np.x * ca - np.y * sa + center.x;
            p.y = np.x * sa + np.y * ca + center.y;
        }
    }

    void move(dvec2 movement) {
        for (int i = 0; i < 4; i++) {
            (*this)[i] += movement;
        }
    }

    void scale(double scalar, std::optional<dvec2> Center = std::nullopt) {
        // Scale around the center of the quad
        dvec2 center;
        if (!Center.has_value()) {
            Center = dvec2{
                (((v1.x + v2.x + v3.x) / 3) + ((v1.x + v3.x + v4.x) / 3)) / 2,
                (((v1.y + v2.y + v3.y) / 3) + ((v1.y + v3.y + v4.y) / 3)) / 2,
            };
        }
        center = Center.value();

        
        
        for (int i = 0; i < 4; i++) {
            dvec2& p = (*this)[i];

            // Calculate the difference vector from the center to the vertex
            dvec2 diff = {
                p.x - center.x,
                p.y - center.y
            };

            // Scale the difference vector
            diff.x *= scalar;
            diff.y *= scalar;

            // Update the vertex position
            p.x = center.x + diff.x;
            p.y = center.y + diff.y;
        }
    }

    void scale(dvec2 scalar, std::optional<dvec2> Center = std::nullopt) {

        dvec2 center;
        // Scale around the center of the quad
        if (!Center.has_value()) {
            Center = {
                (((v1.x + v2.x + v3.x) / 3) + ((v1.x + v3.x + v4.x) / 3)) / 2,
                (((v1.y + v2.y + v3.y) / 3) + ((v1.y + v3.y + v4.y) / 3)) / 2,
            };
        }
        center = Center.value();
        
        for (int i = 0; i < 4; i++) {
            dvec2& p = (*this)[i];

            // Calculate the difference vector from the center to the vertex
            dvec2 diff = {
                p.x - center.x,
                p.y - center.y
            };

            // Scale the difference vector
            diff.x *= scalar.x;
            diff.y *= scalar.y;

            // Update the vertex position
            p.x = center.x + diff.x;
            p.y = center.y + diff.y;
        }
    }

};


inline int rand_int(int min, int max) {
    int ret = rand();
    srand(ret * 23);

    return min + (ret % (max-min));
}

inline float rand_percent(int precision = 100) {
    float ret = rand_int(0, precision);
    srand(rand_int(0, 9999999) << rand_int(3, 8));
    return ret / (double)precision;
}

inline double rand_double(double min, double max) {
    double ret = rand_int(-999999999, 999999999);
    srand(rand_int(0, 9999999) << rand_int(3, 8));
    ret = ret / 100000.0;
    return min + (std::fmod(ret, max-min));
}


/*
a structure for storing information about the kinematics of an object
*/
struct kinematics {
    dvec2 position;
    dvec2 velocity;
    dvec2 acceleration;
};

#endif