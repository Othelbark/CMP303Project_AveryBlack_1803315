#pragma once
#include "TargetObject.h"
#include "RemoteObject.h"

class RemoteTarget :
    public TargetObject, public RemoteObject
{
public:
    RemoteTarget();
    ~RemoteTarget();

    void update(float dt) override;

};

