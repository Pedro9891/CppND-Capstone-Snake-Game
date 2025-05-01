#include "food.h"
#include "game.h"

Food::Food(std::size_t grid_width, std::size_t grid_height) : engine(dev()),
    random_w(0, static_cast<int>(grid_width - 1)),
    random_h(0, static_cast<int>(grid_height - 1)),
    position(SDL_Point{random_w(engine), random_h(engine)}) {}

void Food::PlaceFood(const GameContext &gameContext) {
    int temp_x, temp_y;
    while (true) {
        temp_x = random_w(engine);
        temp_y = random_h(engine);
        // Check that the location is not occupied by a snake cell before placing
        // food.
        if (!gameContext.playerSnake.SnakeCell(temp_x, temp_y) && 
            !gameContext.aiSnake.SnakeCell(temp_x, temp_y)) {
            position.x = temp_x;
            position.y = temp_y;
            return;
        }
    }
}

bool Food::TryConsume(const GameContext &gameContext, const SDL_Point &head) {
    std::lock_guard lock(m);
    if (head == gameContext.food.position) {
        PlaceFood(gameContext);
        return true;
    }
    return false;
}