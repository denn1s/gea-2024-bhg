#pragma once
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <SDL2/SDL.h>
#include "Tilemap.h"
#include "Player.h"


// Collision type enum
enum class CollisionType {
  NONE,
  WALL,
  TRIGGER,
  DAMAGE,
  COLLECTIBLE,
  // Add more collision types as needed
};

// Updated BoxColliderComponent
struct BoxColliderComponent {
  SDL_Rect rect;
  SDL_Color color;
  CollisionType collisionType = CollisionType::NONE;
};

class ColliderResetSystem : public UpdateSystem {
public:
  void run(float dt) override {
    auto view = scene->r.view<BoxColliderComponent>();
    for (auto entity : view) {
      auto& collider = view.get<BoxColliderComponent>(entity);
      collider.collisionType = CollisionType::NONE;
    }
  }
};

class ColliderRenderSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) override {
    auto view = scene->r.view<PositionComponent, BoxColliderComponent>();
    for (auto entity : view) {
      auto [position, collider] = view.get<PositionComponent, BoxColliderComponent>(entity);

      SDL_Rect renderRect = {
        position.x + collider.rect.x,
        position.y + collider.rect.y,
        collider.rect.w,
        collider.rect.h
      };

      SDL_SetRenderDrawColor(renderer, 
                             collider.color.r, 
                             collider.color.g, 
                             collider.color.b, 
                             collider.color.a);
      SDL_RenderDrawRect(renderer, &renderRect);
    }
  }
};


class TilemapColliderSetupSystem : public SetupSystem {
public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
      auto& tilemap = view.get<TilemapComponent>(entity);

      for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
          int index = y * tilemap.width + x;
          const Tile& tile = tilemap.tiles[index];

          createColliderEntity(x, y, tilemap.tileSize * tilemap.scale, tile.type);
        }
      }
    }
  }

private:
  void createColliderEntity(int x, int y, int size, TileType type) {
    Entity* colliderEntity = scene->createEntity("TILE");

    colliderEntity->addComponent<PositionComponent>(x * size, y * size);

    SDL_Rect colliderRect = {0, 0, size, size};
    if (type == TileType::WALL) {
      SDL_Color colliderColor = {0, 0, 255, 255};
      colliderEntity->addComponent<BoxColliderComponent>(colliderRect, colliderColor, CollisionType::WALL);
    }
  }
};


class PlayerWallCollisionDetectionSystem : public UpdateSystem {
public:
    void run(float dt) override {
        auto playerView = scene->r.view<PlayerComponent, BoxColliderComponent>();
        auto wallView = scene->r.view<BoxColliderComponent>();

        for (auto playerEntity : playerView) {
            auto& playerCollider = playerView.get<BoxColliderComponent>(playerEntity);
            auto playerPos = scene->r.get<PositionComponent>(playerEntity);

            SDL_Rect playerRect = {
                playerPos.x + playerCollider.rect.x,
                playerPos.y + playerCollider.rect.y,
                playerCollider.rect.w,
                playerCollider.rect.h
            };

            for (auto wallEntity : wallView) {
                if (playerEntity == wallEntity) continue; // Skip self-collision

                auto& wallCollider = wallView.get<BoxColliderComponent>(wallEntity);
                if (wallCollider.collisionType != CollisionType::WALL) continue;

                auto wallPos = scene->r.get<PositionComponent>(wallEntity);
                SDL_Rect wallRect = {
                    wallPos.x + wallCollider.rect.x,
                    wallPos.y + wallCollider.rect.y,
                    wallCollider.rect.w,
                    wallCollider.rect.h
                };

                if (SDL_HasIntersection(&playerRect, &wallRect)) {
                    playerCollider.collisionType = CollisionType::WALL;
                    break; // Exit the loop once a collision is detected
                }
            }
        }
    }
};

class PlayerWallHitResponseSystem : public UpdateSystem {
public:
    void run(float dt) override {
        auto view = scene->r.view<PlayerComponent, BoxColliderComponent, VelocityComponent>();

        for (auto entity : view) {
            auto [collider, velocity] = view.get<BoxColliderComponent, VelocityComponent>(entity);

            if (collider.collisionType == CollisionType::WALL) {
                // Stop the player's movement
                velocity.x = -velocity.x;
                velocity.y = -velocity.y;

                // Reset collision type
                collider.collisionType = CollisionType::NONE;
            }
        }
    }
};

