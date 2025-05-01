
## Install & Build instructions:
- run sudo apt-get install libsdl2-dev libsdl2-ttf-dev
- Make a build directory in the top level directory: `mkdir build && cd build`
- Compile: `cmake .. && make`
- Run it: `./SnakeGame`.

## New features
- Enter name
- Save/Load highscore in highscore.txt. Score updated in console at the end of game.
- AI snake controlled using A-star in AISnake clas.
- async threads for player and AI snake

## Loop, Functions I/O.
- Use of switch cases for enum game state, Running, StatMenu, Exit.. See Game::RunGame method.
- Game reads and writes to highscore.txt to keep track of highscore which is printed out in console at the end. See Game::HighScore method.
- Added new inputs to enter name, also ESC to exit game. See Game::EnterName method.

## Object Oriented Programming
- Added Food class with TryConsume and PlaceFood methods. Snakes call TryConsume in their respesctive thread.
- Food members are initialized through initizliation list such as Position.
- Struct GameContext(see game.h) is created with constructor which uses initializer list to construct the struct which contains Snakes and food.
- Snake::MoveHead is pure virtual. AISnake and PlayerSnake have their own implementation, AiSnake using A-Star and PlayerSnake using direction from keyboard input.

## Memory Management
- Use of references in function declarations are being used, specially GameContext object which is passed around to renderer, snake.. etc.
- Project uses shared ptr in for parent member in Node struct(see snake.h) which is used in A-Star algorithm, since multiple neighbor Nodes can share the same parent and to avoid copying it around.
- RAII use example in lock_guard used in Food::TryConsume to prevent multiple snakes eating, lockguard is destroyed on scope exit also unlocking the mutex.

## Concurrency
- Project uses async multithreading in the Game::Update method. Async thread is started for AISnake and PlayerSnake using their respective Snake::Update method. A future of type int is expected which describes the score(1 or 0) respective snake has got.
- Food class has been added which has TryConsume method that each snake can call once they have reached food cell. TryConsume is lockguarded with mutex, meaning only one of the snakes can consume the food before its instantly respawned.    
