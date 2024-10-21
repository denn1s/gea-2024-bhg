#pragma once
#include <random>
#include <chrono>
#include <sol/forward.hpp>
#include <string>
#include <iostream>
#include <sol/sol.hpp>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Sprites.h"
#include "Player.h"

struct EnemyComponent {
  float spawnTime;
};

struct LuaScriptComponent {
  std::string scriptPath;
  sol::environment env;
  sol::function updateFunc;
};

class EnemySpawnSystem : public UpdateSystem {
private:
  float lastSpawnTime;
  std::mt19937 rng;

  void createEnemy(const int pos, const int height) {
    std::uniform_int_distribution<int> yDist(0, height);
    int randomY = yDist(rng);
    std::string snake = "assets/Sprites/snake.png";
    TextureManager::LoadTexture(snake, scene->renderer);

    Entity* enemy = scene->createEntity("ENEMY", pos, randomY);
    enemy->addComponent<EnemyComponent>(SDL_GetTicks() / 1000.0f);
    enemy->addComponent<TextureComponent>(snake);
    enemy->addComponent<SpriteComponent>(snake, 8, 8, 10, 3, 1000);
    enemy->addComponent<LuaScriptComponent>("assets/Scripts/enemy_move.lua");
  }

public:
  EnemySpawnSystem() : lastSpawnTime(0) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng = std::mt19937(seed);
  }

  void run(float dT) override {
    auto cameraView = scene->r.view<CameraComponent, PositionComponent>();
    auto& cameraPosition = scene->mainCamera->get<PositionComponent>();
    auto& cameraComponent = scene->mainCamera->get<CameraComponent>();

    float currentTime = SDL_GetTicks() / 1000.0f;

    if ((currentTime - lastSpawnTime) < 5.0f) {
      return;
    }

    /* if (cameraPosition.x >= 1000 && cameraPosition.x <= 10000) { */
      createEnemy(cameraPosition.x + cameraComponent.vw * cameraComponent.zoom + 100, cameraComponent.vh - 80);
      lastSpawnTime = currentTime;
    /* } */
  }
};

class LuaScriptSetupSystem : public UpdateSystem {
private:
  sol::state lua;

public:
  LuaScriptSetupSystem() {
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
  }

  void run(float dT) override {
    auto view = scene->r.view<LuaScriptComponent>();
    for (auto entity : view) {
      auto& script = view.get<LuaScriptComponent>(entity);
      if (script.env == sol::nil) {
        initializeScript(script);
      }
    }
  }

private:
  void initializeScript(LuaScriptComponent& script) {
    try {
      script.env = sol::environment(lua, sol::create, lua.globals());
      lua.script_file(script.scriptPath, script.env);
      script.updateFunc = script.env["update"];
    } catch (const sol::error& e) {
      std::cerr << "Error initializing Lua script: " << e.what() << std::endl;
    }
  }
};

class EnemyMoveScriptSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto playerView = scene->r.view<PlayerComponent, PositionComponent>();
    auto enemyView = scene->r.view<EnemyComponent, PositionComponent, LuaScriptComponent>();

    // Assume there's only one player
    auto playerEntity = playerView.front();
    auto& playerPos = playerView.get<PositionComponent>(playerEntity);

    for (auto entity : enemyView) {
      auto& enemyPos = enemyView.get<PositionComponent>(entity);
      auto& script = enemyView.get<LuaScriptComponent>(entity);

      if (script.env != sol::nil && script.updateFunc.valid()) {
        sol::table params = script.env.create();
        params["deltaTime"] = dT;
        params["time"] = SDL_GetTicks() / 1000.0f;
        params["playerX"] = playerPos.x;
        params["playerY"] = playerPos.y;
        params["enemyX"] = enemyPos.x;
        params["enemyY"] = enemyPos.y;

        auto result = script.updateFunc(params);

        if (result.valid()) {
          sol::table newPos = result;
          enemyPos.x = newPos["x"].get<int>();
          enemyPos.y = newPos["y"].get<int>();
        } else {
          sol::error err = result;
          std::cerr << "Error running Lua script: " << err.what() << std::endl;
        }
      }
    }
  }
};

class EnemyLifetimeSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<EnemyComponent>();
        float currentTime = SDL_GetTicks() / 1000.0f;

        for (auto entity : view) {
            auto& enemy = view.get<EnemyComponent>(entity);
            
            if (currentTime - enemy.spawnTime > 5.0f) {
        std::cout << "destroy enemy" << std::endl;
                scene->r.destroy(entity);
            }
        }
    }
};
