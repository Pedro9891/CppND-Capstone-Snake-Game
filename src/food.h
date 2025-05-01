#ifndef FOOD_H
#define FOOD_H

#include <random>
#include <mutex>
#include "snake.h"
#include "utils.h"

class Food {
private:
    std::random_device dev;
    std::mt19937 engine;
    std::uniform_int_distribution<int> random_w;
    std::uniform_int_distribution<int> random_h;
    std::mutex m;
public:
    Food(std::size_t grid_width, std::size_t grid_height);
    bool TryConsume(const GameContext &gameContext, const SDL_Point &head);
    void PlaceFood(const GameContext &gameContext);
    SDL_Point position;
};

#endif