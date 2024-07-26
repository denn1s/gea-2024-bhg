#pragma once

#include <string>
#include <entt/entt.hpp>

class Entity;
class SetupSystem;  // loading

class Scene {
public:
  std::vector<SetupSystem*> setupSystems;

  Scene(const std::string&, entt::registry&);
  ~Scene();

  Entity* createEntity(const std::string&);
  Entity* createEntity(const std::string&,int,int);

  void setup();

  entt::registry& r;
  std::string name;
};
