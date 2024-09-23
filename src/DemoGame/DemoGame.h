#include "DemoGame/Tilemap.h"
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <SDL_events.h>
#include <SDL_timer.h>
#include <print>
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Backgrounds.h"
#include "Colliders.h"
#include "Player.h"

class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square = scene->createEntity("SQUARE", 10, 10); 
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("assets/Sprites/cat.png");
    square->addComponent<SpriteComponent>("assets/Sprites/cat.png", 8, 8, 10, 8, 1000);
    square->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 8 * 10, 8 * 10}, SDL_Color{255, 0, 0, 255});
  }
};

class CameraSetupSystem : public SetupSystem {
  void run() {
    int width = 1024;
    int height = 768;
    scene->mainCamera = scene->createEntity("CAMERA", 0, 0); 
    scene->mainCamera->addComponent<CameraComponent>(1, width, height, width * 10, height * 10);
  }
};

class CameraFollowUpdateSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, PlayerComponent, SpriteComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto cameraComponent = scene->mainCamera->get<CameraComponent>();

    for (auto e : view) {
      auto playerPosition = view.get<PositionComponent>(e);
      auto playerSprite = view.get<SpriteComponent>(e);

      int spriteWidth = playerSprite.width * playerSprite.scale * cameraComponent.zoom;
      int spriteHeight = playerSprite.height * playerSprite.scale * cameraComponent.zoom;

      int px = playerPosition.x - cameraComponent.vw / 2 + spriteWidth / 2;
      int py = playerPosition.y - cameraComponent.vh / 2 + spriteHeight / 2;

      if (px > 0 && px < cameraComponent.ww - cameraComponent.vw) {
        cameraPosition.x = px;
      }

      if (py > 0 && py < cameraComponent.wh - cameraComponent.vh) {
        cameraPosition.y = py;
      }
    }
  }
};

class MovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      pos.x += vel.x * dT;
      pos.y += vel.y * dT;
    }
  }
};

class MovementInputSystem : public EventSystem {
  void run(SDL_Event event) {
   auto view = scene->r.view<PlayerComponent, VelocityComponent>();
    for (auto e : view) {
      auto& vel = view.get<VelocityComponent>(e);
      
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
          vel.x = -vel.speed;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          vel.x = vel.speed;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          vel.y = -vel.speed;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          vel.y = vel.speed;
        }
      } else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_LEFT) {
          vel.x = 0;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          vel.x = 0;
        }
        if (event.key.keysym.sym == SDLK_UP) {
          vel.y = 0;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          vel.y = 0;
        }
      }
    }
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
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);
    addSetupSystem<CameraSetupSystem>(sampleScene);
    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<ProceduralTilemapSetupSystem>(sampleScene);
    addSetupSystem<AdvancedAutoTilingSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);
    addSetupSystem<TilemapColliderSetupSystem>(sampleScene);
    
    addEventSystem<MovementInputSystem>(sampleScene);
    addUpdateSystem<CameraFollowUpdateSystem>(sampleScene);
    addUpdateSystem<SpriteMovementSystem>(sampleScene);

    addUpdateSystem<PlayerWallCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<PlayerWallHitResponseSystem>(sampleScene);

    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<SpriteAnimationSystem>(sampleScene);
    addRenderSystem<TilemapRenderSystem>(sampleScene);
    addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<ColliderRenderSystem>(sampleScene);

    setScene(sampleScene);
  }
}; 
