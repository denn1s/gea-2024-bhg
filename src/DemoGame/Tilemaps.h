#pragma once
#include <string>
#include <iostream>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Sprites.h"

struct TilemapComponent {
  std::string tilesetFilename;
  std::vector<std::vector<int>> map;
  int tileSize;
  int scale = 1;
};

// TilemapSetupSystem to initialize the tilemap
class TilemapSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* tilemap = scene->createEntity("TILEMAP");

    // Example 10x10 tilemap
    std::vector<std::vector<int>> map = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    tilemap->addComponent<TilemapComponent>("assets/Tilesets/tileset.png", map, 8, 8);
    tilemap->addComponent<TextureComponent>("assets/Tilesets/tileset.png");
  }
};

class AutoTilingSetupSystem : public SetupSystem {
public:
    void run() override {
        auto view = scene->r.view<TilemapComponent>();
        
        for (auto entity : view) {
            auto& tilemap = view.get<TilemapComponent>(entity);
            
            std::vector<std::vector<int>> newMap = tilemap.map;
            
            for (size_t y = 0; y < tilemap.map.size(); ++y) {
                for (size_t x = 0; x < tilemap.map[y].size(); ++x) {
                    if (tilemap.map[y][x] == 1) {
                        int mask = 0;
                        
                        // Check North
                        if (y > 0 && tilemap.map[y-1][x] == 1) mask |= 1;
                        // Check West
                        if (x > 0 && tilemap.map[y][x-1] == 1) mask |= 2;
                        // Check East
                        if (x < tilemap.map[y].size() - 1 && tilemap.map[y][x+1] == 1) mask |= 4;
                        // Check South
                        if (y < tilemap.map.size() - 1 && tilemap.map[y+1][x] == 1) mask |= 8;
                        
                        /* std::cout << "mask: " << mask << std::endl; */
                        // Convert mask to tileset index (1-based)
                        newMap[y][x] = mask;
                    } else {
                        newMap[y][x] = -1;
                    }
                }
            }
            
            tilemap.map = newMap;
        }
    }
};

class TilemapRenderSystem : public RenderSystem {
public:
  void run(SDL_Renderer* renderer) override {
    auto view = scene->r.view<TilemapComponent, TextureComponent>();

    for (auto entity : view) {
      auto [tilemap, texture] = view.get<TilemapComponent, TextureComponent>(entity);

      Texture* tilesetTexture = TextureManager::GetTexture(texture.filename);
      if (!tilesetTexture) continue;
      int tileSize = tilemap.tileSize * tilemap.scale;

      for (size_t y = 0; y < tilemap.map.size(); ++y) {
        for (size_t x = 0; x < tilemap.map[y].size(); ++x) {
          int tileIndex = tilemap.map[y][x];
          if (tileIndex >= 0) {  // Only render non-empty tiles
            SDL_Rect destRect = {
              static_cast<int>(x * tileSize),
              static_cast<int>(y * tileSize),
              tileSize,
              tileSize
            };

            // Calculate source rectangle based on tile index
            int tileColumn = tileIndex % 4;
            int tileRow = tileIndex / 4;

            SDL_Rect srcRect = {
              tileColumn * tilemap.tileSize,
              tileRow * tilemap.tileSize,
              tilemap.tileSize,
              tilemap.tileSize
            };

            tilesetTexture->render(renderer, destRect.x, destRect.y, destRect.w, destRect.h, &srcRect);
          }
        }
      }
    }
  }
};
