#ifndef GAME_H
#define GAME_H

#include <random>
#include <SDL2/SDL_ttf.h>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "GameState.h"
#include <string>
#include <future>
#include "food.h"

struct GameContext {
  Food &food;
  //std::vector<std::shared_ptr<Snake>> snakeList;
  Snake &playerSnake;
  Snake &aiSnake;
  GameContext(Food &food, Snake &playerSnake, Snake &aiSnake) : food(food), playerSnake(playerSnake), aiSnake(aiSnake) {}
};

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void RunGame(Controller const &controller, Renderer &renderer, TTF_Font* font,
    std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  std::string GetPlayerName() const;

 private:
  PlayerSnake playerSnake;
  AISnake aiSnake;
  Food food;
  GameContext gameContext;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  std::string playerName;
  GameState gameState;

  void Update();
  void EnterName(Renderer &renderer, TTF_Font* font);
  void HighScore();
  void Run(Controller const &controller, Renderer &renderer,
    std::size_t target_frame_duration);
};

#endif