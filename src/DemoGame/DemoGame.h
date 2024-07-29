#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Systems.h"
#include <print>
#include <entt/entt.hpp>

class SquareRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_Rect r = { 10, 10, 100, 100 };
    SDL_RenderFillRect(renderer, &r);
  }
}; 


class DemoGame : public Game {
  public:
    Scene* sampleScene;
    entt::registry r;

  public:
    DemoGame()
      : Game("SAMPLE", 1024, 768)
    { }

    void setup() {
      std::print("HELLO WORLD\n");  
      sampleScene = new Scene("SAMPLE SCENE", r);
      addRenderSystem<SquareRenderSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 
