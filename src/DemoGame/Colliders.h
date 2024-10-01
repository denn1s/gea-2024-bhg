#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "DemoGame/Tilemap.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
/* #include "Tilemap.h" */
#include "Player.h"

enum class CollisionType {
  NONE,
  WALL,
  TRIGGER
};


struct BoxColliderComponent {
  SDL_Rect rect;
  SDL_Color color;
  CollisionType collisionType = CollisionType::NONE;
  bool isTriggered = false;
};


class ColliderResetSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto view = scene->r.view<BoxColliderComponent>();

    for (auto entity : view) {
      auto &collider = view.get<BoxColliderComponent>(entity);

      collider.collisionType = CollisionType::NONE;
    }
  }
};

class ColliderRenderSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) override {
    auto view = scene->r.view<PositionComponent, BoxColliderComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto& cameraComponent = scene->mainCamera->get<CameraComponent>();

    for (auto entity : view) {
      auto [position, collider] = view.get<PositionComponent, BoxColliderComponent>(entity);

      SDL_Rect renderRect = {
        position.x + collider.rect.x - cameraPosition.x,
        position.y + collider.rect.y - cameraPosition.y,
        collider.rect.w * cameraComponent.zoom,
        collider.rect.h * cameraComponent.zoom,
      };

      SDL_SetRenderDrawColor(renderer, collider.color.r, collider.color.g, collider.color.b, collider.color.a);
      SDL_RenderDrawRect(renderer, &renderRect);
    }

  }
};

class PlayerPowerUpCollisionDetectionSystem : public UpdateSystem {
public: 
  void run (float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();
    auto powerUpView = scene->r.view<PowerUpComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto position = playerView.get<PositionComponent>(player);
      auto& collider = playerView.get<BoxColliderComponent>(player);

      SDL_Rect playerRect = {
        position.x + collider.rect.x,
        position.y + collider.rect.y,
        collider.rect.w,
        collider.rect.h,
      };

      for (auto powerUp: powerUpView) {
        auto [pposition, pcollider] = powerUpView.get<PositionComponent, BoxColliderComponent>(powerUp);

        SDL_Rect powerUpRect = {
          pposition.x + pcollider.rect.x,
          pposition.y + pcollider.rect.y,
          pcollider.rect.w,
          pcollider.rect.h,
        };

        if (SDL_HasIntersection(&playerRect, &powerUpRect)) {
          collider.collisionType = CollisionType::TRIGGER; 
        }
      }
    }
  }
};


class PlayerPowerUpCollisionSystem : public UpdateSystem {
public: 
  void run (float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent>();

    for (auto player : playerView) {
      auto& collider = playerView.get<BoxColliderComponent>(player);

      if (collider.collisionType == CollisionType::TRIGGER && !collider.isTriggered) {
        std::cout << "player collider with power up" << std::endl;
        collider.isTriggered = true;
      }
    }
  }
};


class PlayerTileCollisionDetectionSystem : public UpdateSystem {
public: 
  void run (float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, PositionComponent, VelocityComponent>();
    auto tileView = scene->r.view<TileComponent, BoxColliderComponent, PositionComponent>(); 

    for (auto player : playerView) {
      auto position = playerView.get<PositionComponent>(player);
      auto velocity = playerView.get<VelocityComponent>(player);
      auto& collider = playerView.get<BoxColliderComponent>(player);

      int newPlayerX = position.x + velocity.x * dT;
      int newPlayerY = position.y + velocity.y * dT;

      SDL_Rect playerRect = {
        newPlayerX + collider.rect.x,
        newPlayerY + collider.rect.y,
        collider.rect.w,
        collider.rect.h,
      };

      for (auto tile: tileView) {
        auto [tposition, tcollider] = tileView.get<PositionComponent, BoxColliderComponent>(tile);

        SDL_Rect tileRect = {
          tposition.x + tcollider.rect.x,
          tposition.y + tcollider.rect.y,
          tcollider.rect.w,
          tcollider.rect.h,
        };

        if (SDL_HasIntersection(&playerRect, &tileRect)) {
          collider.collisionType = CollisionType::WALL; 
        }
      }
    }
  }
};

class PlayerWallCollisionSystem : public UpdateSystem {
public: 
  void run (float dT) override {
    auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent, VelocityComponent>();

    for (auto player : playerView) {
      auto& collider = playerView.get<BoxColliderComponent>(player);
      auto& velocity = playerView.get<VelocityComponent>(player);

      if (collider.collisionType == CollisionType::WALL) {
        velocity.x = 0;
        velocity.y = 0;
      }
    }
  }
};


