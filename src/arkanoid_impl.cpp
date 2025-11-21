#include "arkanoid_impl.h"

#include <GLFW/glfw3.h>
#include <iostream>

#ifdef USE_ARKANOID_IMPL
Arkanoid* create_arkanoid()
{
    return new ArkanoidImpl();
}
#endif

int* scoreptr; bool godemode;

void ArkanoidImpl::addBall() {
    
    Ball ball;
    ball.ball_position = carriage.pos_min;
    ball.ball_position.x = (carriage.pos_max.x - carriage.pos_min.x) / 2 + carriage.pos_min.x;
    ball.ball_position.y *= 0.90f;
    ball.ball_velocity = Vect(ball_initial_speed);
    ball.ball_velocity.x = 0;
    ball.ball_radius = balls[0].ball_radius;
    balls.push_back(ball);
    

    
}


bool ArkanoidImpl::ResolveCollision(ArkanoidDebugData& debug_data, Brick& brick, Ball &ball) {

    if (brick.destroyed) return false;
    //высчитываем ближайшие точки
    float closestX = std::clamp(ball.ball_position.x, brick.pos_min.x, brick.pos_max.x);
    float closestY = std::clamp(ball.ball_position.y, brick.pos_min.y, brick.pos_max.y);
    //дельты расто€ний 
    float deltaX = ball.ball_position.x - closestX;
    float deltaY = ball.ball_position.y - closestY;
    float distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
    float radiusSquared = ball.ball_radius * ball.ball_radius;
    //false значит коллизии нет
    if (distanceSquared > radiusSquared) {

        return false;
    }
    
    //расчитываем правильно коллизию
    float distance = std::sqrt(distanceSquared);
    float normalX = deltaX / (distance + 0.0000001f);
    float normalY = deltaY / (distance + 0.0000001f);
    float penetrationDepth = ball.ball_radius - distance;

    ball.ball_position.x += normalX * penetrationDepth;
    ball.ball_position.y += normalY * penetrationDepth;
    float dotProduct = (ball.ball_velocity.x * normalX) + (ball.ball_velocity.y * normalY);

    brick.destroyed = true;
    *scoreptr += 10;

    ball.ball_velocity.x = ball.ball_velocity.x - 2.0f * dotProduct * normalX;
    ball.ball_velocity.y = ball.ball_velocity.y - 2.0f * dotProduct * normalY;
    if (ball.ball_velocity.x > ball_initial_speed) ball.ball_velocity.x = ball_initial_speed;
    if (ball.ball_velocity.y > ball_initial_speed) ball.ball_velocity.y = ball_initial_speed;
    add_debug_hit(debug_data, Vect(ball.ball_position.x, ball.ball_position.y),
        Vect(ball.ball_velocity.x / (std::abs(ball.ball_velocity.y) + std::abs(ball.ball_velocity.x)), 
            ball.ball_velocity.y / (std::abs(ball.ball_velocity.y) + std::abs(ball.ball_velocity.x))));

    return true;

}


void ArkanoidImpl::CheckCollisionBallCarriage(ArkanoidDebugData& debug_data, Ball &ball) {


    //высчитываем ближайшие точки
    float closestX = std::clamp(ball.ball_position.x, carriage.pos_min.x, carriage.pos_max.x);
    float closestY = std::clamp(ball.ball_position.y, carriage.pos_min.y, carriage.pos_max.y);
    //дельты расто€ний 
    float deltaX = ball.ball_position.x - closestX;
    float deltaY = ball.ball_position.y - closestY;
    float distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
    float radiusSquared = ball.ball_radius * ball.ball_radius;
    if (distanceSquared > radiusSquared) {
        return;
    }


    float distance = std::sqrt(distanceSquared);
    float penetrationDepth = ball.ball_radius - distance;

    //  орректируем позицию шарика, чтобы он не проникал внутрь
    ball.ball_position.x += deltaX / (distance+ 0.0000001f)*penetrationDepth;
    ball.ball_position.y += deltaY / (distance+ 0.0000001f)*penetrationDepth;

    // 1. Ќаходим центр
    float carriageCenterX = (carriage.pos_min.x + carriage.pos_max.x) / 2.0f;
    float carriageCenterY = (carriage.pos_min.y + carriage.pos_max.y) / 2.0f;

    // 2. ¬ычисл€ем вектор от центра к центру шарика
    float directionX = ball.ball_position.x - carriageCenterX;
    float directionY = ball.ball_position.y - carriageCenterY;

    // 3. Ќормализуем этот вектор
    float directionLength = std::sqrt(directionX * directionX + directionY * directionY);
    if (directionLength > 0.0001f) {
        directionX /= directionLength;
        directionY /= directionLength;
    }
    else {
        directionX = 0.0f;
        directionY = -1.0f;
    }
    // ”станавливаем новую скорость шарика на основе этого направлени€ и исходной скорости
    ball.ball_velocity.x = directionX * ball_initial_speed * 0.4f;
    ball.ball_velocity.y = -std::abs(ball_initial_speed);


    add_debug_hit(debug_data, Vect(ball.ball_position.x, ball.ball_position.y),
        Vect(ball.ball_velocity.x / (std::abs(ball.ball_velocity.y) + std::abs(ball.ball_velocity.x)), 
            ball.ball_velocity.y / (std::abs(ball.ball_velocity.y) + std::abs(ball.ball_velocity.x))));




    return;



}


void ArkanoidImpl::reset(const ArkanoidSettings &settings)
{
    
    settings.score = 0;
    godemode = settings.godemode;
    scoreptr = &settings.score;
    world_size.x = settings.world_size[0];
    world_size.y = settings.world_size[1];

    carriage_width = settings.carriage_width;
    carriage.speed = settings.carriage_speed;

    carriage.pos_min.x = world_size.x * 0.4f;
    carriage.pos_min.y = world_size.y * 0.9f;

    carriage.pos_max.x = carriage.pos_min.x + carriage_width;
    carriage.pos_max.y = carriage.pos_min.y + 40;

    balls.clear();
    ball_initial_speed = settings.ball_speed;
    Ball ball;
    ball.ball_position = carriage.pos_min;
    ball.ball_position.x = (carriage.pos_max.x - carriage.pos_min.x) / 2 + carriage.pos_min.x;

    ball.ball_position.y *= 0.90f;
    ball.ball_velocity = Vect(ball_initial_speed);
    ball.ball_velocity.x = 0;
    ball.ball_radius = settings.ball_radius;
    balls.push_back(ball);
   
    
 
    
    
 
    brick_matrix.clear();
    float maxWallSizeY = world_size.y / 3;
    float bricks_rows_count = settings.bricks_rows_count;
    float rowSize = maxWallSizeY / bricks_rows_count;
    float bricks_collumn_count = settings.bricks_columns_count;
    float collumnWidth = world_size.x / bricks_collumn_count;
    float collumn_padding = settings.bricks_columns_padding;
    float rows_padding = settings.bricks_rows_padding;
    for (int i = 0; i < bricks_rows_count; i++) {
        std::vector<Brick> row;
        brick_matrix.push_back(row);
        for (int j = 0; j < bricks_collumn_count; j++) {
            Brick brick;
            brick_matrix[i].push_back(brick);
            brick_matrix[i][j].pos_min = Vect((collumnWidth)*j + collumn_padding / 2, rowSize * i +rows_padding/2);
            brick_matrix[i][j].pos_max = Vect((collumnWidth) * (j + 1) - collumn_padding / 2, rowSize * (i + 1) -rows_padding/2);
        }

    }
    

    

    
}

void ArkanoidImpl::update(ImGuiIO& io, ArkanoidDebugData& debug_data, float elapsed)
{
    // TODO:
    // Implement you Arkanoid user input handling
    // and game logic.
    // ...
    world_to_screen = Vect(io.DisplaySize.x / world_size.x, io.DisplaySize.y / world_size.y);
    
    
    // process user input
    if (io.KeysDown[GLFW_KEY_A]) {
        //ball_velocity.x -= 2.0f;
        if (!( carriage.pos_min.x < 0)) {
            carriage.pos_max.x -= carriage.speed * elapsed;
            carriage.pos_min.x -= carriage.speed * elapsed;
        }
    }

    if (io.KeysDown[GLFW_KEY_D]) {
        //ball_velocity.x += 2.0f;
        if (!(carriage.pos_max.x > world_size.x )) {
            carriage.pos_max.x += carriage.speed * elapsed;
            carriage.pos_min.x += carriage.speed * elapsed;
        }
    }
    if (balls.size()!=0 && *scoreptr/balls.size()>=50) {
        Ball ball;
        ball.ball_position = carriage.pos_min;
        ball.ball_position.x = (carriage.pos_max.x - carriage.pos_min.x) / 2 + carriage.pos_min.x;
        ball.ball_position.y *= 0.90f;
        ball.ball_velocity = Vect(ball_initial_speed);
        ball.ball_velocity.x = 0;
        ball.ball_radius = balls[0].ball_radius;
        balls.push_back(ball);
    }
    

    // update ball position according
    // its velocity and elapsed time
    for (auto &ball : balls) {
        if (ball.deleted && godemode == false) continue;
        ball.ball_position += ball.ball_velocity * elapsed;
        if (ball.ball_position.x < ball.ball_radius)
        {
            ball.ball_position.x += (ball.ball_radius - ball.ball_position.x) * 2.0f;
            ball.ball_velocity.x *= -1.0f;

            add_debug_hit(debug_data, Vect(0, ball.ball_position.y), Vect(1, 0));
        }

        else if (ball.ball_position.x > (world_size.x - ball.ball_radius))
        {
            ball.ball_position.x -= (ball.ball_position.x - (world_size.x - ball.ball_radius)) * 2.0f;
            ball.ball_velocity.x *= -1.0f;

            add_debug_hit(debug_data, Vect(world_size.x, ball.ball_position.y), Vect(-1, 0));
        }

        if (ball.ball_position.y < ball.ball_radius)
        {
            ball.ball_position.y += (ball.ball_radius - ball.ball_position.y) * 2.0f;
            ball.ball_velocity.y *= -1.0f;

            add_debug_hit(debug_data, Vect(ball.ball_position.x, 0), Vect(0, 1));
        }
        else if (ball.ball_position.y > (world_size.y - ball.ball_radius))
        {
            ball.ball_position.y -= (ball.ball_position.y - (world_size.y - ball.ball_radius)) * 2.0f;
            ball.ball_velocity.y *= -1.0f;
            ball.deleted = true;
            
            add_debug_hit(debug_data, Vect(ball.ball_position.x, world_size.y), Vect(0, -1));
        }

        CheckCollisionBallCarriage(debug_data, ball);
        for (int i = 0; i < brick_matrix.size(); i++) {

            for (int j = 0; j < brick_matrix[i].size(); j++) {
                if (ResolveCollision(debug_data, brick_matrix[i][j], ball));
            }
        }

    }
    



}

void ArkanoidImpl::draw(ImGuiIO& io, ImDrawList &draw_list)
{
    // TODO:
    // Implement you Arkanoid drawing
    // ...

    
    Carriage screen_carriage = carriage;
    

    screen_carriage.pos_min.x *= world_to_screen.x;
    screen_carriage.pos_min.y *= world_to_screen.y;

    screen_carriage.pos_max.x *= world_to_screen.x;
    screen_carriage.pos_max.y *= world_to_screen.y;

    draw_list.AddRectFilled(screen_carriage.pos_min, screen_carriage.pos_max, ImColor(255, 255, 255));

    for (auto& ball : balls) {
     
        if (ball.deleted && godemode==false) continue;
        Vect screen_pos = ball.ball_position * world_to_screen;
        float screen_radius = ball.ball_radius * world_to_screen.x;
        draw_list.AddCircleFilled(screen_pos, screen_radius, ImColor(255, 0, 0));

    }


    for (int i = 0; i < brick_matrix.size(); i++) {

        for (int j = 0; j < brick_matrix[i].size(); j++) {
            if (brick_matrix[i][j].destroyed) continue;
            Brick screen_brick;
            screen_brick.pos_min.x = brick_matrix[i][j].pos_min.x * world_to_screen.x;
            screen_brick.pos_min.y = brick_matrix[i][j].pos_min.y * world_to_screen.y;
            screen_brick.pos_max.x = brick_matrix[i][j].pos_max.x * world_to_screen.x;
            screen_brick.pos_max.y = brick_matrix[i][j].pos_max.y * world_to_screen.y;
            draw_list.AddRectFilled(screen_brick.pos_min, screen_brick.pos_max, ImColor(100, 255, 100));
        }

    }
}

void ArkanoidImpl::add_debug_hit(ArkanoidDebugData& debug_data, const Vect& world_pos, const Vect& normal)
{
    ArkanoidDebugData::Hit hit;
    hit.screen_pos = world_pos * world_to_screen;
    hit.normal = normal;
    debug_data.hits.push_back(std::move(hit));
}



