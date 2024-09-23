#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>


class PixelsGame : public Game {
public:
  Scene* gameplayScene;
  entt::registry r;

public:
  PixelsGame()
  : Game("SAMPLE", 1000, 1000)
  { }

  void setup() {
    gameplayScene = new Scene("GAMEPLAY SCENE", r, renderer);
    
    setScene(gameplayScene);
  }
}; 
