#pragma once
#include <SDL2/SDL.h>
#include "Engine/Components.h"
#include "Engine/Systems.h"

// Define the size of the fat pixel grid
const int FAT_PIXEL_GRID_WIDTH = 8;
const int FAT_PIXEL_GRID_HEIGHT = 8;

// Component to store the fat pixel grid
struct FatPixelComponent {
    unsigned long grid;
    int scale;
    SDL_Color color;
};

// System to render fat pixels
class FatPixelRenderSystem : public RenderSystem {
public:
    void run(SDL_Renderer* renderer) override {
        auto view = scene->r.view<PositionComponent, FatPixelComponent>();
        
        for (auto entity : view) {
            auto [position, fatPixel] = view.get<PositionComponent, FatPixelComponent>(entity);
            
            SDL_SetRenderDrawColor(renderer, fatPixel.color.r, fatPixel.color.g, fatPixel.color.b, fatPixel.color.a);
            
            for (int y = 0; y < FAT_PIXEL_GRID_HEIGHT; ++y) {
                for (int x = 0; x < FAT_PIXEL_GRID_WIDTH; ++x) {
                    if (fatPixel.grid & (1UL << (y * FAT_PIXEL_GRID_WIDTH + x))) {
                        SDL_Rect pixelRect = {
                            position.x + x * fatPixel.scale,
                            position.y + y * fatPixel.scale,
                            fatPixel.scale,
                            fatPixel.scale
                        };
                        SDL_RenderFillRect(renderer, &pixelRect);
                    }
                }
            }
        }
    }
};
