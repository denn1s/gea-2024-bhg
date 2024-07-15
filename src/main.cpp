#include <string>
#include <print>
#include <SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 5;

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window* window = SDL_CreateWindow("", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  bool quit = false;
  SDL_Event e;

  int x = 0;
  int y = 0;

  Uint32 frameCount = 1;
  Uint32 frameStartTimestamp;
  Uint32 frameEndTimestamp;
  Uint32 lastUpdateTime = 0;
  float frameDuration = (1.0/MAX_FPS) * 1000.0;
  float actualFrameDuration;
  int FPS = MAX_FPS;
  
  while (!quit) {
    frameStartTimestamp = SDL_GetTicks();

    // poll events
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    // update
    if (x < SCREEN_WIDTH) {
      x++;
    }
    if (y < SCREEN_HEIGHT) {
      y++;
    }

    // render
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_Rect fillRect = {x, y, 100, 100};
    SDL_RenderFillRect(renderer, &fillRect);

    SDL_RenderPresent(renderer);


    frameEndTimestamp = SDL_GetTicks();
    actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

    if (actualFrameDuration < frameDuration) {
      SDL_Delay(frameDuration - actualFrameDuration);
    }

    // fps calculation
    frameCount++;
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - lastUpdateTime;
    if (elapsedTime > 1000) {
      FPS = (float)frameCount / (elapsedTime / 1000.0);
      lastUpdateTime = currentTime;
      frameCount = 0;
    }
    SDL_SetWindowTitle(window, ("FPS: " + std::to_string(FPS)).c_str());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
