#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include "GameState.h"

class Controller {
 public:
  void HandleInput(GameState &gameState, Snake &snake) const;

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif