#pragma once
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include "Sprites.h"
#include "Camera.h"
#include "Tilemap.h"
#include "Sound.h"
#include "Enemy.h"

#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <box2d/box2d.h>

// physics constants
const float PIXELS_TO_METERS = 0.01f;
const float GRAVITY = 9.8f;
const float JUMP_FORCE = 3.0f;
const float FORWARD_FORCE = 2.0f;

// components
struct PhysicsWorldComponent {
  b2WorldId worldId;
};

struct RigidBodyComponent {
  b2BodyId bodyId;
};

class PhysicsSetupSystem : public SetupSystem {
public:
  void run() override {
    b2Vec2 gravity(0.0f, GRAVITY);
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity;
    /* worldDef.enableSleep = false; */
    b2WorldId worldId = b2CreateWorld(&worldDef);

    Entity* worldEntity = scene->createEntity("Physics World");
    worldEntity->addComponent<PhysicsWorldComponent>(worldId);
  }
};

class PhysicsSimulationSytem : public UpdateSystem {
public:
  void run(float dT) override {
    auto worldView = scene->r.view<PhysicsWorldComponent>();
    auto worldId = worldView.get<PhysicsWorldComponent>(worldView.front()).worldId;

    b2World_Step(worldId, dT, 4);
  }
};


class PlayerSetupSystem : public SetupSystem {
public:
  void run() override {
    float px = 100;
    float py = 300;
    float size = 8;
    float scale = 10;

    Entity* player = scene->createEntity("PLAYER", px, py);
    player->addComponent<PlayerComponent>();
    player->addComponent<TextureComponent>("assets/Sprites/cat.png");
    player->addComponent<SpriteComponent>("assets/Sprites/cat.png", size, size, scale, 8, 1000);

    // physics stuff
    auto worldView = scene->r.view<PhysicsWorldComponent>();
    auto worldId = worldView.get<PhysicsWorldComponent>(worldView.front()).worldId;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){px * PIXELS_TO_METERS, py * PIXELS_TO_METERS};
    bodyDef.fixedRotation = true;
    /* bodyDef.enableSleep = false; */
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 0.0f;

    b2Polygon dynamicBox = b2MakeBox(
      ((size * scale) / 2.0f) * PIXELS_TO_METERS,
      ((size * scale) / 2.0f) * PIXELS_TO_METERS
    );

    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);

    player->addComponent<RigidBodyComponent>(bodyId);
  }
};

class PositionUpdateSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto view = scene->r.view<PositionComponent, RigidBodyComponent>();

    for (auto entity : view) {
      auto& position = view.get<PositionComponent>(entity);
      auto rigidBody = view.get<RigidBodyComponent>(entity);

      b2Vec2 physical_position = b2Body_GetPosition(rigidBody.bodyId);
      position.x = physical_position.x / PIXELS_TO_METERS;
      position.y = physical_position.y / PIXELS_TO_METERS;
    }
  }
};

class PlayerInputSystem : public EventSystem {
public:
  void run(SDL_Event event) override {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
      auto view = scene->r.view<PlayerComponent, RigidBodyComponent>();

      for (auto entity : view) {
        auto rigidBody = view.get<RigidBodyComponent>(entity);
        b2Vec2 velocity = b2Body_GetLinearVelocity(rigidBody.bodyId);
        velocity.y = 0;
        b2Body_SetLinearVelocity(rigidBody.bodyId, velocity);

        b2Vec2 impulse(0, -JUMP_FORCE);
        b2Body_ApplyLinearImpulseToCenter(rigidBody.bodyId, impulse, true);
        playJumpSound();
      }
    }
  }
private:
  void playJumpSound() {
    auto view = scene->r.view<SoundComponent>();

    for (auto entity : view) {
      auto& soundComponent = view.get<SoundComponent>(entity);
      if (soundComponent.type == SoundType::EFFECT && soundComponent.filename == "assets/Sounds/jump.wav") {
        if (soundComponent.channel) {
          soundComponent.channel->stop();
        }
        game->soundManager->playSound(soundComponent.sound, nullptr, false, &soundComponent.channel);
      }
    }

  }
};

class PlayerForwardSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto view = scene->r.view<PlayerComponent, RigidBodyComponent>();

    for (auto entity : view) {
      auto rigidBody = view.get<RigidBodyComponent>(entity);
      b2Vec2 velocity = b2Body_GetLinearVelocity(rigidBody.bodyId);
      velocity.x = FORWARD_FORCE;
      b2Body_SetLinearVelocity(rigidBody.bodyId, velocity);
    }
  }
};

class PlayerJumpAnimationUpdateSystem : public UpdateSystem {
public:
  void run(float dT) override {
    auto view = scene->r.view<PlayerComponent, RigidBodyComponent, SpriteComponent>();

    for (auto entity : view) {
      auto rigidBody = view.get<RigidBodyComponent>(entity);
      auto& spr = view.get<SpriteComponent>(entity);

      b2Vec2 velocity = b2Body_GetLinearVelocity(rigidBody.bodyId);
      if (velocity.y < 0) {
        spr.yIndex = 1;
      } else {
        spr.yIndex = 0;
      }
    }
  }
};

class FlappyCatGame : public Game {
public:
  Scene* gameScene;
  entt::registry r;

  FlappyCatGame()
  : Game("Flappy Cat The Game", 1980, 1080) { }

  void setup() override {
    gameScene = new Scene("gameplay", r, renderer);

    // setup systems
    addSetupSystem<CameraSetupSystem>(gameScene);
    addSetupSystem<PhysicsSetupSystem>(gameScene);
    addSetupSystem<PlayerSetupSystem>(gameScene);

    addSetupSystem<ProceduralTilemapSetupSystem>(gameScene);
    addSetupSystem<AdvancedAutoTilingSetupSystem>(gameScene);

    addSetupSystem<TextureSetupSystem>(gameScene);

    // sound systems
    addSetupSystem<BackgroundMusicSetupSystem>(gameScene);
    /* addSetupSystem<FxMusicSetupSystem>(gameScene); */
    addSetupSystem<SoundSetupSystem>(gameScene);
    /* addSetupSystem<BackgroundMusicPlaySetupSystem>(gameScene); */

    // update systems
    addUpdateSystem<SpriteAnimationSystem>(gameScene);
    addUpdateSystem<PlayerForwardSystem>(gameScene);
    addUpdateSystem<PhysicsSimulationSytem>(gameScene);
    addUpdateSystem<PositionUpdateSystem>(gameScene);
    addUpdateSystem<CameraFollowUpdateSystem>(gameScene);
    addUpdateSystem<PlayerJumpAnimationUpdateSystem>(gameScene);
    addUpdateSystem<EnemySpawnSystem>(gameScene);
    addUpdateSystem<LuaScriptSetupSystem>(gameScene);
    addUpdateSystem<EnemyMoveScriptSystem>(gameScene);
    addUpdateSystem<EnemyLifetimeSystem>(gameScene);

    // input systems
    addEventSystem<PlayerInputSystem>(gameScene);

    // render systems
    /* addRenderSystem<TilemapRenderSystem>(gameScene); */
    addRenderSystem<SpriteRenderSystem>(gameScene);

    setScene(gameScene);
  }
};
