#pragma once
#include <SDL2/SDL.h>
#include "Engine/Systems.h"
#include "Engine/Components.h"

// Global scale for fat pixels
const int FAT_PIXEL_SCALE = 4;

// Component to store the fat pixel grid
struct FatPixelComponent {
  unsigned long grid;
  SDL_Color color;
  unsigned int width;
  unsigned int height;
};

// System to render fat pixels
class FatPixelRenderSystem : public RenderSystem {
public:
    void run(SDL_Renderer* renderer) override {
        auto view = scene->r.view<PositionComponent, FatPixelComponent>();
        
        for (auto entity : view) {
            auto [position, fatPixel] = view.get<PositionComponent, FatPixelComponent>(entity);
            
            SDL_SetRenderDrawColor(renderer, fatPixel.color.r, fatPixel.color.g, fatPixel.color.b, fatPixel.color.a);
            
            for (int y = 0; y < fatPixel.height; ++y) {
                for (int x = 0; x < fatPixel.width; ++x) {
                    if (fatPixel.grid & (1UL << (y * fatPixel.width + x))) {
                        SDL_Rect pixelRect = {
                            position.x + x * FAT_PIXEL_SCALE,
                            position.y + y * FAT_PIXEL_SCALE,
                            FAT_PIXEL_SCALE,
                            FAT_PIXEL_SCALE
                        };
                        SDL_RenderFillRect(renderer, &pixelRect);
                    }
                }
            }
        }
    }
};
