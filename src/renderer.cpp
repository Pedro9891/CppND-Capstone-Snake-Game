#include "renderer.h"
#include <iostream>
#include <string>
#include "game.h"

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::RenderGame(const GameContext &gameContext) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = gameContext.food.position.x * block.w;
  block.y = gameContext.food.position.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render AI's body
  auto& AIsnake = gameContext.aiSnake;
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : AIsnake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render AI's head
  block.x = static_cast<int>(AIsnake.head_x) * block.w;
  block.y = static_cast<int>(AIsnake.head_y) * block.h;
  if (AIsnake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  auto& snake = gameContext.playerSnake;
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::RenderStartMenu(std::string &playerName, TTF_Font* font) {
  if (!font) {
    SDL_Log("RenderMenu: font is NULL!");
    return;
  }

  // Clear background
  SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255); // Black
  SDL_RenderClear(sdl_renderer);

  SDL_Color white = {255, 255, 255, 255}; // White color for text

  // Render "Enter Name" label
  SDL_Surface* labelSurface = TTF_RenderText_Solid(font, "Enter Name ", white);
  if (!labelSurface) {
    SDL_Log("RenderMenu: Failed to create label surface: %s", TTF_GetError());
    return;
  }
  SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(sdl_renderer, labelSurface);
  SDL_Rect labelDst = {120, 100, labelSurface->w, labelSurface->h};
  SDL_FreeSurface(labelSurface);
  SDL_RenderCopy(sdl_renderer, labelTexture, NULL, &labelDst);
  SDL_DestroyTexture(labelTexture);

  // Draw input box 
  SDL_Rect inputBox = {270, 90, 220, 50}; // Input area
  SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255); // White box
  SDL_RenderDrawRect(sdl_renderer, &inputBox);

  // Render player name if not empty
  if (!playerName.empty()) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, playerName.c_str(), white);
    if (!surface) {
      SDL_Log("RenderMenu: Failed to create playerName surface: %s", TTF_GetError());
      return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
    SDL_Rect dst = {275, 100, surface->w, surface->h};
    SDL_FreeSurface(surface);

    if (texture) {
      SDL_RenderCopy(sdl_renderer, texture, NULL, &dst);
      SDL_DestroyTexture(texture);
    }
  }

  // Present everything to the screen
  SDL_RenderPresent(sdl_renderer);
  SDL_Delay(16);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
