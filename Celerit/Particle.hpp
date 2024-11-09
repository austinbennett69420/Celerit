#ifndef PARTICLE
#define PARTICLE

#include "util.hpp"
#include "renderer.hpp"


namespace Particle {
    enum emission_BEHAVIOR {
        /*
        Linear behavior:

        •-> ------

        Spread behavior:

          \ |  /
           \|/
        --- •-> ---
           /|\
          / | \
        

        alternating behavior:
        <--- •-> --->
        (swaps directions)
        */



        LINEAR,
        SPREAD,
        ALTERNATING 
    } ;


    struct Instance {
        //stores info about the instance of a particle

        public:
        dvec2 position;
        dvec2 velocity;
        dvec2 acceleration;
        seconds_t remaining_life_span;

        bool rotate_with_velocity = false;
        seconds_t original_life_span;

        seconds_t last_update_time;
        
        Instance(dvec2 pos, dvec2 vel, dvec2 acc, seconds_t life_span, bool rotate_to_velocity = false) {
            position = pos;
            velocity = vel;
            acceleration = acc;
            remaining_life_span = life_span;
            rotate_with_velocity = rotate_to_velocity;
            original_life_span = life_span;
            last_update_time = getUTCTime();
        }

        bool isAlive() {
            return remaining_life_span > 0;
        }

        bool update() {
            //updates position and returns whether the particle is still alive or not
            if (!isAlive()) return false;

            position += velocity;
            velocity += acceleration;

            remaining_life_span -= (getUTCTime() - last_update_time);
            if (remaining_life_span <= 0) return false;
            last_update_time = getUTCTime();
            return true;
        }

    };
}


using Particle::emission_BEHAVIOR, Particle::Instance;

class ParticleEmitter {
    public:




    protected:

    texture image;
    Instance* instances;
    int MAX_PARTICLES;
    int w;
    int h;
    renderer* rend;
    dvec2 emission_vector;
    dvec2 position;

    emission_BEHAVIOR behavior;
    arcdegrees spread_angle_current = 0;
    bool alternating_dir = true;//true for forward, false for backwards
    dvec2* scroll = nullptr;
    bool rotate_with_velocity = false;

    public:
    ParticleEmitter(renderer& r, dvec2 position, int max_particles, int instance_width, int instance_height, emission_BEHAVIOR behavior = Particle::LINEAR) : 
    image(r, instance_width, instance_height) {
        MAX_PARTICLES = max_particles;
        instances = (Instance*)malloc(sizeof(Instance) * max_particles);
        rend = &r;
        w = instance_width;
        h = instance_height;
        rend->set_render_target(image);
        drawPoint();
        rend->reset_target();
        this->behavior = behavior;

        for (int i = 0; i < MAX_PARTICLES; i++) {
            instances[i] = Instance({0, 0}, {0, 0}, {0, 0}, 0.0);
        }

        this->position = position;
    }

    void use_scroll_behavior(dvec2& vec) {
        scroll = &vec;
    }

    void set_rotate_with_velocity(bool val) {
        rotate_with_velocity = val;
    }

    void set_emission_angle(arcdegrees ang) {
        //angles the emmiter to spit particles out at the desired angle
        //uses degrees
        emission_vector.x = std::cos(ang * DEGREE_CONVERSION);
        emission_vector.y = std::sin(ang * DEGREE_CONVERSION);
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void set_emission_vector(v2<T> vec) {
        //similar to ParticleEmitter::set_emission_angle, except it uses a vector to point towards
        //the direction at which particles will be emmited
        vec = vec.normalize();
        emission_vector = vec.template convert_data<double>();
    }


    void draw() {
        dvec2 pos_offset = {0, 0};
        if (scroll != nullptr) {
            pos_offset = *scroll;
        }
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (instances[i].isAlive()) {
                if (instances[i].rotate_with_velocity) {
                    rend->blit_texture(image, instances[i].position-pos_offset, instances[i].velocity.get_horizantal_angle(),
                                        dvec2{static_cast<double>((double)w/2.0), static_cast<double>((double)h/2.0)});
                } else {
                    rend->blit_texture(image, instances[i].position-pos_offset);
                }
            }
        }
    }

    void update() {
        //updates all particles
        for (int i = 0; i < MAX_PARTICLES; i++) {
            instances[i].update();
        }
    }

    int get_alive_particles() {
        int c = 0;
        for (int i = 0; i < MAX_PARTICLES; i++) {
            c += instances[i].isAlive();
        }

        return c;
    }



    inline virtual kinematics get_initial_kinematics() const {
        /*
        a function that returns the inital displaced kinematics of a particle
        the way this works is simply put, when the emission angle is 0 and the emission behavior is linear,
        this function will describe the intial state of that particle

        these transforms are applied to the vectors in the kinematics state:

        position: the displacement from the origin of the emmiter rotated by the emission angle around said origin
        velocity: the velocity of the particle rotated by the emission angle around the origin of the emmiter
        acceleration: the acceleration of the the particle, rotated by the emission angle around the origin of the emmiter
        */


        return {
            {1, 1},
            {1, 1},
            {0, 0}
        };
    }


    void spawn_particles(int amount) {
        //spawns **amount** particles so long as the number of alive particles plus amount is less than max particles
        int spawn_count = 0;
        kinematics init_kin = get_initial_kinematics();
        arcdegrees emission_angle = emission_vector.get_horizantal_angle() + spread_angle_current;

        init_kin.position = init_kin.position.get_rotated(emission_angle, position);
        init_kin.velocity = init_kin.velocity.get_rotated(emission_angle, position);
        init_kin.acceleration = init_kin.acceleration.get_rotated(emission_angle, position);

        if (!alternating_dir) init_kin.position = init_kin.position.get_opposite();
        if (!alternating_dir) init_kin.velocity = init_kin.velocity.get_opposite();
        if (!alternating_dir) init_kin.acceleration = init_kin.acceleration.get_opposite();

        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (!instances[i].isAlive()) {
                instances[i] = Instance(init_kin.position, init_kin.velocity, init_kin.acceleration, 
                10, rotate_with_velocity);

                spawn_count++;
                if (behavior == Particle::ALTERNATING) alternating_dir = !alternating_dir;
                if (behavior == Particle::SPREAD) spread_angle_current = rotation_clamp(spread_angle_current+20, 0.0, 360.0);

                init_kin = get_initial_kinematics();

                emission_angle = emission_vector.get_horizantal_angle() + spread_angle_current;

                init_kin.position = init_kin.position.get_rotated(emission_angle, position);
                init_kin.velocity = init_kin.velocity.get_rotated(emission_angle, position);
                init_kin.acceleration = init_kin.acceleration.get_rotated(emission_angle, position);

                if (!alternating_dir) init_kin.position = init_kin.position.get_opposite();
                if (!alternating_dir) init_kin.velocity = init_kin.velocity.get_opposite();
                if (!alternating_dir) init_kin.acceleration = init_kin.acceleration.get_opposite();
            }
            
            if (spawn_count == amount) break;
        }
    }

    ~ParticleEmitter() {
        free(instances);
    }

    protected:

    virtual void drawPoint() {
        rend->draw_line(0, h/2, w, h/2, BLUE, 1);
    }
};


class AnimatedParticleEmitter {
    //unlike a regular particle emmiter, this emmiter draws particles using functional procedural animation
    public:


    


    

    


    protected:

    Instance* instances;
    int MAX_PARTICLES;
    renderer* rend;
    dvec2 emission_vector;
    dvec2 position;

    emission_BEHAVIOR behavior;
    arcdegrees spread_angle_current = 0;
    bool alternating_dir = true;//true for forward, false for backwards
    dvec2* scroll = nullptr;

    public:
    AnimatedParticleEmitter(renderer& r, dvec2 position, int max_particles, emission_BEHAVIOR behavior = Particle::LINEAR) {
        MAX_PARTICLES = max_particles;
        instances = (Instance*)malloc(sizeof(Instance) * max_particles);
        rend = &r;
        this->behavior = behavior;

        for (int i = 0; i < MAX_PARTICLES; i++) {
            instances[i] = Instance({0, 0}, {0, 0}, {0, 0}, 0.0);
        }

        this->position = position;
    }

    void use_scroll_behavior(dvec2& vec) {
        scroll = &vec;
    }


    void set_emission_angle(arcdegrees ang) {
        //angles the emmiter to spit particles out at the desired angle
        //uses degrees
        emission_vector.x = std::cos(ang * DEGREE_CONVERSION);
        emission_vector.y = std::sin(ang * DEGREE_CONVERSION);
    }

    template<typename T = int, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void set_emission_vector(v2<T> vec) {
        //similar to ParticleEmitter::set_emission_angle, except it uses a vector to point towards
        //the direction at which particles will be emmited
        vec = vec.normalize();
        emission_vector = vec.template convert_data<double>();
    }


    void draw() {
        dvec2 pos_offset = {0, 0};
        if (scroll != nullptr) {
            pos_offset = *scroll;
        }
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (instances[i].isAlive()) {
                drawPoint(instances[i], instances[i].position, pos_offset);
            }
        }
    }

    void update() {
        //updates all particles
        for (int i = 0; i < MAX_PARTICLES; i++) {
            instances[i].update();
        }
    }

    int get_alive_particles() {
        int c = 0;
        for (int i = 0; i < MAX_PARTICLES; i++) {
            c += instances[i].isAlive();
        }

        return c;
    }



    inline virtual kinematics get_initial_kinematics() const {
        /*
        a function that returns the inital displaced kinematics of a particle
        the way this works is simply put, when the emission angle is 0 and the emission behavior is linear,
        this function will describe the intial state of that particle

        these transforms are applied to the vectors in the kinematics state:

        position: the displacement from the origin of the emmiter rotated by the emission angle around said origin
        velocity: the velocity of the particle rotated by the emission angle around the origin of the emmiter
        acceleration: the acceleration of the the particle, rotated by the emission angle around the origin of the emmiter
        */


        return {
            {1, 1},
            {1, 1},
            {0, 0}
        };
    }


    void spawn_particles(int amount) {
        //spawns **amount** particles so long as the number of alive particles plus amount is less than max particles
        int spawn_count = 0;
        kinematics init_kin = get_initial_kinematics();
        arcdegrees emission_angle = emission_vector.get_horizantal_angle() + spread_angle_current;

        init_kin.position = init_kin.position.get_rotated(emission_angle, position);
        init_kin.velocity = init_kin.velocity.get_rotated(emission_angle, position);
        init_kin.acceleration = init_kin.acceleration.get_rotated(emission_angle, position);

        if (!alternating_dir) init_kin.position = init_kin.position.get_opposite();
        if (!alternating_dir) init_kin.velocity = init_kin.velocity.get_opposite();
        if (!alternating_dir) init_kin.acceleration = init_kin.acceleration.get_opposite();

        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (!instances[i].isAlive()) {
                instances[i] = Instance(init_kin.position, init_kin.velocity, init_kin.acceleration, 
                10, false);

                spawn_count++;
                if (behavior == Particle::ALTERNATING) alternating_dir = !alternating_dir;
                if (behavior == Particle::SPREAD) spread_angle_current = rotation_clamp(spread_angle_current+20, 0.0, 360.0);

                init_kin = get_initial_kinematics();

                emission_angle = emission_vector.get_horizantal_angle() + spread_angle_current;

                init_kin.position = init_kin.position.get_rotated(emission_angle, position);
                init_kin.velocity = init_kin.velocity.get_rotated(emission_angle, position);
                init_kin.acceleration = init_kin.acceleration.get_rotated(emission_angle, position);

                if (!alternating_dir) init_kin.position = init_kin.position.get_opposite();
                if (!alternating_dir) init_kin.velocity = init_kin.velocity.get_opposite();
                if (!alternating_dir) init_kin.acceleration = init_kin.acceleration.get_opposite();
            }
            
            if (spawn_count == amount) break;
        }
    }

    ~AnimatedParticleEmitter() {
        free(instances);
    }

    protected:

    virtual void drawPoint(Instance i, dvec2 pos, dvec2 scroll) {
        pos -= scroll;
        rend->draw_line(pos.x, pos.y, pos.x+i.velocity.x*5, pos.y+i.velocity.y*5, {
        rotation_clamp(static_cast<uint8_t>(i.position.x), (uint8_t)0, (uint8_t)255), 
        rotation_clamp(static_cast<uint8_t>(i.position.y), (uint8_t)0, (uint8_t)255), 
        rotation_clamp(static_cast<uint8_t>(i.velocity.get_distance()), (uint8_t)0, (uint8_t)255)}
        , 1);
    }
};



#endif