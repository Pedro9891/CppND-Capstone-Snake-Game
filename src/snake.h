#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "SDL.h"
#include <memory>

class GameContext;

struct Node {
  SDL_Point position;
  int g;  //cost
  int h;  //heuristic
  std::shared_ptr<Node> parent = nullptr;

  Node() = default;
  Node(SDL_Point pos, int gCost, int hCost, std::shared_ptr<Node> par = nullptr) :
    position(pos), g(gCost), h(hCost), parent(par) {}
};


class Snake {
public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : head_x(grid_width / 2),
        head_y(grid_height / 2),
        grid_width(grid_width),
        grid_height(grid_height) {}

  int Update(const GameContext &gameContext);

  void GrowBody();
  bool SnakeCell(int x, int y);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

 protected:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell, const GameContext &gameContext);
  virtual void MoveHead(const GameContext &gameContext) = 0;

  bool growing{false};
  int grid_width;
  int grid_height;
};

class PlayerSnake : public Snake {
public:
  PlayerSnake(int grid_width, int grid_height) 
    : Snake(grid_width, grid_height) {}
private:
  void MoveHead(const GameContext &gameContext) override;
};

class AISnake : public Snake {
public:
  AISnake(int grid_width, int grid_height) 
    : Snake(grid_width, grid_height) {}
private:
  void MoveHead(const GameContext &gameContext) override;
  void CellSort(std::vector<Node> &openList);
  int Heuristic(const SDL_Point &a, const SDL_Point &b);
  bool GoalFound(Node &currentNode, const GameContext &gameContext);
  void AddNeighbor(const Node &currentNode, const GameContext &gameContext, std::vector<Node> &closedList, std::vector<Node> &openList);
};



#endif