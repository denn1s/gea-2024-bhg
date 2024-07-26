#include <string>
#include <print>
#include "Scene/Components.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Systems.h"
#include <entt/entt.hpp>

class PlayerSetupSystem : public SetupSystem {
  void run() {
    Entity* player1 = scene->createEntity("PLAYER 1");
    Entity* player2 = scene->createEntity("PLAYER 2");
    Entity* player3 = scene->createEntity("PLAYER 3");
  }
};

class PrintEntitiesSetupSystem : public SetupSystem {
  void run() {
    auto view = scene->r.view<NameComponent>();

    for (auto entity : view) {
      const auto nameComponent = scene->r.get<NameComponent>(entity);

      std::print("nameComponent.name: {}\n", nameComponent.name);
    }
  }
};

int main() {
  entt::registry r;

  Scene* field = new Scene("FIELD", r);


  auto playerSetupSystem = new PlayerSetupSystem();
  playerSetupSystem->setScene(field);
  field->setupSystems.push_back(playerSetupSystem);

  auto printEntitySetupSystem = new PrintEntitiesSetupSystem();
  printEntitySetupSystem->setScene(field);
  field->setupSystems.push_back(printEntitySetupSystem);

  field->setup();

  delete field;
  return 0;
}
