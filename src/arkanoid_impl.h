#pragma once

#include "arkanoid.h"
#include <algorithm>
#include <cmath>
#include <vector>

#define USE_ARKANOID_IMPL

struct Carriage {
    Vect pos_min;
    Vect pos_max;
    float speed;
};

struct Brick {
    Vect pos_min;
    Vect pos_max;
    bool destroyed =false;
};
struct Ball {
    Vect ball_position = Vect(0.0f);
    Vect ball_velocity = Vect(0.0f);
    float ball_radius = 0.0f;
    bool deleted = false;
};





class ArkanoidImpl : public Arkanoid
{
public:
    void reset(const ArkanoidSettings& settings) override;
    void update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed) override;
    void draw(ImGuiIO& io, ImDrawList& draw_list) override;
    void addBall() override;
private:
    

    Vect world_size = Vect(0.0f);
    Vect world_to_screen = Vect(0.0f);

    float ball_initial_speed = 0.0f;
    float carriage_width = 0.0f;
    Carriage carriage;
    std::vector<Ball> balls;
    std::vector<std::vector<Brick>> brick_matrix;
    

    void CheckCollisionBallCarriage(ArkanoidDebugData& debug_data, Ball &ball);
    bool ResolveCollision(ArkanoidDebugData& debug_data, Brick& brick, Ball &ball);




    void add_debug_hit(ArkanoidDebugData& debug_data, const Vect& pos, const Vect& normal);
    Vect demo_world_size = Vect(0.0f);
    Vect demo_world_to_screen = Vect(0.0f);
    Vect demo_ball_position = Vect(0.0f);
    Vect demo_ball_velocity = Vect(0.0f);
    float demo_ball_radius = 0.0f;
    float demo_ball_initial_speed = 0.0f;
};
