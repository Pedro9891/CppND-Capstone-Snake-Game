#include "snake.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "utils.h"
#include "game.h"


// Heuristic function, manhattan distance
int AISnake::Heuristic(const SDL_Point &a, const SDL_Point &b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
}

void AISnake::MoveHead(const GameContext &gameContext) {

  // A-star algorithm

  std::vector<Node> openList;
  std::vector<Node> closedList;
  Node currentNode;
  SDL_Point endPoint = gameContext.food.position;

  // If goal has already been reached, keep same direction.
  SDL_Point startinPoint = {static_cast<int>(head_x), static_cast<int>(head_y)};
  if (startinPoint.x == endPoint.x && startinPoint.y == endPoint.y){
    UpdateHead();
    return;
  }
  
  // Insert snake head as starting node
  openList.emplace_back(startinPoint, 0, Heuristic(startinPoint, endPoint));

  // Loop through all possible neighbors.
  while (!openList.empty())
  {
    CellSort(openList); // sort openList

    currentNode = openList.back();
    closedList.push_back(openList.back());
    openList.pop_back();

    if (GoalFound(currentNode, gameContext))
      return;
    // Expand neighbors.
    AddNeighbor(currentNode, gameContext, closedList, openList);
  }
  std::cout << "NO PATH FOUND" << std::endl;
}

bool AISnake::GoalFound(Node &currentNode, const GameContext &gameContext) {
  SDL_Point endPoint = gameContext.food.position;

  if (currentNode.position.x == endPoint.x && 
    currentNode.position.y == endPoint.y) {
    while (currentNode.parent->parent != nullptr)
    {
      currentNode = *currentNode.parent;
    }

    // Update direction
    float delta_x = currentNode.position.x - static_cast<int>(head_x);
    float delta_y = currentNode.position.y - static_cast<int>(head_y);

    if (std::abs(delta_x) > std::abs(delta_y)) {
      direction = delta_x > 0 ? Direction::kRight : Direction::kLeft;
      UpdateHead();
    } else {
      direction = delta_y > 0 ? Direction::kDown : Direction::kUp;
      UpdateHead();
    }
    return true;  // Goal found and direction updated, return true.
  }
  return false;
}

void AISnake::CellSort(std::vector<Node> &openList) {
  std::sort(openList.begin(), openList.end(), [](const Node &a, const Node &b) {
    int a_F = a.h + a.g;
    int b_F = b.h + b.g;
    if (a_F == b_F)
    {
      return a.h > b.h;
    }
    
    return a_F > b_F;
  });
}

void AISnake::AddNeighbor(const Node &currentNode, const GameContext &gameContext, std::vector<Node> &closedList, std::vector<Node> &openList) {
  SDL_Point endPoint = gameContext.food.position;
  
  // Generate children/neighbors
  std::vector<SDL_Point> neighborList;
  neighborList.emplace_back(SDL_Point{currentNode.position.x + 1, currentNode.position.y + 0});
  neighborList.emplace_back(SDL_Point{currentNode.position.x + 0, currentNode.position.y + 1});
  neighborList.emplace_back(SDL_Point{currentNode.position.x + 0, currentNode.position.y - 1});
  neighborList.emplace_back(SDL_Point{currentNode.position.x - 1, currentNode.position.y + 0});

  for (SDL_Point childPoint : neighborList)
  {

    // Avoid collision with itself and player snake
    if (CheckCollision(body, childPoint) || CheckCollision(gameContext.playerSnake.body, childPoint))
      continue;

    // Search for child in closedlist. If it exist, continue onto the next child.
    auto it = std::find_if(closedList.begin(), closedList.end(), [&](const Node &other) {
      return childPoint == other.position;
    });
    if (it != closedList.end())
      continue;
    
    // Create childNode and update openList
    int g = currentNode.g + 1;
    int h = Heuristic(childPoint, endPoint);

    it = std::find_if(openList.begin(), openList.end(), [&](const Node &other) {
      return childPoint == other.position;
    });

    if (it != openList.end()) {
      if (it->g > g) {
        it->g = g;
        it->h = h;
      } else {
        continue;
      }
    }
    Node node(childPoint, g, h, std::make_shared<Node>(currentNode));
    openList.push_back(node);
  }
}

int Snake::Update(const GameContext &gameContext) {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  MoveHead(gameContext);
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  int score = 0;
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    if (gameContext.food.TryConsume(gameContext, current_cell)) {
      score++;
      this->GrowBody();
      if (this->speed < 0.2) {
        gameContext.aiSnake.speed += 0.02;
        gameContext.playerSnake.speed += 0.02;
      }
    }
    UpdateBody(current_cell, prev_cell, gameContext);
  }
  return score;
}

void PlayerSnake::MoveHead(const GameContext &gameContext) {
  UpdateHead();
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell, const GameContext &gameContext) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }

  // Check if the snake has died.
  if (CheckCollision(gameContext.playerSnake.body, current_head_cell) || CheckCollision(gameContext.aiSnake.body, current_head_cell))
    alive = false;
}

void Snake::GrowBody() { growing = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}