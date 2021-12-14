#pragma once
#include "Framework/GameObject.h"

class TargetObject :
    public GameObject
{
public:


    GameObject* getFor() { return targetFor; };
    void setFor(GameObject* f) { targetFor = f; };
private:

    GameObject* targetFor;
};

