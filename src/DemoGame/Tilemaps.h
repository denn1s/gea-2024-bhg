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
      {0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    tilemap->addComponent<TilemapComponent>("assets/Tilesets/tileset.png", map, 8, 8);
    tilemap->addComponent<TextureComponent>("assets/Tilesets/tileset.png");
  }
};

class AutoTilingSetupSystem : public SetupSystem {
private:
    bool isTile(const std::vector<std::vector<int>>& map, int x, int y) {
        return (x >= 0 && x < map[0].size() && y >= 0 && y < map.size() && map[y][x] == 1);
    }

public:
    void run() override {
        auto view = scene->r.view<TilemapComponent>();
        
        for (auto entity : view) {
            auto& tilemap = view.get<TilemapComponent>(entity);
            
            const size_t mapHeight = tilemap.map.size();
            const size_t mapWidth = tilemap.map[0].size();
            
            std::vector<std::vector<int>> newMap = tilemap.map;
            
            for (size_t y = 0; y < mapHeight; ++y) {
                for (size_t x = 0; x < mapWidth; ++x) {
                    if (tilemap.map[y][x] == 1) {
                        bool north = isTile(tilemap.map, x, y-1);
                        bool south = isTile(tilemap.map, x, y+1);
                        bool west = isTile(tilemap.map, x-1, y);
                        bool east = isTile(tilemap.map, x+1, y);

                        int mask = 0;
                        if (north) mask |= 1;
                        if (west) mask |= 2;
                        if (east) mask |= 4;
                        if (south) mask |= 8;

                        newMap[y][x] = mask;
                    } else {
                        newMap[y][x] = -1;  // Empty space
                    }
                }
            }
            
            tilemap.map = newMap;
        }
    }
};

class AdvancedAutoTilingSetupSystem : public SetupSystem {
private:
  std::unordered_map<int, int> maskToTileIndex = {
    {2, 1}, {8, 2}, {10, 3}, {11, 4}, {16, 5}, {18, 6}, {22, 7}, {24, 8},
    {26, 9}, {27, 10}, {30, 11}, {31, 12}, {64, 13}, {66, 14}, {72, 15},
    {74, 16}, {75, 17}, {80, 18}, {82, 19}, {86, 20}, {88, 21}, {90, 22},
    {91, 23}, {94, 24}, {95, 25}, {104, 26}, {106, 27}, {107, 28}, {120, 29},
    {122, 30}, {123, 31}, {126, 32}, {127, 33}, {208, 34}, {210, 35},
    {214, 36}, {216, 37}, {218, 38}, {219, 39}, {222, 40}, {223, 41},
    {248, 42}, {250, 43}, {251, 44}, {254, 45}, {255, 46}, {0, 47}
  };


  bool isTile(const std::vector<std::vector<int>>& map, int x, int y) {
    return (x >= 0 && x < map[0].size() && y >= 0 && y < map.size() && map[y][x] == 1);
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
      auto& tilemap = view.get<TilemapComponent>(entity);

      const size_t mapHeight = tilemap.map.size();
      const size_t mapWidth = tilemap.map[0].size();

      std::vector<std::vector<int>> newMap = tilemap.map;

      for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
          if (tilemap.map[y][x] == 1) {
            int mask = 0;

            bool north = isTile(tilemap.map, x, y-1);
            bool south = isTile(tilemap.map, x, y+1);
            bool west = isTile(tilemap.map, x-1, y);
            bool east = isTile(tilemap.map, x+1, y);

            // Cardinal directions
            if (north) mask |= 2;
            if (west) mask |= 8;
            if (east) mask |= 16;
            if (south) mask |= 64;

            // Corners (with redundancy check)
            if (north && west && isTile(tilemap.map, x-1, y-1)) mask |= 1;
            if (north && east && isTile(tilemap.map, x+1, y-1)) mask |= 4;
            if (south && west && isTile(tilemap.map, x-1, y+1)) mask |= 32;
            if (south && east && isTile(tilemap.map, x+1, y+1)) mask |= 128;

            // Map the mask to the tile index
            auto it = maskToTileIndex.find(mask);
            if (it != maskToTileIndex.end()) {
              newMap[y][x] = it->second;
            } else {
              // If the mask doesn't have a mapping, use a default tile
              newMap[y][x] = 47;  // Assuming 47 is your default tile
            }
          } else {
            newMap[y][x] = -1;  // Empty space
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
