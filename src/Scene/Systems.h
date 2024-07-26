#pragma once
#include "Scene.h"

class System {
protected:
  Scene* scene;

public:
  void setScene(Scene* s) { scene = s; };
};

class SetupSystem : public System {
public:
  virtual void run() = 0;
};

