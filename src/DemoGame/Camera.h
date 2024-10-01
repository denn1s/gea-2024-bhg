#pragma once

#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <SDL_events.h>
#include <SDL_timer.h>
#include <print>
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Player.h"


class CameraSetupSystem : public SetupSystem {
  void run() {
    int width = 1024;
    int height = 768;
    scene->mainCamera = scene->createEntity("CAMERA", 0, 0); 
    scene->mainCamera->addComponent<CameraComponent>(
      1,
      width,
      height,
      width * 10,
      height
    );
  }
};

class CameraFollowUpdateSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PlayerComponent, PositionComponent, SpriteComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto& cameraComponent = scene->mainCamera->get<CameraComponent>();

    for (auto e : view) {
      auto playerPos = view.get<PositionComponent>(e);
      auto playerSpr = view.get<SpriteComponent>(e);

      int spriteWidth = playerSpr.width * playerSpr.scale * cameraComponent.zoom;
      int spriteHeight = playerSpr.height * playerSpr.scale * cameraComponent.zoom;

      int px = playerPos.x - cameraComponent.vw / 2 + spriteWidth / 2;
      int py = playerPos.y - cameraComponent.vh / 2 + spriteHeight / 2;

      if (px > 0 && px < cameraComponent.ww - cameraComponent.vw) {
        cameraPosition.x = px;
      }

      if (py > 0 && py < cameraComponent.wh - cameraComponent.vh) {
        cameraPosition.y = py;
      }
    }
  }
};
