#pragma once

#include <fmod/fmod.hpp>
#include <string>
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Game.h"

// Enum to specify sound type
enum class SoundType {
  BACKGROUND,
  EFFECT
};

// Component to contain sound information
struct SoundComponent {
  SoundType type;
  std::string identifier;
  FMOD::Sound* sound;
  FMOD::Channel* channel;
};

// System to initialize and load sounds
class SoundSetupSystem : public SetupSystem {
public:
  void run() override {
    // Load background music
    Entity* bgMusic = scene->createEntity("BACKGROUND_MUSIC");
    FMOD::Sound* music;
    game->fmodSystem->createStream("assets/Sounds/background.wav", FMOD_LOOP_NORMAL, nullptr, &music);
    bgMusic->addComponent<SoundComponent>(SoundType::BACKGROUND, "background", music, nullptr);

    // Load jump sound effect
    Entity* jumpSound = scene->createEntity("JUMP_SOUND");
    FMOD::Sound* jump;
    game->fmodSystem->createSound("assets/Sounds/jump.wav", FMOD_DEFAULT, nullptr, &jump);
    jumpSound->addComponent<SoundComponent>(SoundType::EFFECT, "jump", jump, nullptr);
  }
};

class BackgroundMusicSystem : public SetupSystem {
public:
  void run() override {
    FMOD::System* fmodSystem = game->fmodSystem;

    auto view = scene->r.view<SoundComponent>();
    for (auto entity : view) {
      auto& sound = view.get<SoundComponent>(entity);
      if (sound.type == SoundType::BACKGROUND) {
        if (sound.channel) {
          // If the channel exists, stop the current playback
          sound.channel->stop();
        }
        // Start playing the background music
        fmodSystem->playSound(sound.sound, nullptr, false, &sound.channel);
        // Set the channel mode to loop
        sound.channel->setMode(FMOD_LOOP_NORMAL);
        break;
      }
    }
  }
};
