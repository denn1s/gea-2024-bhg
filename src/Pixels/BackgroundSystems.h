#pragma once
#include <SDL2/SDL.h>
#include "Engine/Systems.h"
#include "Engine/Entity.h"
#include "PixelSystems.h"

struct MiniFBComponent  {
};

class MiniFBSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* framebuffer = scene->createEntity("MINI FRAMEBUFFER");

    FatPixelComponent fatPixelComp;
    fatPixelComp.grid = 0;
    fatPixelComp.color = {255, 255, 255, 255};
    fatPixelComp.width = 10;
    fatPixelComp.height = 10;
    framebuffer->addComponent<FatPixelComponent>(fatPixelComp);
    framebuffer->addComponent<MiniFBComponent>();
  }
};

class MiniFBRenderSystem : public RenderSystem {
public:
    void run(SDL_Renderer* renderer) override {
        auto view = scene->r.view<MiniFBComponent, FatPixelComponent>();
        
        for (auto entity : view) {
            auto fatPixel = view.get<FatPixelComponent>(entity);
            /* auto [minifb] = view.get<MiniFBComponent, FatPixelComponent>(entity); */
            
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
