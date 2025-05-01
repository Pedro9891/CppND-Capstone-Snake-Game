#include "game.h"
#include <iostream>
#include <fstream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height) :
      playerSnake(grid_width, grid_height),
      aiSnake(grid_width, grid_height),
      food(grid_width, grid_height),
      gameContext(food, playerSnake, aiSnake) {}

void Game::RunGame(Controller const &controller, Renderer &renderer, TTF_Font* font,
  std::size_t target_frame_duration) {
    gameState = GameState::STARTMENU;
    while (gameState != GameState::EXIT) {
      switch (gameState)
      {
      case GameState::STARTMENU:
        EnterName(renderer, font);
        break;
      case GameState::RUNNING:
        Run(controller, renderer, target_frame_duration);
        break;
      case GameState::GAMEOVER:
        HighScore();
        break;
      case GameState::EXIT:
        break;
      }
    }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;

  while (gameState == GameState::RUNNING) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(gameState, gameContext.playerSnake);
    Update();
    renderer.RenderGame(gameContext);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
  
}

void Game::Update() {
  if (!gameContext.playerSnake.alive || !gameContext.aiSnake.alive) {
    gameState = GameState::GAMEOVER;
    return;
  }
  std::future<int> f1 = std::async(std::launch::async, [&]() {
    return gameContext.aiSnake.Update(gameContext);
  });
  std::future<int> f2 = std::async(std::launch::async, [&]() {
    return gameContext.playerSnake.Update(gameContext);
  });
  f1.get();
  score += f2.get();

}

void Game::EnterName(Renderer &renderer, TTF_Font* font) {

  SDL_Event event;
  SDL_StartTextInput();
  while (gameState == GameState::STARTMENU) {
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
          gameState = GameState::EXIT;
        if (event.type == SDL_TEXTINPUT && playerName.size() < 15)
          playerName += event.text.text;  // Add character to playerName
        else if (event.type == SDL_KEYDOWN)
        {
          if (event.key.keysym.sym == SDLK_BACKSPACE && !playerName.empty())
            playerName.pop_back();
          if (event.key.keysym.sym == SDLK_RETURN)
            gameState = GameState::RUNNING;
        }
      }
    renderer.RenderStartMenu(playerName, font);
  }
  SDL_StopTextInput();

}

void Game::HighScore() {
  while (gameState == GameState::GAMEOVER) {
    std::string name;
    int currentHighscore;

    std::ifstream in("highscore.txt");
    while (in >> name >> currentHighscore)
    {
      if (currentHighscore < score) {
        std::cout << "New highscore: " << playerName << ", " << score << std::endl;
      } else {
        std::cout << "Highscore: " << name << ", " << score << std::endl;
        gameState = GameState::EXIT;
        return;
      }
    }
    
    // Overwrite new highscore
    std::ofstream out("highscore.txt", std::ios::trunc);
    out << playerName << " " << score;  
    gameState = GameState::EXIT;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return gameContext.playerSnake.size; }
std::string Game::GetPlayerName() const {return playerName;}