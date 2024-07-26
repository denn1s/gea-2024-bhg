#include "Scene.h"
#include <print>

#include "Entity.h"
#include "Components.h"
#include "Systems.h"

Scene::Scene(const std::string& n, entt::registry& r)
  : name(n), r(r)
{
  std::print("Scene Initialized: {}\n", n);
}

Scene::~Scene() {
  setupSystems.clear();
  std::print("Scene Destroyed: {}\n", name);
}

Entity* Scene::createEntity(const std::string& name) {
  Entity* entity = new Entity(r.create(), this);
  entity->addComponent<NameComponent>(name);
  return entity;
}

Entity* Scene::createEntity(const std::string& name, int x, int y) {
  Entity* entity = new Entity(r.create(), this);
  entity->addComponent<NameComponent>(name);
  entity->addComponent<PositionComponent>(x, y);

  return entity;
}

void Scene::setup() {
  std::print("Scene setting up!\n");

  for (auto sys: setupSystems) {
    sys->run();
  }
}
